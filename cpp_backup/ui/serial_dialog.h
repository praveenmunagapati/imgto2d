#pragma once
/**
 * serial_dialog.h — UI for hardware streaming to plotters.
 */

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>
#include "core/serial_manager.h"

class SerialDialog : public QDialog {
    Q_OBJECT
public:
    explicit SerialDialog(const QString& gcodeData, QWidget* parent = nullptr);

private slots:
    void onRefreshPorts();
    void onConnectToggle();
    void onStreamToggle();
    void onPauseToggle();
    void onStop();
    void onStatusMessage(QString type, QString msg, int progress);

private:
    SerialManager m_mgr;
    QString m_gcodeData;

    QComboBox* m_portCombo;
    QComboBox* m_baudCombo;
    QPushButton* m_connectBtn;
    QPushButton* m_streamBtn;
    QPushButton* m_pauseBtn;
    QPushButton* m_stopBtn;
    QLabel* m_statusLbl;
    QProgressBar* m_progress;
    QTextEdit* m_console;
};
