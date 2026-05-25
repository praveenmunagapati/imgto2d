#include "ui/serial_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

SerialDialog::SerialDialog(const QString& gcodeData, QWidget* parent) 
    : QDialog(parent), m_gcodeData(gcodeData) {
    setWindowTitle("Hardware Serial Streamer");
    setMinimumSize(500, 400);

    auto* mainVBox = new QVBoxLayout(this);

    // Connection Group
    auto* connGroup = new QGroupBox("Connection");
    auto* connHBox = new QHBoxLayout(connGroup);
    m_portCombo = new QComboBox;
    m_baudCombo = new QComboBox;
    m_baudCombo->addItems({"9600", "57600", "115200"});
    m_baudCombo->setCurrentText("115200");
    auto* refreshBtn = new QPushButton("Refresh");
    m_connectBtn = new QPushButton("Connect");
    
    connHBox->addWidget(new QLabel("Port:"));
    connHBox->addWidget(m_portCombo);
    connHBox->addWidget(refreshBtn);
    connHBox->addWidget(new QLabel("Baud:"));
    connHBox->addWidget(m_baudCombo);
    connHBox->addWidget(m_connectBtn);
    mainVBox->addWidget(connGroup);

    // Stream Controls
    auto* ctrlGroup = new QGroupBox("Streaming");
    auto* ctrlHBox = new QHBoxLayout(ctrlGroup);
    m_streamBtn = new QPushButton("Stream G-Code");
    m_streamBtn->setEnabled(false);
    m_pauseBtn = new QPushButton("Pause");
    m_pauseBtn->setEnabled(false);
    m_stopBtn = new QPushButton("Stop / Reset");
    m_stopBtn->setEnabled(false);
    
    ctrlHBox->addWidget(m_streamBtn);
    ctrlHBox->addWidget(m_pauseBtn);
    ctrlHBox->addWidget(m_stopBtn);
    mainVBox->addWidget(ctrlGroup);

    // Status and Progress
    m_statusLbl = new QLabel("Disconnected");
    m_progress = new QProgressBar;
    m_progress->setRange(0, 100);
    m_progress->setValue(0);
    mainVBox->addWidget(m_statusLbl);
    mainVBox->addWidget(m_progress);

    // Console
    m_console = new QTextEdit;
    m_console->setReadOnly(true);
    m_console->setStyleSheet("background-color: #1e1e1e; color: #00ff00; font-family: monospace;");
    mainVBox->addWidget(m_console);

    // Signals
    connect(refreshBtn, &QPushButton::clicked, this, &SerialDialog::onRefreshPorts);
    connect(m_connectBtn, &QPushButton::clicked, this, &SerialDialog::onConnectToggle);
    connect(m_streamBtn, &QPushButton::clicked, this, &SerialDialog::onStreamToggle);
    connect(m_pauseBtn, &QPushButton::clicked, this, &SerialDialog::onPauseToggle);
    connect(m_stopBtn, &QPushButton::clicked, this, &SerialDialog::onStop);
    connect(&m_mgr, &SerialManager::statusMessage, this, &SerialDialog::onStatusMessage);

    onRefreshPorts();
}

void SerialDialog::onRefreshPorts() {
    m_portCombo->clear();
    m_portCombo->addItems(SerialManager::getAvailablePorts());
}

void SerialDialog::onConnectToggle() {
    if (m_mgr.isConnected()) {
        m_mgr.disconnectPort();
        m_connectBtn->setText("Connect");
        m_streamBtn->setEnabled(false);
        m_pauseBtn->setEnabled(false);
        m_stopBtn->setEnabled(false);
    } else {
        if (m_portCombo->currentText().isEmpty()) return;
        if (m_mgr.connectToPort(m_portCombo->currentText(), m_baudCombo->currentText().toInt())) {
            m_connectBtn->setText("Disconnect");
            m_streamBtn->setEnabled(true);
            m_stopBtn->setEnabled(true);
        }
    }
}

void SerialDialog::onStreamToggle() {
    if (!m_mgr.isConnected()) return;
    m_mgr.startStream(m_gcodeData);
    m_streamBtn->setEnabled(false);
    m_pauseBtn->setEnabled(true);
}

void SerialDialog::onPauseToggle() {
    if (m_pauseBtn->text() == "Pause") {
        m_mgr.pauseStream();
        m_pauseBtn->setText("Resume");
    } else {
        m_mgr.resumeStream();
        m_pauseBtn->setText("Pause");
    }
}

void SerialDialog::onStop() {
    m_mgr.stopStream();
    m_streamBtn->setEnabled(true);
    m_pauseBtn->setEnabled(false);
    m_pauseBtn->setText("Pause");
    m_progress->setValue(0);
}

void SerialDialog::onStatusMessage(QString type, QString msg, int progress) {
    if (type == "error" || type == "warning") {
        m_console->append("<font color='red'>" + msg + "</font>");
    } else if (type == "console_rx") {
        m_console->append("<font color='white'>" + msg + "</font>");
    } else {
        m_console->append("<font color='#00ff00'>" + msg + "</font>");
    }
    m_statusLbl->setText(msg);
    m_progress->setValue(progress);
}
