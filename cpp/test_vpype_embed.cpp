#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>
#include <string>

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

static bool runVpypeEmbedded(const QString& tempPath, const QString& pipeline, const QString& filepath, QString& outErrorMsg) {
    PyGILState_STATE gstate = PyGILState_Ensure();

    bool success = false;
    PyObject* pModule = PyImport_ImportModule("vpype_cli");
    if (!pModule) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        if (pvalue) {
            PyObject* pstr = PyObject_Str(pvalue);
            if (pstr) {
                outErrorMsg = QString("Could not import vpype_cli: %1").arg(PyUnicode_AsUTF8(pstr));
                Py_DECREF(pstr);
            } else {
                outErrorMsg = "Could not import vpype_cli module (unknown error).";
            }
            Py_DECREF(pvalue);
        } else {
            outErrorMsg = "Could not import vpype_cli module.";
        }
        Py_XDECREF(ptype);
        Py_XDECREF(ptraceback);
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* pFunc = PyObject_GetAttrString(pModule, "execute");
    if (!pFunc || !PyCallable_Check(pFunc)) {
        outErrorMsg = "vpype_cli.execute is not callable.";
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
        PyGILState_Release(gstate);
        return false;
    }

    QString fullPipeline = QString("read \"%1\" %2 write \"%3\"")
                               .arg(tempPath)
                               .arg(pipeline)
                               .arg(filepath);

    PyObject* pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(fullPipeline.toUtf8().constData()));

    PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
    Py_DECREF(pArgs);

    if (pResult) {
        success = true;
        Py_DECREF(pResult);
    } else {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        if (pvalue) {
            PyObject* pstr = PyObject_Str(pvalue);
            if (pstr) {
                outErrorMsg = QString("vpype error: %1").arg(PyUnicode_AsUTF8(pstr));
                Py_DECREF(pstr);
            } else {
                outErrorMsg = "vpype execution failed (unknown error).";
            }
            Py_DECREF(pvalue);
        } else {
            outErrorMsg = "vpype execution failed.";
        }
        Py_XDECREF(ptype);
        Py_XDECREF(ptraceback);
    }

    Py_DECREF(pFunc);
    Py_DECREF(pModule);

    PyGILState_Release(gstate);
    return success;
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    
    std::cout << "Initializing embedded Python..." << std::endl;
    initEmbeddedPython();
    std::cout << "Embedded Python initialized." << std::endl;

    QString inputSvg = "test_shape.svg";
    QStringList svgCandidates = {
        "../test_shape.svg",
        "../../test_shape.svg",
        "../../../test_shape.svg",
        "test_shape.svg"
    };
    for (const QString& candidate : svgCandidates) {
        if (QFile::exists(candidate)) {
            inputSvg = candidate;
            break;
        }
    }

    std::cout << "Input file: " << QFileInfo(inputSvg).absoluteFilePath().toStdString() << std::endl;
    if (!QFile::exists(inputSvg)) {
        std::cerr << "Error: Input SVG not found!" << std::endl;
        Py_Finalize();
        return 1;
    }

    QString outputSvg = "test_output_vpype.svg";
    QString pipeline = "linemerge linesimplify";

    std::cout << "Executing vpype pipeline..." << std::endl;
    QString errorMsg;
    bool ok = runVpypeEmbedded(inputSvg, pipeline, outputSvg, errorMsg);

    if (ok) {
        std::cout << "Success! Saved output to: " << QFileInfo(outputSvg).absoluteFilePath().toStdString() << std::endl;
    } else {
        std::cerr << "Fail! Error: " << errorMsg.toStdString() << std::endl;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();
    Py_Finalize();
    
    return ok ? 0 : 2;
}
