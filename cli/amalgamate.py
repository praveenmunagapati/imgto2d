import os
import re

directories_to_scan = [
    "../cpp/core",
    "../cpp/pfm",
    "../cpp/filters",
    "../cpp/export"
]

output_file = "main_amalgamated.cpp"

# Minimal replacements: keep Qt type names, but strip Q_OBJECT and signals/slots macros.
replacements = [
    (r'#include\s+<Q.*?>\n?', ''),
    (r'Q_OBJECT\n?', ''),
    (r'signals:', '// signals:'),
    (r'public slots:', 'public:'),
    (r'private slots:', 'private:'),
    (r'emit\s+', '// emit '),
    (r'#include\s+"ui/.*?"\n?', ''),
    (r'#pragma once\n?', ''),
]

headers = []
sources = []
standard_includes = set()

def process_file(filepath):
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    include_pattern = re.compile(r'#include\s+<([^>]+)>')
    for match in include_pattern.finditer(content):
        inc = match.group(0)
        if not inc.startswith("#include <Q"):
            standard_includes.add(inc)
    
    content = re.sub(r'#include\s+<[^>]+>\n?', '', content)
    content = re.sub(r'#include\s+".*?"\n?', '', content)

    for pattern, replacement in replacements:
        content = re.sub(pattern, replacement, content)
        
    return content

print("Scanning directories...")
for directory in directories_to_scan:
    for root, _, files in os.walk(directory):
        for file in files:
            filepath = os.path.join(root, file)
            if file.endswith('.h'):
                headers.append((filepath, process_file(filepath)))
            elif file.endswith('.cpp'):
                sources.append((filepath, process_file(filepath)))

print(f"Found {len(headers)} headers and {len(sources)} source files.")

with open(output_file, 'w', encoding='utf-8') as out:
    out.write("// ==========================================\n")
    out.write("// AMALGAMATED IMGTO2D CLI SOURCE (AUTO-GENERATED)\n")
    out.write("// ==========================================\n\n")

    out.write("#include <iostream>\n")
    out.write("#include <string>\n")
    out.write("#include <vector>\n")
    out.write("#include <map>\n")
    out.write("#include <any>\n")
    out.write("#include <cmath>\n")
    out.write("#include <thread>\n")
    out.write("#include <mutex>\n")
    out.write("#include <atomic>\n")
    out.write("#include <algorithm>\n")
    out.write("#include <fstream>\n")
    out.write("#include <sstream>\n")
    out.write("#include <opencv2/core.hpp>\n")
    out.write("#include <opencv2/imgcodecs.hpp>\n")
    out.write("#include <opencv2/imgproc.hpp>\n\n")
    
    for inc in sorted(standard_includes):
        if not inc.startswith("#include <opencv") and not "Q" in inc:
            out.write(inc + "\n")

    out.write('''
// =========================================
// QT STUBS (Mimicking Qt API)
// =========================================
#define Q_ENUM(x)
#define Q_FLAG(x)
#define Q_DECLARE_METATYPE(x)
#define Q_PROPERTY(x)

namespace Qt {
    enum PenCapStyle { RoundCap };
    enum PenJoinStyle { RoundJoin };
}
class QString : public std::string {
public:
    QString() {}
    QString(const char* s) : std::string(s) {}
    QString(const std::string& s) : std::string(s) {}
    template<typename T> QString arg(T val, int=0, char='f', int=0) const { return *this; }
    QString arg(const QString& s) const { return *this; }
    QString& replace(const QString& a, const QString& b) { return *this; }
    bool isEmpty() const { return empty(); }
    int toInt() const { return 0; }
    double toDouble() const { return 0.0; }
};
class QStringList : public std::vector<QString> {
public:
    QString join(const QString& sep) const { return QString(); }
    void append(const QString& s) { push_back(s); }
};
template<typename T> class QVector : public std::vector<T> {
public:
    void append(const T& val) { this->push_back(val); }
    bool isEmpty() const { return this->empty(); }
};
template<typename K, typename V> class QMap : public std::map<K, V> {};
class QVariant {
public:
    QVariant() {}
    template<typename T> QVariant(const T&) {}
    bool isNull() const { return true; }
    double toDouble() const { return 0.0; }
    int toInt() const { return 0; }
    bool toBool() const { return false; }
    QString toString() const { return QString(); }
};
class QObject {
public:
    QObject(QObject* = nullptr) {}
    virtual ~QObject() {}
    void blockSignals(bool) {}
};
struct QColor { 
    QColor() {} 
    QColor(int,int,int) {} 
    QString name() const { return "#000000"; }
};
struct QPen { 
    QPen(QColor) {} 
    void setWidthF(double) {} 
    void setCapStyle(Qt::PenCapStyle) {} 
    void setJoinStyle(Qt::PenJoinStyle) {}
};
struct QPainterPath {
    void moveTo(double, double) {}
    void lineTo(double, double) {}
};
struct QFile {
    QFile(const QString&) {}
    bool open(int) { return true; }
    void close() {}
};
struct QIODevice { enum { WriteOnly = 1, Text = 2 }; };
struct QTextStream {
    QTextStream(QFile*) {}
    QTextStream(QString*) {}
    template<typename T> QTextStream& operator<<(const T&) { return *this; }
};
typedef std::atomic<int> QAtomicInt;
struct QMutex { void lock(){} void unlock(){} };
struct QMutexLocker { QMutexLocker(QMutex*){} };
struct QPdfWriter {
    QPdfWriter(const QString&) {}
    void setPageSize(int) {}
    void setPageMargins(int) {}
};
struct QPageSize {
    enum { Millimeter = 0 };
    QPageSize(int, int) {}
};
struct QSizeF { QSizeF(double,double) {} };
struct QMarginsF { QMarginsF(double,double,double,double) {} };
struct QPainter {
    enum { Antialiasing = 1 };
    QPainter(void*) {}
    bool isActive() const { return true; }
    void setRenderHint(int) {}
};
struct qWarning {
    template<typename T> qWarning& operator<<(const T&) { return *this; }
};
struct QJsonObject {};
struct QJsonArray {};
struct QJsonDocument {};

// Iterator stubs
template<typename Map> struct QMapIterator {
    typename Map::const_iterator it;
    typename Map::const_iterator end;
    QMapIterator(const Map& m) : it(m.begin()), end(m.end()) {}
    bool hasNext() const { return it != end; }
    void next() { ++it; }
    typename Map::key_type key() const { return it->first; }
    typename Map::mapped_type value() const { return it->second; }
};
''')

    out.write("\n// --- HEADERS ---\n\n")
    for filepath, content in headers:
        out.write(f"// Original File: {filepath}\n")
        out.write(content + "\n\n")

    out.write("\n// --- SOURCES ---\n\n")
    for filepath, content in sources:
        out.write(f"// Original File: {filepath}\n")
        out.write(content + "\n\n")

    out.write("""
// ===========================================================================
// Main Entry Point
// ===========================================================================
int main(int argc, char* argv[]) {
    std::cout << "Amalgamated CLI Ready." << std::endl;
    return 0;
}
""")

print(f"Amalgamation complete! Wrote to {output_file}")
