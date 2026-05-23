import os
import time
import threading
import subprocess

# Ensure we can import project modules
import sys
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
## Provide a minimal fake `serial` module when pyserial isn't installed so tests run in CI
if 'serial' not in sys.modules:
    import types
    fake_serial = types.ModuleType('serial')

    class FakeSerialPort:
        def __init__(self, port, baudrate, timeout=1.0):
            self.port = port
            self.baudrate = baudrate
            self.timeout = timeout
            self._read_buf = bytearray()
            self._write_buf = bytearray()
            self.is_open = True
            self.in_waiting = 0
            self._lock = threading.Lock()

        def reset_input_buffer(self):
            with self._lock:
                self._read_buf.clear()
                self.in_waiting = 0

        def reset_output_buffer(self):
            with self._lock:
                self._write_buf.clear()

        def write(self, data: bytes):
            with self._lock:
                self._write_buf += data
            return len(data)

        def read(self, n):
            with self._lock:
                if not self._read_buf:
                    return b''
                nread = min(n, len(self._read_buf))
                chunk = self._read_buf[:nread]
                self._read_buf = self._read_buf[nread:]
                self.in_waiting = len(self._read_buf)
                return bytes(chunk)

        def close(self):
            self.is_open = False

    fake_serial.Serial = FakeSerialPort
    # minimal tools.list_ports module
    tools_mod = types.ModuleType('serial.tools')
    list_ports_mod = types.ModuleType('serial.tools.list_ports')
    list_ports_mod.comports = lambda: []
    # register modules so `import serial.tools.list_ports` works
    sys.modules['serial'] = fake_serial
    sys.modules['serial.tools'] = tools_mod
    sys.modules['serial.tools.list_ports'] = list_ports_mod

from serial_manager import SerialManager
from app.export import vpype_exporter

# Test 1: SerialManager streaming with FakeSerial
class FakeSerial:
    def __init__(self, port, baudrate, timeout=1.0):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self._read_buf = bytearray()
        self._write_buf = bytearray()
        self.is_open = True
        self.in_waiting = 0
        self._lock = threading.Lock()

    def reset_input_buffer(self):
        with self._lock:
            self._read_buf.clear()
            self.in_waiting = 0

    def reset_output_buffer(self):
        with self._lock:
            self._write_buf.clear()

    def write(self, data: bytes):
        # Emulate GRBL: for every command written, enqueue an 'ok\n' response
        with self._lock:
            self._write_buf += data
            # respond after a short delay on a background thread
            def respond():
                time.sleep(0.01)
                resp = b"ok\n"
                with self._lock:
                    self._read_buf += resp
                    self.in_waiting = len(self._read_buf)
            threading.Thread(target=respond, daemon=True).start()
        return len(data)

    def read(self, n):
        # return up to n bytes
        start = time.time()
        while True:
            with self._lock:
                if self._read_buf:
                    nread = min(n, len(self._read_buf))
                    chunk = self._read_buf[:nread]
                    self._read_buf = self._read_buf[nread:]
                    self.in_waiting = len(self._read_buf)
                    return bytes(chunk)
            if time.time() - start > self.timeout:
                return b''
            time.sleep(0.001)

    def close(self):
        self.is_open = False

# Monkeypatch serial.Serial to FakeSerial inside serial_manager module
import importlib
import serial_manager as sm
sm.serial.Serial = FakeSerial

# Collect events
events = []

def status_cb(tp, msg, pct):
    events.append((tp, msg, pct))
    # print for debug
    print('EVENT:', tp, msg, pct)

print('Starting SerialManager test...')
manager = SerialManager()
connected = manager.connect('COM_TEST', 115200, status_callback=status_cb)
if not connected:
    print('Failed to connect in test')
    raise SystemExit(1)

# Prepare simple G-code
gcode = '\n'.join([f'G1 X{i} Y{i} F100' for i in range(10)])
started = manager.start_stream(gcode)
if not started:
    print('Failed to start stream')
    raise SystemExit(1)

# Wait for stream_finish event or timeout
deadline = time.time() + 5.0
finished = False
while time.time() < deadline:
    if any(e[0] == 'stream_finish' for e in events):
        finished = True
        break
    time.sleep(0.02)

manager.disconnect()
print('SerialManager test finished, success=' , finished)

# Test 2: VPype exporter with mocked subprocess
print('\nStarting VPype exporter test...')
from unittest import mock

# Mock export_svg to write a simple SVG
def fake_export_svg(path, geometries, drawing_area, active_pens, path_opt):
    with open(path, 'w', encoding='utf-8') as fh:
        fh.write('<svg></svg>')

vpype_exporter.export_svg = fake_export_svg

# Mock shutil.which to return None so code uses sys.executable -m vpype
import shutil
orig_which = shutil.which
shutil.which = lambda name: None

# Mock subprocess.run to simulate vpype producing output file
def fake_run(cmd, check, capture_output, text):
    # simulate vpype writing the final file
    out_path = cmd[-1]
    with open(out_path, 'w', encoding='utf-8') as fh:
        fh.write('<svg>vpype</svg>')
    class R: pass
    return R()

import app.export.vpype_exporter as ve
with mock.patch('subprocess.run', side_effect=fake_run):
    outfile = os.path.join(os.path.dirname(__file__), 'temp_vpype_out.svg')
    if os.path.exists(outfile):
        os.remove(outfile)
    try:
        ve.export_svg_with_vpype(outfile, None, None, None, {})
        ok = os.path.exists(outfile)
    finally:
        shutil.which = orig_which

print('VPype exporter produced file:', ok)

# Summary
all_ok = finished and ok
print('\nALL TESTS PASSED:', all_ok)
if not all_ok:
    raise SystemExit(2)
