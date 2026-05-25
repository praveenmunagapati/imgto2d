#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include "ui/main_window.h"

#ifdef _DEBUG
  #undef _DEBUG
  #pragma push_macro("slots")
  #undef slots
  #include <Python.h>
  #pragma pop_macro("slots")
  #define _DEBUG
#else
  #pragma push_macro("slots")
  #undef slots
  #include <Python.h>
  #pragma pop_macro("slots")
#endif

void customLogHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    static QFile logFile("app_crash.log");
    if (!logFile.isOpen()) {
        logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    QTextStream ts(&logFile);
    ts << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz") << " [" << type << "] " << msg << "\n";
    ts.flush();
}

void initEmbeddedPython() {
    QString pythonHome;
    QString venvSitePackages;
    
    // Candidate Python locations (virtualenv or embeddable zip folder)
    QStringList candidates = {
        QDir::current().absoluteFilePath("python/runtime/python"),
        QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../../../python/runtime/python"),
        QDir::current().absoluteFilePath("runtime/python"),
        QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../../../runtime/python"),
        QDir::current().absoluteFilePath("python/.venv"),
        QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../../../python/.venv"),
        QDir::current().absoluteFilePath(".venv"),
        QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../../../.venv")
    };

    for (const QString& candidate : candidates) {
        if (!QFileInfo::exists(candidate)) continue;
        
        QString cfg = QDir(candidate).filePath("pyvenv.cfg");
        if (QFileInfo::exists(cfg)) {
            // It's a virtual environment
            QFile file(cfg);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                while (!in.atEnd()) {
                    QString line = in.readLine().trimmed();
                    if (line.startsWith("home") && line.contains("=")) {
                        pythonHome = line.section('=', 1).trimmed();
                        break;
                    }
                }
            }
            venvSitePackages = QDir(candidate).absoluteFilePath("Lib/site-packages");
            break;
        } else {
            // Check if it looks like an embeddable package folder (contains python3*.dll, python3*.zip, or python3*._pth)
            QDir dir(candidate);
            QStringList filters = {"python3*.dll", "python3*.zip", "python3*._pth"};
            if (!dir.entryList(filters, QDir::Files).isEmpty()) {
                pythonHome = candidate;
                venvSitePackages = candidate;
                break;
            }
        }
    }

    // Set Python home if discovered
    if (!pythonHome.isEmpty()) {
        QString cleanHome = QDir(pythonHome).canonicalPath();
        if (cleanHome.isEmpty()) {
            cleanHome = QDir::cleanPath(pythonHome);
        }
        pythonHome = QDir::toNativeSeparators(cleanHome);
        qDebug() << "Setting Python Home:" << pythonHome;
        static std::wstring homeWStr = pythonHome.toStdWString();
        Py_SetPythonHome(homeWStr.c_str());
    } else {
        qDebug() << "Could not find virtual environment or embeddable Python. Using system default Python environment.";
    }

    Py_Initialize();

    // Add candidate site-packages or the folder itself to sys.path
    if (!venvSitePackages.isEmpty()) {
        QString cleanSite = QDir(venvSitePackages).canonicalPath();
        if (cleanSite.isEmpty()) {
            cleanSite = QDir::cleanPath(venvSitePackages);
        }
        venvSitePackages = QDir::toNativeSeparators(cleanSite);
        qDebug() << "Appending Python site-packages/paths to sys.path from:" << venvSitePackages;
        
        QString extraLib = QDir(venvSitePackages).filePath("Lib/site-packages");
        QString extraSite = QDir(venvSitePackages).filePath("site-packages");
        
        QString script = QString(
            "import sys\n"
            "import os\n"
            "paths = [r'%1', r'%2', r'%3']\n"
            "for p in paths:\n"
            "    if os.path.exists(p) and p not in sys.path:\n"
            "        sys.path.append(p)\n"
        ).arg(venvSitePackages).arg(extraLib).arg(extraSite);
        
        PyRun_SimpleString(script.toUtf8().constData());
    }
}

int main(int argc, char* argv[]) {
    qInstallMessageHandler(customLogHandler);
    QApplication app(argc, argv);
    app.setApplicationName("imgto2d");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("imgto2d");
    app.setStyle(QStyleFactory::create("Fusion"));

    initEmbeddedPython();

    qDebug() << "Application started";

    MainWindow win;
    win.show();

    int ret = app.exec();
    
    // Acquire GIL before calling Py_Finalize (precaution for multiple threads)
    PyGILState_STATE gstate = PyGILState_Ensure();
    Py_Finalize();
    
    return ret;
}
