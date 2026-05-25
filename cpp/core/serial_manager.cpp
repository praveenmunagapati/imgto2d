#include "core/serial_manager.h"
#include <QThread>

SerialManager::SerialManager(QObject* parent) : QObject(parent) {
    connect(&m_serial, &QSerialPort::readyRead, this, &SerialManager::onReadyRead);
}

SerialManager::~SerialManager() {
    disconnectPort();
}

QStringList SerialManager::getAvailablePorts() {
    QStringList ports;
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
        ports << info.portName();
    }
    return ports;
}

bool SerialManager::connectToPort(const QString& portName, int baudRate) {
    disconnectPort();
    m_serial.setPortName(portName);
    m_serial.setBaudRate(baudRate);
    
    if (m_serial.open(QIODevice::ReadWrite)) {
        // Wait for GRBL init
        QThread::msleep(1500);
        m_serial.clear();
        
        m_isStreaming = false;
        m_isPaused = false;
        
        // Send wake up
        m_serial.write("\r\n\r\n");
        m_serial.flush();
        QThread::msleep(200);
        m_serial.clear();
        
        emit statusMessage("connect", "Connected to " + portName, 0);
        return true;
    }
    emit statusMessage("error", "Connection failed to " + portName, 0);
    return false;
}

void SerialManager::disconnectPort() {
    if (m_serial.isOpen()) {
        m_serial.write("M5\r\n"); // Laser off
        m_serial.flush();
        QThread::msleep(100);
        m_serial.close();
    }
    m_isStreaming = false;
    m_isPaused = false;
    emit statusMessage("disconnect", "Disconnected", 0);
}

bool SerialManager::isConnected() const {
    return m_serial.isOpen();
}

bool SerialManager::writeCommand(const QString& cmd) {
    if (!isConnected()) return false;
    
    if (m_isStreaming) {
        if (cmd == "!" || cmd == "~" || cmd == "?" || cmd == "\x18") {
            m_serial.write(cmd.toUtf8());
            return true;
        }
        return false; // No normal commands during stream
    }
    
    QString formatted = cmd.trimmed() + "\n";
    m_serial.write(formatted.toUtf8());
    emit statusMessage("console_tx", cmd.trimmed(), 0);
    return true;
}

bool SerialManager::startStream(const QString& gcodeText) {
    if (!isConnected() || m_isStreaming) return false;
    
    m_gcodeLines.clear();
    QStringList lines = gcodeText.split('\n');
    for (const QString& line : lines) {
        QString clean = line.trimmed();
        int idx = clean.indexOf(';');
        if (idx >= 0) clean = clean.left(idx).trimmed();
        idx = clean.indexOf('(');
        if (idx >= 0) clean = clean.left(idx).trimmed();
        
        if (!clean.isEmpty()) {
            m_gcodeLines << clean;
        }
    }
    
    m_totalLines = m_gcodeLines.size();
    m_currentIdx = 0;
    m_grblBuffer.clear();
    
    m_isStreaming = true;
    m_isPaused = false;
    
    emit statusMessage("stream_start", "G-code streaming started.", 0);
    sendNextLine();
    return true;
}

void SerialManager::pauseStream() {
    if (!isConnected() || !m_isStreaming) return;
    m_isPaused = true;
    m_serial.write("!"); // Feed Hold
    emit statusMessage("stream_paused", "Stream paused.", getProgressPct());
}

void SerialManager::resumeStream() {
    if (!isConnected() || !m_isStreaming) return;
    m_isPaused = false;
    m_serial.write("~"); // Cycle Start
    emit statusMessage("stream_resumed", "Stream resumed.", getProgressPct());
    sendNextLine();
}

void SerialManager::stopStream() {
    if (!isConnected()) return;
    
    // Soft reset
    m_serial.write("\x18");
    m_serial.flush();
    QThread::msleep(100);
    m_serial.write("$X\r\nM5\r\n");
    
    m_isStreaming = false;
    m_isPaused = false;
    m_gcodeLines.clear();
    emit statusMessage("stream_stopped", "Stream aborted and reset.", 0);
}

int SerialManager::getProgressPct() const {
    if (m_totalLines == 0) return 0;
    return (m_currentIdx * 100) / m_totalLines;
}

void SerialManager::onReadyRead() {
    QByteArray data = m_serial.readAll();
    m_readBuffer += QString::fromUtf8(data);
    
    while (m_readBuffer.contains('\n')) {
        int idx = m_readBuffer.indexOf('\n');
        QString line = m_readBuffer.left(idx).trimmed();
        m_readBuffer.remove(0, idx + 1);
        
        if (!line.isEmpty()) {
            emit statusMessage("console_rx", line, getProgressPct());
            
            if (m_isStreaming) {
                QString lower = line.toLower();
                if (lower.contains("ok") || lower.contains("error")) {
                    if (!m_grblBuffer.isEmpty()) {
                        m_grblBuffer.removeFirst();
                    }
                    if (lower.contains("error")) {
                        emit statusMessage("warning", "GRBL error: " + line, getProgressPct());
                    }
                    sendNextLine(); // Response received, buffer space freed, send next
                }
            }
        }
    }
}

void SerialManager::sendNextLine() {
    if (!m_isStreaming || m_isPaused || !isConnected()) return;
    
    // Max GRBL buffer is 127 chars
    while (m_currentIdx < m_totalLines) {
        QString nextLine = m_gcodeLines[m_currentIdx];
        int lineLen = nextLine.length() + 1; // +1 for \n
        
        int currentBufferLen = 0;
        for (int l : m_grblBuffer) currentBufferLen += l;
        
        if (currentBufferLen + lineLen < 127) {
            m_serial.write((nextLine + "\n").toUtf8());
            m_grblBuffer.append(lineLen);
            m_currentIdx++;
            
            emit statusMessage("stream_line", QString("[%1/%2] %3")
                               .arg(m_currentIdx).arg(m_totalLines).arg(nextLine), getProgressPct());
        } else {
            // Wait for 'ok' response to free buffer
            break;
        }
    }
    
    if (m_currentIdx >= m_totalLines && m_grblBuffer.isEmpty()) {
        m_isStreaming = false;
        emit statusMessage("stream_finish", "G-code streaming finished successfully.", 100);
    }
}
