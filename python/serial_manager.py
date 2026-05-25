import serial
import serial.tools.list_ports
import threading
import time
import queue

class SerialManager:
    def __init__(self):
        self.conn = None
        self.port = ""
        self.baudrate = 115200
        
        # Thread control
        self.stream_thread = None
        self.read_thread = None
        self.is_running = False
        
        # Streaming state variables (thread-safe lock)
        self.state_lock = threading.Lock()
        self.is_connected = False
        self.is_streaming = False
        self.is_paused = False
        
        # Streaming indices
        self.gcode_lines = []
        self.total_lines = 0
        self.current_idx = 0
        
        # Event callbacks
        self.status_callback = None # Signature: (status_type, message, progress_pct)
        
        # Queue for thread-safe UI commands
        self.cmd_queue = queue.Queue()

    def get_ports(self):
        """Returns a list of available COM ports."""
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports]

    def connect(self, port, baudrate=115200, status_callback=None):
        """Attempts to connect to the specified COM port."""
        self.disconnect() # Close any existing connection first
        
        self.port = port
        self.baudrate = baudrate
        self.status_callback = status_callback
        
        try:
            # Open serial port with 1 second timeout
            self.conn = serial.Serial(port, baudrate, timeout=1.0)
            
            # Wait for GRBL to initialize and send greeting (usually takes 1-2 seconds after opening port)
            time.sleep(1.5)
            self.conn.reset_input_buffer()
            self.conn.reset_output_buffer()
            
            with self.state_lock:
                self.is_connected = True
                self.is_streaming = False
                self.is_paused = False
                self.is_running = True
                
            # Start background read/stream thread
            self.read_thread = threading.Thread(target=self._read_loop, daemon=True)
            self.read_thread.start()
            
            # Send wake up character and soft reset
            self.conn.write(b"\r\n\r\n")
            time.sleep(0.2)
            self.conn.reset_input_buffer()
            
            self._notify("connect", f"Connected to {port} at {baudrate}", 0)
            return True
        except Exception as e:
            self._notify("error", f"Connection failed: {e}", 0)
            self.disconnect()
            return False

    def disconnect(self):
        """Closes the serial connection and halts background threads."""
        self.is_running = False
        
        if self.stream_thread and self.stream_thread.is_alive():
            # Wait for it to stop
            self.stream_thread.join(timeout=1.0)
            
        if self.read_thread and self.read_thread.is_alive():
            self.read_thread.join(timeout=1.0)
            
        with self.state_lock:
            self.is_connected = False
            self.is_streaming = False
            self.is_paused = False
            
        if self.conn and self.conn.is_open:
            try:
                # Turn off laser/spindle just in case
                self.conn.write(b"M5\r\n")
                time.sleep(0.1)
                self.conn.close()
            except:
                pass
        self.conn = None
        self._notify("disconnect", "Disconnected", 0)

    def write_command(self, cmd):
        """Writes a manual command (e.g. jog or pen test) to GRBL if not streaming."""
        if not self.is_connected or self.conn is None:
            return False
            
        with self.state_lock:
            if self.is_streaming:
                # Only allow real-time commands while streaming
                if cmd in ("!", "~", "?", "\x18"):
                    self.conn.write(cmd.encode())
                    return True
                return False
                
        try:
            # Normal command, append newline
            formatted = cmd.strip() + "\n"
            self.conn.write(formatted.encode('ascii'))
            self._notify("console_tx", cmd.strip(), 0)
            return True
        except Exception as e:
            self._notify("error", f"Write failed: {e}", 0)
            self.disconnect()
            return False

    def start_stream(self, gcode_text):
        """Starts streaming the provided G-code string."""
        if not self.is_connected or self.conn is None:
            self._notify("error", "Cannot stream: Not connected.", 0)
            return False
            
        with self.state_lock:
            if self.is_streaming:
                self._notify("error", "Cannot stream: Already streaming.", 0)
                return False
                
            # Filter and sanitize G-code lines
            self.gcode_lines = []
            for line in gcode_text.splitlines():
                clean = line.strip()
                # Remove comments
                if ';' in clean:
                    clean = clean.split(';', 1)[0].strip()
                if '(' in clean and ')' in clean:
                    # remove block comments
                    clean = clean.split('(', 1)[0].strip() # simple strip
                if clean:
                    self.gcode_lines.append(clean)
                    
            self.total_lines = len(self.gcode_lines)
            self.current_idx = 0
            self.is_streaming = True
            self.is_paused = False
            
        # Start background stream loop
        self.stream_thread = threading.Thread(target=self._stream_loop, daemon=True)
        self.stream_thread.start()
        self._notify("stream_start", "G-code streaming started.", 0)
        return True

    def pause_stream(self):
        """Pauses the active stream using GRBL feed hold."""
        if not self.is_connected or not self.is_streaming:
            return
            
        with self.state_lock:
            self.is_paused = True
            
        try:
            self.conn.write(b"!") # Feed Hold (instant pause)
            self._notify("stream_paused", "Stream paused.", self._get_progress())
        except Exception as e:
            self._notify("error", f"Pause command failed: {e}", 0)
            self.disconnect()

    def resume_stream(self):
        """Resumes a paused G-code stream using GRBL cycle start."""
        if not self.is_connected or not self.is_streaming:
            return
            
        with self.state_lock:
            self.is_paused = False
            
        try:
            self.conn.write(b"~") # Cycle Start (instant resume)
            self._notify("stream_resumed", "Stream resumed.", self._get_progress())
        except Exception as e:
            self._notify("error", f"Resume command failed: {e}", 0)
            self.disconnect()

    def stop_stream(self):
        """Aborts the active stream, resets GRBL, and lifts the pen."""
        if not self.is_connected:
            return
            
        try:
            # Soft reset (Ctrl-X)
            self.conn.write(b"\x18")
            time.sleep(0.1)
            # Unlock / Turn off spindle/pen
            self.conn.write(b"$X\r\n")
            self.conn.write(b"M5\r\n")
            self.conn.write(b"G0 Z5 F1000\r\n") # retract stepper if present
        except:
            pass
            
        with self.state_lock:
            self.is_streaming = False
            self.is_paused = False
            self.gcode_lines = []
            self.current_idx = 0
            
        self._notify("stream_stopped", "Stream aborted and reset.", 0)

    def _get_progress(self):
        if self.total_lines == 0:
            return 0
        return int((self.current_idx / self.total_lines) * 100)

    def _notify(self, status_type, message, progress_pct):
        """Triggers the UI event callback."""
        if self.status_callback:
            try:
                self.status_callback(status_type, message, progress_pct)
            except Exception as e:
                print(f"Error in serial callback: {e}")

    def _read_loop(self):
        """Thread loop that reads from the serial port constantly and prints to console."""
        buffer = ""
        while self.is_running and self.conn and self.conn.is_open:
            try:
                if self.conn.in_waiting > 0:
                    data = self.conn.read(self.conn.in_waiting).decode('utf-8', errors='ignore')
                    buffer += data
                    
                    while "\n" in buffer:
                        line, buffer = buffer.split("\n", 1)
                        line = line.strip()
                        if line:
                            self._notify("console_rx", line, self._get_progress())
                            # Put incoming line in command queue to let the streamer thread process responses
                            self.cmd_queue.put(line)
                else:
                    time.sleep(0.01) # save CPU
            except Exception as e:
                if self.is_running:
                    self._notify("error", f"Read thread error: {e}", 0)
                    self.disconnect()
                break

    def _stream_loop(self):
        """
        Thread loop that streams G-code using the character-counting protocol.
        Maintains a local buffer tracking character counts in GRBL's RX buffer (127 byte limit).
        """
        grbl_buffer_max = 127
        grbl_buffer = [] # list of lengths of lines currently in GRBL's serial RX buffer
        
        # Flush queue
        while not self.cmd_queue.empty():
            self.cmd_queue.get()
            
        self.current_idx = 0
        
        while self.is_running and self.is_connected:
            # Check if streaming has been stopped in the meantime
            with self.state_lock:
                if not self.is_streaming:
                    break
                current_idx = self.current_idx
                total_lines = self.total_lines
                is_paused = self.is_paused
                
            # If paused, wait
            if is_paused:
                time.sleep(0.05)
                continue
                
            # 1. Send lines if GRBL buffer has space and we have lines left
            if current_idx < total_lines:
                next_line = self.gcode_lines[current_idx]
                line_len = len(next_line) + 1 # Include '\n' char
                
                # Check if it fits in GRBL's RX buffer
                if sum(grbl_buffer) + line_len < grbl_buffer_max:
                    try:
                        # Send line
                        self.conn.write((next_line + "\n").encode('ascii'))
                        grbl_buffer.append(line_len)
                        
                        with self.state_lock:
                            self.current_idx += 1
                            progress = self._get_progress()
                            
                        self._notify("stream_line", f"[{self.current_idx}/{total_lines}] {next_line}", progress)
                        continue # immediately try to send more if it fits
                    except Exception as e:
                        self._notify("error", f"Stream write failed: {e}", 0)
                        self.disconnect()
                        break
            
            # 2. Wait for 'ok' or 'error' to clear GRBL buffer space
            # If buffer is full or no lines left, we must wait for responses
            if grbl_buffer:
                try:
                    # Wait for a line from the read queue (blocks up to 0.1s)
                    response = self.cmd_queue.get(timeout=0.05)
                    response_clean = response.strip().lower()
                    
                    if "ok" in response_clean or "error" in response_clean:
                        # GRBL completed a command, remove its length from buffer
                        if grbl_buffer:
                            grbl_buffer.pop(0)
                            
                        if "error" in response_clean:
                            self._notify("warning", f"GRBL returned error on line {self.current_idx}: {response}", self._get_progress())
                except queue.Empty:
                    pass # Timeout, loop again
            else:
                # Buffer empty and no lines left to send
                if current_idx >= total_lines:
                    # Finished!
                    break
                else:
                    time.sleep(0.01) # wait for connection speed
                    
        # Streaming complete
        with self.state_lock:
            was_streaming = self.is_streaming
            self.is_streaming = False
            
        if was_streaming and self.is_connected:
            self._notify("stream_finish", "G-code streaming finished successfully.", 100)
