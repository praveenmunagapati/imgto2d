#pragma once
/**
 * serial_manager.h — Serial Port manager for GRBL streaming
 */

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QString>
#include <QTimer>
#include <QList>

class SerialManager : public QObject {
    Q_OBJECT
public:
    explicit SerialManager(QObject* parent = nullptr);
    ~SerialManager();

    static QStringList getAvailablePorts();

    bool connectToPort(const QString& portName, int baudRate = 115200);
    void disconnectPort();
    bool isConnected() const;

    bool startStream(const QString& gcodeText);
    void pauseStream();
    void resumeStream();
    void stopStream();
    
    bool writeCommand(const QString& cmd); // Manual command

    int getProgressPct() const;

signals:
    void statusMessage(QString type, QString msg, int progress);

private slots:
    void onReadyRead();
    void sendNextLine();

private:
    QSerialPort m_serial;
    
    bool m_isStreaming = false;
    bool m_isPaused = false;
    
    QStringList m_gcodeLines;
    int m_totalLines = 0;
    int m_currentIdx = 0;
    
    QList<int> m_grblBuffer; // Tracks length of lines sent (max 127 bytes in GRBL)
    
    QString m_readBuffer;
};
