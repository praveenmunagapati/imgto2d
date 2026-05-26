// ==========================================
// AMALGAMATED IMGTO2D CLI SOURCE (AUTO-GENERATED)
// ==========================================

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <any>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <random>
#include <string>
#include <utility>
#include <vector>

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

// --- HEADERS ---

// Original File: ../cpp/core\color_separation.h
/**
 * color_separation.h — OpenCV-based color separation algorithms.
 */


// Returns the image as a single grayscale channel
std::vector<cv::Mat> splitGrayscale(const cv::Mat& bgrImage);

// Splits a BGR image into inverted Red, Green, and Blue density maps (0-255).
// 0 = no ink, 255 = max ink. Output order: R, G, B
std::vector<cv::Mat> splitRGB(const cv::Mat& bgrImage);

// Splits a BGR image into Cyan, Magenta, Yellow, and Key (Black) density maps (0-255).
// Output order: C, M, Y, K
std::vector<cv::Mat> splitCMYK(const cv::Mat& bgrImage);


// Original File: ../cpp/core\coordinates.h
/**
 * coordinates.h — Coordinate transforms and mm conversions.
 */


// Equivalent to python ScalingMode
enum class ScalingMode {
    Fit,
    Crop,
    Stretch
};

// Represents the DrawingArea setup from the UI
struct DrawingAreaConfig {
    double width_mm = 210.0;
    double height_mm = 297.0;
    double padding_left_mm = 0.0;
    double padding_top_mm = 0.0;
    double padding_right_mm = 0.0;
    double padding_bottom_mm = 0.0;
    ScalingMode scaling_mode = ScalingMode::Fit;

    double usableWidth() const {
        return std::max(0.0, width_mm - padding_left_mm - padding_right_mm);
    }
    double usableHeight() const {
        return std::max(0.0, height_mm - padding_top_mm - padding_bottom_mm);
    }
};

// Computes the sx, sy, ox, oy parameters for mapping
void compute_scaling(double srcW, double srcH, double dstW, double dstH, ScalingMode mode,
                     double& sx, double& sy, double& ox, double& oy);

// Converts a path from raw image pixel coordinates into physical mm paper coordinates
Path pixel_path_to_mm(const Path& path, double imgW, double imgH, const DrawingAreaConfig& config);


// Original File: ../cpp/core\FastNoiseLite.h
// MIT License
//
// Copyright(c) 2023 Jordan Peck (jordan.me2@gmail.com)
// Copyright(c) 2023 Contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// .'',;:cldxkO00KKXXNNWWWNNXKOkxdollcc::::::;:::ccllloooolllllllllooollc:,'...        ...........',;cldxkO000Okxdlc::;;;,,;;;::cclllllll
// ..',;:ldxO0KXXNNNNNNNNXXK0kxdolcc::::::;;;,,,,,,;;;;;;;;;;:::cclllllc:;'....       ...........',;:ldxO0KXXXK0Okxdolc::;;;;::cllodddddo
// ...',:loxO0KXNNNNNXXKK0Okxdolc::;::::::::;;;,,'''''.....''',;:clllllc:;,'............''''''''',;:loxO0KXNNNNNXK0Okxdollccccllodxxxxxxd
// ....';:ldkO0KXXXKK00Okxdolcc:;;;;;::cclllcc:;;,''..... ....',;clooddolcc:;;;;,,;;;;;::::;;;;;;:cloxk0KXNWWWWWWNXKK0Okxddoooddxxkkkkkxx
// .....';:ldxkOOOOOkxxdolcc:;;;,,,;;:cllooooolcc:;'...      ..,:codxkkkxddooollloooooooollcc:::::clodkO0KXNWWWWWWNNXK00Okxxxxxxxxkkkkxxx
// . ....';:cloddddo___________,,,,;;:clooddddoolc:,...      ..,:ldx__00OOOkkk___kkkkkkxxdollc::::cclodkO0KXXNNNNNNXXK0OOkxxxxxxxxxxxxddd
// .......',;:cccc:|           |,,,;;:cclooddddoll:;'..     ..';cox|  \KKK000|   |KK00OOkxdocc___;::clldxxkO0KKKKK00Okkxdddddddddddddddoo
// .......'',,,,,''|   ________|',,;;::cclloooooolc:;'......___:ldk|   \KK000|   |XKKK0Okxolc|   |;;::cclodxxkkkkxxdoolllcclllooodddooooo
// ''......''''....|   |  ....'',,,,;;;::cclloooollc:;,''.'|   |oxk|    \OOO0|   |KKK00Oxdoll|___|;;;;;::ccllllllcc::;;,,;;;:cclloooooooo
// ;;,''.......... |   |_____',,;;;____:___cllo________.___|   |___|     \xkk|   |KK_______ool___:::;________;;;_______...'',;;:ccclllloo
// c:;,''......... |         |:::/     '   |lo/        |           |      \dx|   |0/       \d|   |cc/        |'/       \......',,;;:ccllo
// ol:;,'..........|    _____|ll/    __    |o/   ______|____    ___|   |   \o|   |/   ___   \|   |o/   ______|/   ___   \ .......'',;:clo
// dlc;,...........|   |::clooo|    /  |   |x\___   \KXKKK0|   |dol|   |\   \|   |   |   |   |   |d\___   \..|   |  /   /       ....',:cl
// xoc;'...  .....'|   |llodddd|    \__|   |_____\   \KKK0O|   |lc:|   |'\       |   |___|   |   |_____\   \.|   |_/___/...      ...',;:c
// dlc;'... ....',;|   |oddddddo\          |          |Okkx|   |::;|   |..\      |\         /|   |          | \         |...    ....',;:c
// ol:,'.......',:c|___|xxxddollc\_____,___|_________/ddoll|___|,,,|___|...\_____|:\ ______/l|___|_________/...\________|'........',;::cc
// c:;'.......';:codxxkkkkxxolc::;::clodxkOO0OOkkxdollc::;;,,''''',,,,''''''''''',,'''''',;:loxkkOOkxol:;,'''',,;:ccllcc:;,'''''',;::ccll
// ;,'.......',:codxkOO0OOkxdlc:;,,;;:cldxxkkxxdolc:;;,,''.....'',;;:::;;,,,'''''........,;cldkO0KK0Okdoc::;;::cloodddoolc:;;;;;::ccllooo
// .........',;:lodxOO0000Okdoc:,,',,;:clloddoolc:;,''.......'',;:clooollc:;;,,''.......',:ldkOKXNNXX0Oxdolllloddxxxxxxdolccccccllooodddd
// .    .....';:cldxkO0000Okxol:;,''',,;::cccc:;,,'.......'',;:cldxxkkxxdolc:;;,'.......';coxOKXNWWWNXKOkxddddxxkkkkkkxdoollllooddxxxxkkk
//       ....',;:codxkO000OOxdoc:;,''',,,;;;;,''.......',,;:clodkO00000Okxolc::;,,''..',;:ldxOKXNWWWNNK0OkkkkkkkkkkkxxddooooodxxkOOOOO000
//       ....',;;clodxkkOOOkkdolc:;,,,,,,,,'..........,;:clodxkO0KKXKK0Okxdolcc::;;,,,;;:codkO0XXNNNNXKK0OOOOOkkkkxxdoollloodxkO0KKKXXXXX
//
// VERSION: 1.1.1
// https://github.com/Auburn/FastNoiseLite

#ifndef FASTNOISELITE_H
#define FASTNOISELITE_H


class FastNoiseLite
{
public:
    enum NoiseType
    {
        NoiseType_OpenSimplex2,
        NoiseType_OpenSimplex2S,
        NoiseType_Cellular,
        NoiseType_Perlin,
        NoiseType_ValueCubic,
        NoiseType_Value
    };

    enum RotationType3D
    {
        RotationType3D_None,
        RotationType3D_ImproveXYPlanes,
        RotationType3D_ImproveXZPlanes
    };

    enum FractalType
    {
        FractalType_None,
        FractalType_FBm,
        FractalType_Ridged,
        FractalType_PingPong,
        FractalType_DomainWarpProgressive,
        FractalType_DomainWarpIndependent
    };

    enum CellularDistanceFunction
    {
        CellularDistanceFunction_Euclidean,
        CellularDistanceFunction_EuclideanSq,
        CellularDistanceFunction_Manhattan,
        CellularDistanceFunction_Hybrid
    };

    enum CellularReturnType
    {
        CellularReturnType_CellValue,
        CellularReturnType_Distance,
        CellularReturnType_Distance2,
        CellularReturnType_Distance2Add,
        CellularReturnType_Distance2Sub,
        CellularReturnType_Distance2Mul,
        CellularReturnType_Distance2Div
    };

    enum DomainWarpType
    {
        DomainWarpType_OpenSimplex2,
        DomainWarpType_OpenSimplex2Reduced,
        DomainWarpType_BasicGrid
    };

    /// <summary>
    /// Create new FastNoise object with optional seed
    /// </summary>
    FastNoiseLite(int seed = 1337)
    {
        mSeed = seed;
        mFrequency = 0.01f;
        mNoiseType = NoiseType_OpenSimplex2;
        mRotationType3D = RotationType3D_None;
        mTransformType3D = TransformType3D_DefaultOpenSimplex2;

        mFractalType = FractalType_None;
        mOctaves = 3;
        mLacunarity = 2.0f;
        mGain = 0.5f;
        mWeightedStrength = 0.0f;
        mPingPongStrength = 2.0f;

        mFractalBounding = 1 / 1.75f;

        mCellularDistanceFunction = CellularDistanceFunction_EuclideanSq;
        mCellularReturnType = CellularReturnType_Distance;
        mCellularJitterModifier = 1.0f;

        mDomainWarpType = DomainWarpType_OpenSimplex2;
        mWarpTransformType3D = TransformType3D_DefaultOpenSimplex2;
        mDomainWarpAmp = 1.0f;
    }

    /// <summary>
    /// Sets seed used for all noise types
    /// </summary>
    /// <remarks>
    /// Default: 1337
    /// </remarks>
    void SetSeed(int seed) { mSeed = seed; }

    /// <summary>
    /// Sets frequency for all noise types
    /// </summary>
    /// <remarks>
    /// Default: 0.01
    /// </remarks>
    void SetFrequency(float frequency) { mFrequency = frequency; }

    /// <summary>
    /// Sets noise algorithm used for GetNoise(...)
    /// </summary>
    /// <remarks>
    /// Default: OpenSimplex2
    /// </remarks>
    void SetNoiseType(NoiseType noiseType)
    {
        mNoiseType = noiseType;
        UpdateTransformType3D();
    }

    /// <summary>
    /// Sets domain rotation type for 3D Noise and 3D DomainWarp.
    /// Can aid in reducing directional artifacts when sampling a 2D plane in 3D
    /// </summary>
    /// <remarks>
    /// Default: None
    /// </remarks>
    void SetRotationType3D(RotationType3D rotationType3D)
    {
        mRotationType3D = rotationType3D;
        UpdateTransformType3D();
        UpdateWarpTransformType3D();
    }

    /// <summary>
    /// Sets method for combining octaves in all fractal noise types
    /// </summary>
    /// <remarks>
    /// Default: None
    /// Note: FractalType_DomainWarp... only affects DomainWarp(...)
    /// </remarks>
    void SetFractalType(FractalType fractalType) { mFractalType = fractalType; }

    /// <summary>
    /// Sets octave count for all fractal noise types 
    /// </summary>
    /// <remarks>
    /// Default: 3
    /// </remarks>
    void SetFractalOctaves(int octaves)
    {
        mOctaves = octaves;
        CalculateFractalBounding();
    }

    /// <summary>
    /// Sets octave lacunarity for all fractal noise types
    /// </summary>
    /// <remarks>
    /// Default: 2.0
    /// </remarks>
    void SetFractalLacunarity(float lacunarity) { mLacunarity = lacunarity; }

    /// <summary>
    /// Sets octave gain for all fractal noise types
    /// </summary>
    /// <remarks>
    /// Default: 0.5
    /// </remarks>
    void SetFractalGain(float gain)
    {
        mGain = gain;
        CalculateFractalBounding();
    }

    /// <summary>
    /// Sets octave weighting for all none DomainWarp fratal types
    /// </summary>
    /// <remarks>
    /// Default: 0.0
    /// Note: Keep between 0...1 to maintain -1...1 output bounding
    /// </remarks>
    void SetFractalWeightedStrength(float weightedStrength) { mWeightedStrength = weightedStrength; }

    /// <summary>
    /// Sets strength of the fractal ping pong effect
    /// </summary>
    /// <remarks>
    /// Default: 2.0
    /// </remarks>
    void SetFractalPingPongStrength(float pingPongStrength) { mPingPongStrength = pingPongStrength; }


    /// <summary>
    /// Sets distance function used in cellular noise calculations
    /// </summary>
    /// <remarks>
    /// Default: Distance
    /// </remarks>
    void SetCellularDistanceFunction(CellularDistanceFunction cellularDistanceFunction) { mCellularDistanceFunction = cellularDistanceFunction; }

    /// <summary>
    /// Sets return type from cellular noise calculations
    /// </summary>
    /// <remarks>
    /// Default: EuclideanSq
    /// </remarks>
    void SetCellularReturnType(CellularReturnType cellularReturnType) { mCellularReturnType = cellularReturnType; }

    /// <summary>
    /// Sets the maximum distance a cellular point can move from it's grid position
    /// </summary>
    /// <remarks>
    /// Default: 1.0
    /// Note: Setting this higher than 1 will cause artifacts
    /// </remarks> 
    void SetCellularJitter(float cellularJitter) { mCellularJitterModifier = cellularJitter; }


    /// <summary>
    /// Sets the warp algorithm when using DomainWarp(...)
    /// </summary>
    /// <remarks>
    /// Default: OpenSimplex2
    /// </remarks>
    void SetDomainWarpType(DomainWarpType domainWarpType)
    {
        mDomainWarpType = domainWarpType;
        UpdateWarpTransformType3D();
    }


    /// <summary>
    /// Sets the maximum warp distance from original position when using DomainWarp(...)
    /// </summary>
    /// <remarks>
    /// Default: 1.0
    /// </remarks>
    void SetDomainWarpAmp(float domainWarpAmp) { mDomainWarpAmp = domainWarpAmp; }


    /// <summary>
    /// 2D noise at given position using current settings
    /// </summary>
    /// <returns>
    /// Noise output bounded between -1...1
    /// </returns>
    template <typename FNfloat>
    float GetNoise(FNfloat x, FNfloat y) const
    {
        Arguments_must_be_floating_point_values<FNfloat>();

        TransformNoiseCoordinate(x, y);

        switch (mFractalType)
        {
        default:
            return GenNoiseSingle(mSeed, x, y);
        case FractalType_FBm:
            return GenFractalFBm(x, y);
        case FractalType_Ridged:
            return GenFractalRidged(x, y);
        case FractalType_PingPong:
            return GenFractalPingPong(x, y);
        }
    }

    /// <summary>
    /// 3D noise at given position using current settings
    /// </summary>
    /// <returns>
    /// Noise output bounded between -1...1
    /// </returns>
    template <typename FNfloat>
    float GetNoise(FNfloat x, FNfloat y, FNfloat z) const
    {
        Arguments_must_be_floating_point_values<FNfloat>();

        TransformNoiseCoordinate(x, y, z);

        switch (mFractalType)
        {
        default:
            return GenNoiseSingle(mSeed, x, y, z);
        case FractalType_FBm:
            return GenFractalFBm(x, y, z);
        case FractalType_Ridged:
            return GenFractalRidged(x, y, z);
        case FractalType_PingPong:
            return GenFractalPingPong(x, y, z);
        }
    }


    /// <summary>
    /// 2D warps the input position using current domain warp settings
    /// </summary>
    /// <example>
    /// Example usage with GetNoise
    /// <code>DomainWarp(x, y)
    /// noise = GetNoise(x, y)</code>
    /// </example>
    template <typename FNfloat>
    void DomainWarp(FNfloat& x, FNfloat& y) const
    {
        Arguments_must_be_floating_point_values<FNfloat>();

        switch (mFractalType)
        {
        default:
            DomainWarpSingle(x, y);
            break;
        case FractalType_DomainWarpProgressive:
            DomainWarpFractalProgressive(x, y);
            break;
        case FractalType_DomainWarpIndependent:
            DomainWarpFractalIndependent(x, y);
            break;
        }
    }

    /// <summary>
    /// 3D warps the input position using current domain warp settings
    /// </summary>
    /// <example>
    /// Example usage with GetNoise
    /// <code>DomainWarp(x, y, z)
    /// noise = GetNoise(x, y, z)</code>
    /// </example>
    template <typename FNfloat>
    void DomainWarp(FNfloat& x, FNfloat& y, FNfloat& z) const
    {
        Arguments_must_be_floating_point_values<FNfloat>();

        switch (mFractalType)
        {
        default:
            DomainWarpSingle(x, y, z);
            break;
        case FractalType_DomainWarpProgressive:
            DomainWarpFractalProgressive(x, y, z);
            break;
        case FractalType_DomainWarpIndependent:
            DomainWarpFractalIndependent(x, y, z);
            break;
        }
    }

private:
    template <typename T>
    struct Arguments_must_be_floating_point_values;

    enum TransformType3D
    {
        TransformType3D_None,
        TransformType3D_ImproveXYPlanes,
        TransformType3D_ImproveXZPlanes,
        TransformType3D_DefaultOpenSimplex2
    };

    int mSeed;
    float mFrequency;
    NoiseType mNoiseType;
    RotationType3D mRotationType3D;
    TransformType3D mTransformType3D;

    FractalType mFractalType;
    int mOctaves;
    float mLacunarity;
    float mGain;
    float mWeightedStrength;
    float mPingPongStrength;

    float mFractalBounding;

    CellularDistanceFunction mCellularDistanceFunction;
    CellularReturnType mCellularReturnType;
    float mCellularJitterModifier;

    DomainWarpType mDomainWarpType;
    TransformType3D mWarpTransformType3D;
    float mDomainWarpAmp;


    template <typename T>
    struct Lookup
    {
        static const T Gradients2D[];
        static const T Gradients3D[];
        static const T RandVecs2D[];
        static const T RandVecs3D[];
    };

    static float FastMin(float a, float b) { return a < b ? a : b; }

    static float FastMax(float a, float b) { return a > b ? a : b; }

    static float FastAbs(float f) { return f < 0 ? -f : f; }

    static float FastSqrt(float f) { return sqrtf(f); }

    template <typename FNfloat>
    static int FastFloor(FNfloat f) { return f >= 0 ? (int)f : (int)f - 1; }

    template <typename FNfloat>
    static int FastRound(FNfloat f) { return f >= 0 ? (int)(f + 0.5f) : (int)(f - 0.5f); }

    static float Lerp(float a, float b, float t) { return a + t * (b - a); }

    static float InterpHermite(float t) { return t * t * (3 - 2 * t); }

    static float InterpQuintic(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

    static float CubicLerp(float a, float b, float c, float d, float t)
    {
        float p = (d - c) - (a - b);
        return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b;
    }

    static float PingPong(float t)
    {
        t -= (int)(t * 0.5f) * 2;
        return t < 1 ? t : 2 - t;
    }

    void CalculateFractalBounding()
    {
        float gain = FastAbs(mGain);
        float amp = gain;
        float ampFractal = 1.0f;
        for (int i = 1; i < mOctaves; i++)
        {
            ampFractal += amp;
            amp *= gain;
        }
        mFractalBounding = 1 / ampFractal;
    }

    // Hashing
    static const int PrimeX = 501125321;
    static const int PrimeY = 1136930381;
    static const int PrimeZ = 1720413743;

    static int Hash(int seed, int xPrimed, int yPrimed)
    {
        int hash = seed ^ xPrimed ^ yPrimed;

        hash *= 0x27d4eb2d;
        return hash;
    }


    static int Hash(int seed, int xPrimed, int yPrimed, int zPrimed)
    {
        int hash = seed ^ xPrimed ^ yPrimed ^ zPrimed;

        hash *= 0x27d4eb2d;
        return hash;
    }


    static float ValCoord(int seed, int xPrimed, int yPrimed)
    {
        int hash = Hash(seed, xPrimed, yPrimed);

        hash *= hash;
        hash ^= hash << 19;
        return hash * (1 / 2147483648.0f);
    }


    static float ValCoord(int seed, int xPrimed, int yPrimed, int zPrimed)
    {
        int hash = Hash(seed, xPrimed, yPrimed, zPrimed);

        hash *= hash;
        hash ^= hash << 19;
        return hash * (1 / 2147483648.0f);
    }


    float GradCoord(int seed, int xPrimed, int yPrimed, float xd, float yd) const
    {
        int hash = Hash(seed, xPrimed, yPrimed);
        hash ^= hash >> 15;
        hash &= 127 << 1;

        float xg = Lookup<float>::Gradients2D[hash];
        float yg = Lookup<float>::Gradients2D[hash | 1];

        return xd * xg + yd * yg;
    }


    float GradCoord(int seed, int xPrimed, int yPrimed, int zPrimed, float xd, float yd, float zd) const
    {
        int hash = Hash(seed, xPrimed, yPrimed, zPrimed);
        hash ^= hash >> 15;
        hash &= 63 << 2;

        float xg = Lookup<float>::Gradients3D[hash];
        float yg = Lookup<float>::Gradients3D[hash | 1];
        float zg = Lookup<float>::Gradients3D[hash | 2];

        return xd * xg + yd * yg + zd * zg;
    }


    void GradCoordOut(int seed, int xPrimed, int yPrimed, float& xo, float& yo) const
    {
        int hash = Hash(seed, xPrimed, yPrimed) & (255 << 1);

        xo = Lookup<float>::RandVecs2D[hash];
        yo = Lookup<float>::RandVecs2D[hash | 1];
    }


    void GradCoordOut(int seed, int xPrimed, int yPrimed, int zPrimed, float& xo, float& yo, float& zo) const
    {
        int hash = Hash(seed, xPrimed, yPrimed, zPrimed) & (255 << 2);

        xo = Lookup<float>::RandVecs3D[hash];
        yo = Lookup<float>::RandVecs3D[hash | 1];
        zo = Lookup<float>::RandVecs3D[hash | 2];
    }


    void GradCoordDual(int seed, int xPrimed, int yPrimed, float xd, float yd, float& xo, float& yo) const
    {
        int hash = Hash(seed, xPrimed, yPrimed);
        int index1 = hash & (127 << 1);
        int index2 = (hash >> 7) & (255 << 1);

        float xg = Lookup<float>::Gradients2D[index1];
        float yg = Lookup<float>::Gradients2D[index1 | 1];
        float value = xd * xg + yd * yg;

        float xgo = Lookup<float>::RandVecs2D[index2];
        float ygo = Lookup<float>::RandVecs2D[index2 | 1];

        xo = value * xgo;
        yo = value * ygo;
    }


    void GradCoordDual(int seed, int xPrimed, int yPrimed, int zPrimed, float xd, float yd, float zd, float& xo, float& yo, float& zo) const
    {
        int hash = Hash(seed, xPrimed, yPrimed, zPrimed);
        int index1 = hash & (63 << 2);
        int index2 = (hash >> 6) & (255 << 2);

        float xg = Lookup<float>::Gradients3D[index1];
        float yg = Lookup<float>::Gradients3D[index1 | 1];
        float zg = Lookup<float>::Gradients3D[index1 | 2];
        float value = xd * xg + yd * yg + zd * zg;

        float xgo = Lookup<float>::RandVecs3D[index2];
        float ygo = Lookup<float>::RandVecs3D[index2 | 1];
        float zgo = Lookup<float>::RandVecs3D[index2 | 2];

        xo = value * xgo;
        yo = value * ygo;
        zo = value * zgo;
    }


    // Generic noise gen

    template <typename FNfloat>
    float GenNoiseSingle(int seed, FNfloat x, FNfloat y) const
    {
        switch (mNoiseType)
        {
        case NoiseType_OpenSimplex2:
            return SingleSimplex(seed, x, y);
        case NoiseType_OpenSimplex2S:
            return SingleOpenSimplex2S(seed, x, y);
        case NoiseType_Cellular:
            return SingleCellular(seed, x, y);
        case NoiseType_Perlin:
            return SinglePerlin(seed, x, y);
        case NoiseType_ValueCubic:
            return SingleValueCubic(seed, x, y);
        case NoiseType_Value:
            return SingleValue(seed, x, y);
        default:
            return 0;
        }
    }

    template <typename FNfloat>
    float GenNoiseSingle(int seed, FNfloat x, FNfloat y, FNfloat z) const
    {
        switch (mNoiseType)
        {
        case NoiseType_OpenSimplex2:
            return SingleOpenSimplex2(seed, x, y, z);
        case NoiseType_OpenSimplex2S:
            return SingleOpenSimplex2S(seed, x, y, z);
        case NoiseType_Cellular:
            return SingleCellular(seed, x, y, z);
        case NoiseType_Perlin:
            return SinglePerlin(seed, x, y, z);
        case NoiseType_ValueCubic:
            return SingleValueCubic(seed, x, y, z);
        case NoiseType_Value:
            return SingleValue(seed, x, y, z);
        default:
            return 0;
        }
    }


    // Noise Coordinate Transforms (frequency, and possible skew or rotation)

    template <typename FNfloat>
    void TransformNoiseCoordinate(FNfloat& x, FNfloat& y) const
    {
        x *= mFrequency;
        y *= mFrequency;

        switch (mNoiseType)
        {
        case NoiseType_OpenSimplex2:
        case NoiseType_OpenSimplex2S:
            {
                const FNfloat SQRT3 = (FNfloat)1.7320508075688772935274463415059;
                const FNfloat F2 = 0.5f * (SQRT3 - 1);
                FNfloat t = (x + y) * F2;
                x += t;
                y += t;
            }
            break;
        default:
            break;
        }
    }

    template <typename FNfloat>
    void TransformNoiseCoordinate(FNfloat& x, FNfloat& y, FNfloat& z) const
    {
        x *= mFrequency;
        y *= mFrequency;
        z *= mFrequency;

        switch (mTransformType3D)
        {
        case TransformType3D_ImproveXYPlanes:
            {
                FNfloat xy = x + y;
                FNfloat s2 = xy * -(FNfloat)0.211324865405187;
                z *= (FNfloat)0.577350269189626;
                x += s2 - z;
                y = y + s2 - z;
                z += xy * (FNfloat)0.577350269189626;
            }
            break;
        case TransformType3D_ImproveXZPlanes:
            {
                FNfloat xz = x + z;
                FNfloat s2 = xz * -(FNfloat)0.211324865405187;
                y *= (FNfloat)0.577350269189626;
                x += s2 - y;
                z += s2 - y;
                y += xz * (FNfloat)0.577350269189626;
            }
            break;
        case TransformType3D_DefaultOpenSimplex2:
            {
                const FNfloat R3 = (FNfloat)(2.0 / 3.0);
                FNfloat r = (x + y + z) * R3; // Rotation, not skew
                x = r - x;
                y = r - y;
                z = r - z;
            }
            break;
        default:
            break;
        }
    }

    void UpdateTransformType3D()
    {
        switch (mRotationType3D)
        {
        case RotationType3D_ImproveXYPlanes:
            mTransformType3D = TransformType3D_ImproveXYPlanes;
            break;
        case RotationType3D_ImproveXZPlanes:
            mTransformType3D = TransformType3D_ImproveXZPlanes;
            break;
        default:
            switch (mNoiseType)
            {
            case NoiseType_OpenSimplex2:
            case NoiseType_OpenSimplex2S:
                mTransformType3D = TransformType3D_DefaultOpenSimplex2;
                break;
            default:
                mTransformType3D = TransformType3D_None;
                break;
            }
            break;
        }
    }


    // Domain Warp Coordinate Transforms

    template <typename FNfloat>
    void TransformDomainWarpCoordinate(FNfloat& x, FNfloat& y) const
    {
        switch (mDomainWarpType)
        {
        case DomainWarpType_OpenSimplex2:
        case DomainWarpType_OpenSimplex2Reduced:
            {
                const FNfloat SQRT3 = (FNfloat)1.7320508075688772935274463415059;
                const FNfloat F2 = 0.5f * (SQRT3 - 1);
                FNfloat t = (x + y) * F2;
                x += t;
                y += t;
            }
            break;
        default:
            break;
        }
    }

    template <typename FNfloat>
    void TransformDomainWarpCoordinate(FNfloat& x, FNfloat& y, FNfloat& z) const
    {
        switch (mWarpTransformType3D)
        {
        case TransformType3D_ImproveXYPlanes:
            {
                FNfloat xy = x + y;
                FNfloat s2 = xy * -(FNfloat)0.211324865405187;
                z *= (FNfloat)0.577350269189626;
                x += s2 - z;
                y = y + s2 - z;
                z += xy * (FNfloat)0.577350269189626;
            }
            break;
        case TransformType3D_ImproveXZPlanes:
            {
                FNfloat xz = x + z;
                FNfloat s2 = xz * -(FNfloat)0.211324865405187;
                y *= (FNfloat)0.577350269189626;
                x += s2 - y;
                z += s2 - y;
                y += xz * (FNfloat)0.577350269189626;
            }
            break;
        case TransformType3D_DefaultOpenSimplex2:
            {
                const FNfloat R3 = (FNfloat)(2.0 / 3.0);
                FNfloat r = (x + y + z) * R3; // Rotation, not skew
                x = r - x;
                y = r - y;
                z = r - z;
            }
            break;
        default:
            break;
        }
    }

    void UpdateWarpTransformType3D()
    {
        switch (mRotationType3D)
        {
        case RotationType3D_ImproveXYPlanes:
            mWarpTransformType3D = TransformType3D_ImproveXYPlanes;
            break;
        case RotationType3D_ImproveXZPlanes:
            mWarpTransformType3D = TransformType3D_ImproveXZPlanes;
            break;
        default:
            switch (mDomainWarpType)
            {
            case DomainWarpType_OpenSimplex2:
            case DomainWarpType_OpenSimplex2Reduced:
                mWarpTransformType3D = TransformType3D_DefaultOpenSimplex2;
                break;
            default:
                mWarpTransformType3D = TransformType3D_None;
                break;
            }
            break;
        }
    }


    // Fractal FBm

    template <typename FNfloat>
    float GenFractalFBm(FNfloat x, FNfloat y) const
    {
        int seed = mSeed;
        float sum = 0;
        float amp = mFractalBounding;

        for (int i = 0; i < mOctaves; i++)
        {
            float noise = GenNoiseSingle(seed++, x, y);
            sum += noise * amp;
            amp *= Lerp(1.0f, FastMin(noise + 1, 2) * 0.5f, mWeightedStrength);

            x *= mLacunarity;
            y *= mLacunarity;
            amp *= mGain;
        }

        return sum;
    }

    template <typename FNfloat>
    float GenFractalFBm(FNfloat x, FNfloat y, FNfloat z) const
    {
        int seed = mSeed;
        float sum = 0;
        float amp = mFractalBounding;

        for (int i = 0; i < mOctaves; i++)
        {
            float noise = GenNoiseSingle(seed++, x, y, z);
            sum += noise * amp;
            amp *= Lerp(1.0f, (noise + 1) * 0.5f, mWeightedStrength);

            x *= mLacunarity;
            y *= mLacunarity;
            z *= mLacunarity;
            amp *= mGain;
        }

        return sum;
    }


    // Fractal Ridged

    template <typename FNfloat>
    float GenFractalRidged(FNfloat x, FNfloat y) const
    {
        int seed = mSeed;
        float sum = 0;
        float amp = mFractalBounding;

        for (int i = 0; i < mOctaves; i++)
        {
            float noise = FastAbs(GenNoiseSingle(seed++, x, y));
            sum += (noise * -2 + 1) * amp;
            amp *= Lerp(1.0f, 1 - noise, mWeightedStrength);

            x *= mLacunarity;
            y *= mLacunarity;
            amp *= mGain;
        }

        return sum;
    }

    template <typename FNfloat>
    float GenFractalRidged(FNfloat x, FNfloat y, FNfloat z) const
    {
        int seed = mSeed;
        float sum = 0;
        float amp = mFractalBounding;

        for (int i = 0; i < mOctaves; i++)
        {
            float noise = FastAbs(GenNoiseSingle(seed++, x, y, z));
            sum += (noise * -2 + 1) * amp;
            amp *= Lerp(1.0f, 1 - noise, mWeightedStrength);

            x *= mLacunarity;
            y *= mLacunarity;
            z *= mLacunarity;
            amp *= mGain;
        }

        return sum;
    }


    // Fractal PingPong 

    template <typename FNfloat>
    float GenFractalPingPong(FNfloat x, FNfloat y) const
    {
        int seed = mSeed;
        float sum = 0;
        float amp = mFractalBounding;

        for (int i = 0; i < mOctaves; i++)
        {
            float noise = PingPong((GenNoiseSingle(seed++, x, y) + 1) * mPingPongStrength);
            sum += (noise - 0.5f) * 2 * amp;
            amp *= Lerp(1.0f, noise, mWeightedStrength);

            x *= mLacunarity;
            y *= mLacunarity;
            amp *= mGain;
        }

        return sum;
    }

    template <typename FNfloat>
    float GenFractalPingPong(FNfloat x, FNfloat y, FNfloat z) const
    {
        int seed = mSeed;
        float sum = 0;
        float amp = mFractalBounding;

        for (int i = 0; i < mOctaves; i++)
        {
            float noise = PingPong((GenNoiseSingle(seed++, x, y, z) + 1) * mPingPongStrength);
            sum += (noise - 0.5f) * 2 * amp;
            amp *= Lerp(1.0f, noise, mWeightedStrength);

            x *= mLacunarity;
            y *= mLacunarity;
            z *= mLacunarity;
            amp *= mGain;
        }

        return sum;
    }


    // Simplex/OpenSimplex2 Noise

    template <typename FNfloat>
    float SingleSimplex(int seed, FNfloat x, FNfloat y) const
    {
        // 2D OpenSimplex2 case uses the same algorithm as ordinary Simplex.

        const float SQRT3 = 1.7320508075688772935274463415059f;
        const float G2 = (3 - SQRT3) / 6;

        /*
         * --- Skew moved to TransformNoiseCoordinate method ---
         * const FNfloat F2 = 0.5f * (SQRT3 - 1);
         * FNfloat s = (x + y) * F2;
         * x += s; y += s;
        */

        int i = FastFloor(x);
        int j = FastFloor(y);
        float xi = (float)(x - i);
        float yi = (float)(y - j);

        float t = (xi + yi) * G2;
        float x0 = (float)(xi - t);
        float y0 = (float)(yi - t);

        i *= PrimeX;
        j *= PrimeY;

        float n0, n1, n2;

        float a = 0.5f - x0 * x0 - y0 * y0;
        if (a <= 0) n0 = 0;
        else
        {
            n0 = (a * a) * (a * a) * GradCoord(seed, i, j, x0, y0);
        }

        float c = (float)(2 * (1 - 2 * G2) * (1 / G2 - 2)) * t + ((float)(-2 * (1 - 2 * G2) * (1 - 2 * G2)) + a);
        if (c <= 0) n2 = 0;
        else
        {
            float x2 = x0 + (2 * (float)G2 - 1);
            float y2 = y0 + (2 * (float)G2 - 1);
            n2 = (c * c) * (c * c) * GradCoord(seed, i + PrimeX, j + PrimeY, x2, y2);
        }

        if (y0 > x0)
        {
            float x1 = x0 + (float)G2;
            float y1 = y0 + ((float)G2 - 1);
            float b = 0.5f - x1 * x1 - y1 * y1;
            if (b <= 0) n1 = 0;
            else
            {
                n1 = (b * b) * (b * b) * GradCoord(seed, i, j + PrimeY, x1, y1);
            }
        }
        else
        {
            float x1 = x0 + ((float)G2 - 1);
            float y1 = y0 + (float)G2;
            float b = 0.5f - x1 * x1 - y1 * y1;
            if (b <= 0) n1 = 0;
            else
            {
                n1 = (b * b) * (b * b) * GradCoord(seed, i + PrimeX, j, x1, y1);
            }
        }

        return (n0 + n1 + n2) * 99.83685446303647f;
    }

    template <typename FNfloat>
    float SingleOpenSimplex2(int seed, FNfloat x, FNfloat y, FNfloat z) const
    {
        // 3D OpenSimplex2 case uses two offset rotated cube grids.

        /*
         * --- Rotation moved to TransformNoiseCoordinate method ---
         * const FNfloat R3 = (FNfloat)(2.0 / 3.0);
         * FNfloat r = (x + y + z) * R3; // Rotation, not skew
         * x = r - x; y = r - y; z = r - z;
        */

        int i = FastRound(x);
        int j = FastRound(y);
        int k = FastRound(z);
        float x0 = (float)(x - i);
        float y0 = (float)(y - j);
        float z0 = (float)(z - k);

        int xNSign = (int)(-1.0f - x0) | 1;
        int yNSign = (int)(-1.0f - y0) | 1;
        int zNSign = (int)(-1.0f - z0) | 1;

        float ax0 = xNSign * -x0;
        float ay0 = yNSign * -y0;
        float az0 = zNSign * -z0;

        i *= PrimeX;
        j *= PrimeY;
        k *= PrimeZ;

        float value = 0;
        float a = (0.6f - x0 * x0) - (y0 * y0 + z0 * z0);

        for (int l = 0; ; l++)
        {
            if (a > 0)
            {
                value += (a * a) * (a * a) * GradCoord(seed, i, j, k, x0, y0, z0);
            }

            float b = a + 1;
            int i1 = i;
            int j1 = j;
            int k1 = k;
            float x1 = x0;
            float y1 = y0;
            float z1 = z0;

            if (ax0 >= ay0 && ax0 >= az0)
            {
                x1 += xNSign;
                b -= xNSign * 2 * x1;
                i1 -= xNSign * PrimeX;
            }
            else if (ay0 > ax0 && ay0 >= az0)
            {
                y1 += yNSign;
                b -= yNSign * 2 * y1;
                j1 -= yNSign * PrimeY;
            }
            else
            {
                z1 += zNSign;
                b -= zNSign * 2 * z1;
                k1 -= zNSign * PrimeZ;
            }

            if (b > 0)
            {
                value += (b * b) * (b * b) * GradCoord(seed, i1, j1, k1, x1, y1, z1);
            }

            if (l == 1) break;

            ax0 = 0.5f - ax0;
            ay0 = 0.5f - ay0;
            az0 = 0.5f - az0;

            x0 = xNSign * ax0;
            y0 = yNSign * ay0;
            z0 = zNSign * az0;

            a += (0.75f - ax0) - (ay0 + az0);

            i += (xNSign >> 1) & PrimeX;
            j += (yNSign >> 1) & PrimeY;
            k += (zNSign >> 1) & PrimeZ;

            xNSign = -xNSign;
            yNSign = -yNSign;
            zNSign = -zNSign;

            seed = ~seed;
        }

        return value * 32.69428253173828125f;
    }


    // OpenSimplex2S Noise

    template <typename FNfloat>
    float SingleOpenSimplex2S(int seed, FNfloat x, FNfloat y) const
    {
        // 2D OpenSimplex2S case is a modified 2D simplex noise.

        const FNfloat SQRT3 = (FNfloat)1.7320508075688772935274463415059;
        const FNfloat G2 = (3 - SQRT3) / 6;

        /*
         * --- Skew moved to TransformNoiseCoordinate method ---
         * const FNfloat F2 = 0.5f * (SQRT3 - 1);
         * FNfloat s = (x + y) * F2;
         * x += s; y += s;
        */

        int i = FastFloor(x);
        int j = FastFloor(y);
        float xi = (float)(x - i);
        float yi = (float)(y - j);

        i *= PrimeX;
        j *= PrimeY;
        int i1 = i + PrimeX;
        int j1 = j + PrimeY;

        float t = (xi + yi) * (float)G2;
        float x0 = xi - t;
        float y0 = yi - t;

        float a0 = (2.0f / 3.0f) - x0 * x0 - y0 * y0;
        float value = (a0 * a0) * (a0 * a0) * GradCoord(seed, i, j, x0, y0);

        float a1 = (float)(2 * (1 - 2 * G2) * (1 / G2 - 2)) * t + ((float)(-2 * (1 - 2 * G2) * (1 - 2 * G2)) + a0);
        float x1 = x0 - (float)(1 - 2 * G2);
        float y1 = y0 - (float)(1 - 2 * G2);
        value += (a1 * a1) * (a1 * a1) * GradCoord(seed, i1, j1, x1, y1);

        // Nested conditionals were faster than compact bit logic/arithmetic.
        float xmyi = xi - yi;
        if (t > G2)
        {
            if (xi + xmyi > 1)
            {
                float x2 = x0 + (float)(3 * G2 - 2);
                float y2 = y0 + (float)(3 * G2 - 1);
                float a2 = (2.0f / 3.0f) - x2 * x2 - y2 * y2;
                if (a2 > 0)
                {
                    value += (a2 * a2) * (a2 * a2) * GradCoord(seed, i + (PrimeX << 1), j + PrimeY, x2, y2);
                }
            }
            else
            {
                float x2 = x0 + (float)G2;
                float y2 = y0 + (float)(G2 - 1);
                float a2 = (2.0f / 3.0f) - x2 * x2 - y2 * y2;
                if (a2 > 0)
                {
                    value += (a2 * a2) * (a2 * a2) * GradCoord(seed, i, j + PrimeY, x2, y2);
                }
            }

            if (yi - xmyi > 1)
            {
                float x3 = x0 + (float)(3 * G2 - 1);
                float y3 = y0 + (float)(3 * G2 - 2);
                float a3 = (2.0f / 3.0f) - x3 * x3 - y3 * y3;
                if (a3 > 0)
                {
                    value += (a3 * a3) * (a3 * a3) * GradCoord(seed, i + PrimeX, j + (PrimeY << 1), x3, y3);
                }
            }
            else
            {
                float x3 = x0 + (float)(G2 - 1);
                float y3 = y0 + (float)G2;
                float a3 = (2.0f / 3.0f) - x3 * x3 - y3 * y3;
                if (a3 > 0)
                {
                    value += (a3 * a3) * (a3 * a3) * GradCoord(seed, i + PrimeX, j, x3, y3);
                }
            }
        }
        else
        {
            if (xi + xmyi < 0)
            {
                float x2 = x0 + (float)(1 - G2);
                float y2 = y0 - (float)G2;
                float a2 = (2.0f / 3.0f) - x2 * x2 - y2 * y2;
                if (a2 > 0)
                {
                    value += (a2 * a2) * (a2 * a2) * GradCoord(seed, i - PrimeX, j, x2, y2);
                }
            }
            else
            {
                float x2 = x0 + (float)(G2 - 1);
                float y2 = y0 + (float)G2;
                float a2 = (2.0f / 3.0f) - x2 * x2 - y2 * y2;
                if (a2 > 0)
                {
                    value += (a2 * a2) * (a2 * a2) * GradCoord(seed, i + PrimeX, j, x2, y2);
                }
            }

            if (yi < xmyi)
            {
                float x2 = x0 - (float)G2;
                float y2 = y0 - (float)(G2 - 1);
                float a2 = (2.0f / 3.0f) - x2 * x2 - y2 * y2;
                if (a2 > 0)
                {
                    value += (a2 * a2) * (a2 * a2) * GradCoord(seed, i, j - PrimeY, x2, y2);
                }
            }
            else
            {
                float x2 = x0 + (float)G2;
                float y2 = y0 + (float)(G2 - 1);
                float a2 = (2.0f / 3.0f) - x2 * x2 - y2 * y2;
                if (a2 > 0)
                {
                    value += (a2 * a2) * (a2 * a2) * GradCoord(seed, i, j + PrimeY, x2, y2);
                }
            }
        }

        return value * 18.24196194486065f;
    }

    template <typename FNfloat>
    float SingleOpenSimplex2S(int seed, FNfloat x, FNfloat y, FNfloat z) const
    {
        // 3D OpenSimplex2S case uses two offset rotated cube grids.

        /*
         * --- Rotation moved to TransformNoiseCoordinate method ---
         * const FNfloat R3 = (FNfloat)(2.0 / 3.0);
         * FNfloat r = (x + y + z) * R3; // Rotation, not skew
         * x = r - x; y = r - y; z = r - z;
        */

        int i = FastFloor(x);
        int j = FastFloor(y);
        int k = FastFloor(z);
        float xi = (float)(x - i);
        float yi = (float)(y - j);
        float zi = (float)(z - k);

        i *= PrimeX;
        j *= PrimeY;
        k *= PrimeZ;
        int seed2 = seed + 1293373;

        int xNMask = (int)(-0.5f - xi);
        int yNMask = (int)(-0.5f - yi);
        int zNMask = (int)(-0.5f - zi);

        float x0 = xi + xNMask;
        float y0 = yi + yNMask;
        float z0 = zi + zNMask;
        float a0 = 0.75f - x0 * x0 - y0 * y0 - z0 * z0;
        float value = (a0 * a0) * (a0 * a0) * GradCoord(seed,
                                                        i + (xNMask & PrimeX), j + (yNMask & PrimeY), k + (zNMask & PrimeZ), x0, y0, z0);

        float x1 = xi - 0.5f;
        float y1 = yi - 0.5f;
        float z1 = zi - 0.5f;
        float a1 = 0.75f - x1 * x1 - y1 * y1 - z1 * z1;
        value += (a1 * a1) * (a1 * a1) * GradCoord(seed2,
                                                   i + PrimeX, j + PrimeY, k + PrimeZ, x1, y1, z1);

        float xAFlipMask0 = ((xNMask | 1) << 1) * x1;
        float yAFlipMask0 = ((yNMask | 1) << 1) * y1;
        float zAFlipMask0 = ((zNMask | 1) << 1) * z1;
        float xAFlipMask1 = (-2 - (xNMask << 2)) * x1 - 1.0f;
        float yAFlipMask1 = (-2 - (yNMask << 2)) * y1 - 1.0f;
        float zAFlipMask1 = (-2 - (zNMask << 2)) * z1 - 1.0f;

        bool skip5 = false;
        float a2 = xAFlipMask0 + a0;
        if (a2 > 0)
        {
            float x2 = x0 - (xNMask | 1);
            float y2 = y0;
            float z2 = z0;
            value += (a2 * a2) * (a2 * a2) * GradCoord(seed,
                                                       i + (~xNMask & PrimeX), j + (yNMask & PrimeY), k + (zNMask & PrimeZ), x2, y2, z2);
        }
        else
        {
            float a3 = yAFlipMask0 + zAFlipMask0 + a0;
            if (a3 > 0)
            {
                float x3 = x0;
                float y3 = y0 - (yNMask | 1);
                float z3 = z0 - (zNMask | 1);
                value += (a3 * a3) * (a3 * a3) * GradCoord(seed,
                                                           i + (xNMask & PrimeX), j + (~yNMask & PrimeY), k + (~zNMask & PrimeZ), x3, y3, z3);
            }

            float a4 = xAFlipMask1 + a1;
            if (a4 > 0)
            {
                float x4 = (xNMask | 1) + x1;
                float y4 = y1;
                float z4 = z1;
                value += (a4 * a4) * (a4 * a4) * GradCoord(seed2,
                                                           i + (xNMask & (PrimeX * 2)), j + PrimeY, k + PrimeZ, x4, y4, z4);
                skip5 = true;
            }
        }

        bool skip9 = false;
        float a6 = yAFlipMask0 + a0;
        if (a6 > 0)
        {
            float x6 = x0;
            float y6 = y0 - (yNMask | 1);
            float z6 = z0;
            value += (a6 * a6) * (a6 * a6) * GradCoord(seed,
                                                       i + (xNMask & PrimeX), j + (~yNMask & PrimeY), k + (zNMask & PrimeZ), x6, y6, z6);
        }
        else
        {
            float a7 = xAFlipMask0 + zAFlipMask0 + a0;
            if (a7 > 0)
            {
                float x7 = x0 - (xNMask | 1);
                float y7 = y0;
                float z7 = z0 - (zNMask | 1);
                value += (a7 * a7) * (a7 * a7) * GradCoord(seed,
                                                           i + (~xNMask & PrimeX), j + (yNMask & PrimeY), k + (~zNMask & PrimeZ), x7, y7, z7);
            }

            float a8 = yAFlipMask1 + a1;
            if (a8 > 0)
            {
                float x8 = x1;
                float y8 = (yNMask | 1) + y1;
                float z8 = z1;
                value += (a8 * a8) * (a8 * a8) * GradCoord(seed2,
                                                           i + PrimeX, j + (yNMask & (PrimeY << 1)), k + PrimeZ, x8, y8, z8);
                skip9 = true;
            }
        }

        bool skipD = false;
        float aA = zAFlipMask0 + a0;
        if (aA > 0)
        {
            float xA = x0;
            float yA = y0;
            float zA = z0 - (zNMask | 1);
            value += (aA * aA) * (aA * aA) * GradCoord(seed,
                                                       i + (xNMask & PrimeX), j + (yNMask & PrimeY), k + (~zNMask & PrimeZ), xA, yA, zA);
        }
        else
        {
            float aB = xAFlipMask0 + yAFlipMask0 + a0;
            if (aB > 0)
            {
                float xB = x0 - (xNMask | 1);
                float yB = y0 - (yNMask | 1);
                float zB = z0;
                value += (aB * aB) * (aB * aB) * GradCoord(seed,
                                                           i + (~xNMask & PrimeX), j + (~yNMask & PrimeY), k + (zNMask & PrimeZ), xB, yB, zB);
            }

            float aC = zAFlipMask1 + a1;
            if (aC > 0)
            {
                float xC = x1;
                float yC = y1;
                float zC = (zNMask | 1) + z1;
                value += (aC * aC) * (aC * aC) * GradCoord(seed2,
                                                           i + PrimeX, j + PrimeY, k + (zNMask & (PrimeZ << 1)), xC, yC, zC);
                skipD = true;
            }
        }

        if (!skip5)
        {
            float a5 = yAFlipMask1 + zAFlipMask1 + a1;
            if (a5 > 0)
            {
                float x5 = x1;
                float y5 = (yNMask | 1) + y1;
                float z5 = (zNMask | 1) + z1;
                value += (a5 * a5) * (a5 * a5) * GradCoord(seed2,
                                                           i + PrimeX, j + (yNMask & (PrimeY << 1)), k + (zNMask & (PrimeZ << 1)), x5, y5, z5);
            }
        }

        if (!skip9)
        {
            float a9 = xAFlipMask1 + zAFlipMask1 + a1;
            if (a9 > 0)
            {
                float x9 = (xNMask | 1) + x1;
                float y9 = y1;
                float z9 = (zNMask | 1) + z1;
                value += (a9 * a9) * (a9 * a9) * GradCoord(seed2,
                                                           i + (xNMask & (PrimeX * 2)), j + PrimeY, k + (zNMask & (PrimeZ << 1)), x9, y9, z9);
            }
        }

        if (!skipD)
        {
            float aD = xAFlipMask1 + yAFlipMask1 + a1;
            if (aD > 0)
            {
                float xD = (xNMask | 1) + x1;
                float yD = (yNMask | 1) + y1;
                float zD = z1;
                value += (aD * aD) * (aD * aD) * GradCoord(seed2,
                                                           i + (xNMask & (PrimeX << 1)), j + (yNMask & (PrimeY << 1)), k + PrimeZ, xD, yD, zD);
            }
        }

        return value * 9.046026385208288f;
    }


    // Cellular Noise

    template <typename FNfloat>
    float SingleCellular(int seed, FNfloat x, FNfloat y) const
    {
        int xr = FastRound(x);
        int yr = FastRound(y);

        float distance0 = 1e10f;
        float distance1 = 1e10f;
        int closestHash = 0;

        float cellularJitter = 0.43701595f * mCellularJitterModifier;

        int xPrimed = (xr - 1) * PrimeX;
        int yPrimedBase = (yr - 1) * PrimeY;

        switch (mCellularDistanceFunction)
        {
        default:
        case CellularDistanceFunction_Euclidean:
        case CellularDistanceFunction_EuclideanSq:
            for (int xi = xr - 1; xi <= xr + 1; xi++)
            {
                int yPrimed = yPrimedBase;

                for (int yi = yr - 1; yi <= yr + 1; yi++)
                {
                    int hash = Hash(seed, xPrimed, yPrimed);
                    int idx = hash & (255 << 1);

                    float vecX = (float)(xi - x) + Lookup<float>::RandVecs2D[idx] * cellularJitter;
                    float vecY = (float)(yi - y) + Lookup<float>::RandVecs2D[idx | 1] * cellularJitter;

                    float newDistance = vecX * vecX + vecY * vecY;

                    distance1 = FastMax(FastMin(distance1, newDistance), distance0);
                    if (newDistance < distance0)
                    {
                        distance0 = newDistance;
                        closestHash = hash;
                    }
                    yPrimed += PrimeY;
                }
                xPrimed += PrimeX;
            }
            break;
        case CellularDistanceFunction_Manhattan:
            for (int xi = xr - 1; xi <= xr + 1; xi++)
            {
                int yPrimed = yPrimedBase;

                for (int yi = yr - 1; yi <= yr + 1; yi++)
                {
                    int hash = Hash(seed, xPrimed, yPrimed);
                    int idx = hash & (255 << 1);

                    float vecX = (float)(xi - x) + Lookup<float>::RandVecs2D[idx] * cellularJitter;
                    float vecY = (float)(yi - y) + Lookup<float>::RandVecs2D[idx | 1] * cellularJitter;

                    float newDistance = FastAbs(vecX) + FastAbs(vecY);

                    distance1 = FastMax(FastMin(distance1, newDistance), distance0);
                    if (newDistance < distance0)
                    {
                        distance0 = newDistance;
                        closestHash = hash;
                    }
                    yPrimed += PrimeY;
                }
                xPrimed += PrimeX;
            }
            break;
        case CellularDistanceFunction_Hybrid:
            for (int xi = xr - 1; xi <= xr + 1; xi++)
            {
                int yPrimed = yPrimedBase;

                for (int yi = yr - 1; yi <= yr + 1; yi++)
                {
                    int hash = Hash(seed, xPrimed, yPrimed);
                    int idx = hash & (255 << 1);

                    float vecX = (float)(xi - x) + Lookup<float>::RandVecs2D[idx] * cellularJitter;
                    float vecY = (float)(yi - y) + Lookup<float>::RandVecs2D[idx | 1] * cellularJitter;

                    float newDistance = (FastAbs(vecX) + FastAbs(vecY)) + (vecX * vecX + vecY * vecY);

                    distance1 = FastMax(FastMin(distance1, newDistance), distance0);
                    if (newDistance < distance0)
                    {
                        distance0 = newDistance;
                        closestHash = hash;
                    }
                    yPrimed += PrimeY;
                }
                xPrimed += PrimeX;
            }
            break;
        }

        if (mCellularDistanceFunction == CellularDistanceFunction_Euclidean && mCellularReturnType >= CellularReturnType_Distance)
        {
            distance0 = FastSqrt(distance0);

            if (mCellularReturnType >= CellularReturnType_Distance2)
            {
                distance1 = FastSqrt(distance1);
            }
        }

        switch (mCellularReturnType)
        {
        case CellularReturnType_CellValue:
            return closestHash * (1 / 2147483648.0f);
        case CellularReturnType_Distance:
            return distance0 - 1;
        case CellularReturnType_Distance2:
            return distance1 - 1;
        case CellularReturnType_Distance2Add:
            return (distance1 + distance0) * 0.5f - 1;
        case CellularReturnType_Distance2Sub:
            return distance1 - distance0 - 1;
        case CellularReturnType_Distance2Mul:
            return distance1 * distance0 * 0.5f - 1;
        case CellularReturnType_Distance2Div:
            return distance0 / distance1 - 1;
        default:
            return 0;
        }
    }

    template <typename FNfloat>
    float SingleCellular(int seed, FNfloat x, FNfloat y, FNfloat z) const
    {
        int xr = FastRound(x);
        int yr = FastRound(y);
        int zr = FastRound(z);

        float distance0 = 1e10f;
        float distance1 = 1e10f;
        int closestHash = 0;

        float cellularJitter = 0.39614353f * mCellularJitterModifier;

        int xPrimed = (xr - 1) * PrimeX;
        int yPrimedBase = (yr - 1) * PrimeY;
        int zPrimedBase = (zr - 1) * PrimeZ;

        switch (mCellularDistanceFunction)
        {
        case CellularDistanceFunction_Euclidean:
        case CellularDistanceFunction_EuclideanSq:
            for (int xi = xr - 1; xi <= xr + 1; xi++)
            {
                int yPrimed = yPrimedBase;

                for (int yi = yr - 1; yi <= yr + 1; yi++)
                {
                    int zPrimed = zPrimedBase;

                    for (int zi = zr - 1; zi <= zr + 1; zi++)
                    {
                        int hash = Hash(seed, xPrimed, yPrimed, zPrimed);
                        int idx = hash & (255 << 2);

                        float vecX = (float)(xi - x) + Lookup<float>::RandVecs3D[idx] * cellularJitter;
                        float vecY = (float)(yi - y) + Lookup<float>::RandVecs3D[idx | 1] * cellularJitter;
                        float vecZ = (float)(zi - z) + Lookup<float>::RandVecs3D[idx | 2] * cellularJitter;

                        float newDistance = vecX * vecX + vecY * vecY + vecZ * vecZ;

                        distance1 = FastMax(FastMin(distance1, newDistance), distance0);
                        if (newDistance < distance0)
                        {
                            distance0 = newDistance;
                            closestHash = hash;
                        }
                        zPrimed += PrimeZ;
                    }
                    yPrimed += PrimeY;
                }
                xPrimed += PrimeX;
            }
            break;
        case CellularDistanceFunction_Manhattan:
            for (int xi = xr - 1; xi <= xr + 1; xi++)
            {
                int yPrimed = yPrimedBase;

                for (int yi = yr - 1; yi <= yr + 1; yi++)
                {
                    int zPrimed = zPrimedBase;

                    for (int zi = zr - 1; zi <= zr + 1; zi++)
                    {
                        int hash = Hash(seed, xPrimed, yPrimed, zPrimed);
                        int idx = hash & (255 << 2);

                        float vecX = (float)(xi - x) + Lookup<float>::RandVecs3D[idx] * cellularJitter;
                        float vecY = (float)(yi - y) + Lookup<float>::RandVecs3D[idx | 1] * cellularJitter;
                        float vecZ = (float)(zi - z) + Lookup<float>::RandVecs3D[idx | 2] * cellularJitter;

                        float newDistance = FastAbs(vecX) + FastAbs(vecY) + FastAbs(vecZ);

                        distance1 = FastMax(FastMin(distance1, newDistance), distance0);
                        if (newDistance < distance0)
                        {
                            distance0 = newDistance;
                            closestHash = hash;
                        }
                        zPrimed += PrimeZ;
                    }
                    yPrimed += PrimeY;
                }
                xPrimed += PrimeX;
            }
            break;
        case CellularDistanceFunction_Hybrid:
            for (int xi = xr - 1; xi <= xr + 1; xi++)
            {
                int yPrimed = yPrimedBase;

                for (int yi = yr - 1; yi <= yr + 1; yi++)
                {
                    int zPrimed = zPrimedBase;

                    for (int zi = zr - 1; zi <= zr + 1; zi++)
                    {
                        int hash = Hash(seed, xPrimed, yPrimed, zPrimed);
                        int idx = hash & (255 << 2);

                        float vecX = (float)(xi - x) + Lookup<float>::RandVecs3D[idx] * cellularJitter;
                        float vecY = (float)(yi - y) + Lookup<float>::RandVecs3D[idx | 1] * cellularJitter;
                        float vecZ = (float)(zi - z) + Lookup<float>::RandVecs3D[idx | 2] * cellularJitter;

                        float newDistance = (FastAbs(vecX) + FastAbs(vecY) + FastAbs(vecZ)) + (vecX * vecX + vecY * vecY + vecZ * vecZ);

                        distance1 = FastMax(FastMin(distance1, newDistance), distance0);
                        if (newDistance < distance0)
                        {
                            distance0 = newDistance;
                            closestHash = hash;
                        }
                        zPrimed += PrimeZ;
                    }
                    yPrimed += PrimeY;
                }
                xPrimed += PrimeX;
            }
            break;
        default:
            break;
        }

        if (mCellularDistanceFunction == CellularDistanceFunction_Euclidean && mCellularReturnType >= CellularReturnType_Distance)
        {
            distance0 = FastSqrt(distance0);

            if (mCellularReturnType >= CellularReturnType_Distance2)
            {
                distance1 = FastSqrt(distance1);
            }
        }

        switch (mCellularReturnType)
        {
        case CellularReturnType_CellValue:
            return closestHash * (1 / 2147483648.0f);
        case CellularReturnType_Distance:
            return distance0 - 1;
        case CellularReturnType_Distance2:
            return distance1 - 1;
        case CellularReturnType_Distance2Add:
            return (distance1 + distance0) * 0.5f - 1;
        case CellularReturnType_Distance2Sub:
            return distance1 - distance0 - 1;
        case CellularReturnType_Distance2Mul:
            return distance1 * distance0 * 0.5f - 1;
        case CellularReturnType_Distance2Div:
            return distance0 / distance1 - 1;
        default:
            return 0;
        }
    }


    // Perlin Noise

    template <typename FNfloat>
    float SinglePerlin(int seed, FNfloat x, FNfloat y) const
    {
        int x0 = FastFloor(x);
        int y0 = FastFloor(y);

        float xd0 = (float)(x - x0);
        float yd0 = (float)(y - y0);
        float xd1 = xd0 - 1;
        float yd1 = yd0 - 1;

        float xs = InterpQuintic(xd0);
        float ys = InterpQuintic(yd0);

        x0 *= PrimeX;
        y0 *= PrimeY;
        int x1 = x0 + PrimeX;
        int y1 = y0 + PrimeY;

        float xf0 = Lerp(GradCoord(seed, x0, y0, xd0, yd0), GradCoord(seed, x1, y0, xd1, yd0), xs);
        float xf1 = Lerp(GradCoord(seed, x0, y1, xd0, yd1), GradCoord(seed, x1, y1, xd1, yd1), xs);

        return Lerp(xf0, xf1, ys) * 1.4247691104677813f;
    }

    template <typename FNfloat>
    float SinglePerlin(int seed, FNfloat x, FNfloat y, FNfloat z) const
    {
        int x0 = FastFloor(x);
        int y0 = FastFloor(y);
        int z0 = FastFloor(z);

        float xd0 = (float)(x - x0);
        float yd0 = (float)(y - y0);
        float zd0 = (float)(z - z0);
        float xd1 = xd0 - 1;
        float yd1 = yd0 - 1;
        float zd1 = zd0 - 1;

        float xs = InterpQuintic(xd0);
        float ys = InterpQuintic(yd0);
        float zs = InterpQuintic(zd0);

        x0 *= PrimeX;
        y0 *= PrimeY;
        z0 *= PrimeZ;
        int x1 = x0 + PrimeX;
        int y1 = y0 + PrimeY;
        int z1 = z0 + PrimeZ;

        float xf00 = Lerp(GradCoord(seed, x0, y0, z0, xd0, yd0, zd0), GradCoord(seed, x1, y0, z0, xd1, yd0, zd0), xs);
        float xf10 = Lerp(GradCoord(seed, x0, y1, z0, xd0, yd1, zd0), GradCoord(seed, x1, y1, z0, xd1, yd1, zd0), xs);
        float xf01 = Lerp(GradCoord(seed, x0, y0, z1, xd0, yd0, zd1), GradCoord(seed, x1, y0, z1, xd1, yd0, zd1), xs);
        float xf11 = Lerp(GradCoord(seed, x0, y1, z1, xd0, yd1, zd1), GradCoord(seed, x1, y1, z1, xd1, yd1, zd1), xs);

        float yf0 = Lerp(xf00, xf10, ys);
        float yf1 = Lerp(xf01, xf11, ys);

        return Lerp(yf0, yf1, zs) * 0.964921414852142333984375f;
    }


    // Value Cubic Noise

    template <typename FNfloat>
    float SingleValueCubic(int seed, FNfloat x, FNfloat y) const
    {
        int x1 = FastFloor(x);
        int y1 = FastFloor(y);

        float xs = (float)(x - x1);
        float ys = (float)(y - y1);

        x1 *= PrimeX;
        y1 *= PrimeY;
        int x0 = x1 - PrimeX;
        int y0 = y1 - PrimeY;
        int x2 = x1 + PrimeX;
        int y2 = y1 + PrimeY;
        int x3 = x1 + (int)((long)PrimeX << 1);
        int y3 = y1 + (int)((long)PrimeY << 1);

        return CubicLerp(
            CubicLerp(ValCoord(seed, x0, y0), ValCoord(seed, x1, y0), ValCoord(seed, x2, y0), ValCoord(seed, x3, y0),
                      xs),
            CubicLerp(ValCoord(seed, x0, y1), ValCoord(seed, x1, y1), ValCoord(seed, x2, y1), ValCoord(seed, x3, y1),
                      xs),
            CubicLerp(ValCoord(seed, x0, y2), ValCoord(seed, x1, y2), ValCoord(seed, x2, y2), ValCoord(seed, x3, y2),
                      xs),
            CubicLerp(ValCoord(seed, x0, y3), ValCoord(seed, x1, y3), ValCoord(seed, x2, y3), ValCoord(seed, x3, y3),
                      xs),
            ys) * (1 / (1.5f * 1.5f));
    }

    template <typename FNfloat>
    float SingleValueCubic(int seed, FNfloat x, FNfloat y, FNfloat z) const
    {
        int x1 = FastFloor(x);
        int y1 = FastFloor(y);
        int z1 = FastFloor(z);

        float xs = (float)(x - x1);
        float ys = (float)(y - y1);
        float zs = (float)(z - z1);

        x1 *= PrimeX;
        y1 *= PrimeY;
        z1 *= PrimeZ;

        int x0 = x1 - PrimeX;
        int y0 = y1 - PrimeY;
        int z0 = z1 - PrimeZ;
        int x2 = x1 + PrimeX;
        int y2 = y1 + PrimeY;
        int z2 = z1 + PrimeZ;
        int x3 = x1 + (int)((long)PrimeX << 1);
        int y3 = y1 + (int)((long)PrimeY << 1);
        int z3 = z1 + (int)((long)PrimeZ << 1);


        return CubicLerp(
            CubicLerp(
                CubicLerp(ValCoord(seed, x0, y0, z0), ValCoord(seed, x1, y0, z0), ValCoord(seed, x2, y0, z0), ValCoord(seed, x3, y0, z0), xs),
                CubicLerp(ValCoord(seed, x0, y1, z0), ValCoord(seed, x1, y1, z0), ValCoord(seed, x2, y1, z0), ValCoord(seed, x3, y1, z0), xs),
                CubicLerp(ValCoord(seed, x0, y2, z0), ValCoord(seed, x1, y2, z0), ValCoord(seed, x2, y2, z0), ValCoord(seed, x3, y2, z0), xs),
                CubicLerp(ValCoord(seed, x0, y3, z0), ValCoord(seed, x1, y3, z0), ValCoord(seed, x2, y3, z0), ValCoord(seed, x3, y3, z0), xs),
                ys),
            CubicLerp(
                CubicLerp(ValCoord(seed, x0, y0, z1), ValCoord(seed, x1, y0, z1), ValCoord(seed, x2, y0, z1), ValCoord(seed, x3, y0, z1), xs),
                CubicLerp(ValCoord(seed, x0, y1, z1), ValCoord(seed, x1, y1, z1), ValCoord(seed, x2, y1, z1), ValCoord(seed, x3, y1, z1), xs),
                CubicLerp(ValCoord(seed, x0, y2, z1), ValCoord(seed, x1, y2, z1), ValCoord(seed, x2, y2, z1), ValCoord(seed, x3, y2, z1), xs),
                CubicLerp(ValCoord(seed, x0, y3, z1), ValCoord(seed, x1, y3, z1), ValCoord(seed, x2, y3, z1), ValCoord(seed, x3, y3, z1), xs),
                ys),
            CubicLerp(
                CubicLerp(ValCoord(seed, x0, y0, z2), ValCoord(seed, x1, y0, z2), ValCoord(seed, x2, y0, z2), ValCoord(seed, x3, y0, z2), xs),
                CubicLerp(ValCoord(seed, x0, y1, z2), ValCoord(seed, x1, y1, z2), ValCoord(seed, x2, y1, z2), ValCoord(seed, x3, y1, z2), xs),
                CubicLerp(ValCoord(seed, x0, y2, z2), ValCoord(seed, x1, y2, z2), ValCoord(seed, x2, y2, z2), ValCoord(seed, x3, y2, z2), xs),
                CubicLerp(ValCoord(seed, x0, y3, z2), ValCoord(seed, x1, y3, z2), ValCoord(seed, x2, y3, z2), ValCoord(seed, x3, y3, z2), xs),
                ys),
            CubicLerp(
                CubicLerp(ValCoord(seed, x0, y0, z3), ValCoord(seed, x1, y0, z3), ValCoord(seed, x2, y0, z3), ValCoord(seed, x3, y0, z3), xs),
                CubicLerp(ValCoord(seed, x0, y1, z3), ValCoord(seed, x1, y1, z3), ValCoord(seed, x2, y1, z3), ValCoord(seed, x3, y1, z3), xs),
                CubicLerp(ValCoord(seed, x0, y2, z3), ValCoord(seed, x1, y2, z3), ValCoord(seed, x2, y2, z3), ValCoord(seed, x3, y2, z3), xs),
                CubicLerp(ValCoord(seed, x0, y3, z3), ValCoord(seed, x1, y3, z3), ValCoord(seed, x2, y3, z3), ValCoord(seed, x3, y3, z3), xs),
                ys),
            zs) * (1 / (1.5f * 1.5f * 1.5f));
    }


    // Value Noise

    template <typename FNfloat>
    float SingleValue(int seed, FNfloat x, FNfloat y) const
    {
        int x0 = FastFloor(x);
        int y0 = FastFloor(y);

        float xs = InterpHermite((float)(x - x0));
        float ys = InterpHermite((float)(y - y0));

        x0 *= PrimeX;
        y0 *= PrimeY;
        int x1 = x0 + PrimeX;
        int y1 = y0 + PrimeY;

        float xf0 = Lerp(ValCoord(seed, x0, y0), ValCoord(seed, x1, y0), xs);
        float xf1 = Lerp(ValCoord(seed, x0, y1), ValCoord(seed, x1, y1), xs);

        return Lerp(xf0, xf1, ys);
    }

    template <typename FNfloat>
    float SingleValue(int seed, FNfloat x, FNfloat y, FNfloat z) const
    {
        int x0 = FastFloor(x);
        int y0 = FastFloor(y);
        int z0 = FastFloor(z);

        float xs = InterpHermite((float)(x - x0));
        float ys = InterpHermite((float)(y - y0));
        float zs = InterpHermite((float)(z - z0));

        x0 *= PrimeX;
        y0 *= PrimeY;
        z0 *= PrimeZ;
        int x1 = x0 + PrimeX;
        int y1 = y0 + PrimeY;
        int z1 = z0 + PrimeZ;

        float xf00 = Lerp(ValCoord(seed, x0, y0, z0), ValCoord(seed, x1, y0, z0), xs);
        float xf10 = Lerp(ValCoord(seed, x0, y1, z0), ValCoord(seed, x1, y1, z0), xs);
        float xf01 = Lerp(ValCoord(seed, x0, y0, z1), ValCoord(seed, x1, y0, z1), xs);
        float xf11 = Lerp(ValCoord(seed, x0, y1, z1), ValCoord(seed, x1, y1, z1), xs);

        float yf0 = Lerp(xf00, xf10, ys);
        float yf1 = Lerp(xf01, xf11, ys);

        return Lerp(yf0, yf1, zs);
    }


    // Domain Warp

    template <typename FNfloat>
    void DoSingleDomainWarp(int seed, float amp, float freq, FNfloat x, FNfloat y, FNfloat& xr, FNfloat& yr) const
    {
        switch (mDomainWarpType)
        {
        case DomainWarpType_OpenSimplex2:
            SingleDomainWarpSimplexGradient(seed, amp * 38.283687591552734375f, freq, x, y, xr, yr, false);
            break;
        case DomainWarpType_OpenSimplex2Reduced:
            SingleDomainWarpSimplexGradient(seed, amp * 16.0f, freq, x, y, xr, yr, true);
            break;
        case DomainWarpType_BasicGrid:
            SingleDomainWarpBasicGrid(seed, amp, freq, x, y, xr, yr);
            break;
        }
    }

    template <typename FNfloat>
    void DoSingleDomainWarp(int seed, float amp, float freq, FNfloat x, FNfloat y, FNfloat z, FNfloat& xr, FNfloat& yr, FNfloat& zr) const
    {
        switch (mDomainWarpType)
        {
        case DomainWarpType_OpenSimplex2:
            SingleDomainWarpOpenSimplex2Gradient(seed, amp * 32.69428253173828125f, freq, x, y, z, xr, yr, zr, false);
            break;
        case DomainWarpType_OpenSimplex2Reduced:
            SingleDomainWarpOpenSimplex2Gradient(seed, amp * 7.71604938271605f, freq, x, y, z, xr, yr, zr, true);
            break;
        case DomainWarpType_BasicGrid:
            SingleDomainWarpBasicGrid(seed, amp, freq, x, y, z, xr, yr, zr);
            break;
        }
    }


    // Domain Warp Single Wrapper

    template <typename FNfloat>
    void DomainWarpSingle(FNfloat& x, FNfloat& y) const
    {
        int seed = mSeed;
        float amp = mDomainWarpAmp * mFractalBounding;
        float freq = mFrequency;

        FNfloat xs = x;
        FNfloat ys = y;
        TransformDomainWarpCoordinate(xs, ys);

        DoSingleDomainWarp(seed, amp, freq, xs, ys, x, y);
    }

    template <typename FNfloat>
    void DomainWarpSingle(FNfloat& x, FNfloat& y, FNfloat& z) const
    {
        int seed = mSeed;
        float amp = mDomainWarpAmp * mFractalBounding;
        float freq = mFrequency;

        FNfloat xs = x;
        FNfloat ys = y;
        FNfloat zs = z;
        TransformDomainWarpCoordinate(xs, ys, zs);

        DoSingleDomainWarp(seed, amp, freq, xs, ys, zs, x, y, z);
    }


    // Domain Warp Fractal Progressive

    template <typename FNfloat>
    void DomainWarpFractalProgressive(FNfloat& x, FNfloat& y) const
    {
        int seed = mSeed;
        float amp = mDomainWarpAmp * mFractalBounding;
        float freq = mFrequency;

        for (int i = 0; i < mOctaves; i++)
        {
            FNfloat xs = x;
            FNfloat ys = y;
            TransformDomainWarpCoordinate(xs, ys);

            DoSingleDomainWarp(seed, amp, freq, xs, ys, x, y);

            seed++;
            amp *= mGain;
            freq *= mLacunarity;
        }
    }

    template <typename FNfloat>
    void DomainWarpFractalProgressive(FNfloat& x, FNfloat& y, FNfloat& z) const
    {
        int seed = mSeed;
        float amp = mDomainWarpAmp * mFractalBounding;
        float freq = mFrequency;

        for (int i = 0; i < mOctaves; i++)
        {
            FNfloat xs = x;
            FNfloat ys = y;
            FNfloat zs = z;
            TransformDomainWarpCoordinate(xs, ys, zs);

            DoSingleDomainWarp(seed, amp, freq, xs, ys, zs, x, y, z);

            seed++;
            amp *= mGain;
            freq *= mLacunarity;
        }
    }


    // Domain Warp Fractal Independant

    template <typename FNfloat>
    void DomainWarpFractalIndependent(FNfloat& x, FNfloat& y) const
    {
        FNfloat xs = x;
        FNfloat ys = y;
        TransformDomainWarpCoordinate(xs, ys);

        int seed = mSeed;
        float amp = mDomainWarpAmp * mFractalBounding;
        float freq = mFrequency;

        for (int i = 0; i < mOctaves; i++)
        {
            DoSingleDomainWarp(seed, amp, freq, xs, ys, x, y);

            seed++;
            amp *= mGain;
            freq *= mLacunarity;
        }
    }

    template <typename FNfloat>
    void DomainWarpFractalIndependent(FNfloat& x, FNfloat& y, FNfloat& z) const
    {
        FNfloat xs = x;
        FNfloat ys = y;
        FNfloat zs = z;
        TransformDomainWarpCoordinate(xs, ys, zs);

        int seed = mSeed;
        float amp = mDomainWarpAmp * mFractalBounding;
        float freq = mFrequency;

        for (int i = 0; i < mOctaves; i++)
        {
            DoSingleDomainWarp(seed, amp, freq, xs, ys, zs, x, y, z);

            seed++;
            amp *= mGain;
            freq *= mLacunarity;
        }
    }


    // Domain Warp Basic Grid

    template <typename FNfloat>
    void SingleDomainWarpBasicGrid(int seed, float warpAmp, float frequency, FNfloat x, FNfloat y, FNfloat& xr, FNfloat& yr) const
    {
        FNfloat xf = x * frequency;
        FNfloat yf = y * frequency;

        int x0 = FastFloor(xf);
        int y0 = FastFloor(yf);

        float xs = InterpHermite((float)(xf - x0));
        float ys = InterpHermite((float)(yf - y0));

        x0 *= PrimeX;
        y0 *= PrimeY;
        int x1 = x0 + PrimeX;
        int y1 = y0 + PrimeY;

        int hash0 = Hash(seed, x0, y0) & (255 << 1);
        int hash1 = Hash(seed, x1, y0) & (255 << 1);

        float lx0x = Lerp(Lookup<float>::RandVecs2D[hash0], Lookup<float>::RandVecs2D[hash1], xs);
        float ly0x = Lerp(Lookup<float>::RandVecs2D[hash0 | 1], Lookup<float>::RandVecs2D[hash1 | 1], xs);

        hash0 = Hash(seed, x0, y1) & (255 << 1);
        hash1 = Hash(seed, x1, y1) & (255 << 1);

        float lx1x = Lerp(Lookup<float>::RandVecs2D[hash0], Lookup<float>::RandVecs2D[hash1], xs);
        float ly1x = Lerp(Lookup<float>::RandVecs2D[hash0 | 1], Lookup<float>::RandVecs2D[hash1 | 1], xs);

        xr += Lerp(lx0x, lx1x, ys) * warpAmp;
        yr += Lerp(ly0x, ly1x, ys) * warpAmp;
    }

    template <typename FNfloat>
    void SingleDomainWarpBasicGrid(int seed, float warpAmp, float frequency, FNfloat x, FNfloat y, FNfloat z, FNfloat& xr, FNfloat& yr, FNfloat& zr) const
    {
        FNfloat xf = x * frequency;
        FNfloat yf = y * frequency;
        FNfloat zf = z * frequency;

        int x0 = FastFloor(xf);
        int y0 = FastFloor(yf);
        int z0 = FastFloor(zf);

        float xs = InterpHermite((float)(xf - x0));
        float ys = InterpHermite((float)(yf - y0));
        float zs = InterpHermite((float)(zf - z0));

        x0 *= PrimeX;
        y0 *= PrimeY;
        z0 *= PrimeZ;
        int x1 = x0 + PrimeX;
        int y1 = y0 + PrimeY;
        int z1 = z0 + PrimeZ;

        int hash0 = Hash(seed, x0, y0, z0) & (255 << 2);
        int hash1 = Hash(seed, x1, y0, z0) & (255 << 2);

        float lx0x = Lerp(Lookup<float>::RandVecs3D[hash0], Lookup<float>::RandVecs3D[hash1], xs);
        float ly0x = Lerp(Lookup<float>::RandVecs3D[hash0 | 1], Lookup<float>::RandVecs3D[hash1 | 1], xs);
        float lz0x = Lerp(Lookup<float>::RandVecs3D[hash0 | 2], Lookup<float>::RandVecs3D[hash1 | 2], xs);

        hash0 = Hash(seed, x0, y1, z0) & (255 << 2);
        hash1 = Hash(seed, x1, y1, z0) & (255 << 2);

        float lx1x = Lerp(Lookup<float>::RandVecs3D[hash0], Lookup<float>::RandVecs3D[hash1], xs);
        float ly1x = Lerp(Lookup<float>::RandVecs3D[hash0 | 1], Lookup<float>::RandVecs3D[hash1 | 1], xs);
        float lz1x = Lerp(Lookup<float>::RandVecs3D[hash0 | 2], Lookup<float>::RandVecs3D[hash1 | 2], xs);

        float lx0y = Lerp(lx0x, lx1x, ys);
        float ly0y = Lerp(ly0x, ly1x, ys);
        float lz0y = Lerp(lz0x, lz1x, ys);

        hash0 = Hash(seed, x0, y0, z1) & (255 << 2);
        hash1 = Hash(seed, x1, y0, z1) & (255 << 2);

        lx0x = Lerp(Lookup<float>::RandVecs3D[hash0], Lookup<float>::RandVecs3D[hash1], xs);
        ly0x = Lerp(Lookup<float>::RandVecs3D[hash0 | 1], Lookup<float>::RandVecs3D[hash1 | 1], xs);
        lz0x = Lerp(Lookup<float>::RandVecs3D[hash0 | 2], Lookup<float>::RandVecs3D[hash1 | 2], xs);

        hash0 = Hash(seed, x0, y1, z1) & (255 << 2);
        hash1 = Hash(seed, x1, y1, z1) & (255 << 2);

        lx1x = Lerp(Lookup<float>::RandVecs3D[hash0], Lookup<float>::RandVecs3D[hash1], xs);
        ly1x = Lerp(Lookup<float>::RandVecs3D[hash0 | 1], Lookup<float>::RandVecs3D[hash1 | 1], xs);
        lz1x = Lerp(Lookup<float>::RandVecs3D[hash0 | 2], Lookup<float>::RandVecs3D[hash1 | 2], xs);

        xr += Lerp(lx0y, Lerp(lx0x, lx1x, ys), zs) * warpAmp;
        yr += Lerp(ly0y, Lerp(ly0x, ly1x, ys), zs) * warpAmp;
        zr += Lerp(lz0y, Lerp(lz0x, lz1x, ys), zs) * warpAmp;
    }


    // Domain Warp Simplex/OpenSimplex2

    template <typename FNfloat>
    void SingleDomainWarpSimplexGradient(int seed, float warpAmp, float frequency, FNfloat x, FNfloat y, FNfloat& xr, FNfloat& yr, bool outGradOnly) const
    {
        const float SQRT3 = 1.7320508075688772935274463415059f;
        const float G2 = (3 - SQRT3) / 6;

        x *= frequency;
        y *= frequency;

        /*
         * --- Skew moved to TransformNoiseCoordinate method ---
         * const FNfloat F2 = 0.5f * (SQRT3 - 1);
         * FNfloat s = (x + y) * F2;
         * x += s; y += s;
        */

        int i = FastFloor(x);
        int j = FastFloor(y);
        float xi = (float)(x - i);
        float yi = (float)(y - j);

        float t = (xi + yi) * G2;
        float x0 = (float)(xi - t);
        float y0 = (float)(yi - t);

        i *= PrimeX;
        j *= PrimeY;

        float vx, vy;
        vx = vy = 0;

        float a = 0.5f - x0 * x0 - y0 * y0;
        if (a > 0)
        {
            float aaaa = (a * a) * (a * a);
            float xo, yo;
            if (outGradOnly)
                GradCoordOut(seed, i, j, xo, yo);
            else
                GradCoordDual(seed, i, j, x0, y0, xo, yo);
            vx += aaaa * xo;
            vy += aaaa * yo;
        }

        float c = (float)(2 * (1 - 2 * G2) * (1 / G2 - 2)) * t + ((float)(-2 * (1 - 2 * G2) * (1 - 2 * G2)) + a);
        if (c > 0)
        {
            float x2 = x0 + (2 * (float)G2 - 1);
            float y2 = y0 + (2 * (float)G2 - 1);
            float cccc = (c * c) * (c * c);
            float xo, yo;
            if (outGradOnly)
                GradCoordOut(seed, i + PrimeX, j + PrimeY, xo, yo);
            else
                GradCoordDual(seed, i + PrimeX, j + PrimeY, x2, y2, xo, yo);
            vx += cccc * xo;
            vy += cccc * yo;
        }

        if (y0 > x0)
        {
            float x1 = x0 + (float)G2;
            float y1 = y0 + ((float)G2 - 1);
            float b = 0.5f - x1 * x1 - y1 * y1;
            if (b > 0)
            {
                float bbbb = (b * b) * (b * b);
                float xo, yo;
                if (outGradOnly)
                    GradCoordOut(seed, i, j + PrimeY, xo, yo);
                else
                    GradCoordDual(seed, i, j + PrimeY, x1, y1, xo, yo);
                vx += bbbb * xo;
                vy += bbbb * yo;
            }
        }
        else
        {
            float x1 = x0 + ((float)G2 - 1);
            float y1 = y0 + (float)G2;
            float b = 0.5f - x1 * x1 - y1 * y1;
            if (b > 0)
            {
                float bbbb = (b * b) * (b * b);
                float xo, yo;
                if (outGradOnly)
                    GradCoordOut(seed, i + PrimeX, j, xo, yo);
                else
                    GradCoordDual(seed, i + PrimeX, j, x1, y1, xo, yo);
                vx += bbbb * xo;
                vy += bbbb * yo;
            }
        }

        xr += vx * warpAmp;
        yr += vy * warpAmp;
    }

    template <typename FNfloat>
    void SingleDomainWarpOpenSimplex2Gradient(int seed, float warpAmp, float frequency, FNfloat x, FNfloat y, FNfloat z, FNfloat& xr, FNfloat& yr, FNfloat& zr, bool outGradOnly) const
    {
        x *= frequency;
        y *= frequency;
        z *= frequency;

        /*
         * --- Rotation moved to TransformDomainWarpCoordinate method ---
         * const FNfloat R3 = (FNfloat)(2.0 / 3.0);
         * FNfloat r = (x + y + z) * R3; // Rotation, not skew
         * x = r - x; y = r - y; z = r - z;
        */

        int i = FastRound(x);
        int j = FastRound(y);
        int k = FastRound(z);
        float x0 = (float)x - i;
        float y0 = (float)y - j;
        float z0 = (float)z - k;

        int xNSign = (int)(-x0 - 1.0f) | 1;
        int yNSign = (int)(-y0 - 1.0f) | 1;
        int zNSign = (int)(-z0 - 1.0f) | 1;

        float ax0 = xNSign * -x0;
        float ay0 = yNSign * -y0;
        float az0 = zNSign * -z0;

        i *= PrimeX;
        j *= PrimeY;
        k *= PrimeZ;

        float vx, vy, vz;
        vx = vy = vz = 0;

        float a = (0.6f - x0 * x0) - (y0 * y0 + z0 * z0);
        for (int l = 0; l < 2; l++)
        {
            if (a > 0)
            {
                float aaaa = (a * a) * (a * a);
                float xo, yo, zo;
                if (outGradOnly)
                    GradCoordOut(seed, i, j, k, xo, yo, zo);
                else
                    GradCoordDual(seed, i, j, k, x0, y0, z0, xo, yo, zo);
                vx += aaaa * xo;
                vy += aaaa * yo;
                vz += aaaa * zo;
            }

            float b = a + 1;
            int i1 = i;
            int j1 = j;
            int k1 = k;
            float x1 = x0;
            float y1 = y0;
            float z1 = z0;

            if (ax0 >= ay0 && ax0 >= az0)
            {
                x1 += xNSign;
                b -= xNSign * 2 * x1;
                i1 -= xNSign * PrimeX;
            }
            else if (ay0 > ax0 && ay0 >= az0)
            {
                y1 += yNSign;
                b -= yNSign * 2 * y1;
                j1 -= yNSign * PrimeY;
            }
            else
            {
                z1 += zNSign;
                b -= zNSign * 2 * z1;
                k1 -= zNSign * PrimeZ;
            }

            if (b > 0)
            {
                float bbbb = (b * b) * (b * b);
                float xo, yo, zo;
                if (outGradOnly)
                    GradCoordOut(seed, i1, j1, k1, xo, yo, zo);
                else
                    GradCoordDual(seed, i1, j1, k1, x1, y1, z1, xo, yo, zo);
                vx += bbbb * xo;
                vy += bbbb * yo;
                vz += bbbb * zo;
            }

            if (l == 1) break;

            ax0 = 0.5f - ax0;
            ay0 = 0.5f - ay0;
            az0 = 0.5f - az0;

            x0 = xNSign * ax0;
            y0 = yNSign * ay0;
            z0 = zNSign * az0;

            a += (0.75f - ax0) - (ay0 + az0);

            i += (xNSign >> 1) & PrimeX;
            j += (yNSign >> 1) & PrimeY;
            k += (zNSign >> 1) & PrimeZ;

            xNSign = -xNSign;
            yNSign = -yNSign;
            zNSign = -zNSign;

            seed += 1293373;
        }

        xr += vx * warpAmp;
        yr += vy * warpAmp;
        zr += vz * warpAmp;
    }
};

template <>
struct FastNoiseLite::Arguments_must_be_floating_point_values<float> {};
template <>
struct FastNoiseLite::Arguments_must_be_floating_point_values<double> {};
template <>
struct FastNoiseLite::Arguments_must_be_floating_point_values<long double> {};

template <typename T>
const T FastNoiseLite::Lookup<T>::Gradients2D[] =
{
    0.130526192220052f, 0.99144486137381f, 0.38268343236509f, 0.923879532511287f, 0.608761429008721f, 0.793353340291235f, 0.793353340291235f, 0.608761429008721f,
    0.923879532511287f, 0.38268343236509f, 0.99144486137381f, 0.130526192220051f, 0.99144486137381f, -0.130526192220051f, 0.923879532511287f, -0.38268343236509f,
    0.793353340291235f, -0.60876142900872f, 0.608761429008721f, -0.793353340291235f, 0.38268343236509f, -0.923879532511287f, 0.130526192220052f, -0.99144486137381f,
    -0.130526192220052f, -0.99144486137381f, -0.38268343236509f, -0.923879532511287f, -0.608761429008721f, -0.793353340291235f, -0.793353340291235f, -0.608761429008721f,
    -0.923879532511287f, -0.38268343236509f, -0.99144486137381f, -0.130526192220052f, -0.99144486137381f, 0.130526192220051f, -0.923879532511287f, 0.38268343236509f,
    -0.793353340291235f, 0.608761429008721f, -0.608761429008721f, 0.793353340291235f, -0.38268343236509f, 0.923879532511287f, -0.130526192220052f, 0.99144486137381f,
    0.130526192220052f, 0.99144486137381f, 0.38268343236509f, 0.923879532511287f, 0.608761429008721f, 0.793353340291235f, 0.793353340291235f, 0.608761429008721f,
    0.923879532511287f, 0.38268343236509f, 0.99144486137381f, 0.130526192220051f, 0.99144486137381f, -0.130526192220051f, 0.923879532511287f, -0.38268343236509f,
    0.793353340291235f, -0.60876142900872f, 0.608761429008721f, -0.793353340291235f, 0.38268343236509f, -0.923879532511287f, 0.130526192220052f, -0.99144486137381f,
    -0.130526192220052f, -0.99144486137381f, -0.38268343236509f, -0.923879532511287f, -0.608761429008721f, -0.793353340291235f, -0.793353340291235f, -0.608761429008721f,
    -0.923879532511287f, -0.38268343236509f, -0.99144486137381f, -0.130526192220052f, -0.99144486137381f, 0.130526192220051f, -0.923879532511287f, 0.38268343236509f,
    -0.793353340291235f, 0.608761429008721f, -0.608761429008721f, 0.793353340291235f, -0.38268343236509f, 0.923879532511287f, -0.130526192220052f, 0.99144486137381f,
    0.130526192220052f, 0.99144486137381f, 0.38268343236509f, 0.923879532511287f, 0.608761429008721f, 0.793353340291235f, 0.793353340291235f, 0.608761429008721f,
    0.923879532511287f, 0.38268343236509f, 0.99144486137381f, 0.130526192220051f, 0.99144486137381f, -0.130526192220051f, 0.923879532511287f, -0.38268343236509f,
    0.793353340291235f, -0.60876142900872f, 0.608761429008721f, -0.793353340291235f, 0.38268343236509f, -0.923879532511287f, 0.130526192220052f, -0.99144486137381f,
    -0.130526192220052f, -0.99144486137381f, -0.38268343236509f, -0.923879532511287f, -0.608761429008721f, -0.793353340291235f, -0.793353340291235f, -0.608761429008721f,
    -0.923879532511287f, -0.38268343236509f, -0.99144486137381f, -0.130526192220052f, -0.99144486137381f, 0.130526192220051f, -0.923879532511287f, 0.38268343236509f,
    -0.793353340291235f, 0.608761429008721f, -0.608761429008721f, 0.793353340291235f, -0.38268343236509f, 0.923879532511287f, -0.130526192220052f, 0.99144486137381f,
    0.130526192220052f, 0.99144486137381f, 0.38268343236509f, 0.923879532511287f, 0.608761429008721f, 0.793353340291235f, 0.793353340291235f, 0.608761429008721f,
    0.923879532511287f, 0.38268343236509f, 0.99144486137381f, 0.130526192220051f, 0.99144486137381f, -0.130526192220051f, 0.923879532511287f, -0.38268343236509f,
    0.793353340291235f, -0.60876142900872f, 0.608761429008721f, -0.793353340291235f, 0.38268343236509f, -0.923879532511287f, 0.130526192220052f, -0.99144486137381f,
    -0.130526192220052f, -0.99144486137381f, -0.38268343236509f, -0.923879532511287f, -0.608761429008721f, -0.793353340291235f, -0.793353340291235f, -0.608761429008721f,
    -0.923879532511287f, -0.38268343236509f, -0.99144486137381f, -0.130526192220052f, -0.99144486137381f, 0.130526192220051f, -0.923879532511287f, 0.38268343236509f,
    -0.793353340291235f, 0.608761429008721f, -0.608761429008721f, 0.793353340291235f, -0.38268343236509f, 0.923879532511287f, -0.130526192220052f, 0.99144486137381f,
    0.130526192220052f, 0.99144486137381f, 0.38268343236509f, 0.923879532511287f, 0.608761429008721f, 0.793353340291235f, 0.793353340291235f, 0.608761429008721f,
    0.923879532511287f, 0.38268343236509f, 0.99144486137381f, 0.130526192220051f, 0.99144486137381f, -0.130526192220051f, 0.923879532511287f, -0.38268343236509f,
    0.793353340291235f, -0.60876142900872f, 0.608761429008721f, -0.793353340291235f, 0.38268343236509f, -0.923879532511287f, 0.130526192220052f, -0.99144486137381f,
    -0.130526192220052f, -0.99144486137381f, -0.38268343236509f, -0.923879532511287f, -0.608761429008721f, -0.793353340291235f, -0.793353340291235f, -0.608761429008721f,
    -0.923879532511287f, -0.38268343236509f, -0.99144486137381f, -0.130526192220052f, -0.99144486137381f, 0.130526192220051f, -0.923879532511287f, 0.38268343236509f,
    -0.793353340291235f, 0.608761429008721f, -0.608761429008721f, 0.793353340291235f, -0.38268343236509f, 0.923879532511287f, -0.130526192220052f, 0.99144486137381f,
    0.38268343236509f, 0.923879532511287f, 0.923879532511287f, 0.38268343236509f, 0.923879532511287f, -0.38268343236509f, 0.38268343236509f, -0.923879532511287f,
    -0.38268343236509f, -0.923879532511287f, -0.923879532511287f, -0.38268343236509f, -0.923879532511287f, 0.38268343236509f, -0.38268343236509f, 0.923879532511287f,
};

template <typename T>
const T FastNoiseLite::Lookup<T>::RandVecs2D[] =
{
    -0.2700222198f, -0.9628540911f, 0.3863092627f, -0.9223693152f, 0.04444859006f, -0.999011673f, -0.5992523158f, -0.8005602176f, -0.7819280288f, 0.6233687174f, 0.9464672271f, 0.3227999196f, -0.6514146797f, -0.7587218957f, 0.9378472289f, 0.347048376f,
    -0.8497875957f, -0.5271252623f, -0.879042592f, 0.4767432447f, -0.892300288f, -0.4514423508f, -0.379844434f, -0.9250503802f, -0.9951650832f, 0.0982163789f, 0.7724397808f, -0.6350880136f, 0.7573283322f, -0.6530343002f, -0.9928004525f, -0.119780055f,
    -0.0532665713f, 0.9985803285f, 0.9754253726f, -0.2203300762f, -0.7665018163f, 0.6422421394f, 0.991636706f, 0.1290606184f, -0.994696838f, 0.1028503788f, -0.5379205513f, -0.84299554f, 0.5022815471f, -0.8647041387f, 0.4559821461f, -0.8899889226f,
    -0.8659131224f, -0.5001944266f, 0.0879458407f, -0.9961252577f, -0.5051684983f, 0.8630207346f, 0.7753185226f, -0.6315704146f, -0.6921944612f, 0.7217110418f, -0.5191659449f, -0.8546734591f, 0.8978622882f, -0.4402764035f, -0.1706774107f, 0.9853269617f,
    -0.9353430106f, -0.3537420705f, -0.9992404798f, 0.03896746794f, -0.2882064021f, -0.9575683108f, -0.9663811329f, 0.2571137995f, -0.8759714238f, -0.4823630009f, -0.8303123018f, -0.5572983775f, 0.05110133755f, -0.9986934731f, -0.8558373281f, -0.5172450752f,
    0.09887025282f, 0.9951003332f, 0.9189016087f, 0.3944867976f, -0.2439375892f, -0.9697909324f, -0.8121409387f, -0.5834613061f, -0.9910431363f, 0.1335421355f, 0.8492423985f, -0.5280031709f, -0.9717838994f, -0.2358729591f, 0.9949457207f, 0.1004142068f,
    0.6241065508f, -0.7813392434f, 0.662910307f, 0.7486988212f, -0.7197418176f, 0.6942418282f, -0.8143370775f, -0.5803922158f, 0.104521054f, -0.9945226741f, -0.1065926113f, -0.9943027784f, 0.445799684f, -0.8951327509f, 0.105547406f, 0.9944142724f,
    -0.992790267f, 0.1198644477f, -0.8334366408f, 0.552615025f, 0.9115561563f, -0.4111755999f, 0.8285544909f, -0.5599084351f, 0.7217097654f, -0.6921957921f, 0.4940492677f, -0.8694339084f, -0.3652321272f, -0.9309164803f, -0.9696606758f, 0.2444548501f,
    0.08925509731f, -0.996008799f, 0.5354071276f, -0.8445941083f, -0.1053576186f, 0.9944343981f, -0.9890284586f, 0.1477251101f, 0.004856104961f, 0.9999882091f, 0.9885598478f, 0.1508291331f, 0.9286129562f, -0.3710498316f, -0.5832393863f, -0.8123003252f,
    0.3015207509f, 0.9534596146f, -0.9575110528f, 0.2883965738f, 0.9715802154f, -0.2367105511f, 0.229981792f, 0.9731949318f, 0.955763816f, -0.2941352207f, 0.740956116f, 0.6715534485f, -0.9971513787f, -0.07542630764f, 0.6905710663f, -0.7232645452f,
    -0.290713703f, -0.9568100872f, 0.5912777791f, -0.8064679708f, -0.9454592212f, -0.325740481f, 0.6664455681f, 0.74555369f, 0.6236134912f, 0.7817328275f, 0.9126993851f, -0.4086316587f, -0.8191762011f, 0.5735419353f, -0.8812745759f, -0.4726046147f,
    0.9953313627f, 0.09651672651f, 0.9855650846f, -0.1692969699f, -0.8495980887f, 0.5274306472f, 0.6174853946f, -0.7865823463f, 0.8508156371f, 0.52546432f, 0.9985032451f, -0.05469249926f, 0.1971371563f, -0.9803759185f, 0.6607855748f, -0.7505747292f,
    -0.03097494063f, 0.9995201614f, -0.6731660801f, 0.739491331f, -0.7195018362f, -0.6944905383f, 0.9727511689f, 0.2318515979f, 0.9997059088f, -0.0242506907f, 0.4421787429f, -0.8969269532f, 0.9981350961f, -0.061043673f, -0.9173660799f, -0.3980445648f,
    -0.8150056635f, -0.5794529907f, -0.8789331304f, 0.4769450202f, 0.0158605829f, 0.999874213f, -0.8095464474f, 0.5870558317f, -0.9165898907f, -0.3998286786f, -0.8023542565f, 0.5968480938f, -0.5176737917f, 0.8555780767f, -0.8154407307f, -0.5788405779f,
    0.4022010347f, -0.9155513791f, -0.9052556868f, -0.4248672045f, 0.7317445619f, 0.6815789728f, -0.5647632201f, -0.8252529947f, -0.8403276335f, -0.5420788397f, -0.9314281527f, 0.363925262f, 0.5238198472f, 0.8518290719f, 0.7432803869f, -0.6689800195f,
    -0.985371561f, -0.1704197369f, 0.4601468731f, 0.88784281f, 0.825855404f, 0.5638819483f, 0.6182366099f, 0.7859920446f, 0.8331502863f, -0.553046653f, 0.1500307506f, 0.9886813308f, -0.662330369f, -0.7492119075f, -0.668598664f, 0.743623444f,
    0.7025606278f, 0.7116238924f, -0.5419389763f, -0.8404178401f, -0.3388616456f, 0.9408362159f, 0.8331530315f, 0.5530425174f, -0.2989720662f, -0.9542618632f, 0.2638522993f, 0.9645630949f, 0.124108739f, -0.9922686234f, -0.7282649308f, -0.6852956957f,
    0.6962500149f, 0.7177993569f, -0.9183535368f, 0.3957610156f, -0.6326102274f, -0.7744703352f, -0.9331891859f, -0.359385508f, -0.1153779357f, -0.9933216659f, 0.9514974788f, -0.3076565421f, -0.08987977445f, -0.9959526224f, 0.6678496916f, 0.7442961705f,
    0.7952400393f, -0.6062947138f, -0.6462007402f, -0.7631674805f, -0.2733598753f, 0.9619118351f, 0.9669590226f, -0.254931851f, -0.9792894595f, 0.2024651934f, -0.5369502995f, -0.8436138784f, -0.270036471f, -0.9628500944f, -0.6400277131f, 0.7683518247f,
    -0.7854537493f, -0.6189203566f, 0.06005905383f, -0.9981948257f, -0.02455770378f, 0.9996984141f, -0.65983623f, 0.751409442f, -0.6253894466f, -0.7803127835f, -0.6210408851f, -0.7837781695f, 0.8348888491f, 0.5504185768f, -0.1592275245f, 0.9872419133f,
    0.8367622488f, 0.5475663786f, -0.8675753916f, -0.4973056806f, -0.2022662628f, -0.9793305667f, 0.9399189937f, 0.3413975472f, 0.9877404807f, -0.1561049093f, -0.9034455656f, 0.4287028224f, 0.1269804218f, -0.9919052235f, -0.3819600854f, 0.924178821f,
    0.9754625894f, 0.2201652486f, -0.3204015856f, -0.9472818081f, -0.9874760884f, 0.1577687387f, 0.02535348474f, -0.9996785487f, 0.4835130794f, -0.8753371362f, -0.2850799925f, -0.9585037287f, -0.06805516006f, -0.99768156f, -0.7885244045f, -0.6150034663f,
    0.3185392127f, -0.9479096845f, 0.8880043089f, 0.4598351306f, 0.6476921488f, -0.7619021462f, 0.9820241299f, 0.1887554194f, 0.9357275128f, -0.3527237187f, -0.8894895414f, 0.4569555293f, 0.7922791302f, 0.6101588153f, 0.7483818261f, 0.6632681526f,
    -0.7288929755f, -0.6846276581f, 0.8729032783f, -0.4878932944f, 0.8288345784f, 0.5594937369f, 0.08074567077f, 0.9967347374f, 0.9799148216f, -0.1994165048f, -0.580730673f, -0.8140957471f, -0.4700049791f, -0.8826637636f, 0.2409492979f, 0.9705377045f,
    0.9437816757f, -0.3305694308f, -0.8927998638f, -0.4504535528f, -0.8069622304f, 0.5906030467f, 0.06258973166f, 0.9980393407f, -0.9312597469f, 0.3643559849f, 0.5777449785f, 0.8162173362f, -0.3360095855f, -0.941858566f, 0.697932075f, -0.7161639607f,
    -0.002008157227f, -0.9999979837f, -0.1827294312f, -0.9831632392f, -0.6523911722f, 0.7578824173f, -0.4302626911f, -0.9027037258f, -0.9985126289f, -0.05452091251f, -0.01028102172f, -0.9999471489f, -0.4946071129f, 0.8691166802f, -0.2999350194f, 0.9539596344f,
    0.8165471961f, 0.5772786819f, 0.2697460475f, 0.962931498f, -0.7306287391f, -0.6827749597f, -0.7590952064f, -0.6509796216f, -0.907053853f, 0.4210146171f, -0.5104861064f, -0.8598860013f, 0.8613350597f, 0.5080373165f, 0.5007881595f, -0.8655698812f,
    -0.654158152f, 0.7563577938f, -0.8382755311f, -0.545246856f, 0.6940070834f, 0.7199681717f, 0.06950936031f, 0.9975812994f, 0.1702942185f, -0.9853932612f, 0.2695973274f, 0.9629731466f, 0.5519612192f, -0.8338697815f, 0.225657487f, -0.9742067022f,
    0.4215262855f, -0.9068161835f, 0.4881873305f, -0.8727388672f, -0.3683854996f, -0.9296731273f, -0.9825390578f, 0.1860564427f, 0.81256471f, 0.5828709909f, 0.3196460933f, -0.9475370046f, 0.9570913859f, 0.2897862643f, -0.6876655497f, -0.7260276109f,
    -0.9988770922f, -0.047376731f, -0.1250179027f, 0.992154486f, -0.8280133617f, 0.560708367f, 0.9324863769f, -0.3612051451f, 0.6394653183f, 0.7688199442f, -0.01623847064f, -0.9998681473f, -0.9955014666f, -0.09474613458f, -0.81453315f, 0.580117012f,
    0.4037327978f, -0.9148769469f, 0.9944263371f, 0.1054336766f, -0.1624711654f, 0.9867132919f, -0.9949487814f, -0.100383875f, -0.6995302564f, 0.7146029809f, 0.5263414922f, -0.85027327f, -0.5395221479f, 0.841971408f, 0.6579370318f, 0.7530729462f,
    0.01426758847f, -0.9998982128f, -0.6734383991f, 0.7392433447f, 0.639412098f, -0.7688642071f, 0.9211571421f, 0.3891908523f, -0.146637214f, -0.9891903394f, -0.782318098f, 0.6228791163f, -0.5039610839f, -0.8637263605f, -0.7743120191f, -0.6328039957f,
};

template <typename T>
const T FastNoiseLite::Lookup<T>::Gradients3D[] =
{
    0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
    1, 0, 1, 0, -1, 0, 1, 0,  1, 0,-1, 0, -1, 0,-1, 0,
    1, 1, 0, 0, -1, 1, 0, 0,  1,-1, 0, 0, -1,-1, 0, 0,
    0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
    1, 0, 1, 0, -1, 0, 1, 0,  1, 0,-1, 0, -1, 0,-1, 0,
    1, 1, 0, 0, -1, 1, 0, 0,  1,-1, 0, 0, -1,-1, 0, 0,
    0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
    1, 0, 1, 0, -1, 0, 1, 0,  1, 0,-1, 0, -1, 0,-1, 0,
    1, 1, 0, 0, -1, 1, 0, 0,  1,-1, 0, 0, -1,-1, 0, 0,
    0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
    1, 0, 1, 0, -1, 0, 1, 0,  1, 0,-1, 0, -1, 0,-1, 0,
    1, 1, 0, 0, -1, 1, 0, 0,  1,-1, 0, 0, -1,-1, 0, 0,
    0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
    1, 0, 1, 0, -1, 0, 1, 0,  1, 0,-1, 0, -1, 0,-1, 0,
    1, 1, 0, 0, -1, 1, 0, 0,  1,-1, 0, 0, -1,-1, 0, 0,
    1, 1, 0, 0,  0,-1, 1, 0, -1, 1, 0, 0,  0,-1,-1, 0
};

template <typename T>
const T FastNoiseLite::Lookup<T>::RandVecs3D[] =
{
    -0.7292736885f, -0.6618439697f, 0.1735581948f, 0, 0.790292081f, -0.5480887466f, -0.2739291014f, 0, 0.7217578935f, 0.6226212466f, -0.3023380997f, 0, 0.565683137f, -0.8208298145f, -0.0790000257f, 0, 0.760049034f, -0.5555979497f, -0.3370999617f, 0, 0.3713945616f, 0.5011264475f, 0.7816254623f, 0, -0.1277062463f, -0.4254438999f, -0.8959289049f, 0, -0.2881560924f, -0.5815838982f, 0.7607405838f, 0,
    0.5849561111f, -0.662820239f, -0.4674352136f, 0, 0.3307171178f, 0.0391653737f, 0.94291689f, 0, 0.8712121778f, -0.4113374369f, -0.2679381538f, 0, 0.580981015f, 0.7021915846f, 0.4115677815f, 0, 0.503756873f, 0.6330056931f, -0.5878203852f, 0, 0.4493712205f, 0.601390195f, 0.6606022552f, 0, -0.6878403724f, 0.09018890807f, -0.7202371714f, 0, -0.5958956522f, -0.6469350577f, 0.475797649f, 0,
    -0.5127052122f, 0.1946921978f, -0.8361987284f, 0, -0.9911507142f, -0.05410276466f, -0.1212153153f, 0, -0.2149721042f, 0.9720882117f, -0.09397607749f, 0, -0.7518650936f, -0.5428057603f, 0.3742469607f, 0, 0.5237068895f, 0.8516377189f, -0.02107817834f, 0, 0.6333504779f, 0.1926167129f, -0.7495104896f, 0, -0.06788241606f, 0.3998305789f, 0.9140719259f, 0, -0.5538628599f, -0.4729896695f, -0.6852128902f, 0,
    -0.7261455366f, -0.5911990757f, 0.3509933228f, 0, -0.9229274737f, -0.1782808786f, 0.3412049336f, 0, -0.6968815002f, 0.6511274338f, 0.3006480328f, 0, 0.9608044783f, -0.2098363234f, -0.1811724921f, 0, 0.06817146062f, -0.9743405129f, 0.2145069156f, 0, -0.3577285196f, -0.6697087264f, -0.6507845481f, 0, -0.1868621131f, 0.7648617052f, -0.6164974636f, 0, -0.6541697588f, 0.3967914832f, 0.6439087246f, 0,
    0.6993340405f, -0.6164538506f, 0.3618239211f, 0, -0.1546665739f, 0.6291283928f, 0.7617583057f, 0, -0.6841612949f, -0.2580482182f, -0.6821542638f, 0, 0.5383980957f, 0.4258654885f, 0.7271630328f, 0, -0.5026987823f, -0.7939832935f, -0.3418836993f, 0, 0.3202971715f, 0.2834415347f, 0.9039195862f, 0, 0.8683227101f, -0.0003762656404f, -0.4959995258f, 0, 0.791120031f, -0.08511045745f, 0.6057105799f, 0,
    -0.04011016052f, -0.4397248749f, 0.8972364289f, 0, 0.9145119872f, 0.3579346169f, -0.1885487608f, 0, -0.9612039066f, -0.2756484276f, 0.01024666929f, 0, 0.6510361721f, -0.2877799159f, -0.7023778346f, 0, -0.2041786351f, 0.7365237271f, 0.644859585f, 0, -0.7718263711f, 0.3790626912f, 0.5104855816f, 0, -0.3060082741f, -0.7692987727f, 0.5608371729f, 0, 0.454007341f, -0.5024843065f, 0.7357899537f, 0,
    0.4816795475f, 0.6021208291f, -0.6367380315f, 0, 0.6961980369f, -0.3222197429f, 0.641469197f, 0, -0.6532160499f, -0.6781148932f, 0.3368515753f, 0, 0.5089301236f, -0.6154662304f, -0.6018234363f, 0, -0.1635919754f, -0.9133604627f, -0.372840892f, 0, 0.52408019f, -0.8437664109f, 0.1157505864f, 0, 0.5902587356f, 0.4983817807f, -0.6349883666f, 0, 0.5863227872f, 0.494764745f, 0.6414307729f, 0,
    0.6779335087f, 0.2341345225f, 0.6968408593f, 0, 0.7177054546f, -0.6858979348f, 0.120178631f, 0, -0.5328819713f, -0.5205125012f, 0.6671608058f, 0, -0.8654874251f, -0.0700727088f, -0.4960053754f, 0, -0.2861810166f, 0.7952089234f, 0.5345495242f, 0, -0.04849529634f, 0.9810836427f, -0.1874115585f, 0, -0.6358521667f, 0.6058348682f, 0.4781800233f, 0, 0.6254794696f, -0.2861619734f, 0.7258696564f, 0,
    -0.2585259868f, 0.5061949264f, -0.8227581726f, 0, 0.02136306781f, 0.5064016808f, -0.8620330371f, 0, 0.200111773f, 0.8599263484f, 0.4695550591f, 0, 0.4743561372f, 0.6014985084f, -0.6427953014f, 0, 0.6622993731f, -0.5202474575f, -0.5391679918f, 0, 0.08084972818f, -0.6532720452f, 0.7527940996f, 0, -0.6893687501f, 0.0592860349f, 0.7219805347f, 0, -0.1121887082f, -0.9673185067f, 0.2273952515f, 0,
    0.7344116094f, 0.5979668656f, -0.3210532909f, 0, 0.5789393465f, -0.2488849713f, 0.7764570201f, 0, 0.6988182827f, 0.3557169806f, -0.6205791146f, 0, -0.8636845529f, -0.2748771249f, -0.4224826141f, 0, -0.4247027957f, -0.4640880967f, 0.777335046f, 0, 0.5257722489f, -0.8427017621f, 0.1158329937f, 0, 0.9343830603f, 0.316302472f, -0.1639543925f, 0, -0.1016836419f, -0.8057303073f, -0.5834887393f, 0,
    -0.6529238969f, 0.50602126f, -0.5635892736f, 0, -0.2465286165f, -0.9668205684f, -0.06694497494f, 0, -0.9776897119f, -0.2099250524f, -0.007368825344f, 0, 0.7736893337f, 0.5734244712f, 0.2694238123f, 0, -0.6095087895f, 0.4995678998f, 0.6155736747f, 0, 0.5794535482f, 0.7434546771f, 0.3339292269f, 0, -0.8226211154f, 0.08142581855f, 0.5627293636f, 0, -0.510385483f, 0.4703667658f, 0.7199039967f, 0,
    -0.5764971849f, -0.07231656274f, -0.8138926898f, 0, 0.7250628871f, 0.3949971505f, -0.5641463116f, 0, -0.1525424005f, 0.4860840828f, -0.8604958341f, 0, -0.5550976208f, -0.4957820792f, 0.667882296f, 0, -0.1883614327f, 0.9145869398f, 0.357841725f, 0, 0.7625556724f, -0.5414408243f, -0.3540489801f, 0, -0.5870231946f, -0.3226498013f, -0.7424963803f, 0, 0.3051124198f, 0.2262544068f, -0.9250488391f, 0,
    0.6379576059f, 0.577242424f, -0.5097070502f, 0, -0.5966775796f, 0.1454852398f, -0.7891830656f, 0, -0.658330573f, 0.6555487542f, -0.3699414651f, 0, 0.7434892426f, 0.2351084581f, 0.6260573129f, 0, 0.5562114096f, 0.8264360377f, -0.0873632843f, 0, -0.3028940016f, -0.8251527185f, 0.4768419182f, 0, 0.1129343818f, -0.985888439f, -0.1235710781f, 0, 0.5937652891f, -0.5896813806f, 0.5474656618f, 0,
    0.6757964092f, -0.5835758614f, -0.4502648413f, 0, 0.7242302609f, -0.1152719764f, 0.6798550586f, 0, -0.9511914166f, 0.0753623979f, -0.2992580792f, 0, 0.2539470961f, -0.1886339355f, 0.9486454084f, 0, 0.571433621f, -0.1679450851f, -0.8032795685f, 0, -0.06778234979f, 0.3978269256f, 0.9149531629f, 0, 0.6074972649f, 0.733060024f, -0.3058922593f, 0, -0.5435478392f, 0.1675822484f, 0.8224791405f, 0,
    -0.5876678086f, -0.3380045064f, -0.7351186982f, 0, -0.7967562402f, 0.04097822706f, -0.6029098428f, 0, -0.1996350917f, 0.8706294745f, 0.4496111079f, 0, -0.02787660336f, -0.9106232682f, -0.4122962022f, 0, -0.7797625996f, -0.6257634692f, 0.01975775581f, 0, -0.5211232846f, 0.7401644346f, -0.4249554471f, 0, 0.8575424857f, 0.4053272873f, -0.3167501783f, 0, 0.1045223322f, 0.8390195772f, -0.5339674439f, 0,
    0.3501822831f, 0.9242524096f, -0.1520850155f, 0, 0.1987849858f, 0.07647613266f, 0.9770547224f, 0, 0.7845996363f, 0.6066256811f, -0.1280964233f, 0, 0.09006737436f, -0.9750989929f, -0.2026569073f, 0, -0.8274343547f, -0.542299559f, 0.1458203587f, 0, -0.3485797732f, -0.415802277f, 0.840000362f, 0, -0.2471778936f, -0.7304819962f, -0.6366310879f, 0, -0.3700154943f, 0.8577948156f, 0.3567584454f, 0,
    0.5913394901f, -0.548311967f, -0.5913303597f, 0, 0.1204873514f, -0.7626472379f, -0.6354935001f, 0, 0.616959265f, 0.03079647928f, 0.7863922953f, 0, 0.1258156836f, -0.6640829889f, -0.7369967419f, 0, -0.6477565124f, -0.1740147258f, -0.7417077429f, 0, 0.6217889313f, -0.7804430448f, -0.06547655076f, 0, 0.6589943422f, -0.6096987708f, 0.4404473475f, 0, -0.2689837504f, -0.6732403169f, -0.6887635427f, 0,
    -0.3849775103f, 0.5676542638f, 0.7277093879f, 0, 0.5754444408f, 0.8110471154f, -0.1051963504f, 0, 0.9141593684f, 0.3832947817f, 0.131900567f, 0, -0.107925319f, 0.9245493968f, 0.3654593525f, 0, 0.377977089f, 0.3043148782f, 0.8743716458f, 0, -0.2142885215f, -0.8259286236f, 0.5214617324f, 0, 0.5802544474f, 0.4148098596f, -0.7008834116f, 0, -0.1982660881f, 0.8567161266f, -0.4761596756f, 0,
    -0.03381553704f, 0.3773180787f, -0.9254661404f, 0, -0.6867922841f, -0.6656597827f, 0.2919133642f, 0, 0.7731742607f, -0.2875793547f, -0.5652430251f, 0, -0.09655941928f, 0.9193708367f, -0.3813575004f, 0, 0.2715702457f, -0.9577909544f, -0.09426605581f, 0, 0.2451015704f, -0.6917998565f, -0.6792188003f, 0, 0.977700782f, -0.1753855374f, 0.1155036542f, 0, -0.5224739938f, 0.8521606816f, 0.02903615945f, 0,
    -0.7734880599f, -0.5261292347f, 0.3534179531f, 0, -0.7134492443f, -0.269547243f, 0.6467878011f, 0, 0.1644037271f, 0.5105846203f, -0.8439637196f, 0, 0.6494635788f, 0.05585611296f, 0.7583384168f, 0, -0.4711970882f, 0.5017280509f, -0.7254255765f, 0, -0.6335764307f, -0.2381686273f, -0.7361091029f, 0, -0.9021533097f, -0.270947803f, -0.3357181763f, 0, -0.3793711033f, 0.872258117f, 0.3086152025f, 0,
    -0.6855598966f, -0.3250143309f, 0.6514394162f, 0, 0.2900942212f, -0.7799057743f, -0.5546100667f, 0, -0.2098319339f, 0.85037073f, 0.4825351604f, 0, -0.4592603758f, 0.6598504336f, -0.5947077538f, 0, 0.8715945488f, 0.09616365406f, -0.4807031248f, 0, -0.6776666319f, 0.7118504878f, -0.1844907016f, 0, 0.7044377633f, 0.312427597f, 0.637304036f, 0, -0.7052318886f, -0.2401093292f, -0.6670798253f, 0,
    0.081921007f, -0.7207336136f, -0.6883545647f, 0, -0.6993680906f, -0.5875763221f, -0.4069869034f, 0, -0.1281454481f, 0.6419895885f, 0.7559286424f, 0, -0.6337388239f, -0.6785471501f, -0.3714146849f, 0, 0.5565051903f, -0.2168887573f, -0.8020356851f, 0, -0.5791554484f, 0.7244372011f, -0.3738578718f, 0, 0.1175779076f, -0.7096451073f, 0.6946792478f, 0, -0.6134619607f, 0.1323631078f, 0.7785527795f, 0,
    0.6984635305f, -0.02980516237f, -0.715024719f, 0, 0.8318082963f, -0.3930171956f, 0.3919597455f, 0, 0.1469576422f, 0.05541651717f, -0.9875892167f, 0, 0.708868575f, -0.2690503865f, 0.6520101478f, 0, 0.2726053183f, 0.67369766f, -0.68688995f, 0, -0.6591295371f, 0.3035458599f, -0.6880466294f, 0, 0.4815131379f, -0.7528270071f, 0.4487723203f, 0, 0.9430009463f, 0.1675647412f, -0.2875261255f, 0,
    0.434802957f, 0.7695304522f, -0.4677277752f, 0, 0.3931996188f, 0.594473625f, 0.7014236729f, 0, 0.7254336655f, -0.603925654f, 0.3301814672f, 0, 0.7590235227f, -0.6506083235f, 0.02433313207f, 0, -0.8552768592f, -0.3430042733f, 0.3883935666f, 0, -0.6139746835f, 0.6981725247f, 0.3682257648f, 0, -0.7465905486f, -0.5752009504f, 0.3342849376f, 0, 0.5730065677f, 0.810555537f, -0.1210916791f, 0,
    -0.9225877367f, -0.3475211012f, -0.167514036f, 0, -0.7105816789f, -0.4719692027f, -0.5218416899f, 0, -0.08564609717f, 0.3583001386f, 0.929669703f, 0, -0.8279697606f, -0.2043157126f, 0.5222271202f, 0, 0.427944023f, 0.278165994f, 0.8599346446f, 0, 0.5399079671f, -0.7857120652f, -0.3019204161f, 0, 0.5678404253f, -0.5495413974f, -0.6128307303f, 0, -0.9896071041f, 0.1365639107f, -0.04503418428f, 0,
    -0.6154342638f, -0.6440875597f, 0.4543037336f, 0, 0.1074204368f, -0.7946340692f, 0.5975094525f, 0, -0.3595449969f, -0.8885529948f, 0.28495784f, 0, -0.2180405296f, 0.1529888965f, 0.9638738118f, 0, -0.7277432317f, -0.6164050508f, -0.3007234646f, 0, 0.7249729114f, -0.00669719484f, 0.6887448187f, 0, -0.5553659455f, -0.5336586252f, 0.6377908264f, 0, 0.5137558015f, 0.7976208196f, -0.3160000073f, 0,
    -0.3794024848f, 0.9245608561f, -0.03522751494f, 0, 0.8229248658f, 0.2745365933f, -0.4974176556f, 0, -0.5404114394f, 0.6091141441f, 0.5804613989f, 0, 0.8036581901f, -0.2703029469f, 0.5301601931f, 0, 0.6044318879f, 0.6832968393f, 0.4095943388f, 0, 0.06389988817f, 0.9658208605f, -0.2512108074f, 0, 0.1087113286f, 0.7402471173f, -0.6634877936f, 0, -0.713427712f, -0.6926784018f, 0.1059128479f, 0,
    0.6458897819f, -0.5724548511f, -0.5050958653f, 0, -0.6553931414f, 0.7381471625f, 0.159995615f, 0, 0.3910961323f, 0.9188871375f, -0.05186755998f, 0, -0.4879022471f, -0.5904376907f, 0.6429111375f, 0, 0.6014790094f, 0.7707441366f, -0.2101820095f, 0, -0.5677173047f, 0.7511360995f, 0.3368851762f, 0, 0.7858573506f, 0.226674665f, 0.5753666838f, 0, -0.4520345543f, -0.604222686f, -0.6561857263f, 0,
    0.002272116345f, 0.4132844051f, -0.9105991643f, 0, -0.5815751419f, -0.5162925989f, 0.6286591339f, 0, -0.03703704785f, 0.8273785755f, 0.5604221175f, 0, -0.5119692504f, 0.7953543429f, -0.3244980058f, 0, -0.2682417366f, -0.9572290247f, -0.1084387619f, 0, -0.2322482736f, -0.9679131102f, -0.09594243324f, 0, 0.3554328906f, -0.8881505545f, 0.2913006227f, 0, 0.7346520519f, -0.4371373164f, 0.5188422971f, 0,
    0.9985120116f, 0.04659011161f, -0.02833944577f, 0, -0.3727687496f, -0.9082481361f, 0.1900757285f, 0, 0.91737377f, -0.3483642108f, 0.1925298489f, 0, 0.2714911074f, 0.4147529736f, -0.8684886582f, 0, 0.5131763485f, -0.7116334161f, 0.4798207128f, 0, -0.8737353606f, 0.18886992f, -0.4482350644f, 0, 0.8460043821f, -0.3725217914f, 0.3814499973f, 0, 0.8978727456f, -0.1780209141f, -0.4026575304f, 0,
    0.2178065647f, -0.9698322841f, -0.1094789531f, 0, -0.1518031304f, -0.7788918132f, -0.6085091231f, 0, -0.2600384876f, -0.4755398075f, -0.8403819825f, 0, 0.572313509f, -0.7474340931f, -0.3373418503f, 0, -0.7174141009f, 0.1699017182f, -0.6756111411f, 0, -0.684180784f, 0.02145707593f, -0.7289967412f, 0, -0.2007447902f, 0.06555605789f, -0.9774476623f, 0, -0.1148803697f, -0.8044887315f, 0.5827524187f, 0,
    -0.7870349638f, 0.03447489231f, 0.6159443543f, 0, -0.2015596421f, 0.6859872284f, 0.6991389226f, 0, -0.08581082512f, -0.10920836f, -0.9903080513f, 0, 0.5532693395f, 0.7325250401f, -0.396610771f, 0, -0.1842489331f, -0.9777375055f, -0.1004076743f, 0, 0.0775473789f, -0.9111505856f, 0.4047110257f, 0, 0.1399838409f, 0.7601631212f, -0.6344734459f, 0, 0.4484419361f, -0.845289248f, 0.2904925424f, 0
};

#endif


// Original File: ../cpp/core\geometry.h
/**
 * geometry.h — Core geometry primitives, path operations, and curve generators.
 * Header-only C++ port of app/core/geometry.py
 */

#define _USE_MATH_DEFINES
#define _USE_MATH_DEFINES

// ---------------------------------------------------------------------------
// Type aliases
// ---------------------------------------------------------------------------
using Point = std::pair<double, double>;
using Path  = std::vector<Point>;

// ---------------------------------------------------------------------------
// Basic math helpers
// ---------------------------------------------------------------------------
inline double geom_distance(const Point& a, const Point& b) {
    double dx = b.first  - a.first;
    double dy = b.second - a.second;
    return std::hypot(dx, dy);
}

inline double path_length(const Path& path) {
    double total = 0.0;
    for (std::size_t i = 1; i < path.size(); ++i)
        total += geom_distance(path[i - 1], path[i]);
    return total;
}

// ---------------------------------------------------------------------------
// Douglas-Peucker path simplification
// ---------------------------------------------------------------------------
namespace detail {
inline double point_to_segment_dist(const Point& p,
                                    const Point& a, const Point& b) {
    double ax = p.first  - a.first;
    double ay = p.second - a.second;
    double bx = b.first  - a.first;
    double by = b.second - a.second;
    double len2 = bx * bx + by * by;
    if (len2 < 1e-12) return std::hypot(ax, ay);
    double t = std::max(0.0, std::min(1.0, (ax * bx + ay * by) / len2));
    double dx = ax - t * bx;
    double dy = ay - t * by;
    return std::hypot(dx, dy);
}

inline Path dp_recursive(const Path& pts, std::size_t lo, std::size_t hi,
                          double tol) {
    if (hi - lo < 2) {
        return { pts[lo] };
    }
    double maxDist = 0.0;
    std::size_t maxIdx = lo + 1;
    for (std::size_t i = lo + 1; i < hi; ++i) {
        double d = point_to_segment_dist(pts[i], pts[lo], pts[hi]);
        if (d > maxDist) { maxDist = d; maxIdx = i; }
    }
    if (maxDist > tol) {
        auto left  = dp_recursive(pts, lo, maxIdx, tol);
        auto right = dp_recursive(pts, maxIdx, hi, tol);
        left.insert(left.end(), right.begin(), right.end());
        return left;
    }
    return { pts[lo] };
}
} // namespace detail

inline Path simplify_path_dp(const Path& path, double tolerance) {
    if (path.size() <= 2) return path;
    auto result = detail::dp_recursive(path, 0, path.size() - 1, tolerance);
    result.push_back(path.back());
    return result;
}

// ---------------------------------------------------------------------------
// Path utilities
// ---------------------------------------------------------------------------
inline std::vector<Path> filter_short_paths(const std::vector<Path>& paths,
                                             double minLen) {
    std::vector<Path> out;
    for (auto& p : paths)
        if (path_length(p) >= minLen) out.push_back(p);
    return out;
}

inline std::vector<Path> sort_paths_nearest(const std::vector<Path>& paths,
                                             Point start = {0.0, 0.0}) {
    if (paths.empty()) return {};
    std::vector<bool> used(paths.size(), false);
    std::vector<Path> sorted;
    sorted.reserve(paths.size());
    Point current = start;

    for (std::size_t iter = 0; iter < paths.size(); ++iter) {
        double bestDist = std::numeric_limits<double>::infinity();
        int    bestIdx  = -1;
        bool   bestRev  = false;

        for (std::size_t i = 0; i < paths.size(); ++i) {
            if (used[i] || paths[i].empty()) continue;
            double dFront = geom_distance(current, paths[i].front());
            double dBack  = geom_distance(current, paths[i].back());
            if (dFront < bestDist) { bestDist = dFront; bestIdx = (int)i; bestRev = false; }
            if (dBack  < bestDist) { bestDist = dBack;  bestIdx = (int)i; bestRev = true;  }
        }
        if (bestIdx < 0) break;
        used[bestIdx] = true;
        Path p = paths[bestIdx];
        if (bestRev && p.size() > 1) std::reverse(p.begin(), p.end());
        current = p.back();
        sorted.push_back(std::move(p));
    }
    return sorted;
}

// ---------------------------------------------------------------------------
// Coordinate transforms
// ---------------------------------------------------------------------------
inline std::vector<Path> scale_paths(const std::vector<Path>& paths,
                                      double sx, double sy,
                                      double ox = 0.0, double oy = 0.0) {
    std::vector<Path> out;
    out.reserve(paths.size());
    for (auto& path : paths) {
        Path p;
        p.reserve(path.size());
        for (auto& pt : path)
            p.push_back({ pt.first * sx + ox, pt.second * sy + oy });
        out.push_back(std::move(p));
    }
    return out;
}

inline std::vector<Path> translate_paths(const std::vector<Path>& paths,
                                          double dx, double dy) {
    std::vector<Path> out;
    for (auto& path : paths) {
        Path p;
        p.reserve(path.size());
        for (auto& pt : path) p.push_back({ pt.first + dx, pt.second + dy });
        out.push_back(std::move(p));
    }
    return out;
}

// ---------------------------------------------------------------------------
// Curve generators
// ---------------------------------------------------------------------------

// Quadratic Bézier
inline Point quad_bezier_point(const Point& p0, const Point& p1, const Point& p2,
                                double t) {
    double u = 1.0 - t;
    return { u*u*p0.first  + 2*u*t*p1.first  + t*t*p2.first,
             u*u*p0.second + 2*u*t*p1.second + t*t*p2.second };
}

inline Path quad_bezier_path(const Point& p0, const Point& p1, const Point& p2,
                              int segments = 20) {
    Path result;
    result.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i)
        result.push_back(quad_bezier_point(p0, p1, p2, (double)i / segments));
    return result;
}

// Cubic Bézier
inline Point cubic_bezier_point(const Point& p0, const Point& p1,
                                 const Point& p2, const Point& p3, double t) {
    double u = 1.0 - t;
    return { u*u*u*p0.first  + 3*u*u*t*p1.first  + 3*u*t*t*p2.first  + t*t*t*p3.first,
             u*u*u*p0.second + 3*u*u*t*p1.second + 3*u*t*t*p2.second + t*t*t*p3.second };
}

inline Path cubic_bezier_path(const Point& p0, const Point& p1,
                               const Point& p2, const Point& p3, int segments = 20) {
    Path result;
    result.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i)
        result.push_back(cubic_bezier_point(p0, p1, p2, p3, (double)i / segments));
    return result;
}

// Catmull-Rom spline
inline Point catmull_rom_point(const Point& p0, const Point& p1,
                                const Point& p2, const Point& p3,
                                double t, double alpha = 0.5) {
    auto tj = [&](double ti, const Point& pi, const Point& pj) {
        double dx = pj.first - pi.first, dy = pj.second - pi.second;
        double l = std::sqrt(dx*dx + dy*dy);
        return ti + std::max(l, 1e-10) * alpha;   // Note: alpha power for centripetal
    };
    // Actually: raise to alpha power
    auto tjA = [&](double ti, const Point& pi, const Point& pj) {
        double dx = pj.first - pi.first, dy = pj.second - pi.second;
        double l = std::sqrt(dx*dx + dy*dy);
        return ti + std::pow(std::max(l, 1e-10), alpha);
    };

    double t0 = 0.0;
    double t1 = tjA(t0, p0, p1);
    double t2 = tjA(t1, p1, p2);
    double t3 = tjA(t2, p2, p3);
    double tv = t1 + t * (t2 - t1);

    auto lerp = [](const Point& a, const Point& b, double ta, double tb, double tc) -> Point {
        if (std::abs(tb - ta) < 1e-10) return a;
        double f = (tc - ta) / (tb - ta);
        return { a.first + f*(b.first - a.first), a.second + f*(b.second - a.second) };
    };

    auto a1 = lerp(p0, p1, t0, t1, tv);
    auto a2 = lerp(p1, p2, t1, t2, tv);
    auto a3 = lerp(p2, p3, t2, t3, tv);
    auto b1 = lerp(a1, a2, t0, t2, tv);
    auto b2 = lerp(a2, a3, t1, t3, tv);
    return lerp(b1, b2, t1, t2, tv);
}

inline Path catmull_rom_chain(const std::vector<Point>& pts,
                               int segmentsPerSpan = 20, double alpha = 0.5) {
    if (pts.size() < 2) return pts;
    // Pad endpoints
    std::vector<Point> p = { pts.front() };
    p.insert(p.end(), pts.begin(), pts.end());
    p.push_back(pts.back());

    Path result;
    for (std::size_t i = 1; i < p.size() - 2; ++i) {
        for (int s = 0; s < segmentsPerSpan; ++s) {
            double t = (double)s / segmentsPerSpan;
            result.push_back(catmull_rom_point(p[i-1], p[i], p[i+1], p[i+2], t, alpha));
        }
    }
    result.push_back(pts.back());
    return result;
}

// ---------------------------------------------------------------------------
// Shape generators
// ---------------------------------------------------------------------------
inline Path generate_circle(double cx, double cy, double radius, int segments = 16) {
    Path path;
    path.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i) {
        double angle = (double)i / segments * M_PI * 2.0;
        path.push_back({ cx + std::cos(angle) * radius,
                         cy + std::sin(angle) * radius });
    }
    return path;
}

inline Path generate_ellipse(double cx, double cy, double rx, double ry,
                              double rotationDeg = 0.0, int segments = 36) {
    double cosR = std::cos(rotationDeg * M_PI / 180.0);
    double sinR = std::sin(rotationDeg * M_PI / 180.0);
    Path path;
    path.reserve(segments + 1);
    for (int i = 0; i <= segments; ++i) {
        double angle = 2.0 * M_PI * i / segments;
        double x = rx * std::cos(angle);
        double y = ry * std::sin(angle);
        path.push_back({ cx + x * cosR - y * sinR,
                         cy + x * sinR + y * cosR });
    }
    return path;
}

inline Path generate_rectangle(double cx, double cy, double w, double h,
                                double rotationDeg = 0.0) {
    double hw = w / 2.0, hh = h / 2.0;
    double cosR = std::cos(rotationDeg * M_PI / 180.0);
    double sinR = std::sin(rotationDeg * M_PI / 180.0);
    std::vector<std::pair<double,double>> corners = {
        {-hw,-hh},{hw,-hh},{hw,hh},{-hw,hh},{-hw,-hh}
    };
    Path path;
    for (auto& c : corners)
        path.push_back({ cx + c.first * cosR - c.second * sinR,
                         cy + c.first * sinR + c.second * cosR });
    return path;
}

inline Path generate_polygon(double cx, double cy, double radius, int sides,
                              double angleOffsetDeg = 0.0) {
    Path path;
    double offset = angleOffsetDeg * M_PI / 180.0;
    for (int i = 0; i <= sides; ++i) {
        double angle = offset + (double)i / sides * M_PI * 2.0;
        path.push_back({ cx + std::cos(angle) * radius,
                         cy + std::sin(angle) * radius });
    }
    return path;
}

inline Path generate_star(double cx, double cy, double rOuter,
                           double rInner = -1.0, int points = 5,
                           double rotationDeg = 0.0) {
    if (rInner < 0.0) rInner = rOuter * 0.4;
    int total = points * 2;
    double rotRad = rotationDeg * M_PI / 180.0 - M_PI / 2.0;
    Path path;
    for (int i = 0; i <= total; ++i) {
        double angle = rotRad + 2.0 * M_PI * i / total;
        double r = (i % 2 == 0) ? rOuter : rInner;
        path.push_back({ cx + r * std::cos(angle), cy + r * std::sin(angle) });
    }
    return path;
}


// Original File: ../cpp/core\mask_manager.h
/**
 * mask_manager.h — Image masking (white = process, black = ignore/turn white).
 */


class MaskManager {
public:
    MaskManager() = default;

    bool load(const QString& filepath);
    void clear();

    bool isEnabled() const { return m_enabled; }
    QString maskPath() const { return m_maskPath; }

    // Multiplies the image by the mask. Masked out areas become white (255).
    cv::Mat applyMask(const cv::Mat& image) const;

private:
    cv::Mat m_mask;
    QString m_maskPath;
    bool m_enabled = false;
};


// Original File: ../cpp/core\module_registry.h

class PathFindingModule;
class ImageFilter;

class ModuleRegistry {
public:
    static QVector<std::shared_ptr<PathFindingModule>> createAllPFMs();
    static QVector<std::shared_ptr<ImageFilter>> createAllFilters();
};


// Original File: ../cpp/core\project_processor.h



class PathFindingModule;
class ImageFilter;

struct ProcessResult {
    bool success = false;
    QString errorMessage;
    QVector<DrawingGeometry> geometries;
    QVector<QColor> penColors;
    double penWidthMm = 0.5;
    bool useVpype = false;
    QString vpypePipeline;
    QString pfmName;
    cv::Mat previewImage;
    DrawingAreaConfig drawingArea;
    int imageWidth = 0;
    int imageHeight = 0;
};

class ProjectProcessor {
public:
    ProjectProcessor();

    // Loads a .dbv3 JSON project file. 
    // Optionally overrides the image path if overrideImagePath is not empty.
    bool loadProject(const QString& projectPath, const QString& overrideImagePath = QString());

    // Processes the loaded project synchronously and returns the generated geometries.
    ProcessResult process();

private:
    std::shared_ptr<PathFindingModule> getPFMByName(const QString& name) const;
    std::shared_ptr<ImageFilter> getFilterByName(const QString& name) const;
    QVector<QColor> fallbackPalette(const QString& sepMode, bool exportPalette) const;

    QString m_imagePath;
    cv::Mat m_image;
    
    QString m_pfmName;
    std::shared_ptr<PathFindingModule> m_activePFM;
    
    QVector<std::shared_ptr<ImageFilter>> m_activeFilters;
    
    QString m_separationMode;
    double m_penWidthMm = 0.5;
    QVector<QColor> m_penColors;
    
    bool m_useVpype = false;
    QString m_vpypePipeline;
    DrawingAreaConfig m_drawingArea;
    
    bool m_maskEnabled = false;
    QString m_maskPath;
    cv::Mat m_mask;

    // We cache all available to easily fetch by name
    QVector<std::shared_ptr<PathFindingModule>> m_allPFMs;
    QVector<std::shared_ptr<ImageFilter>> m_allFilters;
};


// Original File: ../cpp/core\serial_manager.h
/**
 * serial_manager.h — Serial Port manager for GRBL streaming
 */


class SerialManager : public QObject {
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

// signals:
    void statusMessage(QString type, QString msg, int progress);

private:
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


// Original File: ../cpp/pfm\adaptive_circular_scribbles_pfm.h

class AdaptiveCircularScribblesPFM : public BaseAdaptivePFM {
    public:
    explicit AdaptiveCircularScribblesPFM(QObject* p = nullptr);
    QString name() const override { return "Adaptive Circular Scribbles"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\adaptive_dashes_pfm.h

class AdaptiveDashesPFM : public BaseAdaptivePFM {
    public:
    explicit AdaptiveDashesPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Dashes"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\adaptive_diagram_pfm.h

class AdaptiveDiagramPFM : public BaseAdaptivePFM {
    public:
    explicit AdaptiveDiagramPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Diagram"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\adaptive_letters_pfm.h

class AdaptiveLettersPFM : public _LettersBasePFM {
    public:
    explicit AdaptiveLettersPFM(QObject* parent = nullptr) : _LettersBasePFM(parent) {}
    QString name() const override { return "Adaptive Letters"; }
protected:
    int getLloydIters() const override { return 0; }
};


// Original File: ../cpp/pfm\adaptive_pfm.h

/**
 * adaptive_pfm.h — Adaptive Stippling PFM (brightness-weighted dot placement).
 * This is the "Adaptive Stippling" class from tk_gui_fast.py's AVAILABLE_PFMS list.
 * It is a simplified C++ implementation: weighted random sampling without
 * the full Lloyd relaxation from the Python pfm_utils (which has heavy scipy deps).
 */
class AdaptiveStipplingPFM : public PathFindingModule {
    public:
    explicit AdaptiveStipplingPFM(QObject* parent = nullptr);
    QString name()        const override { return "Adaptive Stippling"; }
    QString description() const override { return "Brightness-weighted adaptive stipple placement."; }
    QString category()    const override { return "Adaptive"; }
    bool    isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\adaptive_shapes_pfm.h

class AdaptiveShapesPFM : public BaseAdaptivePFM {
    public:
    explicit AdaptiveShapesPFM(QObject* p = nullptr);
    QString name() const override { return "Adaptive Shapes"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\adaptive_tree_pfm.h

class AdaptiveTreePFM : public BaseAdaptivePFM {
    public:
    explicit AdaptiveTreePFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Tree"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\adaptive_triangulation_pfm.h

class AdaptiveTriangulationPFM : public BaseAdaptivePFM {
    public:
    explicit AdaptiveTriangulationPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive Triangulation"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\adaptive_tsppfm.h

class AdaptiveTSPPFM : public BaseAdaptivePFM {
    public:
    explicit AdaptiveTSPPFM(QObject* p = nullptr) : BaseAdaptivePFM(p) {}
    QString name() const override { return "Adaptive TSP"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\ambient_flow_pfm.h

class AmbientFlowPFM : public PathFindingModule {
    public:
    explicit AmbientFlowPFM(QObject* parent = nullptr);
    QString name() const override { return "Ambient Flow"; }
    QString category() const override { return "Special"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\base_adaptive_pfm.h

class BaseAdaptivePFM : public PathFindingModule {
    public:
    explicit BaseAdaptivePFM(QObject* parent = nullptr);
    QString category() const override { return "Adaptive"; }
    bool    isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    // Returns brightness-weighted, Lloyd-relaxed seed points
    std::vector<cv::Point2f> getSeeds(const cv::Mat& image);
};


// Original File: ../cpp/pfm\base_grid_pfm.h

class BaseGridPFM : public PathFindingModule {
    public:
    explicit BaseGridPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\base_maze_pfm.h

class BaseMazePFM : public PathFindingModule {
    public:
    explicit BaseMazePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
    virtual std::pair<float, float> transformPoint(float x, float y);
};


// Original File: ../cpp/pfm\base_mosaic_pfm.h

class BaseMosaicPFM : public PathFindingModule {
    public:
    explicit BaseMosaicPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\base_multi_hatch_pfm.h

class BaseMultiHatchPFM : public PathFindingModule {
    public:
    explicit BaseMultiHatchPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
    virtual std::vector<float> getAngles() const = 0;
};


// Original File: ../cpp/pfm\base_stipple_pfm.h

class BaseStipplePFM : public PathFindingModule {
    public:
    explicit BaseStipplePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual Path generateShape(float cx, float cy, float r) = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\base_streamline_pfm.h

static inline Path trace_streamline(const cv::Mat& fx, const cv::Mat& fy, float x, float y, int max_len, float step_size) {
    Path path;
    int w = fx.cols;
    int h = fx.rows;
    for (int i = 0; i < max_len; ++i) {
        int xi = std::clamp(int(x), 0, w - 1);
        int yi = std::clamp(int(y), 0, h - 1);
        path.push_back({x, y});
        
        float vx = fx.at<float>(yi, xi);
        float vy = fy.at<float>(yi, xi);
        float mag = std::hypot(vx, vy);
        if (mag < 1e-6f) break;
        
        x += (vx / mag) * step_size;
        y += (vy / mag) * step_size;
        if (x < 0 || x >= w || y < 0 || y >= h) break;
    }
    return path;
}

class BaseStreamlinePFM : public PathFindingModule {
    public:
    explicit BaseStreamlinePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\base_tsppfm.h

static inline Path solve_tsp_nn(const std::vector<cv::Point2f>& points,
                                std::function<bool()> isCancelledFunc = []{ return false; },
                                std::function<void(float)> progressFunc = [](float){}) {
    if (points.empty()) return {};
    
    std::vector<bool> visited(points.size(), false);
    Path path;
    path.reserve(points.size());
    
    int current = 0;
    visited[0] = true;
    path.push_back({points[0].x, points[0].y});
    
    int remaining = points.size() - 1;
    int total = remaining;

    while (remaining > 0) {
        if (isCancelledFunc()) return {};
        if (remaining % 1000 == 0) progressFunc(1.0f - (float(remaining) / std::max(1, total)));

        int best_i = -1;
        float best_d = 1e12f;
        float cx = points[current].x;
        float cy = points[current].y;
        
        for (size_t i = 0; i < points.size(); ++i) {
            if (!visited[i]) {
                float dx = points[i].x - cx;
                float dy = points[i].y - cy;
                float d = dx*dx + dy*dy;
                if (d < best_d) {
                    best_d = d;
                    best_i = i;
                }
            }
        }
        if (best_i == -1) break;
        
        visited[best_i] = true;
        path.push_back({points[best_i].x, points[best_i].y});
        current = best_i;
        remaining--;
    }
    return path;
}

class BaseTSPPFM : public PathFindingModule {
    public:
    explicit BaseTSPPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual std::vector<double> getProbabilities(const cv::Mat& image) = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\base_voronoi_extra_pfm.h

static inline float nearest_seed_radius(float cx, float cy, const std::vector<cv::Point2f>& points) {
    float best = 1e12f;
    for (const auto& pt : points) {
        float dx = pt.x - cx;
        float dy = pt.y - cy;
        if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) continue;
        float d = std::hypot(dx, dy);
        if (d < best) best = d;
    }
    return best < 1e11f ? best * 0.45f : 8.0f;
}

class BaseVoronoiExtraPFM : public PathFindingModule {
    public:
    explicit BaseVoronoiExtraPFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<cv::Point2f> getSeeds(const cv::Mat& image);
};


// Original File: ../cpp/pfm\contour_paths_pfm.h

class ContourPathsPFM : public PathFindingModule {
    public:
    explicit ContourPathsPFM(QObject* parent = nullptr);
    QString name() const override { return "Contour Paths"; }
    QString category() const override { return "Special"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\edge_shading_pfm.h

class EdgeShadingPFM : public PathFindingModule {
    public:
    explicit EdgeShadingPFM(QObject* parent = nullptr);
    QString name() const override { return "Edge Shading"; }
    QString category() const override { return "Special"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\grid_dashes_pfm.h

class GridDashesPFM : public BaseGridPFM {
    public: explicit GridDashesPFM(QObject* p=nullptr):BaseGridPFM(p){} QString name() const override{return "Grid Dashes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\grid_letters_pfm.h

class GridLettersPFM : public BaseGridPFM {
    public: explicit GridLettersPFM(QObject* p=nullptr):BaseGridPFM(p){} QString name() const override{return "Grid Letters";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\grid_shapes_pfm.h

class GridShapesPFM : public BaseGridPFM {
    public: explicit GridShapesPFM(QObject* p=nullptr):BaseGridPFM(p){} QString name() const override{return "Grid Shapes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\hatch3_way_pfm.h

class Hatch3WayPFM : public BaseMultiHatchPFM {
    public: explicit Hatch3WayPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch 3-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 45.0f, -45.0f}; } };


// Original File: ../cpp/pfm\hatch4_way_pfm.h

class Hatch4WayPFM : public BaseMultiHatchPFM {
    public: explicit Hatch4WayPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch 4-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f}; } };


// Original File: ../cpp/pfm\hatch5_way_pfm.h

class Hatch5WayPFM : public BaseMultiHatchPFM {
    public: explicit Hatch5WayPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch 5-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f, 22.5f}; } };


// Original File: ../cpp/pfm\hatch6_way_pfm.h

class Hatch6WayPFM : public BaseMultiHatchPFM {
    public: explicit Hatch6WayPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch 6-Way";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f, 45.0f, -45.0f, 22.5f, -22.5f}; } };


// Original File: ../cpp/pfm\hatch_circular_scribbles_pfm.h

class HatchCircularScribblesPFM : public PathFindingModule {
    public:
    explicit HatchCircularScribblesPFM(QObject* parent = nullptr);
    QString name() const override { return "Hatch Circular Scribbles"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\hatch_cross_pfm.h

class HatchCrossPFM : public BaseMultiHatchPFM {
    public: explicit HatchCrossPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Cross";} protected: std::vector<float> getAngles() const override { return {45.0f, -45.0f}; } };


// Original File: ../cpp/pfm\hatch_diagonal1_pfm.h

class HatchDiagonal1PFM : public BaseMultiHatchPFM {
    public: explicit HatchDiagonal1PFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Diagonal 1";} protected: std::vector<float> getAngles() const override { return {45.0f}; } };


// Original File: ../cpp/pfm\hatch_diagonal2_pfm.h

class HatchDiagonal2PFM : public BaseMultiHatchPFM {
    public: explicit HatchDiagonal2PFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Diagonal 2";} protected: std::vector<float> getAngles() const override { return {-45.0f}; } };


// Original File: ../cpp/pfm\hatch_grid_pfm.h

class HatchGridPFM : public BaseMultiHatchPFM {
    public: explicit HatchGridPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Grid";} protected: std::vector<float> getAngles() const override { return {0.0f, 90.0f}; } };


// Original File: ../cpp/pfm\hatch_horizontal_pfm.h

class HatchHorizontalPFM : public BaseMultiHatchPFM {
    public: explicit HatchHorizontalPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Horizontal";} protected: std::vector<float> getAngles() const override { return {0.0f}; } };


// Original File: ../cpp/pfm\hatch_lines.h

class HatchLinesPFM : public PathFindingModule {
    public:
    explicit HatchLinesPFM(QObject* parent = nullptr);
    QString name()        const override { return "Hatch Lines"; }
    QString description() const override { return "Cross-hatching based on image brightness."; }
    QString category()    const override { return "Hatch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\hatch_sawtooth_pfm.h

class HatchSawtoothPFM : public PathFindingModule {
    public:
    explicit HatchSawtoothPFM(QObject* parent = nullptr);
    QString name() const override { return "Hatch Sawtooth"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\hatch_vertical_pfm.h

class HatchVerticalPFM : public BaseMultiHatchPFM {
    public: explicit HatchVerticalPFM(QObject* p=nullptr):BaseMultiHatchPFM(p){} QString name() const override{return "Hatch Vertical";} protected: std::vector<float> getAngles() const override { return {90.0f}; } };


// Original File: ../cpp/pfm\labyrinth_classic_pfm.h

class LabyrinthClassicPFM : public PathFindingModule {
    public:
    explicit LabyrinthClassicPFM(QObject* parent = nullptr);
    QString name() const override { return "Labyrinth Classic"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\labyrinth_roman_pfm.h

class LabyrinthRomanPFM : public LabyrinthClassicPFM {
    public:
    explicit LabyrinthRomanPFM(QObject* p = nullptr) : LabyrinthClassicPFM(p) {}
    QString name() const override { return "Labyrinth Roman"; }
};


// Original File: ../cpp/pfm\layers_pfm.h

class LayersPFM : public PathFindingModule {
    public:
    explicit LayersPFM(QObject* parent = nullptr);
    QString name()     const override { return "Layers PFM"; }
    QString category() const override { return "Composite"; }
    bool isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\lbgtsppfm.h

class LBGTSPPFM : public AdaptiveTSPPFM {
    public:
    explicit LBGTSPPFM(QObject* p = nullptr) : AdaptiveTSPPFM(p) {}
    QString name() const override { return "LBG TSP"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\lbg_circular_scribbles_pfm.h

class LBGCircularScribblesPFM : public AdaptiveCircularScribblesPFM {
    public:
    explicit LBGCircularScribblesPFM(QObject* p = nullptr) : AdaptiveCircularScribblesPFM(p) {}
    QString name() const override { return "LBG Circular Scribbles"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\lbg_dashes_pfm.h

class LBGDashesPFM : public AdaptiveDashesPFM {
    public:
    explicit LBGDashesPFM(QObject* p = nullptr) : AdaptiveDashesPFM(p) {}
    QString name() const override { return "LBG Dashes"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\lbg_diagram_pfm.h

class LBGDiagramPFM : public AdaptiveDiagramPFM {
    public:
    explicit LBGDiagramPFM(QObject* p = nullptr) : AdaptiveDiagramPFM(p) {}
    QString name() const override { return "LBG Diagram"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\lbg_letters_pfm.h

class LBGLettersPFM : public _LettersBasePFM {
    public:
    explicit LBGLettersPFM(QObject* parent = nullptr);
    QString name() const override { return "LBG Letters"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    int getLloydIters() const override;
};


// Original File: ../cpp/pfm\lbg_pfms.h

QVector<PFMSetting> makeLbgSettings(QVector<PFMSetting> settings);


// Original File: ../cpp/pfm\lbg_shapes_pfm.h

class LBGShapesPFM : public AdaptiveShapesPFM {
    public:
    explicit LBGShapesPFM(QObject* p = nullptr) : AdaptiveShapesPFM(p) {}
    QString name() const override { return "LBG Shapes"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\lbg_stippling_pfm.h

class LBGStipplingPFM : public AdaptiveStipplingPFM {
    public:
    explicit LBGStipplingPFM(QObject* p = nullptr) : AdaptiveStipplingPFM(p) {}
    QString name() const override { return "LBG Stippling"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\lbg_tree_pfm.h

class LBGTreePFM : public AdaptiveTreePFM {
    public:
    explicit LBGTreePFM(QObject* p = nullptr) : AdaptiveTreePFM(p) {}
    QString name() const override { return "LBG Tree"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\lbg_triangulation_pfm.h

class LBGTriangulationPFM : public AdaptiveTriangulationPFM {
    public:
    explicit LBGTriangulationPFM(QObject* p = nullptr) : AdaptiveTriangulationPFM(p) {}
    QString name() const override { return "LBG Triangulation"; }
    QString category() const override { return "LBG"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
};


// Original File: ../cpp/pfm\maze_circle_pfm.h

class MazeCirclePFM : public BaseMazePFM {
    public: explicit MazeCirclePFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Circular";} protected: std::pair<float, float> transformPoint(float x, float y) override; };


// Original File: ../cpp/pfm\maze_hex_pfm.h

class MazeHexPFM : public BaseMazePFM {
    public: explicit MazeHexPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Hexagonal";} protected: std::pair<float, float> transformPoint(float x, float y) override; };


// Original File: ../cpp/pfm\maze_hilbert_pfm.h

class MazeHilbertPFM : public PathFindingModule {
    public:
    explicit MazeHilbertPFM(QObject* parent = nullptr);
    QString name() const override { return "Hilbert Curve"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\maze_labyrinth_pfm.h

class MazeLabyrinthPFM : public BaseMazePFM {
    public: explicit MazeLabyrinthPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Labyrinth";} };


// Original File: ../cpp/pfm\maze_peano_pfm.h

class MazePeanoPFM : public PathFindingModule {
    public:
    explicit MazePeanoPFM(QObject* parent = nullptr);
    QString name() const override { return "Peano Curve"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\maze_rect_pfm.h

class MazeRectPFM : public BaseMazePFM {
    public: explicit MazeRectPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Rectangular";} };


// Original File: ../cpp/pfm\maze_tri_pfm.h

class MazeTriPFM : public BaseMazePFM {
    public: explicit MazeTriPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Triangular";} protected: std::pair<float, float> transformPoint(float x, float y) override; };


// Original File: ../cpp/pfm\maze_voronoi_pfm.h

class MazeVoronoiPFM : public BaseMazePFM {
    public: explicit MazeVoronoiPFM(QObject* p=nullptr):BaseMazePFM(p){} QString name() const override{return "Maze Voronoi";} protected: std::pair<float, float> transformPoint(float x, float y) override; };


// Original File: ../cpp/pfm\mosaic_custom_pfm.h

class MosaicCustomPFM : public MosaicRectanglesPFM {
    public: explicit MosaicCustomPFM(QObject* p=nullptr):MosaicRectanglesPFM(p){} QString name() const override{return "Mosaic Custom";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\mosaic_rectangles_pfm.h

class MosaicRectanglesPFM : public BaseMosaicPFM {
    public: explicit MosaicRectanglesPFM(QObject* p=nullptr):BaseMosaicPFM(p){} QString name() const override{return "Mosaic Rectangles";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\mosaic_voronoi_pfm.h

class MosaicVoronoiPFM : public BaseMosaicPFM {
    public: explicit MosaicVoronoiPFM(QObject* p=nullptr):BaseMosaicPFM(p){} QString name() const override{return "Mosaic Voronoi";} protected: QVector<PFMSetting> defineSettings() const override; QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\pfm_base.h
/**
 * pfm_base.h — Abstract base class for all Path Finding Modules (PFMs).
 * C++ port of app/pfm/__init__.py (PathFindingModule, PFMSetting, DrawingGeometry).
 */





// ---------------------------------------------------------------------------
// Setting types
// ---------------------------------------------------------------------------
enum class SettingType {
    Number,
    Integer,
    Percentage,
    Boolean,
    Enum,
    Text,
};

struct PFMSetting {
    QString     key;
    QString     label;
    SettingType type        = SettingType::Number;
    QVariant    defaultVal;
    QVariant    value;          // null = use default
    double      minVal      = 0.0;
    double      maxVal      = 100.0;
    double      safeMin     = 0.0;
    double      safeMax     = 100.0;
    double      step        = 1.0;
    QStringList options;        // for Enum type
    QString     category    = "Default";
    QString     tooltip;
    bool        randomiseExclude = false;

    QVariant currentValue() const {
        return value.isNull() ? defaultVal : value;
    }

    double toDouble() const { return currentValue().toDouble(); }
    int    toInt()    const { return currentValue().toInt(); }
    bool   toBool()   const { return currentValue().toBool(); }
};

// ---------------------------------------------------------------------------
// Output geometry
// ---------------------------------------------------------------------------
struct DrawingGeometry {
    Path path;
    int  penIndex   = 0;
    int  groupIndex = 0;
};

// ---------------------------------------------------------------------------
// Abstract base class
// ---------------------------------------------------------------------------
class PathFindingModule : public QObject {
    public:
    explicit PathFindingModule(QObject* parent = nullptr);
    virtual ~PathFindingModule() = default;

    // Identity
    virtual QString name()        const = 0;
    virtual QString description() const { return {}; }
    virtual QString category()    const { return "General"; }
    virtual bool    isPremium()   const { return false; }

    // Settings
    QVector<PFMSetting>       settingsList() const;
    const QMap<QString, PFMSetting>& settingsMap() const { return m_settings; }
    QVariant get(const QString& key) const;
    void     set(const QString& key, const QVariant& value);
    void     resetAll();

    // Processing
    QVector<DrawingGeometry> process(const cv::Mat& grayImage);
    void cancel() { m_cancelled.storeRelaxed(1); }
    bool isCancelled() const { return m_cancelled.loadRelaxed() != 0; }

    // RNG helpers
    double  randUniform(double lo, double hi);
    double  randGauss(double mean, double sigma);
    int     randInt(int lo, int hi);  // inclusive
    int     weightedChoice(const std::vector<double>& weights);

    // Public helper so free-function algorithm helpers can call it
    void emitProgress(float pct, int shapes, const QString& text);

// signals:
    void progressUpdate(float pct, int shapes, QString text);

protected:
    virtual QVector<PFMSetting>    defineSettings() const = 0;
    virtual QVector<DrawingGeometry> _process(const cv::Mat& image) = 0;


    QMap<QString, PFMSetting> m_settings;
    std::mt19937              m_rng;
    QAtomicInt                m_cancelled;

    // Must be called at end of each subclass constructor
    void initSettings();
};


// Original File: ../cpp/pfm\pfm_sketch_utils.h

/**
 * pfm_sketch_utils.h — Shared utilities for sketch-style PFMs.
 * Provides the common darkest-area loop and erase-line logic used by
 * SketchLines, SketchCurves, SketchBeziers (all share the same core).
 */


namespace sketch_utils {

// ---------------------------------------------------------------------------
// Erase a line segment on a float image (identical logic across all sketch PFMs)
// ---------------------------------------------------------------------------
inline void eraseLine(cv::Mat& img,
                      double x0, double y0, double x1, double y1,
                      double eraseMin, double eraseMax,
                      double radiusMin, double radiusMax,
                      double tone, int W, int H)
{
    double dist = std::hypot(x1 - x0, y1 - y0);
    int steps = std::max(1, (int)dist);
    float* data = img.ptr<float>(0);

    for (int s = 0; s <= steps; ++s) {
        double t       = (double)s / steps;
        double toneT   = (tone > 0.0) ? std::pow(t, 1.0 / std::max(0.01, tone)) : 0.5;
        double eraseVal = eraseMin + (eraseMax - eraseMin) * toneT;
        double radius   = radiusMin + (radiusMax - radiusMin) * toneT;

        double px = x0 + (x1 - x0) * t;
        double py = y0 + (y1 - y0) * t;
        int pxi = (int)px, pyi = (int)py;
        int rInt = std::max(0, (int)radius);

        if (rInt == 0) {
            if (pxi >= 0 && pxi < W && pyi >= 0 && pyi < H)
                data[pyi * W + pxi] = std::min(255.0f, data[pyi * W + pxi] + (float)eraseVal);
        } else {
            int yLo = std::max(0, pyi - rInt), yHi = std::min(H, pyi + rInt + 1);
            int xLo = std::max(0, pxi - rInt), xHi = std::min(W, pxi + rInt + 1);
            for (int ey = yLo; ey < yHi; ++ey)
                for (int ex = xLo; ex < xHi; ++ex)
                    if (std::hypot(ex - px, ey - py) <= radius)
                        data[ey * W + ex] = std::min(255.0f, data[ey * W + ex] + (float)eraseVal);
        }
    }
}

// ---------------------------------------------------------------------------
// Erase a rectangular region (used by Squares and Shapes)
// ---------------------------------------------------------------------------
inline void eraseRect(cv::Mat& img,
                      int x0, int y0, int x1, int y1,
                      double eraseVal, int W, int H)
{
    x0 = std::max(0, x0); y0 = std::max(0, y0);
    x1 = std::min(W - 1, x1); y1 = std::min(H - 1, y1);
    if (x1 < x0 || y1 < y0) return;
    float* data = img.ptr<float>(0);
    for (int ey = y0; ey <= y1; ++ey)
        for (int ex = x0; ex <= x1; ++ex)
            data[ey * W + ex] = std::min(255.0f, data[ey * W + ex] + (float)eraseVal);
}

// ---------------------------------------------------------------------------
// Build edge/sobel/direction maps
// ---------------------------------------------------------------------------
struct EdgeMaps {
    cv::Mat edge;       // float [0..1]
    cv::Mat sobel;      // float [0..1]
    cv::Mat direction;  // float atan2 radians
};

inline EdgeMaps buildEdgeMaps(const cv::Mat& workImg,
                               double edgePower, double sobelPower, double directionality)
{
    EdgeMaps m;
    if (edgePower > 0) {
        cv::Mat edges;
        cv::Canny(workImg, edges, 50, 150);
        edges.convertTo(m.edge, CV_32F, 1.0 / 255.0);
    }
    if (sobelPower > 0 || directionality > 0) {
        cv::Mat sx, sy;
        cv::Sobel(workImg, sx, CV_64F, 1, 0, 3);
        cv::Sobel(workImg, sy, CV_64F, 0, 1, 3);
        if (sobelPower > 0) {
            cv::Mat mag;
            cv::magnitude(sx, sy, mag);
            double mn, mx;
            cv::minMaxLoc(mag, &mn, &mx);
            mag.convertTo(m.sobel, CV_32F, 1.0 / (mx + 1e-10));
        }
        if (directionality > 0) {
            int H = workImg.rows, W = workImg.cols;
            m.direction.create(H, W, CV_32F);
            for (int y = 0; y < H; ++y)
                for (int x = 0; x < W; ++x)
                    m.direction.at<float>(y, x) =
                        (float)std::atan2(sy.at<double>(y, x), sx.at<double>(y, x));
        }
    }
    return m;
}

// ---------------------------------------------------------------------------
// Compute per-iteration density progress
// ---------------------------------------------------------------------------
inline double densityProgress(const cv::Mat& lightened,
                               double initialBrightness, double targetBrightness) {
    cv::Scalar m = cv::mean(lightened);
    double cur = m[0];
    double prog = (cur - initialBrightness) / std::max(1.0, targetBrightness - initialBrightness);
    return std::clamp(prog, 0.0, 1.0);
}

} // namespace sketch_utils


// Original File: ../cpp/pfm\sketch_abstract_pfm.h

class SketchAbstractPFM : public PathFindingModule {
    public:
    explicit SketchAbstractPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Abstract"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_catmull_roms_pfm.h

class SketchCatmullRomsPFM : public PathFindingModule {
    public:
    explicit SketchCatmullRomsPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Catmull-Roms"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_cubic_beziers2_pfm.h

class SketchCubicBeziers2PFM : public PathFindingModule {
    public:
    explicit SketchCubicBeziers2PFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Cubic Beziers 2"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_cubic_beziers_pfm.h

class SketchCubicBeziersPFM : public PathFindingModule {
    public:
    explicit SketchCubicBeziersPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Cubic Beziers"; }
    QString description() const override { return "Transforms an image into Cubic Bezier curves."; }
    QString category()    const override { return "Sketch"; }
    bool    isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_curves.h

enum class CurveOutputMode { CatmullRom, QuadBezier, CubicBezier };

QVector<PFMSetting> makeSketchCommonSettings();
QVector<DrawingGeometry> runSketchLoop(
    PathFindingModule* pfm,
    const cv::Mat& image,
    CurveOutputMode mode,
    int curveSmooth = 10,
    double curveAlpha = 0.5);


// Original File: ../cpp/pfm\sketch_curves_pfm.h

class SketchCurvesPFM : public PathFindingModule {
    public:
    explicit SketchCurvesPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Curves"; }
    QString description() const override { return "Transforms an image into smooth curves using brightness data."; }
    QString category()    const override { return "Sketch"; }
    bool    isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_delaunay_pfm.h

class SketchDelaunayPFM : public PathFindingModule {
    public:
    explicit SketchDelaunayPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Delaunay"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_flow_fields_pfm.h

class SketchFlowFieldsPFM : public PathFindingModule {
    public:
    explicit SketchFlowFieldsPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Flow Fields"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_lines.h

class SketchLinesPFM : public PathFindingModule {
    public:
    explicit SketchLinesPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Lines"; }
    QString description() const override { return "Transforms an image into lines using brightness data."; }
    QString category()    const override { return "Sketch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;

private:
    void eraseLine(cv::Mat& img, double x0, double y0, double x1, double y1,
                   double eraseMin, double eraseMax,
                   double radiusMin, double radiusMax,
                   double tone, int w, int h);
};


// Original File: ../cpp/pfm\sketch_quad_beziers2_pfm.h

class SketchQuadBeziers2PFM : public SketchCubicBeziers2PFM {
    public:
    explicit SketchQuadBeziers2PFM(QObject* parent = nullptr) : SketchCubicBeziers2PFM(parent) {}
    QString name() const override { return "Sketch Quad Beziers 2"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_quad_beziers_pfm.h

class SketchQuadBeziersPFM : public PathFindingModule {
    public:
    explicit SketchQuadBeziersPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Quad Beziers"; }
    QString description() const override { return "Transforms an image into Quadratic Bezier curves."; }
    QString category()    const override { return "Sketch"; }
    bool    isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_radial_pfm.h

class SketchRadialPFM : public PathFindingModule {
    public:
    explicit SketchRadialPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Radial"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_scribble_pfm.h

class SketchScribblePFM : public PathFindingModule {
    public:
    explicit SketchScribblePFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Scribble"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_shapes.h

class SketchShapesPFM : public PathFindingModule {
    public:
    explicit SketchShapesPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Shapes"; }
    QString description() const override { return "Transforms an image into assorted shapes using brightness data."; }
    QString category()    const override { return "Sketch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_sobel_edges_pfm.h

class SketchSobelEdgesPFM : public PathFindingModule {
    public:
    explicit SketchSobelEdgesPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Sobel Edges"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_spirals.h

class SketchSpiralsPFM : public PathFindingModule {
    public:
    explicit SketchSpiralsPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Spirals"; }
    QString description() const override { return "Draws a continuous spiral, tightening in dark areas."; }
    QString category()    const override { return "Sketch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_squares.h

class SketchSquaresPFM : public PathFindingModule {
    public:
    explicit SketchSquaresPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Squares"; }
    QString description() const override { return "Transforms an image into squares using brightness data."; }
    QString category()    const override { return "Sketch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_superformula_pfm.h

class SketchSuperformulaPFM : public PathFindingModule {
    public:
    explicit SketchSuperformulaPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Superformula"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_sweeping_curves_pfm.h

class SketchSweepingCurvesPFM : public SketchCubicBeziers2PFM {
    public:
    explicit SketchSweepingCurvesPFM(QObject* parent = nullptr);
    QString name() const override;
};


// Original File: ../cpp/pfm\sketch_voronoi_pfm.h

class SketchVoronoiPFM : public PathFindingModule {
    public:
    explicit SketchVoronoiPFM(QObject* parent = nullptr);
    QString name() const override { return "Sketch Voronoi"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\sketch_waves.h

class SketchWavesPFM : public PathFindingModule {
    public:
    explicit SketchWavesPFM(QObject* parent = nullptr);
    QString name()        const override { return "Sketch Waves"; }
    QString description() const override { return "Draws sine waves varying amplitude by brightness."; }
    QString category()    const override { return "Sketch"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\spiral_circular_scribbles_pfm.h

class SpiralCircularScribblesPFM : public PathFindingModule {
    public:
    explicit SpiralCircularScribblesPFM(QObject* parent = nullptr);
    QString name() const override { return "Spiral Circular Scribbles"; }
    QString category() const override { return "Spiral"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\spiral_sawtooth_pfm.h

class SpiralSawtoothPFM : public PathFindingModule {
    public:
    explicit SpiralSawtoothPFM(QObject* parent = nullptr);
    QString name() const override { return "Spiral Sawtooth"; }
    QString category() const override { return "Spiral"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\stipple_chaos_pfm.h

class StippleChaosPFM : public BaseStipplePFM {
    public: explicit StippleChaosPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Chaos";} protected: Path generateShape(float cx, float cy, float r) override; };


// Original File: ../cpp/pfm\stipple_circles_pfm.h

class StippleCirclesPFM : public BaseStipplePFM {
    public: explicit StippleCirclesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Circles";} protected: Path generateShape(float cx, float cy, float r) override; };


// Original File: ../cpp/pfm\stipple_crosses_pfm.h

class StippleCrossesPFM : public BaseStipplePFM {
    public: explicit StippleCrossesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Crosses";} protected: Path generateShape(float cx, float cy, float r) override; };


// Original File: ../cpp/pfm\stipple_dots.h

class StippleDotsPFM : public PathFindingModule {
    public:
    explicit StippleDotsPFM(QObject* parent = nullptr);
    QString name()        const override { return "Stipple Dots"; }
    QString description() const override { return "Pointillism based on image brightness."; }
    QString category()    const override { return "Stipple"; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\stipple_hexagons_pfm.h

class StippleHexagonsPFM : public BaseStipplePFM {
    public: explicit StippleHexagonsPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Hexagons";} protected: Path generateShape(float cx, float cy, float r) override; };


// Original File: ../cpp/pfm\stipple_layers_pfm.h

class StippleLayersPFM : public PathFindingModule {
    public:
    explicit StippleLayersPFM(QObject* parent = nullptr);
    QString name()     const override { return "Stipple Layers"; }
    QString category() const override { return "Composite"; }
    bool isPremium()   const override { return true; }
protected:
    QVector<PFMSetting>      defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\stipple_lines_pfm.h

class StippleLinesPFM : public BaseStipplePFM {
    public: explicit StippleLinesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Lines";} protected: Path generateShape(float cx, float cy, float r) override; };


// Original File: ../cpp/pfm\stipple_squares_pfm.h

class StippleSquaresPFM : public BaseStipplePFM {
    public: explicit StippleSquaresPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Squares";} protected: Path generateShape(float cx, float cy, float r) override; };


// Original File: ../cpp/pfm\stipple_stars_pfm.h

class StippleStarsPFM : public BaseStipplePFM {
    public: explicit StippleStarsPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Stars";} protected: Path generateShape(float cx, float cy, float r) override; };


// Original File: ../cpp/pfm\stipple_triangles_pfm.h

class StippleTrianglesPFM : public BaseStipplePFM {
    public: explicit StippleTrianglesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Triangles";} protected: Path generateShape(float cx, float cy, float r) override; };


// Original File: ../cpp/pfm\stipple_variable_circles_pfm.h

class StippleVariableCirclesPFM : public BaseStipplePFM {
    public: explicit StippleVariableCirclesPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Variable Circles";} protected: Path generateShape(float cx, float cy, float r) override; };


// Original File: ../cpp/pfm\stipple_variable_squares_pfm.h

class StippleVariableSquaresPFM : public BaseStipplePFM {
    public: explicit StippleVariableSquaresPFM(QObject* p=nullptr):BaseStipplePFM(p){} QString name() const override{return "Stipple Variable Squares";} protected: Path generateShape(float cx, float cy, float r) override; };


// Original File: ../cpp/pfm\streamlines_edge_field_pfm.h

class StreamlinesEdgeFieldPFM : public BaseStreamlinePFM {
    public:
    explicit StreamlinesEdgeFieldPFM(QObject* parent = nullptr) : BaseStreamlinePFM(parent) {}
    QString name() const override { return "Streamlines Edge Field"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\streamlines_flow_field_pfm.h

class StreamlinesFlowFieldPFM : public BaseStreamlinePFM {
    public:
    explicit StreamlinesFlowFieldPFM(QObject* parent = nullptr) : BaseStreamlinePFM(parent) {}
    QString name() const override { return "Streamlines Flow Field"; }
protected:
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\streamlines_superformula_pfm.h

class StreamlinesSuperformulaPFM : public BaseStreamlinePFM {
    public:
    explicit StreamlinesSuperformulaPFM(QObject* parent = nullptr) : BaseStreamlinePFM(parent) {}
    QString name() const override { return "Streamlines Superformula"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\tspmstpfm.h

class TSPMSTPFM : public PathFindingModule {
    public:
    explicit TSPMSTPFM(QObject* parent = nullptr);
    QString name() const override { return "Minimum Spanning Tree"; }
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\tsp_classic_pfm.h

class TSPClassicPFM : public BaseTSPPFM {
    public:
    explicit TSPClassicPFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Classic"; }
protected:
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\tsp_outline_pfm.h

class TSPOutlinePFM : public BaseTSPPFM {
    public:
    explicit TSPOutlinePFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Outline"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\tsp_shading_pfm.h

class TSPShadingPFM : public BaseTSPPFM {
    public:
    explicit TSPShadingPFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Shading"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\tsp_stipple_pfm.h

class TSPStipplePFM : public BaseTSPPFM {
    public:
    explicit TSPStipplePFM(QObject* parent = nullptr) : BaseTSPPFM(parent) {}
    QString name() const override { return "TSP Stipple"; }
protected:
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\tsp_voronoi_pfm.h

class TSPVoronoiPFM : public BaseTSPPFM {
    public:
    explicit TSPVoronoiPFM(QObject* parent = nullptr);
    QString name() const override { return "TSP Voronoi"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    std::vector<double> getProbabilities(const cv::Mat& image) override;
};


// Original File: ../cpp/pfm\voronoi_circles_pfm.h

class VoronoiCirclesPFM : public BaseVoronoiExtraPFM {
    public: explicit VoronoiCirclesPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Circles";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\voronoi_dashes_pfm.h

class VoronoiDashesPFM : public BaseVoronoiExtraPFM {
    public: explicit VoronoiDashesPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Dashes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\voronoi_diagram_pfm.h

class VoronoiDiagramPFM : public BaseVoronoiExtraPFM {
    public: explicit VoronoiDiagramPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Diagram";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\voronoi_letters_pfm.h

class VoronoiLettersPFM : public _LettersBasePFM {
    public:
    explicit VoronoiLettersPFM(QObject* parent = nullptr) : _LettersBasePFM(parent) {}
    QString name() const override { return "Voronoi Letters"; }
protected:
    int getLloydIters() const override;
};


// Original File: ../cpp/pfm\voronoi_shapes_pfm.h

class VoronoiShapesPFM : public BaseVoronoiExtraPFM {
    public: explicit VoronoiShapesPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Shapes";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\voronoi_stippling_pfm.h

class VoronoiStipplingPFM : public BaseVoronoiExtraPFM {
    public: explicit VoronoiStipplingPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Stippling";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\voronoi_tree_pfm.h

class VoronoiTreePFM : public BaseVoronoiExtraPFM {
    public: explicit VoronoiTreePFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Tree";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\voronoi_triangulation_pfm.h

class VoronoiTriangulationPFM : public BaseVoronoiExtraPFM {
    public: explicit VoronoiTriangulationPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi Triangulation";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\voronoi_tsppfm.h

class VoronoiTSPPFM : public BaseVoronoiExtraPFM {
    public: explicit VoronoiTSPPFM(QObject* p=nullptr):BaseVoronoiExtraPFM(p){} QString name() const override{return "Voronoi TSP";} protected: QVector<DrawingGeometry> _process(const cv::Mat& image) override; };


// Original File: ../cpp/pfm\_letters_base_pfm.h

class _LettersBasePFM : public PathFindingModule {
    public:
    explicit _LettersBasePFM(QObject* parent = nullptr);
    bool isPremium() const override { return true; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    virtual int getLloydIters() const = 0;
    QVector<DrawingGeometry> _process(const cv::Mat& image) override;
};


// Original File: ../cpp/filters\artistic_distort_filters.h

// ---------------------------------------------------------------------------
// Artistic Filters
// ---------------------------------------------------------------------------
class PosterizeFilter : public ImageFilter {
    public:
    explicit PosterizeFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Posterize"; }
    QString category() const override { return "Artistic"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class EdgePreserveFilter : public ImageFilter {
    public:
    explicit EdgePreserveFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Edge Preserve Smooth"; }
    QString category() const override { return "Artistic"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class StylizationFilter : public ImageFilter {
    public:
    explicit StylizationFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Stylization"; }
    QString category() const override { return "Artistic"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class OilPaintingFilter : public ImageFilter {
    public:
    explicit OilPaintingFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Oil Painting"; }
    QString category() const override { return "Artistic"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DetailEnhanceFilter : public ImageFilter {
    public:
    explicit DetailEnhanceFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Detail Enhance"; }
    QString category() const override { return "Artistic"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class PencilSketchFilter : public ImageFilter {
    public:
    explicit PencilSketchFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Pencil Sketch"; }
    QString category() const override { return "Artistic"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class EmbossFilter : public ImageFilter {
    public:
    explicit EmbossFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Emboss"; }
    QString category() const override { return "Stylize"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class QuantizeFilter : public ImageFilter {
    public:
    explicit QuantizeFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Quantize (8 Colors)"; }
    QString category() const override { return "Artistic"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

// ---------------------------------------------------------------------------
// Distort Filters
// ---------------------------------------------------------------------------
class VignetteFilter : public ImageFilter {
    public:
    explicit VignetteFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Vignette"; }
    QString category() const override { return "Distort"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class PixelateFilter : public ImageFilter {
    public:
    explicit PixelateFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Pixelate"; }
    QString category() const override { return "Distort"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class WaveFilter : public ImageFilter {
    public:
    explicit WaveFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Wave Distortion"; }
    QString category() const override { return "Distort"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};


// Original File: ../cpp/filters\base_filter.h
/**
 * base_filter.h — Base class for all image pre-processing filters.
 * Port of app/filters/base_filter.py
 */


 // For PFMSetting and SettingType

class ImageFilter : public QObject {
    public:
    explicit ImageFilter(QObject* parent = nullptr);
    virtual ~ImageFilter() = default;

    virtual QString name() const = 0;
    virtual QString category() const { return "General"; }
    virtual QString description() const { return ""; }

    QVector<PFMSetting> settingsList() const;
    const QMap<QString, PFMSetting>& settingsMap() const { return m_settings; }
    
    QVariant get(const QString& key) const;
    void set(const QString& key, const QVariant& value);
    void resetAll();

    // The main processing function. Takes an image, returns a filtered image.
    virtual cv::Mat process(const cv::Mat& image) = 0;

protected:
    virtual QVector<PFMSetting> defineSettings() const { return {}; }
    
    void initSettings();
    QMap<QString, PFMSetting> m_settings;
};


// Original File: ../cpp/filters\basic_filters.h
/**
 * basic_filters.h — Basic Image Filters (Brightness, Contrast, Threshold, etc.)
 */

class BrightnessFilter : public ImageFilter {
    public:
    explicit BrightnessFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Brightness"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ContrastFilter : public ImageFilter {
    public:
    explicit ContrastFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Contrast"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class InvertFilter : public ImageFilter {
    public:
    explicit InvertFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Invert"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class ThresholdFilter : public ImageFilter {
    public:
    explicit ThresholdFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Threshold"; }
    QString category() const override { return "Artistic"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class UnsharpMaskFilter : public ImageFilter {
    public:
    explicit UnsharpMaskFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Unsharp Mask"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};


// Original File: ../cpp/filters\color_filters.h
/**
 * color_filters.h — Color and Adjustment Filters
 */

class GrayscaleFilter : public ImageFilter {
    public:
    explicit GrayscaleFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Grayscale"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class DesaturateFilter : public ImageFilter {
    public:
    explicit DesaturateFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Desaturate"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class SaturationFilter : public ImageFilter {
    public:
    explicit SaturationFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Saturation"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class HueFilter : public ImageFilter {
    public:
    explicit HueFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Hue Shift"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class GammaFilter : public ImageFilter {
    public:
    explicit GammaFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Gamma Correction"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ExposureFilter : public ImageFilter {
    public:
    explicit ExposureFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Exposure"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SepiaFilter : public ImageFilter {
    public:
    explicit SepiaFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Sepia"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class CLAHEFilter : public ImageFilter {
    public:
    explicit CLAHEFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "CLAHE"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class TemperatureFilter : public ImageFilter {
    public:
    explicit TemperatureFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Temperature"; }
    QString category() const override { return "Color"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};


// Original File: ../cpp/filters\edge_blur_filters.h
/**
 * edge_blur_filters.h — Edge Detection and Blur Filters
 */

// ---------------------------------------------------------------------------
// Edge Filters
// ---------------------------------------------------------------------------
class CannyFilter : public ImageFilter {
    public:
    explicit CannyFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Canny Edge Detection"; }
    QString category() const override { return "Edge Detection"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SobelFilter : public ImageFilter {
    public:
    explicit SobelFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Sobel Edge Detection"; }
    QString category() const override { return "Edge Detection"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class LaplacianFilter : public ImageFilter {
    public:
    explicit LaplacianFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Laplacian Edge Detection"; }
    QString category() const override { return "Edge Detection"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class PrewittFilter : public ImageFilter {
    public:
    explicit PrewittFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Prewitt Edge"; }
    QString category() const override { return "Edge Detection"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class ScharrFilter : public ImageFilter {
    public:
    explicit ScharrFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Scharr Edge"; }
    QString category() const override { return "Edge Detection"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class DoGFilter : public ImageFilter {
    public:
    explicit DoGFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Difference of Gaussians (DoG)"; }
    QString category() const override { return "Edge Detection"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class RidgeDetectionFilter : public ImageFilter {
    public:
    explicit RidgeDetectionFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Ridge Detection (Hessian)"; }
    QString category() const override { return "Edge Detection"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class HighPassFilter : public ImageFilter {
    public:
    explicit HighPassFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "High Pass"; }
    QString category() const override { return "Edge Detection"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

// ---------------------------------------------------------------------------
// Blur Filters
// ---------------------------------------------------------------------------
class GaussianBlurFilter : public ImageFilter {
    public:
    explicit GaussianBlurFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Gaussian Blur"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MedianBlurFilter : public ImageFilter {
    public:
    explicit MedianBlurFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Median Blur"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class BoxBlurFilter : public ImageFilter {
    public:
    explicit BoxBlurFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Box Blur"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MotionBlurFilter : public ImageFilter {
    public:
    explicit MotionBlurFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Motion Blur"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class BilateralFilter : public ImageFilter {
    public:
    explicit BilateralFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Bilateral Filter"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class LowPassFilter : public ImageFilter {
    public:
    explicit LowPassFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Low Pass"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class SharpenMoreFilter : public ImageFilter {
    public:
    explicit SharpenMoreFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Sharpen More"; }
    QString category() const override { return "Blur/Sharpen"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};


// Original File: ../cpp/filters\morph_filters.h

// ---------------------------------------------------------------------------
// Morphological Filters
// ---------------------------------------------------------------------------
class DilationFilter : public ImageFilter {
    public:
    explicit DilationFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Dilation"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ErosionFilter : public ImageFilter {
    public:
    explicit ErosionFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Erosion"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class OpeningFilter : public ImageFilter {
    public:
    explicit OpeningFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Opening"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ClosingFilter : public ImageFilter {
    public:
    explicit ClosingFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Closing"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class MorphGradientFilter : public ImageFilter {
    public:
    explicit MorphGradientFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Morphological Gradient"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class TopHatFilter : public ImageFilter {
    public:
    explicit TopHatFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Top Hat"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class BlackHatFilter : public ImageFilter {
    public:
    explicit BlackHatFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Black Hat"; }
    QString category() const override { return "Morphological"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DilateCrossFilter : public ImageFilter {
    public:
    explicit DilateCrossFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Dilate Cross"; }
    QString category() const override { return "Morphological"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class ErodeCrossFilter : public ImageFilter {
    public:
    explicit ErodeCrossFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Erode Cross"; }
    QString category() const override { return "Morphological"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class DilateEllipseFilter : public ImageFilter {
    public:
    explicit DilateEllipseFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Dilate Ellipse"; }
    QString category() const override { return "Morphological"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class ErodeEllipseFilter : public ImageFilter {
    public:
    explicit ErodeEllipseFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Erode Ellipse"; }
    QString category() const override { return "Morphological"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};


// Original File: ../cpp/filters\noise_filters.h
/**
 * noise_filters.h — Noise and Denoise Filters
 */

class GaussianNoiseFilter : public ImageFilter {
    public:
    explicit GaussianNoiseFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Add Gaussian Noise"; }
    QString category() const override { return "Noise"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class SaltAndPepperFilter : public ImageFilter {
    public:
    explicit SaltAndPepperFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Salt & Pepper Noise"; }
    QString category() const override { return "Noise"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class DenoiseFilter : public ImageFilter {
    public:
    explicit DenoiseFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Denoise (NL Means)"; }
    QString category() const override { return "Noise"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class GaussianNoise2Filter : public ImageFilter {
    public:
    explicit GaussianNoise2Filter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Gaussian Noise 2"; }
    QString category() const override { return "Noise"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class SpeckleNoiseFilter : public ImageFilter {
    public:
    explicit SpeckleNoiseFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Speckle Noise"; }
    QString category() const override { return "Noise"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};



// Original File: ../cpp/filters\threshold_extra_filters.h

// ---------------------------------------------------------------------------
// Threshold Filters
// ---------------------------------------------------------------------------
class OtsuThresholdFilter : public ImageFilter {
    public:
    explicit OtsuThresholdFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Otsu Threshold"; }
    QString category() const override { return "Threshold"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class AdaptiveThresholdFilter : public ImageFilter {
    public:
    explicit AdaptiveThresholdFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Adaptive Threshold"; }
    QString category() const override { return "Threshold"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class TruncateThresholdFilter : public ImageFilter {
    public:
    explicit TruncateThresholdFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Truncate Threshold"; }
    QString category() const override { return "Threshold"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

class ToZeroThresholdFilter : public ImageFilter {
    public:
    explicit ToZeroThresholdFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "To Zero Threshold"; }
    QString category() const override { return "Threshold"; }
protected:
    QVector<PFMSetting> defineSettings() const override;
    cv::Mat process(const cv::Mat& image) override;
};

// ---------------------------------------------------------------------------
// Extra/Color Filters
// ---------------------------------------------------------------------------
class EqualizeHistFilter : public ImageFilter {
    public:
    explicit EqualizeHistFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Equalize Histogram"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class AutoContrastFilter : public ImageFilter {
    public:
    explicit AutoContrastFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Auto Contrast"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class AutoColorFilter : public ImageFilter {
    public:
    explicit AutoColorFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Auto Color"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class ColorizeFilter : public ImageFilter {
    public:
    explicit ColorizeFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Colorize (Tint)"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};

class InvertHueFilter : public ImageFilter {
    public:
    explicit InvertHueFilter(QObject* parent = nullptr) : ImageFilter(parent) { initSettings(); }
    QString name() const override { return "Invert Hue"; }
    QString category() const override { return "Color"; }
protected:
    cv::Mat process(const cv::Mat& image) override;
};


// Original File: ../cpp/export\gcode_exporter.h
/**
 * gcode_exporter.h — Export path geometries to standard G-code for CNC/plotters.
 */


struct GCodeSettings {
    QString startGcode = "G21\nG90";
    QString endGcode = "M30";
    QString penDownCmd = "M3 S90";
    QString penUpCmd = "M5";
    QString startLayerCmd = "";
    QString endLayerCmd = "";
    double xOffset = 0.0;
    double yOffset = 0.0;
    bool centerZero = false;
    double minPathLength = 0.5; // mm
    bool optimizePaths = true;
};

class GCodeExporter {
public:
    static bool exportGCode(const QString& filepath,
                            const QVector<DrawingGeometry>& geometries,
                            const DrawingAreaConfig& drawingArea,
                            int imageWidth,
                            int imageHeight,
                            const GCodeSettings& settings = GCodeSettings());
};


// Original File: ../cpp/export\hpgl_exporter.h
/**
 * hpgl_exporter.h — Export path geometries to HPGL format for legacy pen plotters.
 */


struct HPGLSettings {
    int xMax = 16158;
    int yMax = 11040;
    bool xMirror = false;
    bool yMirror = false;
    int penNumber = 1;
    int penVelocity = 10;
    double curveFlatness = 0.1;
    double minPathLength = 0.5; // mm
    bool optimizePaths = true;
};

class HPGLExporter {
public:
    static bool exportHPGL(const QString& filepath,
                           const QVector<DrawingGeometry>& geometries,
                           const DrawingAreaConfig& drawingArea,
                           int imageWidth,
                           int imageHeight,
                           const HPGLSettings& settings = HPGLSettings());
};


// Original File: ../cpp/export\path_optimizer.h
/**
 * path_optimizer.h — Path filtering and optimization (TSP/Nearest Neighbor).
 */


class PathOptimizer {
public:
    static QMap<int, QVector<Path>> preparePenPaths(
        const QVector<DrawingGeometry>& geometries,
        double minLength = 0.0,
        bool optimize = true);
};


// Original File: ../cpp/export\pdf_exporter.h
/**
 * pdf_exporter.h — Export paths to a vector PDF using QPdfWriter.
 */


class PDFExporter {
public:
    static bool exportPDF(const QString& filepath,
                          const QVector<DrawingGeometry>& geometries,
                          const DrawingAreaConfig& drawingArea,
                          int imageWidth,
                          int imageHeight,
                          const QVector<QColor>& penColors,
                          double penWidthMm = 0.5);
};


// Original File: ../cpp/export\svg_exporter.h
/**
 * svg_exporter.h — Export path geometries to an SVG file.
 */


class SVGExporter {
public:
    static bool exportSVG(const QString& filepath,
                          const QVector<DrawingGeometry>& geometries,
                          const DrawingAreaConfig& drawingArea,
                          int imageWidth,
                          int imageHeight,
                          const QVector<QColor>& penColors,
                          double penWidthMm = 0.5);
};



// --- SOURCES ---

// Original File: ../cpp/core\color_separation.cpp

std::vector<cv::Mat> splitGrayscale(const cv::Mat& bgrImage) {
    if (bgrImage.channels() == 1) {
        return {bgrImage.clone()};
    }
    cv::Mat gray;
    if (bgrImage.channels() == 4) {
        cv::cvtColor(bgrImage, gray, cv::COLOR_BGRA2GRAY);
    } else {
        cv::cvtColor(bgrImage, gray, cv::COLOR_BGR2GRAY);
    }
    return {gray};
}

std::vector<cv::Mat> splitRGB(const cv::Mat& bgrImage) {
    if (bgrImage.channels() == 1) {
        return {bgrImage.clone(), bgrImage.clone(), bgrImage.clone()};
    }
    
    cv::Mat bgr;
    if (bgrImage.channels() == 4) {
        cv::cvtColor(bgrImage, bgr, cv::COLOR_BGRA2BGR);
    } else {
        bgr = bgrImage;
    }
    
    std::vector<cv::Mat> channels;
    cv::split(bgr, channels);
    
    cv::Mat rDensity, gDensity, bDensity;
    cv::bitwise_not(channels[2], rDensity);
    cv::bitwise_not(channels[1], gDensity);
    cv::bitwise_not(channels[0], bDensity);
    
    return {rDensity, gDensity, bDensity};
}

std::vector<cv::Mat> splitCMYK(const cv::Mat& bgrImage) {
    if (bgrImage.channels() == 1) {
        return {bgrImage.clone(), bgrImage.clone(), bgrImage.clone(), bgrImage.clone()};
    }
    
    cv::Mat bgr;
    if (bgrImage.channels() == 4) {
        cv::cvtColor(bgrImage, bgr, cv::COLOR_BGRA2BGR);
    } else {
        bgr = bgrImage;
    }
    
    int rows = bgr.rows;
    int cols = bgr.cols;
    
    cv::Mat cDensity(rows, cols, CV_8UC1);
    cv::Mat mDensity(rows, cols, CV_8UC1);
    cv::Mat yDensity(rows, cols, CV_8UC1);
    cv::Mat kDensity(rows, cols, CV_8UC1);
    
    for (int r = 0; r < rows; ++r) {
        const uchar* ptr = bgr.ptr<uchar>(r);
        uchar* cPtr = cDensity.ptr<uchar>(r);
        uchar* mPtr = mDensity.ptr<uchar>(r);
        uchar* yPtr = yDensity.ptr<uchar>(r);
        uchar* kPtr = kDensity.ptr<uchar>(r);
        
        for (int c = 0; c < cols; ++c) {
            float blue = ptr[c * 3 + 0] / 255.0f;
            float green = ptr[c * 3 + 1] / 255.0f;
            float red = ptr[c * 3 + 2] / 255.0f;
            
            float k = 1.0f - std::max({red, green, blue});
            float cyan = 0.0f;
            float magenta = 0.0f;
            float yellow = 0.0f;
            
            if (k < 1.0f) {
                cyan = (1.0f - red - k) / (1.0f - k);
                magenta = (1.0f - green - k) / (1.0f - k);
                yellow = (1.0f - blue - k) / (1.0f - k);
            }
            
            cPtr[c] = static_cast<uchar>(cyan * 255.0f);
            mPtr[c] = static_cast<uchar>(magenta * 255.0f);
            yPtr[c] = static_cast<uchar>(yellow * 255.0f);
            kPtr[c] = static_cast<uchar>(k * 255.0f);
        }
    }
    
    return {cDensity, mDensity, yDensity, kDensity};
}


// Original File: ../cpp/core\coordinates.cpp

void compute_scaling(double srcW, double srcH, double dstW, double dstH, ScalingMode mode,
                     double& sx, double& sy, double& ox, double& oy) {
    if (mode == ScalingMode::Stretch) {
        sx = (srcW > 0) ? dstW / srcW : 1.0;
        sy = (srcH > 0) ? dstH / srcH : 1.0;
        ox = 0.0;
        oy = 0.0;
        return;
    }

    double srcAspect = (srcH > 0) ? srcW / srcH : 1.0;
    double dstAspect = (dstH > 0) ? dstW / dstH : 1.0;

    if (mode == ScalingMode::Fit) {
        if (srcAspect > dstAspect) {
            sx = sy = (srcW > 0) ? dstW / srcW : 1.0;
            ox = 0.0;
            oy = (dstH - srcH * sy) / 2.0;
        } else {
            sx = sy = (srcH > 0) ? dstH / srcH : 1.0;
            ox = (dstW - srcW * sx) / 2.0;
            oy = 0.0;
        }
    } else if (mode == ScalingMode::Crop) {
        if (srcAspect > dstAspect) {
            sx = sy = (srcH > 0) ? dstH / srcH : 1.0;
            ox = (dstW - srcW * sx) / 2.0;
            oy = 0.0;
        } else {
            sx = sy = (srcW > 0) ? dstW / srcW : 1.0;
            ox = 0.0;
            oy = (dstH - srcH * sy) / 2.0;
        }
    } else {
        sx = sy = 1.0;
        ox = oy = 0.0;
    }
}

Path pixel_path_to_mm(const Path& path, double imgW, double imgH, const DrawingAreaConfig& config) {
    if (path.empty() || imgW <= 0 || imgH <= 0) return path;

    double dw = config.usableWidth();
    double dh = config.usableHeight();
    if (dw <= 0 || dh <= 0) return path;

    double sx, sy, ox, oy;
    compute_scaling(imgW, imgH, dw, dh, config.scaling_mode, sx, sy, ox, oy);

    double pl = config.padding_left_mm;
    double pt = config.padding_top_mm;

    Path scaled;
    scaled.reserve(path.size());
    for (const auto& pt_orig : path) {
        double nx = pt_orig.first * sx + ox + pl;
        double ny = pt_orig.second * sy + oy + pt;
        scaled.push_back({nx, ny});
    }

    return scaled;
}


// Original File: ../cpp/core\mask_manager.cpp

bool MaskManager::load(const QString& filepath) {
    m_mask = cv::imread(filepath.toStdString(), cv::IMREAD_GRAYSCALE);
    if (m_mask.empty()) {
        m_enabled = false;
        m_maskPath.clear();
        return false;
    }
    m_maskPath = filepath;
    m_enabled = true;
    return true;
}

void MaskManager::clear() {
    m_mask = cv::Mat();
    m_maskPath.clear();
    m_enabled = false;
}

cv::Mat MaskManager::applyMask(const cv::Mat& image) const {
    if (!m_enabled || m_mask.empty() || image.empty()) {
        return image;
    }

    cv::Mat m;
    if (m_mask.size() != image.size()) {
        cv::resize(m_mask, m, image.size(), 0, 0, cv::INTER_LINEAR);
    } else {
        m = m_mask.clone();
    }

    // Convert mask to 0.0 - 1.0
    cv::Mat mFloat;
    m.convertTo(mFloat, CV_32F, 1.0 / 255.0);

    // Apply mask: out = img * m + 255 * (1 - m)
    cv::Mat imgFloat;
    image.convertTo(imgFloat, CV_32F);

    cv::Mat outFloat = imgFloat.mul(mFloat) + 255.0 * (1.0 - mFloat);
    
    cv::Mat result;
    outFloat.convertTo(result, image.type());
    return result;
}


// Original File: ../cpp/core\module_registry.cpp

QVector<std::shared_ptr<PathFindingModule>> ModuleRegistry::createAllPFMs() {
    QVector<std::shared_ptr<PathFindingModule>> pfms;
    pfms.append(std::make_shared<SketchLinesPFM>());
    pfms.append(std::make_shared<SketchSquaresPFM>());
    pfms.append(std::make_shared<SketchCurvesPFM>());
    pfms.append(std::make_shared<SketchShapesPFM>());
    pfms.append(std::make_shared<SketchQuadBeziersPFM>());
    pfms.append(std::make_shared<SketchCubicBeziersPFM>());
    pfms.append(std::make_shared<SketchWavesPFM>());
    pfms.append(std::make_shared<SketchSpiralsPFM>());
    pfms.append(std::make_shared<SketchSuperformulaPFM>());
    pfms.append(std::make_shared<SketchCubicBeziers2PFM>());
    pfms.append(std::make_shared<SketchQuadBeziers2PFM>());
    pfms.append(std::make_shared<SketchSweepingCurvesPFM>());
    pfms.append(std::make_shared<SketchFlowFieldsPFM>());
    pfms.append(std::make_shared<SketchVoronoiPFM>());
    pfms.append(std::make_shared<SketchDelaunayPFM>());
    pfms.append(std::make_shared<SketchRadialPFM>());
    pfms.append(std::make_shared<SketchScribblePFM>());
    pfms.append(std::make_shared<SketchAbstractPFM>());
    pfms.append(std::make_shared<SketchCatmullRomsPFM>());
    pfms.append(std::make_shared<SketchSobelEdgesPFM>());
    pfms.append(std::make_shared<HatchLinesPFM>());
    pfms.append(std::make_shared<HatchHorizontalPFM>());
    pfms.append(std::make_shared<HatchVerticalPFM>());
    pfms.append(std::make_shared<HatchDiagonal1PFM>());
    pfms.append(std::make_shared<HatchDiagonal2PFM>());
    pfms.append(std::make_shared<HatchGridPFM>());
    pfms.append(std::make_shared<HatchCrossPFM>());
    pfms.append(std::make_shared<Hatch3WayPFM>());
    pfms.append(std::make_shared<Hatch4WayPFM>());
    pfms.append(std::make_shared<Hatch5WayPFM>());
    pfms.append(std::make_shared<Hatch6WayPFM>());
    pfms.append(std::make_shared<HatchSawtoothPFM>());
    pfms.append(std::make_shared<HatchCircularScribblesPFM>());
    pfms.append(std::make_shared<StippleDotsPFM>());
    pfms.append(std::make_shared<StippleCirclesPFM>());
    pfms.append(std::make_shared<StippleSquaresPFM>());
    pfms.append(std::make_shared<StippleTrianglesPFM>());
    pfms.append(std::make_shared<StippleHexagonsPFM>());
    pfms.append(std::make_shared<StippleStarsPFM>());
    pfms.append(std::make_shared<StippleCrossesPFM>());
    pfms.append(std::make_shared<StippleLinesPFM>());
    pfms.append(std::make_shared<StippleVariableCirclesPFM>());
    pfms.append(std::make_shared<StippleVariableSquaresPFM>());
    pfms.append(std::make_shared<StippleChaosPFM>());
    pfms.append(std::make_shared<AdaptiveStipplingPFM>());
    pfms.append(std::make_shared<AdaptiveCircularScribblesPFM>());
    pfms.append(std::make_shared<AdaptiveShapesPFM>());
    pfms.append(std::make_shared<AdaptiveDashesPFM>());
    pfms.append(std::make_shared<AdaptiveTSPPFM>());
    pfms.append(std::make_shared<AdaptiveTriangulationPFM>());
    pfms.append(std::make_shared<AdaptiveTreePFM>());
    pfms.append(std::make_shared<AdaptiveDiagramPFM>());
    pfms.append(std::make_shared<AdaptiveLettersPFM>());
    pfms.append(std::make_shared<LBGCircularScribblesPFM>());
    pfms.append(std::make_shared<LBGShapesPFM>());
    pfms.append(std::make_shared<LBGTriangulationPFM>());
    pfms.append(std::make_shared<LBGTreePFM>());
    pfms.append(std::make_shared<LBGStipplingPFM>());
    pfms.append(std::make_shared<LBGDashesPFM>());
    pfms.append(std::make_shared<LBGDiagramPFM>());
    pfms.append(std::make_shared<LBGTSPPFM>());
    pfms.append(std::make_shared<LBGLettersPFM>());
    pfms.append(std::make_shared<VoronoiStipplingPFM>());
    pfms.append(std::make_shared<VoronoiCirclesPFM>());
    pfms.append(std::make_shared<VoronoiTriangulationPFM>());
    pfms.append(std::make_shared<VoronoiTreePFM>());
    pfms.append(std::make_shared<VoronoiDashesPFM>());
    pfms.append(std::make_shared<VoronoiDiagramPFM>());
    pfms.append(std::make_shared<VoronoiShapesPFM>());
    pfms.append(std::make_shared<VoronoiTSPPFM>());
    pfms.append(std::make_shared<VoronoiLettersPFM>());
    pfms.append(std::make_shared<TSPClassicPFM>());
    pfms.append(std::make_shared<TSPOutlinePFM>());
    pfms.append(std::make_shared<TSPShadingPFM>());
    pfms.append(std::make_shared<TSPStipplePFM>());
    pfms.append(std::make_shared<TSPVoronoiPFM>());
    pfms.append(std::make_shared<TSPMSTPFM>());
    pfms.append(std::make_shared<MazeRectPFM>());
    pfms.append(std::make_shared<MazeHexPFM>());
    pfms.append(std::make_shared<MazeTriPFM>());
    pfms.append(std::make_shared<MazeVoronoiPFM>());
    pfms.append(std::make_shared<MazeCirclePFM>());
    pfms.append(std::make_shared<MazeLabyrinthPFM>());
    pfms.append(std::make_shared<MazeHilbertPFM>());
    pfms.append(std::make_shared<MazePeanoPFM>());
    pfms.append(std::make_shared<LabyrinthClassicPFM>());
    pfms.append(std::make_shared<LabyrinthRomanPFM>());
    pfms.append(std::make_shared<GridShapesPFM>());
    pfms.append(std::make_shared<GridDashesPFM>());
    pfms.append(std::make_shared<GridLettersPFM>());
    pfms.append(std::make_shared<MosaicRectanglesPFM>());
    pfms.append(std::make_shared<MosaicVoronoiPFM>());
    pfms.append(std::make_shared<MosaicCustomPFM>());
    pfms.append(std::make_shared<SpiralCircularScribblesPFM>());
    pfms.append(std::make_shared<SpiralSawtoothPFM>());
    pfms.append(std::make_shared<StreamlinesFlowFieldPFM>());
    pfms.append(std::make_shared<StreamlinesEdgeFieldPFM>());
    pfms.append(std::make_shared<StreamlinesSuperformulaPFM>());
    pfms.append(std::make_shared<ContourPathsPFM>());
    pfms.append(std::make_shared<AmbientFlowPFM>());
    pfms.append(std::make_shared<EdgeShadingPFM>());
    pfms.append(std::make_shared<LayersPFM>());
    pfms.append(std::make_shared<StippleLayersPFM>());
    return pfms;
}

QVector<std::shared_ptr<ImageFilter>> ModuleRegistry::createAllFilters() {
    QVector<std::shared_ptr<ImageFilter>> filters;
    filters.append(std::make_shared<BrightnessFilter>());
    filters.append(std::make_shared<ContrastFilter>());
    filters.append(std::make_shared<InvertFilter>());
    filters.append(std::make_shared<ThresholdFilter>());
    filters.append(std::make_shared<UnsharpMaskFilter>());
    filters.append(std::make_shared<GrayscaleFilter>());
    filters.append(std::make_shared<DesaturateFilter>());
    filters.append(std::make_shared<SaturationFilter>());
    filters.append(std::make_shared<HueFilter>());
    filters.append(std::make_shared<GammaFilter>());
    filters.append(std::make_shared<ExposureFilter>());
    filters.append(std::make_shared<SepiaFilter>());
    filters.append(std::make_shared<CLAHEFilter>());
    filters.append(std::make_shared<TemperatureFilter>());
    filters.append(std::make_shared<CannyFilter>());
    filters.append(std::make_shared<SobelFilter>());
    filters.append(std::make_shared<LaplacianFilter>());
    filters.append(std::make_shared<PrewittFilter>());
    filters.append(std::make_shared<ScharrFilter>());
    filters.append(std::make_shared<DoGFilter>());
    filters.append(std::make_shared<RidgeDetectionFilter>());
    filters.append(std::make_shared<HighPassFilter>());
    filters.append(std::make_shared<GaussianBlurFilter>());
    filters.append(std::make_shared<MedianBlurFilter>());
    filters.append(std::make_shared<BoxBlurFilter>());
    filters.append(std::make_shared<MotionBlurFilter>());
    filters.append(std::make_shared<BilateralFilter>());
    filters.append(std::make_shared<LowPassFilter>());
    filters.append(std::make_shared<SharpenMoreFilter>());
    filters.append(std::make_shared<GaussianNoiseFilter>());
    filters.append(std::make_shared<SaltAndPepperFilter>());
    filters.append(std::make_shared<DenoiseFilter>());
    filters.append(std::make_shared<GaussianNoise2Filter>());
    filters.append(std::make_shared<SpeckleNoiseFilter>());
    filters.append(std::make_shared<DilationFilter>());
    filters.append(std::make_shared<ErosionFilter>());
    filters.append(std::make_shared<OpeningFilter>());
    filters.append(std::make_shared<ClosingFilter>());
    filters.append(std::make_shared<MorphGradientFilter>());
    filters.append(std::make_shared<TopHatFilter>());
    filters.append(std::make_shared<BlackHatFilter>());
    filters.append(std::make_shared<DilateCrossFilter>());
    filters.append(std::make_shared<ErodeCrossFilter>());
    filters.append(std::make_shared<DilateEllipseFilter>());
    filters.append(std::make_shared<ErodeEllipseFilter>());
    filters.append(std::make_shared<PosterizeFilter>());
    filters.append(std::make_shared<EdgePreserveFilter>());
    filters.append(std::make_shared<StylizationFilter>());
    filters.append(std::make_shared<OilPaintingFilter>());
    filters.append(std::make_shared<DetailEnhanceFilter>());
    filters.append(std::make_shared<PencilSketchFilter>());
    filters.append(std::make_shared<EmbossFilter>());
    filters.append(std::make_shared<QuantizeFilter>());
    filters.append(std::make_shared<VignetteFilter>());
    filters.append(std::make_shared<PixelateFilter>());
    filters.append(std::make_shared<WaveFilter>());
    filters.append(std::make_shared<OtsuThresholdFilter>());
    filters.append(std::make_shared<AdaptiveThresholdFilter>());
    filters.append(std::make_shared<TruncateThresholdFilter>());
    filters.append(std::make_shared<ToZeroThresholdFilter>());
    filters.append(std::make_shared<EqualizeHistFilter>());
    filters.append(std::make_shared<AutoContrastFilter>());
    filters.append(std::make_shared<AutoColorFilter>());
    filters.append(std::make_shared<ColorizeFilter>());
    filters.append(std::make_shared<InvertHueFilter>());
    return filters;
}


// Original File: ../cpp/core\project_processor.cpp



ProjectProcessor::ProjectProcessor() {
    m_allPFMs = ModuleRegistry::createAllPFMs();
    m_allFilters = ModuleRegistry::createAllFilters();
}

std::shared_ptr<PathFindingModule> ProjectProcessor::getPFMByName(const QString& name) const {
    for (auto& pfm : m_allPFMs) {
        if (pfm->name() == name) return pfm;
    }
    return nullptr;
}

std::shared_ptr<ImageFilter> ProjectProcessor::getFilterByName(const QString& name) const {
    for (auto& f : m_allFilters) {
        if (f->name() == name) return f;
    }
    return nullptr;
}

QVector<QColor> ProjectProcessor::fallbackPalette(const QString& sepMode, bool exportPalette) const {
    QVector<QColor> fallback;
    if (sepMode == "CMYK") {
        fallback = {QColor(0, 255, 255), QColor(255, 0, 255), QColor(255, 255, 0), QColor(30, 30, 30)};
    } else if (sepMode == "Colour Match") {
        fallback = {QColor(255, 0, 0), QColor(0, 255, 0), QColor(0, 0, 255)};
    } else {
        fallback = {exportPalette ? QColor(0, 0, 0) : QColor(200, 220, 255)};
    }

    QVector<QColor> palette = m_penColors.isEmpty() ? fallback : m_penColors;
    while (palette.size() < fallback.size())
        palette.append(fallback[palette.size()]);
    return palette;
}

bool ProjectProcessor::loadProject(const QString& projectPath, const QString& overrideImagePath) {
    QFile file(projectPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not read project file:" << file.errorString();
        return false;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qCritical() << "Invalid project JSON:" << err.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    
    m_imagePath = overrideImagePath.isEmpty() ? root.value("image_path").toString() : overrideImagePath;
    m_image = cv::Mat();
    if (!m_imagePath.isEmpty() && QFileInfo::exists(m_imagePath)) {
        cv::Mat img = cv::imread(m_imagePath.toStdString(), cv::IMREAD_COLOR);
        if (!img.empty()) m_image = img;
        else {
            qCritical() << "Failed to load image:" << m_imagePath;
            return false;
        }
    } else {
        qCritical() << "Image path was not found:" << m_imagePath;
        return false;
    }

    m_pfmName = root.value("pfm_name").toString("Sketch Lines");
    m_activePFM = getPFMByName(m_pfmName);
    if (!m_activePFM) {
        qCritical() << "PFM not found:" << m_pfmName;
        return false;
    }

    QJsonObject pfmSettings = root.value("pfm_settings").toObject();
    for (auto it = pfmSettings.constBegin(); it != pfmSettings.constEnd(); ++it) {
        m_activePFM->set(it.key(), it.value().toVariant());
    }

    m_activeFilters.clear();
    for (const auto& v : root.value("filter_chain").toArray()) {
        QJsonObject fObj = v.toObject();
        auto filter = getFilterByName(fObj.value("name").toString());
        if (!filter) {
            qWarning() << "Filter not found:" << fObj.value("name").toString();
            continue;
        }
        QJsonObject settings = fObj.value("settings").toObject();
        for (auto it = settings.constBegin(); it != settings.constEnd(); ++it) {
            filter->set(it.key(), it.value().toVariant());
        }
        m_activeFilters.append(filter);
    }

    QJsonObject mask = root.value("mask_settings").toObject();
    m_maskPath = mask.value("mask_path").toString();
    m_maskEnabled = mask.value("enabled").toBool(false) && !m_maskPath.isEmpty();
    if (m_maskEnabled) {
        if (QFileInfo::exists(m_maskPath)) {
            m_mask = cv::imread(m_maskPath.toStdString(), cv::IMREAD_GRAYSCALE);
            if (m_mask.empty()) m_maskEnabled = false;
        } else {
            qWarning() << "Mask path was not found:" << m_maskPath;
            m_maskEnabled = false;
        }
    }

    m_separationMode = root.value("colour_separation").toString("None");
    m_useVpype = root.value("use_vpype").toBool(false);
    m_vpypePipeline = root.value("vpype_pipeline").toString("linemerge linesimplify");
    m_penWidthMm = root.value("pen_width_mm").toDouble(0.5);
    
    QJsonObject da = root.value("drawing_area").toObject();
    if (!da.isEmpty()) {
        m_drawingArea.width_mm = da.value("width_mm").toDouble(210.0);
        m_drawingArea.height_mm = da.value("height_mm").toDouble(297.0);
        m_drawingArea.padding_left_mm = da.value("padding_left_mm").toDouble(0.0);
        m_drawingArea.padding_top_mm = da.value("padding_top_mm").toDouble(0.0);
        m_drawingArea.padding_right_mm = da.value("padding_right_mm").toDouble(0.0);
        m_drawingArea.padding_bottom_mm = da.value("padding_bottom_mm").toDouble(0.0);
        m_drawingArea.scaling_mode = static_cast<ScalingMode>(da.value("scaling_mode").toInt(0));
    }

    QVector<QColor> loadedPenColors;
    for (const auto& v : root.value("pen_colors").toArray()) {
        QColor color(v.toString());
        if (color.isValid()) loadedPenColors.append(color);
    }
    if (!loadedPenColors.isEmpty()) m_penColors = loadedPenColors;

    return true;
}

ProcessResult ProjectProcessor::process() {
    ProcessResult res;
    res.pfmName = m_pfmName;
    res.penWidthMm = m_penWidthMm;
    res.useVpype = m_useVpype;
    res.vpypePipeline = m_vpypePipeline;
    res.drawingArea = m_drawingArea;
    res.imageWidth = m_image.cols;
    res.imageHeight = m_image.rows;

    if (m_image.empty()) {
        res.errorMessage = "Input image is empty.";
        return res;
    }
    if (!m_activePFM) {
        res.errorMessage = "No active PFM.";
        return res;
    }

    std::cout << "Applying " << m_activeFilters.size() << " filters..." << std::endl;
    cv::Mat filteredImage = m_image.clone();
    for (auto& filter : m_activeFilters) {
        filteredImage = filter->process(filteredImage);
    }

    if (m_maskEnabled && !m_mask.empty()) {
        std::cout << "Applying mask..." << std::endl;
        MaskManager maskMgr;
        maskMgr.load(m_maskPath);
        filteredImage = maskMgr.applyMask(filteredImage);
    }

    res.previewImage = filteredImage.clone();

    std::cout << "Splitting color channels (Mode: " << m_separationMode.toStdString() << ")..." << std::endl;
    std::vector<cv::Mat> channels;
    if (m_separationMode == "CMYK") {
        channels = splitCMYK(filteredImage);
    } else if (m_separationMode == "Colour Match") {
        channels = splitRGB(filteredImage);
    } else {
        channels = splitGrayscale(filteredImage);
    }

    res.penColors = fallbackPalette(m_separationMode, true);
    
    for (size_t i = 0; i < channels.size(); ++i) {
        std::cout << "Processing channel " << (i + 1) << " of " << channels.size() << "..." << std::endl;

        if (m_activePFM->settingsMap().contains("random_seed")) {
            m_activePFM->set("random_seed", m_activePFM->get("random_seed").toInt() + (i * 1000));
        }

        auto geoms = m_activePFM->process(channels[i]);
        
        for (auto& geom : geoms) {
            geom.penIndex = static_cast<int>(i);
        }
        res.geometries.append(geoms);
    }

    std::cout << "Processing complete! Generated " << res.geometries.size() << " geometries." << std::endl;
    res.success = true;
    return res;
}


// Original File: ../cpp/core\serial_manager.cpp

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
        
        // emit statusMessage("connect", "Connected to " + portName, 0);
        return true;
    }
    // emit statusMessage("error", "Connection failed to " + portName, 0);
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
    // emit statusMessage("disconnect", "Disconnected", 0);
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
    // emit statusMessage("console_tx", cmd.trimmed(), 0);
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
    
    // emit statusMessage("stream_start", "G-code streaming started.", 0);
    sendNextLine();
    return true;
}

void SerialManager::pauseStream() {
    if (!isConnected() || !m_isStreaming) return;
    m_isPaused = true;
    m_serial.write("!"); // Feed Hold
    // emit statusMessage("stream_paused", "Stream paused.", getProgressPct());
}

void SerialManager::resumeStream() {
    if (!isConnected() || !m_isStreaming) return;
    m_isPaused = false;
    m_serial.write("~"); // Cycle Start
    // emit statusMessage("stream_resumed", "Stream resumed.", getProgressPct());
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
    // emit statusMessage("stream_stopped", "Stream aborted and reset.", 0);
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
            // emit statusMessage("console_rx", line, getProgressPct());
            
            if (m_isStreaming) {
                QString lower = line.toLower();
                if (lower.contains("ok") || lower.contains("error")) {
                    if (!m_grblBuffer.isEmpty()) {
                        m_grblBuffer.removeFirst();
                    }
                    if (lower.contains("error")) {
                        // emit statusMessage("warning", "GRBL error: " + line, getProgressPct());
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
            
            // emit statusMessage("stream_line", QString("[%1/%2] %3")
                               .arg(m_currentIdx).arg(m_totalLines).arg(nextLine), getProgressPct());
        } else {
            // Wait for 'ok' response to free buffer
            break;
        }
    }
    
    if (m_currentIdx >= m_totalLines && m_grblBuffer.isEmpty()) {
        m_isStreaming = false;
        // emit statusMessage("stream_finish", "G-code streaming finished successfully.", 100);
    }
}


// Original File: ../cpp/pfm\adaptive_circular_scribbles_pfm.cpp

// ---------------------------------------------------------------------------

AdaptiveCircularScribblesPFM::AdaptiveCircularScribblesPFM(QObject* p)
    : BaseAdaptivePFM(p) {}

QVector<PFMSetting> AdaptiveCircularScribblesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.append({"scribble_turns", "Scribble Turns", SettingType::Number, 2.0, QVariant(), 0.5, 8.0, 0.5, 8.0, 0.5});
    s.append({"radius_scale",   "Radius Scale",   SettingType::Number, 3.0, QVariant(), 0.5, 20.0, 0.5, 20.0, 0.5});
    return s;
}

QVector<DrawingGeometry> AdaptiveCircularScribblesPFM::_process(const cv::Mat& image) {
    // Apply plotting_resolution scaling
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    double turns   = m_settings["scribble_turns"].toDouble();
    double rscale  = m_settings["radius_scale"].toDouble();
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / pts.size(), (int)geoms.size(), "Adaptive scribbles...");
        float cx = pts[i].x, cy = pts[i].y;
        int steps = std::max(12, (int)(turns * 24));
        Path path;
        for (int s = 0; s <= steps; ++s) {
            double t = (double)s / steps * turns * M_PI * 2.0;
            path.push_back({cx + std::cos(t) * (float)rscale,
                            cy + std::sin(t) * (float)rscale});
        }
        geoms.push_back(DrawingGeometry{path, 0});
    }
    return geoms;
}


// Original File: ../cpp/pfm\adaptive_dashes_pfm.cpp

// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveDashesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float cx = pts[i].x, cy = pts[i].y;
        float angle  = (float)randUniform(0.0, M_PI);
        float length = (float)randUniform(3.0, 12.0);
        Path path = {
            {cx, cy},
            {cx + std::cos(angle) * length, cy + std::sin(angle) * length}
        };
        geoms.push_back(DrawingGeometry{path, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Adaptive dashes done");
    return geoms;
}


// Original File: ../cpp/pfm\adaptive_diagram_pfm.cpp

// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveDiagramPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};
    emitProgress(0.5f, 0, "Voronoi facets...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, workImg.cols, workImg.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    QVector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        Path path;
        for (const auto& pt : facet) path.push_back({pt.x, pt.y});
        path.push_back({facet[0].x, facet[0].y}); // close
        geoms.push_back(DrawingGeometry{path, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Adaptive diagram done");
    return geoms;
}


// Original File: ../cpp/pfm\adaptive_letters_pfm.cpp




// Original File: ../cpp/pfm\adaptive_pfm.cpp

AdaptiveStipplingPFM::AdaptiveStipplingPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> AdaptiveStipplingPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05,2.0,  0.1, 1.0, 0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,  999999, 0,   999999, 1 },
        { "cell_count",          "Cell Count",          SettingType::Integer, 3000, {}, 100,50000,  100, 20000, 100 },
        { "lloyd_iterations",    "Lloyd Iterations",    SettingType::Integer, 3,    {}, 0,  20,     0,   10,     1 },
        { "min_brightness",      "Min Brightness",      SettingType::Percentage, 80.0, {}, 0.0, 100.0, 0.0, 100.0, 1.0 },
        { "dot_radius",          "Dot Radius",          SettingType::Number,  1.0,  {}, 0.2,8.0,   0.2, 5.0,  0.1 },
    };
}

QVector<DrawingGeometry> AdaptiveStipplingPFM::_process(const cv::Mat& image) {
    // Apply plotting resolution
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;

    const int    cellCount     = get("cell_count").toInt();
    const int    lloydIter     = get("lloyd_iterations").toInt();
    const double minBrightPct  = get("min_brightness").toDouble() / 100.0;
    const double dotRadius     = get("dot_radius").toDouble();

    // Build darkness weights
    cv::Mat imgF;
    workImg.convertTo(imgF, CV_32F);
    std::vector<double> weights(W * H);
    double totalW = 0.0;
    const float* fp = imgF.ptr<float>(0);
    for (int i = 0; i < W * H; ++i) {
        double d = std::max(0.0, 255.0 - (double)fp[i]);
        weights[i] = d;
        totalW += d;
    }
    if (totalW < 1.0) return {};

    // Initial sample (weighted)
    std::vector<std::pair<double,double>> points(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        int idx = weightedChoice(weights);
        points[i] = { (double)(idx % W), (double)(idx / W) };
    }

    // Lloyd relaxation (simplified: re-weight toward local mean)
    for (int iter = 0; iter < lloydIter && !isCancelled(); ++iter) {
        // For each point, move slightly toward the darkest nearby pixel
        for (auto& pt : points) {
            int px = (int)pt.first, py = (int)pt.second;
            // Sample a small neighborhood
            double bestW = -1.0;
            double bx = pt.first, by = pt.second;
            for (int dy = -5; dy <= 5; ++dy)
                for (int dx = -5; dx <= 5; ++dx) {
                    int nx = px + dx, ny = py + dy;
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    double w = weights[ny * W + nx];
                    if (w > bestW) { bestW = w; bx = nx; by = ny; }
                }
            pt = { bx, by };
        }
    }

    // Filter by brightness (min_brightness removes dots in very bright areas)
    double minBrightAbs = minBrightPct * 255.0;

    QVector<DrawingGeometry> geometries;
    geometries.reserve(cellCount);

    for (int i = 0; i < (int)points.size() && !isCancelled(); ++i) {
        double cx = points[i].first, cy = points[i].second;
        int xi = (int)cx, yi = (int)cy;
        if (xi < 0 || xi >= W || yi < 0 || yi >= H) continue;
        double bright = fp[yi * W + xi];
        if (bright > minBrightAbs) continue;  // skip bright areas

        Path circle = generate_circle(cx, cy, dotRadius, 8);
        geometries.append(DrawingGeometry{ circle, 0 });

        if (i % 500 == 0)
            emitProgress((float)i / points.size(), geometries.size(), "Adaptive stippling...");
    }
    return geometries;
}


// Original File: ../cpp/pfm\adaptive_shapes_pfm.cpp

// ---------------------------------------------------------------------------

AdaptiveShapesPFM::AdaptiveShapesPFM(QObject* p) : BaseAdaptivePFM(p) {}

QVector<PFMSetting> AdaptiveShapesPFM::defineSettings() const {
    auto s = BaseAdaptivePFM::defineSettings();
    s.append({"shape_size", "Shape Size", SettingType::Number, 4.0, QVariant(), 1.0, 30.0, 1.0, 30.0, 0.5});
    return s;
}

QVector<DrawingGeometry> AdaptiveShapesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    float size = (float)m_settings["shape_size"].toDouble();
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / pts.size(), (int)geoms.size(), "Adaptive shapes...");
        float cx = pts[i].x, cy = pts[i].y;
        Path path;
        int shapeType = i % 4;
        switch (shapeType) {
            case 0: path = generate_circle(cx, cy, size, 12); break;
            case 1: path = generate_polygon(cx, cy, size, 4, 45); break;
            case 2: path = generate_polygon(cx, cy, size, 3, 30); break;
            case 3: path = generate_polygon(cx, cy, size, 6, 0); break;
        }
        geoms.push_back(DrawingGeometry{path, 0});
    }
    return geoms;
}


// Original File: ../cpp/pfm\adaptive_tree_pfm.cpp

// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveTreePFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};
    emitProgress(0.3f, 0, "Building MST...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, workImg.cols, workImg.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);

    std::map<std::pair<float,float>, int> pt2idx;
    for (size_t i = 0; i < pts.size(); ++i) pt2idx[{pts[i].x, pts[i].y}] = i;

    std::vector<std::vector<std::pair<int,float>>> adj(pts.size());
    for (const auto& e : edges) {
        auto it1 = pt2idx.find({e[0], e[1]});
        auto it2 = pt2idx.find({e[2], e[3]});
        if (it1 != pt2idx.end() && it2 != pt2idx.end()) {
            int u = it1->second, v = it2->second;
            float d = std::hypot(e[0]-e[2], e[1]-e[3]);
            adj[u].push_back({v, d});
            adj[v].push_back({u, d});
        }
    }
    std::vector<bool> in_mst(pts.size(), false);
    std::vector<float> min_w(pts.size(), 1e12f);
    std::vector<int> parent(pts.size(), -1);
    std::priority_queue<std::pair<float,int>, std::vector<std::pair<float,int>>,
                        std::greater<std::pair<float,int>>> pq;
    pq.push({0.f, 0});
    min_w[0] = 0.f;
    QVector<DrawingGeometry> geoms;
    while (!pq.empty()) {
        if (isCancelled()) return {};
        int u = pq.top().second; pq.pop();
        if (in_mst[u]) continue;
        in_mst[u] = true;
        if (parent[u] != -1) {
            geoms.push_back(DrawingGeometry{
                {{pts[parent[u]].x, pts[parent[u]].y}, {pts[u].x, pts[u].y}}, 0});
        }
        for (const auto& edge : adj[u]) {
            int v = edge.first; float w = edge.second;
            if (!in_mst[v] && w < min_w[v]) { min_w[v] = w; parent[v] = u; pq.push({w, v}); }
        }
    }
    emitProgress(1.0f, (int)geoms.size(), "Adaptive tree done");
    return geoms;
}


// Original File: ../cpp/pfm\adaptive_triangulation_pfm.cpp

// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveTriangulationPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.empty()) return {};
    emitProgress(0.5f, 0, "Delaunay triangulation...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, workImg.cols, workImg.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    QVector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        geoms.push_back(DrawingGeometry{{{e[0], e[1]}, {e[2], e[3]}}, 0});
    }
    emitProgress(1.0f, (int)geoms.size(), "Triangulation done");
    return geoms;
}


// Original File: ../cpp/pfm\adaptive_tsppfm.cpp
static Path solve_tsp_adaptive(const std::vector<cv::Point2f>& points,
                                std::function<bool()> isCancelledFn) {
    if (points.empty()) return {};
    std::vector<bool> visited(points.size(), false);
    Path path;
    path.reserve(points.size());
    int current = 0;
    visited[0] = true;
    path.push_back({points[0].x, points[0].y});
    int remaining = (int)points.size() - 1;
    while (remaining > 0) {
        if (isCancelledFn()) return {};
        int best_i = -1;
        float best_d = 1e12f;
        for (size_t i = 0; i < points.size(); ++i) {
            if (!visited[i]) {
                float dx = points[i].x - points[current].x;
                float dy = points[i].y - points[current].y;
                float d = dx*dx + dy*dy;
                if (d < best_d) { best_d = d; best_i = (int)i; }
            }
        }
        if (best_i == -1) break;
        visited[best_i] = true;
        path.push_back({points[best_i].x, points[best_i].y});
        current = best_i;
        remaining--;
    }
    return path;
}

// ---------------------------------------------------------------------------

QVector<DrawingGeometry> AdaptiveTSPPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image;
    }
    auto pts = getSeeds(workImg);
    if (pts.size() < 2) return {};
    emitProgress(0.3f, 0, "TSP solving...");
    Path path = solve_tsp_adaptive(pts, [this](){ return isCancelled(); });
    if (path.size() < 2) return {};
    return {DrawingGeometry{path, 0}};
}


// Original File: ../cpp/pfm\ambient_flow_pfm.cpp
Path trace_streamline_local(const cv::Mat& fx, const cv::Mat& fy, float x, float y, int max_len, float step_size) {
        Path path;
        int w = fx.cols;
        int h = fx.rows;
        for (int i = 0; i < max_len; ++i) {
            int xi = std::clamp(int(x), 0, w - 1);
            int yi = std::clamp(int(y), 0, h - 1);
            path.push_back({x, y});
            
            float vx = fx.at<float>(yi, xi);
            float vy = fy.at<float>(yi, xi);
            float mag = std::hypot(vx, vy);
            if (mag < 1e-6f) break;
            
            x += (vx / mag) * step_size;
            y += (vy / mag) * step_size;
            if (x < 0 || x >= w || y < 0 || y >= h) break;
        }
        return path;
    }

AmbientFlowPFM::AmbientFlowPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> AmbientFlowPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 200, QVariant(), 20, 2000, 20, 2000, 10},
        {"length", "Max Length", SettingType::Integer, 150, QVariant(), 20, 500, 20, 500, 10}
    };
}

QVector<DrawingGeometry> AmbientFlowPFM::_process(const cv::Mat& image) {
    int count = m_settings["lines"].toInt();
    int max_len = m_settings["length"].toInt();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(9, 9), 0);
    cv::GaussianBlur(gy, gy, cv::Size(9, 9), 0);
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Ambient Flow...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline_local(gx, gy, x, y, max_len, 1.2f);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// Original File: ../cpp/pfm\base_adaptive_pfm.cpp

// ---------------------------------------------------------------------------

BaseAdaptivePFM::BaseAdaptivePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseAdaptivePFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, QVariant(), 0.1, 2.0,   0.1, 2.0,   0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  QVariant(), 0,   999999, 0,   999999, 1},
        {"cell_count",          "Cell Count",          SettingType::Integer, 800, QVariant(), 50,  20000,  50,  20000,  50},
        {"lloyd_iterations",    "Lloyd Iterations",    SettingType::Integer, 3,   QVariant(), 0,   20,     0,   10,     1},
        {"min_brightness",      "Min Brightness (%)",  SettingType::Number,  80.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0},
    };
}

std::vector<cv::Point2f> BaseAdaptivePFM::getSeeds(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cellCount   = m_settings["cell_count"].toInt();
    int lloydIters  = m_settings["lloyd_iterations"].toInt();
    float minBright = (float)(m_settings["min_brightness"].toDouble() / 100.0) * 255.0f;

    // Build darkness weight map
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    // Initial weighted sampling
    std::vector<cv::Point2f> pts;
    pts.reserve(cellCount);
    for (int i = 0; i < cellCount; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    // Lloyd relaxation (simplified — move toward dark neighbourhood)
    for (int iter = 0; iter < lloydIters && !isCancelled(); ++iter) {
        emitProgress(float(iter) / lloydIters, 0, "Lloyd relaxation...");
        for (auto& pt : pts) {
            int px = (int)pt.x, py = (int)pt.y;
            double bestW = -1.0;
            float bx = pt.x, by = pt.y;
            for (int dy = -5; dy <= 5; ++dy)
                for (int dx = -5; dx <= 5; ++dx) {
                    int nx = px + dx, ny = py + dy;
                    if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
                    double ww = probs[ny * w + nx];
                    if (ww > bestW) { bestW = ww; bx = nx; by = ny; }
                }
            pt = {bx, by};
        }
    }

    // Filter by min brightness
    std::vector<cv::Point2f> filtered;
    for (const auto& p : pts) {
        int xi = std::clamp((int)p.x, 0, w - 1);
        int yi = std::clamp((int)p.y, 0, h - 1);
        float bright = image.at<uchar>(yi, xi);
        if (bright <= (255.0f - minBright))  // only dark enough pixels
            filtered.push_back(p);
    }
    return filtered;
}


// Original File: ../cpp/pfm\base_grid_pfm.cpp

BaseGridPFM::BaseGridPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseGridPFM::defineSettings() const {
    return {
        {"cols", "Columns", SettingType::Integer, 40, QVariant(), 2, 200, 2, 200, 1},
        {"rows", "Rows", SettingType::Integer, 40, QVariant(), 2, 200, 2, 200, 1},
        {"threshold", "Darkness Threshold", SettingType::Number, 50.0, QVariant(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}


// Original File: ../cpp/pfm\base_maze_pfm.cpp
static Path generate_dfs_maze(int width, int height, BaseMazePFM* pfm) {
    cv::Mat maze = cv::Mat::zeros(height, width, CV_8UC1);
    std::vector<std::pair<int, int>> stack;
    stack.push_back({0, 0});
    maze.at<uint8_t>(0, 0) = 1;
    Path path;
    int dx[] = {0, 2, 0, -2};
    int dy[] = {2, 0, -2, 0};
    while (!stack.empty()) {
        auto [x, y] = stack.back();
        path.push_back({x * 10.0, y * 10.0});
        std::vector<std::pair<int, int>> neighbors;
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i], ny = y + dy[i];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && maze.at<uint8_t>(ny, nx) == 0) {
                neighbors.push_back({nx, ny});
            }
        }
        if (!neighbors.empty()) {
            auto [nx, ny] = neighbors[int(pfm->randUniform(0, neighbors.size() - 0.001))];
            maze.at<uint8_t>(ny, nx) = 1;
            maze.at<uint8_t>(y + (ny - y) / 2, x + (nx - x) / 2) = 1;
            stack.push_back({nx, ny});
        } else {
            stack.pop_back();
        }
    }
    return path;
}

BaseMazePFM::BaseMazePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseMazePFM::defineSettings() const {
    return {
        {"complexity", "Complexity", SettingType::Integer, 10, QVariant(), 1, 50, 1, 50, 1}
    };
}

std::pair<float, float> BaseMazePFM::transformPoint(float x, float y) { return {x, y}; }

QVector<DrawingGeometry> BaseMazePFM::_process(const cv::Mat& image) {
    int comps = m_settings["complexity"].toInt();
    int gw = std::max(5, image.cols / (comps * 2));
    int gh = std::max(5, image.rows / (comps * 2));
    Path path = generate_dfs_maze(gw, gh, this);
    if (path.empty()) return {};

    for (auto& p : path) {
        auto t = transformPoint(p.first, p.second);
        p.first = t.first; p.second = t.second;
    }

    float max_x = 1e-5f, max_y = 1e-5f;
    for (const auto& p : path) {
        if (p.first > max_x) max_x = p.first;
        if (p.second > max_y) max_y = p.second;
    }

    Path scaled;
    for (const auto& p : path) {
        scaled.push_back({(p.first / max_x) * image.cols, (p.second / max_y) * image.rows});
    }

    DrawingGeometry dg; dg.path = scaled;
    return {dg};
}


// Original File: ../cpp/pfm\base_mosaic_pfm.cpp


BaseMosaicPFM::BaseMosaicPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseMosaicPFM::defineSettings() const {
    return {
        {"cols", "Columns", SettingType::Integer, 20, QVariant(), 2, 100, 2, 100, 1},
        {"rows", "Rows", SettingType::Integer, 20, QVariant(), 2, 100, 2, 100, 1},
        {"threshold", "Darkness Threshold", SettingType::Number, 40.0, QVariant(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}


// Original File: ../cpp/pfm\base_multi_hatch_pfm.cpp


BaseMultiHatchPFM::BaseMultiHatchPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseMultiHatchPFM::defineSettings() const {
    return {
        {"spacing", "Spacing", SettingType::Number, 5.0, QVariant(), 1.0, 50.0, 1.0, 50.0, 0.5},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

QVector<DrawingGeometry> BaseMultiHatchPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    std::vector<float> angles = getAngles();
    
    QVector<DrawingGeometry> geoms;
    int w = image.cols;
    int h = image.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    for (float angle : angles) {
        if (isCancelled()) break;
        float rad = angle * M_PI / 180.0f;
        float dx = std::cos(rad);
        float dy = std::sin(rad);
        
        for (int i = 0; i < num_lines; ++i) {
            if (isCancelled()) break;
            if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), QString("Hatching %1 deg...").arg(angle));
            
            float offset = (i - num_lines / 2.0f) * spacing;
            float cx = w / 2.0f + offset * -dy;
            float cy = h / 2.0f + offset * dx;
            float x1 = cx - dx * diag_len;
            float y1 = cy - dy * diag_len;
            float x2 = cx + dx * diag_len;
            float y2 = cy + dy * diag_len;
            
            Path path;
            int steps = std::max(1, int(diag_len));
            for (int step = 0; step < steps; ++step) {
                float px = x1 + (x2 - x1) * (float(step) / steps);
                float py = y1 + (y2 - y1) * (float(step) / steps);
                if (px >= 0 && px < w && py >= 0 && py < h) {
                    if ((255.0f - image.at<float>(int(py), int(px))) > thresh) {
                        path.push_back({px, py});
                    } else {
                        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                        path.clear();
                    }
                }
            }
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\base_stipple_pfm.cpp


BaseStipplePFM::BaseStipplePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseStipplePFM::defineSettings() const {
    return {
        {"num_shapes", "Number of Shapes", SettingType::Integer, 5000, QVariant(), 100, 50000, 100, 50000, 100},
        {"shape_size", "Shape Size", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1}
    };
}

QVector<DrawingGeometry> BaseStipplePFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int num_shapes = m_settings["num_shapes"].toInt();
    double size = m_settings["shape_size"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < num_shapes; ++i) {
        if (isCancelled()) break;
        if (i % 500 == 0) emitProgress(float(i) / num_shapes, geoms.size(), "Stippling...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;
        double local = probs[idx] * sum / 255.0; // recover brightness
        double r = size * (0.3 + 0.7 * local);

        DrawingGeometry dg;
        dg.path = generateShape(cx, cy, r);
        geoms.push_back(dg);
    }
    return geoms;
}


// Original File: ../cpp/pfm\base_streamline_pfm.cpp

BaseStreamlinePFM::BaseStreamlinePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseStreamlinePFM::defineSettings() const {
    return {
        {"line_count", "Line Count", SettingType::Integer, 120, QVariant(), 10, 2000, 10, 2000, 50},
        {"max_length", "Max Length", SettingType::Integer, 200, QVariant(), 20, 2000, 20, 2000, 10},
        {"step_size", "Step Size", SettingType::Number, 1.5, QVariant(), 0.5, 5.0, 0.5, 5.0, 0.1}
    };
}


// Original File: ../cpp/pfm\base_tsppfm.cpp


// -------------------------------------------------------------------------
BaseTSPPFM::BaseTSPPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseTSPPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, QVariant(), 100, 20000, 100, 20000, 100}
    };
}

QVector<DrawingGeometry> BaseTSPPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs = getProbabilities(image);
    if (probs.empty()) return {};
    
    double sum = 0.0;
    for (auto p : probs) sum += p;
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> points;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        points.push_back(cv::Point2f(idx % w, idx / w));
    }

    auto path = solve_tsp_nn(points, [this](){ return isCancelled(); }, [this](float p){ emitProgress(0.5f + p * 0.5f, 0, "Solving TSP..."); });
    if (path.size() >= 2) {
        DrawingGeometry dg;
        dg.path = path;
        return {dg};
    }
    return {};
}


// Original File: ../cpp/pfm\base_voronoi_extra_pfm.cpp

BaseVoronoiExtraPFM::BaseVoronoiExtraPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> BaseVoronoiExtraPFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 800, QVariant(), 50, 20000, 50, 20000, 100},
        {"lloyd_iterations", "Lloyd Iterations", SettingType::Integer, 5, QVariant(), 0, 20, 0, 20, 1},
        {"min_brightness", "Min Brightness", SettingType::Number, 0.0, QVariant(), 0.0, 255.0, 0.0, 255.0, 1.0}
    };
}

std::vector<cv::Point2f> BaseVoronoiExtraPFM::getSeeds(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cell_count = m_settings["cell_count"].toInt();
    int lloyd_iters = m_settings["lloyd_iterations"].toInt();
    float min_brightness = m_settings["min_brightness"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    std::vector<double> original_dark(w * h, 0.0);
    double sum = 0.0;
    
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            original_dark[y * w + x] = d;
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    if (lloyd_iters > 0) {
        int step = std::max(1, int(std::sqrt(h * w / 20000)));
        std::vector<cv::Point2f> sub_coords;
        std::vector<double> sub_w;
        for (int y = 0; y < h; y += step) {
            for (int x = 0; x < w; x += step) {
                sub_coords.push_back(cv::Point2f(x, y));
                sub_w.push_back(probs[y * w + x]);
            }
        }
        
        for (int iter = 0; iter < lloyd_iters; ++iter) {
            if (isCancelled()) return {};
            emitProgress(float(iter) / lloyd_iters, 0, "Lloyd Relaxation...");
            
            std::vector<cv::Point2f> new_pts(pts.size(), cv::Point2f(0,0));
            std::vector<double> weight_sums(pts.size(), 0.0);

            for (size_t i = 0; i < sub_coords.size(); ++i) {
                float best_d = 1e12f;
                int best_c = -1;
                for (size_t c = 0; c < pts.size(); ++c) {
                    float dx = sub_coords[i].x - pts[c].x;
                    float dy = sub_coords[i].y - pts[c].y;
                    float d = dx*dx + dy*dy;
                    if (d < best_d) { best_d = d; best_c = c; }
                }
                if (best_c != -1) {
                    new_pts[best_c].x += sub_coords[i].x * sub_w[i];
                    new_pts[best_c].y += sub_coords[i].y * sub_w[i];
                    weight_sums[best_c] += sub_w[i];
                }
            }

            for (size_t c = 0; c < pts.size(); ++c) {
                if (weight_sums[c] > 1e-9) {
                    pts[c].x = new_pts[c].x / weight_sums[c];
                    pts[c].y = new_pts[c].y / weight_sums[c];
                }
            }
        }
    }

    std::vector<cv::Point2f> filtered_pts;
    for (const auto& p : pts) {
        int xi = std::clamp(int(p.x), 0, w - 1);
        int yi = std::clamp(int(p.y), 0, h - 1);
        if (original_dark[yi * w + xi] >= min_brightness) {
            filtered_pts.push_back(p);
        }
    }
    return filtered_pts;
}


// Original File: ../cpp/pfm\contour_paths_pfm.cpp

// -------------------------------------------------------------------------
ContourPathsPFM::ContourPathsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> ContourPathsPFM::defineSettings() const {
    return {
        {"threshold", "Threshold", SettingType::Integer, 127, QVariant(), 0, 255, 0, 255, 1},
        {"epsilon", "Simplify Epsilon", SettingType::Number, 1.5, QVariant(), 0.0, 10.0, 0.0, 10.0, 0.1}
    };
}

QVector<DrawingGeometry> ContourPathsPFM::_process(const cv::Mat& image) {
    int threshold = m_settings["threshold"].toInt();
    double epsilon = m_settings["epsilon"].toDouble();

    cv::Mat img8u;
    image.convertTo(img8u, CV_8UC1);
    
    cv::Mat binary;
    cv::threshold(img8u, binary, threshold, 255, cv::THRESH_BINARY_INV);
    
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < contours.size(); ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / contours.size(), geoms.size(), "Contours...");
        
        const auto& cnt = contours[i];
        if (cnt.size() < 2) continue;
        
        std::vector<cv::Point> approx;
        cv::approxPolyDP(cnt, approx, epsilon, true);
        
        Path path;
        for (const auto& p : approx) {
            path.push_back({float(p.x), float(p.y)});
        }
        
        if (path.size() >= 2) {
            DrawingGeometry dg; dg.path = path;
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\edge_shading_pfm.cpp

// -------------------------------------------------------------------------
EdgeShadingPFM::EdgeShadingPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> EdgeShadingPFM::defineSettings() const {
    return {
        {"canny1", "Canny Low", SettingType::Integer, 50, QVariant(), 0, 255, 0, 255, 5},
        {"canny2", "Canny High", SettingType::Integer, 150, QVariant(), 0, 255, 0, 255, 5},
        {"spacing", "Hatch Spacing", SettingType::Number, 4.0, QVariant(), 1.0, 30.0, 1.0, 30.0, 0.5}
    };
}

QVector<DrawingGeometry> EdgeShadingPFM::_process(const cv::Mat& image) {
    int canny1 = m_settings["canny1"].toInt();
    int canny2 = m_settings["canny2"].toInt();
    float spacing = m_settings["spacing"].toDouble();
    
    cv::Mat img8u;
    image.convertTo(img8u, CV_8UC1);
    
    cv::Mat edges;
    cv::Canny(img8u, edges, canny1, canny2);
    
    cv::Mat combined;
    cv::Mat edgesF;
    edges.convertTo(edgesF, CV_32F);
    cv::addWeighted(image, 0.5, edgesF, 0.5, 0.0, combined);
    
    QVector<DrawingGeometry> geoms;
    int w = combined.cols;
    int h = combined.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    float rad = 45.0f * M_PI / 180.0f;
    float dx = std::cos(rad);
    float dy = std::sin(rad);
    float depth = 1.0f;
    float thresh = (80.0f / 100.0f) * 255.0f;
    
    for (int i = 0; i < num_lines; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "Edge Shading...");
        
        float offset = (i - num_lines / 2.0f) * spacing;
        float cx = w / 2.0f + offset * -dy;
        float cy = h / 2.0f + offset * dx;
        float x1 = cx - dx * diag_len;
        float y1 = cy - dy * diag_len;
        float x2 = cx + dx * diag_len;
        float y2 = cy + dy * diag_len;
        
        Path path;
        int steps = std::max(1, int(diag_len));
        bool up = true;
        for (int step = 0; step < steps; step += 3) {
            float px = x1 + (x2 - x1) * (float(step) / steps);
            float py = y1 + (y2 - y1) * (float(step) / steps);
            
            if (px >= 0 && px < w && py >= 0 && py < h) {
                float val = combined.at<float>(int(py), int(px));
                float brightness = val / 255.0f;
                if ((255.0f - val) > thresh) {
                    float local_depth = depth * (1.0f - brightness);
                    float wx = px + (up ? -dy : dy) * local_depth;
                    float wy = py + (up ? dx : -dx) * local_depth;
                    path.push_back({wx, wy});
                    up = !up;
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// Original File: ../cpp/pfm\grid_dashes_pfm.cpp

QVector<DrawingGeometry> GridDashesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    QVector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Dashes...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = {{cx + cw*0.2f, cy}, {cx + cw*0.8f, cy}};
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\grid_letters_pfm.cpp
static std::map<char, std::vector<std::pair<float, float>>> GRID_LETTER_GLYPHS = {
    {'A', {{0.1, 1}, {0.5, 0}, {0.9, 1}, {0.7, 0.55}, {0.3, 0.55}}},
    {'B', {{0.2, 0}, {0.2, 1}, {0.6, 1}, {0.8, 0.85}, {0.6, 0.5}, {0.8, 0.15}, {0.6, 0}, {0.2, 0}}},
    {'C', {{0.9, 0.15}, {0.6, 0}, {0.3, 0}, {0.1, 0.3}, {0.1, 0.7}, {0.3, 1}, {0.6, 1}, {0.9, 0.85}}},
    {'D', {{0.2, 0}, {0.2, 1}, {0.55, 1}, {0.85, 0.75}, {0.85, 0.25}, {0.55, 0}, {0.2, 0}}},
    {'E', {{0.8, 0}, {0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.65, 0.5}}},
    {'F', {{0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.7, 0.5}}},
    {'G', {{0.9, 0.2}, {0.6, 0}, {0.3, 0.05}, {0.1, 0.4}, {0.1, 0.7}, {0.35, 1}, {0.75, 0.9}, {0.75, 0.55}, {0.5, 0.55}}},
    {'H', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.8, 0.5}, {0.8, 1}, {0.8, 0}}},
    {'I', {{0.35, 0}, {0.65, 0}, {0.5, 0}, {0.5, 1}, {0.35, 1}, {0.65, 1}}},
    {'J', {{0.7, 0}, {0.3, 0}, {0.3, 0.8}, {0.5, 1}, {0.75, 0.85}}},
    {'K', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.85, 1}, {0.25, 0.5}, {0.85, 0}}},
    {'L', {{0.2, 1}, {0.2, 0}, {0.85, 0}}},
    {'M', {{0.1, 0}, {0.1, 1}, {0.5, 0.5}, {0.9, 1}, {0.9, 0}}},
    {'N', {{0.15, 0}, {0.15, 1}, {0.85, 0}, {0.85, 1}}},
    {'O', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.15, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}}},
    {'P', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}}},
    {'Q', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.2, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}, {0.75, 0.25}, {1, 0}}},
    {'R', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}, {0.85, 0}}},
    {'S', {{0.85, 0.85}, {0.55, 1}, {0.25, 0.9}, {0.15, 0.65}, {0.45, 0.5}, {0.75, 0.35}, {0.85, 0.15}, {0.55, 0}, {0.25, 0.1}}},
    {'T', {{0, 1}, {1, 1}, {0.5, 1}, {0.5, 0}}},
    {'U', {{0.15, 1}, {0.15, 0.25}, {0.35, 0}, {0.65, 0}, {0.85, 0.25}, {0.85, 1}}},
    {'V', {{0, 1}, {0.5, 0}, {1, 1}}},
    {'W', {{0, 1}, {0.25, 0}, {0.5, 0.6}, {0.75, 0}, {1, 1}}},
    {'X', {{0, 0}, {1, 1}, {0, 1}, {1, 0}}},
    {'Y', {{0, 1}, {0.5, 0.45}, {1, 1}, {0.5, 0.45}, {0.5, 0}}},
    {'Z', {{0.1, 1}, {0.9, 1}, {0.1, 0}, {0.9, 0}}}
};

static Path grid_letter_glyph_path(char letter, float cx, float cy, float size) {
    if (GRID_LETTER_GLYPHS.find(letter) == GRID_LETTER_GLYPHS.end()) letter = 'O';
    Path p;
    for (const auto& pt : GRID_LETTER_GLYPHS[letter]) {
        p.push_back({cx + (pt.first - 0.5f) * size, cy + (0.5f - pt.second) * size});
    }
    return p;
}

static char grid_letter_for_brightness(float darkness) {
    int idx = int(std::clamp(darkness / 255.0f, 0.0f, 0.999f) * 26.0f);
    const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return letters[idx];
}

QVector<DrawingGeometry> GridLettersPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    QVector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Letters...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                char letter = grid_letter_for_brightness(dark);
                DrawingGeometry dg;
                dg.path = grid_letter_glyph_path(letter, cx, cy, std::min(cw, ch) * 0.75f);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\grid_shapes_pfm.cpp

QVector<DrawingGeometry> GridShapesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    QVector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Grid Shapes...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_polygon(cx, cy, std::min(cw, ch) * 0.4f, 4 + (r + c) % 3);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\hatch3_way_pfm.cpp




// Original File: ../cpp/pfm\hatch4_way_pfm.cpp




// Original File: ../cpp/pfm\hatch5_way_pfm.cpp




// Original File: ../cpp/pfm\hatch6_way_pfm.cpp




// Original File: ../cpp/pfm\hatch_circular_scribbles_pfm.cpp

HatchCircularScribblesPFM::HatchCircularScribblesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> HatchCircularScribblesPFM::defineSettings() const {
    return {
        {"spacing", "Ring Spacing", SettingType::Number, 6.0, QVariant(), 2.0, 40.0, 2.0, 40.0, 0.5},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0},
        {"turns", "Turns per Ring", SettingType::Number, 1.5, QVariant(), 0.5, 4.0, 0.5, 4.0, 0.1}
    };
}

QVector<DrawingGeometry> HatchCircularScribblesPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    float turns = m_settings["turns"].toDouble();
    
    QVector<DrawingGeometry> geoms;
    float cx = image.cols / 2.0f;
    float cy = image.rows / 2.0f;
    float max_r = std::hypot(cx, cy);
    int num_rings = int(max_r / spacing);
    
    for (int r_idx = 1; r_idx <= num_rings; ++r_idx) {
        if (isCancelled()) break;
        emitProgress(float(r_idx) / num_rings, geoms.size(), "Circular Scribbles...");
        
        float base_r = r_idx * spacing;
        Path path;
        int steps = std::max(36, int(2.0f * M_PI * base_r * turns));
        for (int step = 0; step < steps; ++step) {
            float theta = (float(step) / steps) * 2.0f * M_PI * turns;
            float r = base_r + std::sin(theta * (base_r / 2.0f)) * (spacing * 0.8f);
            
            float px = cx + r * std::cos(theta);
            float py = cy + r * std::sin(theta);
            
            if (px >= 0 && px < image.cols && py >= 0 && py < image.rows) {
                if ((255.0f - image.at<float>(int(py), int(px))) > thresh) {
                    path.push_back({px, py});
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// Original File: ../cpp/pfm\hatch_cross_pfm.cpp




// Original File: ../cpp/pfm\hatch_diagonal1_pfm.cpp




// Original File: ../cpp/pfm\hatch_diagonal2_pfm.cpp




// Original File: ../cpp/pfm\hatch_grid_pfm.cpp




// Original File: ../cpp/pfm\hatch_horizontal_pfm.cpp




// Original File: ../cpp/pfm\hatch_lines.cpp

HatchLinesPFM::HatchLinesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> HatchLinesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution",  SettingType::Number,     1.0,  {}, 0.05, 2.0,  0.1,  1.0,  0.05 },
        { "random_seed",         "Random Seed",          SettingType::Integer,    42,   {}, 0,    999999,0,    999999,1 },
        { "angle1",              "Angle 1",              SettingType::Number,     45.0, {}, -90.0,90.0,-90.0, 90.0, 5.0 },
        { "angle2",              "Angle 2",              SettingType::Number,    -45.0, {}, -90.0,90.0,-90.0, 90.0, 5.0 },
        { "spacing",             "Spacing",              SettingType::Number,     5.0,  {}, 1.0,  50.0, 1.0,  30.0, 0.5 },
        { "threshold",           "Darkness Threshold",   SettingType::Percentage, 50.0, {}, 0.0,  100.0,0.0,  100.0,1.0 },
    };
}

QVector<DrawingGeometry> HatchLinesPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const double spacing = get("spacing").toDouble();
    const double thresh  = get("threshold").toDouble() / 100.0 * 255.0;

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    QVector<DrawingGeometry> geometries;

    for (double angleDeg : { get("angle1").toDouble(), get("angle2").toDouble() }) {
        if (isCancelled()) break;

        const double rad = angleDeg * M_PI / 180.0;
        const double dx = std::cos(rad), dy = std::sin(rad);
        const double diagLen = std::hypot(w, h);
        const int numLines = (int)(diagLen / spacing);

        for (int i = 0; i < numLines && !isCancelled(); ++i) {
            double offset = (i - numLines / 2.0) * spacing;
            double cx0 = w / 2.0 + offset * (-dy);
            double cy0 = h / 2.0 + offset * dx;

            double x1 = cx0 - dx * diagLen, y1 = cy0 - dy * diagLen;
            double x2 = cx0 + dx * diagLen, y2 = cy0 + dy * diagLen;

            Path path;
            int steps = (int)diagLen;
            for (int s = 0; s < steps; ++s) {
                double t  = (double)s / steps;
                double px = x1 + (x2 - x1) * t;
                double py = y1 + (y2 - y1) * t;
                if (px < 0 || px >= w || py < 0 || py >= h) {
                    if (path.size() > 1) geometries.append(DrawingGeometry{ path, 0 });
                    path.clear(); continue;
                }
                float b = 255.0f - imgF.at<float>((int)py, (int)px);
                if (b > thresh) {
                    path.push_back({ px, py });
                } else {
                    if (path.size() > 1) geometries.append(DrawingGeometry{ path, 0 });
                    path.clear();
                }
            }
            if (path.size() > 1) geometries.append(DrawingGeometry{ path, 0 });

            emitProgress((float)i / numLines, geometries.size(), "Hatching...");
        }
    }
    return geometries;
}


// Original File: ../cpp/pfm\hatch_sawtooth_pfm.cpp

HatchSawtoothPFM::HatchSawtoothPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> HatchSawtoothPFM::defineSettings() const {
    return {
        {"angle", "Angle", SettingType::Number, 45.0, QVariant(), -90.0, 90.0, -90.0, 90.0, 5.0},
        {"spacing", "Spacing", SettingType::Number, 5.0, QVariant(), 1.0, 50.0, 1.0, 50.0, 0.5},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0},
        {"tooth_depth", "Tooth Depth", SettingType::Number, 2.0, QVariant(), 0.5, 15.0, 0.5, 15.0, 0.5}
    };
}

QVector<DrawingGeometry> HatchSawtoothPFM::_process(const cv::Mat& image) {
    float angle = m_settings["angle"].toDouble();
    float spacing = m_settings["spacing"].toDouble();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    float depth = m_settings["tooth_depth"].toDouble();
    
    QVector<DrawingGeometry> geoms;
    int w = image.cols;
    int h = image.rows;
    float diag_len = std::hypot(w, h);
    int num_lines = int(diag_len / std::max(1.0f, spacing));
    
    float rad = angle * M_PI / 180.0f;
    float dx = std::cos(rad);
    float dy = std::sin(rad);
    
    for (int i = 0; i < num_lines; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / num_lines, geoms.size(), "Sawtooth Hatching...");
        
        float offset = (i - num_lines / 2.0f) * spacing;
        float cx = w / 2.0f + offset * -dy;
        float cy = h / 2.0f + offset * dx;
        float x1 = cx - dx * diag_len;
        float y1 = cy - dy * diag_len;
        float x2 = cx + dx * diag_len;
        float y2 = cy + dy * diag_len;
        
        Path path;
        int steps = std::max(1, int(diag_len));
        bool up = true;
        for (int step = 0; step < steps; step += 3) {
            float px = x1 + (x2 - x1) * (float(step) / steps);
            float py = y1 + (y2 - y1) * (float(step) / steps);
            
            if (px >= 0 && px < w && py >= 0 && py < h) {
                float brightness = image.at<float>(int(py), int(px)) / 255.0f;
                if ((255.0f - image.at<float>(int(py), int(px))) > thresh) {
                    float local_depth = depth * (1.0f - brightness);
                    float wx = px + (up ? -dy : dy) * local_depth;
                    float wy = py + (up ? dx : -dx) * local_depth;
                    path.push_back({wx, wy});
                    up = !up;
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// Original File: ../cpp/pfm\hatch_vertical_pfm.cpp




// Original File: ../cpp/pfm\labyrinth_classic_pfm.cpp

LabyrinthClassicPFM::LabyrinthClassicPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> LabyrinthClassicPFM::defineSettings() const {
    return {
        {"spacing", "Spiral Spacing", SettingType::Number, 10.0, QVariant(), 2.0, 50.0, 2.0, 50.0, 1.0},
        {"wobble", "Wobble Factor", SettingType::Number, 0.0, QVariant(), 0.0, 10.0, 0.0, 10.0, 0.5}
    };
}

QVector<DrawingGeometry> LabyrinthClassicPFM::_process(const cv::Mat& image) {
    float spacing = m_settings["spacing"].toDouble();
    float wobble = m_settings["wobble"].toDouble();
    float cx = image.cols / 2.0f;
    float cy = image.rows / 2.0f;
    float max_r = std::hypot(cx, cy);
    float theta = 0.0f;
    float b = spacing / (2.0f * M_PI);
    
    QVector<DrawingGeometry> geoms;
    Path path;
    int i = 0;
    while (true) {
        if (isCancelled()) break;
        float r = b * theta;
        if (r > max_r) break;
        
        float x = cx + r * std::cos(theta);
        float y = cy + r * std::sin(theta);
        int ix = int(x), iy = int(y);
        
        if (ix >= 0 && ix < image.cols && iy >= 0 && iy < image.rows) {
            float brightness = image.at<float>(iy, ix) / 255.0f;
            float wx = x + (std::cos(theta * 10.0f) * wobble * (1.0f - brightness));
            float wy = y + (std::sin(theta * 10.0f) * wobble * (1.0f - brightness));
            path.push_back({wx, wy});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
        float step = std::max(0.01f, std::min(0.5f, 2.0f / std::max(1.0f, r)));
        theta += step;
        if (i++ % 5000 == 0) emitProgress(r / max_r, geoms.size(), "Labyrinth...");
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}


// Original File: ../cpp/pfm\labyrinth_roman_pfm.cpp




// Original File: ../cpp/pfm\layers_pfm.cpp

// ---------------------------------------------------------------------------

LayersPFM::LayersPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> LayersPFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0, QVariant(), 0.1, 2.0, 0.1, 2.0, 0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,  QVariant(), 0, 999999, 0, 999999, 1},
        {"num_layers",          "Number of Layers",    SettingType::Integer, 4,   QVariant(), 2, 8, 2, 8, 1},
        {"line_density",        "Line Density (%)",    SettingType::Percentage, 40.0, QVariant(), 0, 100, 10, 100, 5},
    };
}

QVector<DrawingGeometry> LayersPFM::_process(const cv::Mat& image) {
    // Optionally resize to plotting resolution
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }

    int n = m_settings["num_layers"].toInt();
    double lineDensity = m_settings["line_density"].toDouble();
    QVector<DrawingGeometry> geoms;

    // Linearly spaced brightness thresholds
    float step = 200.0f / n;
    for (int layer = 0; layer < n && !isCancelled(); ++layer) {
        float lo = 30.0f + layer * step;
        float hi = lo + step;

        // Build a masked image: only pixels in [lo, hi) brightness range are drawn
        cv::Mat layerImg = cv::Mat::ones(workImg.size(), CV_8UC1) * 255;
        for (int y = 0; y < workImg.rows; ++y) {
            const uchar* src = workImg.ptr<uchar>(y);
            uchar* dst = layerImg.ptr<uchar>(y);
            for (int x = 0; x < workImg.cols; ++x) {
                float v = (float)src[x];
                if (v >= lo && v < hi) dst[x] = src[x];
                // else stays white (255)
            }
        }

        // Run SketchLinesPFM on this layer
        SketchLinesPFM sketch;
        sketch.set("line_density", lineDensity);
        sketch.set("plotting_resolution", 1.0);

        auto layerGeoms = sketch.process(layerImg);
        for (auto& g : layerGeoms) {
            g.penIndex   = layer;
            g.groupIndex = layer;
            geoms.append(g);
        }
        emitProgress(float(layer + 1) / n, geoms.size(),
                     QString("Layer %1/%2...").arg(layer + 1).arg(n));
    }
    return geoms;
}


// Original File: ../cpp/pfm\lbgtsppfm.cpp

QVector<PFMSetting> LBGTSPPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTSPPFM::defineSettings());
}


// Original File: ../cpp/pfm\lbg_circular_scribbles_pfm.cpp

QVector<PFMSetting> LBGCircularScribblesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveCircularScribblesPFM::defineSettings());
}


// Original File: ../cpp/pfm\lbg_dashes_pfm.cpp

QVector<PFMSetting> LBGDashesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveDashesPFM::defineSettings());
}


// Original File: ../cpp/pfm\lbg_diagram_pfm.cpp

QVector<PFMSetting> LBGDiagramPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveDiagramPFM::defineSettings());
}


// Original File: ../cpp/pfm\lbg_letters_pfm.cpp

// -------------------------------------------------------------------------
LBGLettersPFM::LBGLettersPFM(QObject* parent) : _LettersBasePFM(parent) {}

int LBGLettersPFM::getLloydIters() const { 
    int iters = m_settings["lloyd_iterations"].toInt();
    return iters > 0 ? iters : 5; 
}

QVector<PFMSetting> LBGLettersPFM::defineSettings() const {
    auto s = _LettersBasePFM::defineSettings();
    for (auto& setting : s) {
        if (setting.key == "lloyd_iterations") {
            setting.defaultVal = 5;
            setting.value = 5;
        }
    }
    return s;
}


// Original File: ../cpp/pfm\lbg_pfms.cpp

QVector<PFMSetting> makeLbgSettings(QVector<PFMSetting> settings) {
    for (auto& setting : settings) {
        if (setting.key == "lloyd_iterations") {
            setting.defaultVal = 5;
            setting.value = 5;
        }
    }
    return settings;
}


// Original File: ../cpp/pfm\lbg_shapes_pfm.cpp

QVector<PFMSetting> LBGShapesPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveShapesPFM::defineSettings());
}


// Original File: ../cpp/pfm\lbg_stippling_pfm.cpp

QVector<PFMSetting> LBGStipplingPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveStipplingPFM::defineSettings());
}


// Original File: ../cpp/pfm\lbg_tree_pfm.cpp

QVector<PFMSetting> LBGTreePFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTreePFM::defineSettings());
}


// Original File: ../cpp/pfm\lbg_triangulation_pfm.cpp

QVector<PFMSetting> LBGTriangulationPFM::defineSettings() const {
    return makeLbgSettings(AdaptiveTriangulationPFM::defineSettings());
}


// Original File: ../cpp/pfm\maze_circle_pfm.cpp

std::pair<float, float> MazeCirclePFM::transformPoint(float x, float y) {
    float r = y + 10.0f;
    float theta = x / 10.0f;
    return {r * std::cos(theta), r * std::sin(theta)};
}


// Original File: ../cpp/pfm\maze_hex_pfm.cpp

std::pair<float, float> MazeHexPFM::transformPoint(float x, float y) {
    float offset = (int(y / 10.0) % 2 != 0) ? 5.0f : 0.0f;
    return {x + offset, y * std::sqrt(3.0f) / 2.0f};
}


// Original File: ../cpp/pfm\maze_hilbert_pfm.cpp
static void hilbert_curve(float x0, float y0, float xi, float xj, float yi, float yj, int n, Path& pts) {
    if (n <= 0) { pts.push_back({x0 + (xi + yi) / 2.0f, y0 + (xj + yj) / 2.0f}); return; }
    hilbert_curve(x0, y0, yi/2, yj/2, xi/2, xj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2, y0 + xj/2, xi/2, xj/2, yi/2, yj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2 + yi/2, y0 + xj/2 + yj/2, xi/2, xj/2, yi/2, yj/2, n - 1, pts);
    hilbert_curve(x0 + xi/2 + yi, y0 + xj/2 + yj, -yi/2, -yj/2, -xi/2, -xj/2, n - 1, pts);
}

MazeHilbertPFM::MazeHilbertPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> MazeHilbertPFM::defineSettings() const {
    return {
        {"order", "Recursion Order", SettingType::Integer, 7, QVariant(), 3, 10, 3, 10, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

QVector<DrawingGeometry> MazeHilbertPFM::_process(const cv::Mat& image) {
    int order = m_settings["order"].toInt();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    Path pts;
    hilbert_curve(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, order, pts);
    
    QVector<DrawingGeometry> geoms;
    Path path;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 1000 == 0) emitProgress(float(i) / pts.size(), geoms.size(), "Hilbert...");
        int x = std::clamp(int(pts[i].first * image.cols), 0, image.cols - 1);
        int y = std::clamp(int(pts[i].second * image.rows), 0, image.rows - 1);
        
        if ((255.0f - image.at<float>(y, x)) > thresh) {
            path.push_back({float(x), float(y)});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}


// Original File: ../cpp/pfm\maze_labyrinth_pfm.cpp




// Original File: ../cpp/pfm\maze_peano_pfm.cpp
static Path generate_lsystem_path(const std::string& axiom, const std::map<char, std::string>& rules, int iterations, float angle_deg, float step_size) {
    std::string s = axiom;
    for (int i = 0; i < iterations; ++i) {
        std::string next_s = "";
        for (char c : s) {
            if (rules.count(c)) next_s += rules.at(c);
            else next_s += c;
        }
        s = next_s;
    }
    Path path;
    path.push_back({0.0, 0.0});
    float x = 0.0, y = 0.0, angle = 0.0;
    for (char c : s) {
        if (c == 'F') {
            x += step_size * std::cos(angle);
            y += step_size * std::sin(angle);
            path.push_back({x, y});
        } else if (c == '+') {
            angle += angle_deg * M_PI / 180.0f;
        } else if (c == '-') {
            angle -= angle_deg * M_PI / 180.0f;
        }
    }
    return path;
}

MazePeanoPFM::MazePeanoPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> MazePeanoPFM::defineSettings() const {
    return {
        {"order", "Recursion Order", SettingType::Integer, 4, QVariant(), 2, 6, 2, 6, 1},
        {"threshold", "Darkness Threshold", SettingType::Percentage, 50.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

QVector<DrawingGeometry> MazePeanoPFM::_process(const cv::Mat& image) {
    int order = m_settings["order"].toInt();
    float thresh = (m_settings["threshold"].toDouble() / 100.0f) * 255.0f;
    std::map<char, std::string> rules = {{'X', "XFYFX+F+YFXFY-F-XFYFX"}, {'Y', "YFXFY-F-XFYFX+F+YFXFY"}};
    emitProgress(0.2f, 0, "Generating L-System...");
    Path pts = generate_lsystem_path("X", rules, order, 90.0f, 1.0f);
    
    if (pts.empty()) return {};
    float min_x = 1e12f, max_x = -1e12f, min_y = 1e12f, max_y = -1e12f;
    for (auto& p : pts) {
        if (p.first < min_x) min_x = p.first; if (p.first > max_x) max_x = p.first;
        if (p.second < min_y) min_y = p.second; if (p.second > max_y) max_y = p.second;
    }
    
    QVector<DrawingGeometry> geoms;
    Path path;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float nx = (pts[i].first - min_x) / std::max(1e-5f, max_x - min_x);
        float ny = (pts[i].second - min_y) / std::max(1e-5f, max_y - min_y);
        int x = std::clamp(int(nx * image.cols), 0, image.cols - 1);
        int y = std::clamp(int(ny * image.rows), 0, image.rows - 1);
        
        if ((255.0f - image.at<float>(y, x)) > thresh) {
            path.push_back({float(x), float(y)});
        } else {
            if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
            path.clear();
        }
    }
    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    return geoms;
}


// Original File: ../cpp/pfm\maze_rect_pfm.cpp




// Original File: ../cpp/pfm\maze_tri_pfm.cpp

std::pair<float, float> MazeTriPFM::transformPoint(float x, float y) {
    return {(x - y) * std::cos(M_PI / 6.0f), (x + y) * std::sin(M_PI / 6.0f)};
}


// Original File: ../cpp/pfm\maze_voronoi_pfm.cpp

std::pair<float, float> MazeVoronoiPFM::transformPoint(float x, float y) {
    return {x + std::sin(y / 10.0f) * 5.0f, y + std::cos(x / 10.0f) * 5.0f};
}


// Original File: ../cpp/pfm\mosaic_custom_pfm.cpp

QVector<DrawingGeometry> MosaicCustomPFM::_process(const cv::Mat& image) {
    QVector<DrawingGeometry> geoms = MosaicRectanglesPFM::_process(image);
    
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Mosaic Custom...");
        for (int c = 0; c < cols; ++c) {
            if ((r + c) % 2 == 0) continue;
            
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark > thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_circle(cx, cy, std::min(cw, ch) * 0.4f, 10);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\mosaic_rectangles_pfm.cpp

QVector<DrawingGeometry> MosaicRectanglesPFM::_process(const cv::Mat& image) {
    int cols = m_settings["cols"].toInt();
    int rows = m_settings["rows"].toInt();
    float thresh = m_settings["threshold"].toDouble();
    float cw = float(image.cols) / cols;
    float ch = float(image.rows) / rows;
    
    QVector<DrawingGeometry> geoms;
    for (int r = 0; r < rows; ++r) {
        if (isCancelled()) break;
        emitProgress(float(r) / rows, geoms.size(), "Mosaic Rectangles...");
        for (int c = 0; c < cols; ++c) {
            cv::Rect roi(int(c * cw), int(r * ch), int(cw), int(ch));
            if (roi.x + roi.width > image.cols) roi.width = image.cols - roi.x;
            if (roi.y + roi.height > image.rows) roi.height = image.rows - roi.y;
            if (roi.width <= 0 || roi.height <= 0) continue;
            
            double mean = cv::mean(image(roi))[0];
            float dark = 255.0f - mean;
            if (dark >= thresh) {
                float cx = c * cw + cw / 2.0f;
                float cy = r * ch + ch / 2.0f;
                DrawingGeometry dg;
                dg.path = generate_rectangle(cx, cy, cw * 0.9f, ch * 0.9f);
                geoms.push_back(dg);
            }
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\mosaic_voronoi_pfm.cpp

QVector<PFMSetting> MosaicVoronoiPFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 200, QVariant(), 20, 2000, 20, 2000, 50}
    };
}

QVector<DrawingGeometry> MosaicVoronoiPFM::_process(const cv::Mat& image) {
    int cell_count = m_settings["cell_count"].toInt();
    std::vector<double> probs(image.cols * image.rows, 0.0);
    double sum = 0.0;
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            double d = 255.0 - row[x];
            if (d > 0) { probs[y * image.cols + x] = d; sum += d; }
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % image.cols, idx / image.cols));
    }

    emitProgress(0.5f, 0, "Mosaic Voronoi...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    QVector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() >= 2) {
            DrawingGeometry dg;
            for (const auto& pt : facet) dg.path.push_back({pt.x, pt.y});
            dg.path.push_back({facet[0].x, facet[0].y});
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\pfm_base.cpp
/**
 * pfm_base.cpp — Implementation of PathFindingModule base class.
 */


PathFindingModule::PathFindingModule(QObject* parent)
    : QObject(parent), m_rng(42), m_cancelled(0)
{
    // defineSettings() is pure virtual — call from subclass ctors after construction
    // We cannot call it here safely; subclasses must call initSettings() themselves.
}

void PathFindingModule::initSettings() {
    auto list = defineSettings();
    for (auto& s : list) {
        m_settings.insert(s.key, s);
    }
}

QVector<PFMSetting> PathFindingModule::settingsList() const {
    auto vals = m_settings.values();
    return QVector<PFMSetting>(vals.begin(), vals.end());
}

QVariant PathFindingModule::get(const QString& key) const {
    auto it = m_settings.find(key);
    if (it == m_settings.end()) return {};
    return it->currentValue();
}

void PathFindingModule::set(const QString& key, const QVariant& value) {
    auto it = m_settings.find(key);
    if (it != m_settings.end()) it->value = value;
}

void PathFindingModule::resetAll() {
    for (auto& s : m_settings) s.value = QVariant();
}

QVector<DrawingGeometry> PathFindingModule::process(const cv::Mat& grayImage) {
    m_cancelled.storeRelaxed(0);

    // Apply random seed
    QVariant seedVar = get("random_seed");
    quint32 seed = seedVar.isNull() ? 42u : (quint32)seedVar.toInt();
    m_rng.seed(seed);

    emitProgress(0.0f, 0, QString("Starting %1...").arg(name()));
    auto result = _process(grayImage);
    emitProgress(1.0f, result.size(), QString("%1 complete").arg(name()));
    return result;
}

void PathFindingModule::emitProgress(float pct, int shapes, const QString& text) {
    // emit progressUpdate(std::clamp(pct, 0.0f, 1.0f), shapes, text);
}

double PathFindingModule::randUniform(double lo, double hi) {
    std::uniform_real_distribution<double> dist(lo, hi);
    return dist(m_rng);
}

double PathFindingModule::randGauss(double mean, double sigma) {
    std::normal_distribution<double> dist(mean, sigma);
    return dist(m_rng);
}

int PathFindingModule::randInt(int lo, int hi) {
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(m_rng);
}

int PathFindingModule::weightedChoice(const std::vector<double>& weights) {
    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    return dist(m_rng);
}


// Original File: ../cpp/pfm\sketch_abstract_pfm.cpp

SketchAbstractPFM::SketchAbstractPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchAbstractPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 500, QVariant(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 150.0, QVariant(), 10.0, 500.0, 10.0, 500.0, 10.0}
    };
}

QVector<DrawingGeometry> SketchAbstractPFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    float length = m_settings["length"].toDouble();
    int w = image.cols;
    int h = image.rows;
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - image.at<float>(y, x);
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Abstract...");
        int idx = weightedChoice(probs);
        float cx = idx % w;
        float cy = idx / w;
        DrawingGeometry dg;
        dg.path = {{cx - randUniform(0, length), cy - randUniform(0, length)},
                   {cx + randUniform(0, length), cy + randUniform(0, length)}};
        geoms.push_back(dg);
    }
    return geoms;
}


// Original File: ../cpp/pfm\sketch_catmull_roms_pfm.cpp

SketchCatmullRomsPFM::SketchCatmullRomsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchCatmullRomsPFM::defineSettings() const {
    return SketchCurvesPFM().settingsList();
}

QVector<DrawingGeometry> SketchCatmullRomsPFM::_process(const cv::Mat& image) {
    SketchCurvesPFM sketch;
    for (auto it = m_settings.constBegin(); it != m_settings.constEnd(); ++it) {
        sketch.set(it.key(), it.value().currentValue());
    }
    connect(&sketch, &PathFindingModule::progressUpdate, this, &PathFindingModule::progressUpdate);
    return sketch.process(image);
}


// Original File: ../cpp/pfm\sketch_cubic_beziers2_pfm.cpp

// -------------------------------------------------------------------------

SketchCubicBeziers2PFM::SketchCubicBeziers2PFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchCubicBeziers2PFM::defineSettings() const {
    return {
        {"curves", "Curves", SettingType::Integer, 1000, QVariant(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 100.0, QVariant(), 10.0, 500.0, 10.0, 500.0, 10.0}
    };
}

QVector<DrawingGeometry> SketchCubicBeziers2PFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int curves = m_settings["curves"].toInt();
    double length = m_settings["length"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1.0) return {};
    for (auto& p : probs) p /= sum;

    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < curves; ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / curves, geoms.size(), "Generating Curves...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;

        double p0x = cx, p0y = cy;
        double p1x = cx + randUniform(-length, length), p1y = cy + randUniform(-length, length);
        double p2x = cx + randUniform(-length, length), p2y = cy + randUniform(-length, length);
        double p3x = cx + randUniform(-length, length), p3y = cy + randUniform(-length, length);

        int steps = 20;
        Path path;
        for (int s = 0; s <= steps; ++s) {
            double t = double(s) / steps;
            double u = 1.0 - t;
            double x = u*u*u*p0x + 3*u*u*t*p1x + 3*u*t*t*p2x + t*t*t*p3x;
            double y = u*u*u*p0y + 3*u*u*t*p1y + 3*u*t*t*p2y + t*t*t*p3y;
            path.push_back({x, y});
        }
        
        DrawingGeometry dg;
        dg.path = path;
        geoms.push_back(dg);
    }

    return geoms;
}


// Original File: ../cpp/pfm\sketch_cubic_beziers_pfm.cpp

// ---------------------------------------------------------------------------
SketchCubicBeziersPFM::SketchCubicBeziersPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchCubicBeziersPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.append({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 20, {}, 1,100,1,60,1,{},"Curves" });
    return s;
}

QVector<DrawingGeometry> SketchCubicBeziersPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::CubicBezier,
                         std::max(1, get("curve_smoothness").toInt()));
}


// Original File: ../cpp/pfm\sketch_curves.cpp

QVector<PFMSetting> makeSketchCommonSettings() {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,0.1,1.0,0.05,{},"Default" },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,999999,0,999999,1,{},"Default" },
        { "should_lift_pen",     "Should Lift Pen",     SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Style" },
        { "directionality",      "Directionality",      SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "distortion",          "Distortion",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "angularity",          "Angularity",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "drawing_delta_angle", "Drawing Delta Angle", SettingType::Number,   360.0,  {}, -360,360,-360,360,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_min_length",     "Line Min Length",     SettingType::Number,     2.0,  {}, 1,1000,2,500,1,{},"Segments" },
        { "line_max_length",     "Line Max Length",     SettingType::Number,    40.0,  {}, 1,1000,2,500,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "angle_tests",         "Angle Tests",         SettingType::Integer,    72,   {}, 1,720,1,360,1,{},"Segments" },
        { "squiggle_min_length", "Squiggle Min Length", SettingType::Number,     0.0,  {}, 0,10000,0,5000,1,{},"Squiggles" },
        { "squiggle_max_length", "Squiggle Max Length", SettingType::Number,   500.0,  {}, 0,10000,0,5000,1,{},"Squiggles" },
        { "squiggle_max_deviation","Squiggle Max Deviation",SettingType::Percentage,25.0,{},0,100,0,100,1,{},"Squiggles" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_radius_min",    "Erase Radius Min",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,{},"Erasing" },
        { "erase_radius_max",    "Erase Radius Max",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

QVector<DrawingGeometry> runSketchLoop(
    PathFindingModule* pfm,
    const cv::Mat& image,
    CurveOutputMode mode,
    int curveSmooth,
    double curveAlpha)
{
    cv::Mat workImg;
    double plotRes = pfm->get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    const double edgePower   = pfm->get("edge_power").toDouble();
    const double sobelPower  = pfm->get("sobel_power").toDouble();
    const double directional = pfm->get("directionality").toDouble();
    auto em = sketch_utils::buildEdgeMaps(workImg, edgePower, sobelPower, directional);

    const double lineDensity  = pfm->get("line_density").toDouble();
    const int    lineMinLen   = std::max(1, (int)pfm->get("line_min_length").toDouble());
    const int    lineMaxLen   = std::max(1, (int)pfm->get("line_max_length").toDouble());
    const int    lineMaxLimit = pfm->get("line_max_limit").toInt();
    const int    angleTests   = std::max(1, pfm->get("angle_tests").toInt());
    const double lumPower     = pfm->get("luminance_power").toDouble() / 100.0;
    const double distortion   = pfm->get("distortion").toDouble() / 100.0;
    const double angularity   = pfm->get("angularity").toDouble() / 100.0;
    const double deltaAngle   = pfm->get("drawing_delta_angle").toDouble();
    const int    sqgMin       = std::max(0, (int)pfm->get("squiggle_min_length").toDouble());
    const int    sqgMax       = std::max(1, (int)pfm->get("squiggle_max_length").toDouble());
    const double sqgDev       = pfm->get("squiggle_max_deviation").toDouble() / 100.0;
    const double eraseMin     = pfm->get("erase_min").toDouble();
    const double eraseMax     = pfm->get("erase_max").toDouble();
    const double eraseRMin    = pfm->get("erase_radius_min").toDouble();
    const double eraseRMax    = pfm->get("erase_radius_max").toDouble();
    const double tone         = pfm->get("tone").toDouble() / 100.0;

    const double deltaRad = (std::abs(deltaAngle) < 360.0)
                            ? std::abs(deltaAngle) * M_PI / 180.0 : 2.0 * M_PI;
    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    const float* edgePtr  = em.edge.empty()      ? nullptr : em.edge.ptr<float>(0);
    const float* sobelPtr = em.sobel.empty()     ? nullptr : em.sobel.ptr<float>(0);
    const float* dirPtr   = em.direction.empty() ? nullptr : em.direction.ptr<float>(0);
    float* lp = lightened.ptr<float>(0);

    std::vector<double> weights(W * H);
    QVector<DrawingGeometry> geometries;
    int totalSegments = 0, iteration = 0;

    while (iteration < W * H && !pfm->isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (lineMaxLimit > 0 && totalSegments >= lineMaxLimit) break;

        double darkSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int startIdx = pfm->weightedChoice(weights);
        int startY = startIdx / W, startX = startIdx % W;

        // Build squiggle
        std::vector<Point> squiggle = {{ (double)startX, (double)startY }};
        double cx0 = startX, cy0 = startY;
        double startBright = lp[startY * W + startX];
        int segCount = 0;
        double prevAngle = pfm->randUniform(0.0, 2.0 * M_PI);

        for (int segI = 0; segI < sqgMax && !pfm->isCancelled(); ++segI) {
            double bestScore = -std::numeric_limits<double>::infinity();
            double bestX = cx0, bestY = cy0, bestAngle = prevAngle;
            double baseAngle = prevAngle - deltaRad / 2.0;
            double segLen = pfm->randUniform(lineMinLen, lineMaxLen);

            for (int t = 0; t < angleTests; ++t) {
                double testAngle = baseAngle + t * (deltaRad / std::max(1, angleTests));
                double angPenalty = 0.0;
                if (angularity > 0) {
                    double diff = std::abs(testAngle - prevAngle);
                    if (diff > M_PI) diff = 2.0*M_PI - diff;
                    angPenalty = diff / M_PI * angularity;
                }
                if (distortion > 0) testAngle += pfm->randGauss(0.0, distortion * 0.5);

                double ex = std::clamp(cx0 + std::cos(testAngle)*segLen, 0.0, (double)(W-1));
                double ey = std::clamp(cy0 + std::sin(testAngle)*segLen, 0.0, (double)(H-1));
                int mxi = (int)((cx0+ex)/2), myi = (int)((cy0+ey)/2);
                int exi = (int)ex, eyi = (int)ey;

                if (mxi>=0&&mxi<W&&myi>=0&&myi<H&&exi>=0&&exi<W&&eyi>=0&&eyi<H) {
                    double score = -(lp[myi*W+mxi]+lp[eyi*W+exi])/2.0 * lumPower;
                    if (edgePtr)  score += ((double)edgePtr[myi*W+mxi] + edgePtr[eyi*W+exi])/2.0 * edgePower;
                    if (sobelPtr) score += ((double)sobelPtr[myi*W+mxi]+sobelPtr[eyi*W+exi])/2.0 * sobelPower;
                    if (dirPtr && directional > 0) {
                        double ld = dirPtr[myi*W+mxi];
                        double dd = std::abs(testAngle-ld);
                        if (dd>M_PI) dd=2.0*M_PI-dd;
                        score += (1.0-dd/M_PI)*directional*0.5;
                    }
                    score -= angPenalty * 50.0;
                    if (score > bestScore) { bestScore=score; bestX=ex; bestY=ey; bestAngle=testAngle; }
                }
            }

            if (segI > 0 && sqgDev < 1.0) {
                int bxi=(int)bestX, byi=(int)bestY;
                if (bxi>=0&&bxi<W&&byi>=0&&byi<H) {
                    double dev = std::abs((double)lp[byi*W+bxi] - startBright) / 255.0;
                    if (dev > sqgDev && segCount >= sqgMin) break;
                }
            }

            squiggle.push_back({ bestX, bestY });
            ++segCount; ++totalSegments;
            sketch_utils::eraseLine(lightened, cx0, cy0, bestX, bestY,
                                    eraseMin, eraseMax, eraseRMin, eraseRMax, tone, W, H);
            cx0 = bestX; cy0 = bestY; prevAngle = bestAngle;
            if (segCount >= sqgMax) break;
        }

        if (squiggle.size() >= 2) {
            Path curvePts;
            if (mode == CurveOutputMode::CatmullRom) {
                curvePts = catmull_rom_chain(squiggle, curveSmooth, curveAlpha);
            } else {
                // Quad Bezier: use start, mid-control, end
                if (squiggle.size() >= 3) {
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 2];
                    Point p2 = squiggle.back();
                    curvePts = quad_bezier_path(p0, p1, p2, curveSmooth);
                } else if (mode == CurveOutputMode::CubicBezier) {
                // Cubic Bezier: use start, 1/3, 2/3, end as control points
                if (squiggle.size() >= 4) {
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 3];
                    Point p2 = squiggle[2 * squiggle.size() / 3];
                    Point p3 = squiggle.back();
                    curvePts = cubic_bezier_path(p0, p1, p2, p3, curveSmooth);
                } else if (squiggle.size() >= 3) {
                    // Fall back to quad bezier if too few points
                    Point p0 = squiggle.front();
                    Point p1 = squiggle[squiggle.size() / 2];
                    Point p2 = squiggle.back();
                    curvePts = quad_bezier_path(p0, p1, p2, curveSmooth);
                } else {
                    curvePts = squiggle;
                }
            } else {
                    curvePts = squiggle;
                }
            }
            Path scaled;
            scaled.reserve(curvePts.size());
            for (auto& p : curvePts) scaled.push_back({ p.first*sx, p.second*sy });
            geometries.append(DrawingGeometry{ scaled, 0 });
        }

        ++iteration;
        if (iteration % 50 == 0) {
            pfm->emitProgress((float)dp, geometries.size(),
                              QString("%1 shapes %2%").arg(geometries.size()).arg((int)(dp*100)));
        }
    }
    return geometries;
}


// Original File: ../cpp/pfm\sketch_curves_pfm.cpp

// ---------------------------------------------------------------------------
SketchCurvesPFM::SketchCurvesPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchCurvesPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.append({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 10, {}, 1,50,1,30,1,{},"Curves" });
    s.append({ "curve_alpha",      "Curve Alpha",      SettingType::Number,   0.5,{}, 0.0,1.0,0.0,1.0,0.1,{},"Curves" });
    return s;
}

QVector<DrawingGeometry> SketchCurvesPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::CatmullRom,
                         std::max(1, get("curve_smoothness").toInt()),
                         get("curve_alpha").toDouble());
}


// Original File: ../cpp/pfm\sketch_delaunay_pfm.cpp

// -------------------------------------------------------------------------
SketchDelaunayPFM::SketchDelaunayPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchDelaunayPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 500, QVariant(), 10, 5000, 10, 5000, 10}
    };
}

QVector<DrawingGeometry> SketchDelaunayPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);

    QVector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        DrawingGeometry dg;
        dg.path = {{e[0], e[1]}, {e[2], e[3]}};
        geoms.push_back(dg);
    }
    return geoms;
}


// Original File: ../cpp/pfm\sketch_flow_fields_pfm.cpp

// -------------------------------------------------------------------------
SketchFlowFieldsPFM::SketchFlowFieldsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchFlowFieldsPFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 1000, QVariant(), 10, 10000, 10, 10000, 10}
    };
}

QVector<DrawingGeometry> SketchFlowFieldsPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int lines = m_settings["lines"].toInt();

    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    
    cv::GaussianBlur(gx, gx, cv::Size(5, 5), 0);
    cv::GaussianBlur(gy, gy, cv::Size(5, 5), 0);

    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 100 == 0) emitProgress(float(i) / lines, geoms.size(), "Tracing Streamlines...");

        double x = randUniform(0, w - 1);
        double y = randUniform(0, h - 1);

        Path path;
        for (int step = 0; step < 80; ++step) {
            int xi = std::clamp(int(x), 0, w - 1);
            int yi = std::clamp(int(y), 0, h - 1);
            path.push_back({x, y});

            double dx = gx.at<float>(yi, xi) * 1.5;
            double dy = gy.at<float>(yi, xi) * 1.5;

            double mag = std::hypot(dx, dy);
            if (mag < 1e-6) break;
            
            // Normalize and step
            dx = (dx / mag) * 1.5;
            dy = (dy / mag) * 1.5;

            x += dx;
            y += dy;

            if (x < 0 || x >= w || y < 0 || y >= h) break;
        }

        if (path.size() >= 2) {
            DrawingGeometry dg;
            dg.path = path;
            geoms.push_back(dg);
        }
    }

    return geoms;
}


// Original File: ../cpp/pfm\sketch_lines.cpp
/**
 * sketch_lines.cpp — C++ port of the SketchLines PFM algorithm.
 *
 * Algorithm (from DrawingBotV3 docs):
 *   1) Find the darkest area of the image (weighted random sample)
 *   2) Find the darkest pixel in that area
 *   3) Find the next darkest line direction from that pixel (angle tests)
 *   4) Brighten (erase) the part of the image that the line covers
 *   5) Repeat steps 2-4 until squiggle max length is reached → step 1
 *   Stop when line density or line max limit is reached.
 */


SketchLinesPFM::SketchLinesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchLinesPFM::defineSettings() const {
    return {
        // Default
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,-1,-1,-1, 0.05, {}, "Default","Controls the resolution of the image." },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0, 999999, 0, 999999, 1, {}, "Default" },
        // Style
        { "should_lift_pen",     "Should Lift Pen",     SettingType::Boolean,    true, {}, 0,1,0,1,1,{}, "Style", "Lift pen between squiggles." },
        { "directionality",      "Directionality",      SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "distortion",          "Distortion",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "angularity",          "Angularity",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0, 200, 0, 100, 1, {}, "Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0, 200, 0, 200, 1, {}, "Style" },
        { "drawing_delta_angle", "Drawing Delta Angle", SettingType::Number,   360.0,  {}, -360,360,-360,360,1,{}, "Style" },
        // Segments
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,  {}, "Segments" },
        { "line_min_length",     "Line Min Length",     SettingType::Number,     2.0,  {}, 1,1000,2,500,1, {}, "Segments" },
        { "line_max_length",     "Line Max Length",     SettingType::Number,     40.0, {}, 1,1000,2,500,1, {}, "Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,    -1,   {}, -1,1000000,-1,1000000,1,{}, "Segments" },
        { "angle_tests",         "Angle Tests",         SettingType::Integer,    72,   {}, 1,720,1,360,1,  {}, "Segments" },
        // Squiggles
        { "squiggle_min_length",    "Squiggle Min Length",    SettingType::Number, 0.0,  {}, 0,10000,0,5000,1, {}, "Squiggles" },
        { "squiggle_max_length",    "Squiggle Max Length",    SettingType::Number, 500.0,{}, 0,10000,0,5000,1, {}, "Squiggles" },
        { "squiggle_max_deviation", "Squiggle Max Deviation", SettingType::Percentage, 25.0, {}, 0,100,0,100,1, {}, "Squiggles" },
        // Erasing
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,  {}, "Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,  {}, "Erasing" },
        { "erase_radius_min",    "Erase Radius Min",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,  {}, "Erasing" },
        { "erase_radius_max",    "Erase Radius Max",    SettingType::Number,     1.0,  {}, 0,50,0,50,0.5,  {}, "Erasing" },
        { "tone",                "Tone",                SettingType::Percentage,  50.0, {}, 0,100,0,100,1,  {}, "Erasing" },
        // Shading
        { "shading",             "Shading",             SettingType::Boolean,    false,{}, 0,1,0,1,1,{}, "Shading" },
        { "shading_threshold",   "Shading Threshold",   SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{}, "Shading" },
        { "shading_start_angle_min", "Start Angle Min", SettingType::Number, -85.0, {}, -360,360,-360,360,1,{}, "Shading" },
        { "shading_start_angle_max", "Start Angle Max", SettingType::Number,  95.0, {}, -360,360,-360,360,1,{}, "Shading" },
        { "shading_delta_angle", "Shading Delta Angle", SettingType::Number,  360.0, {}, -360,360,-360,360,1,{}, "Shading" },
    };
}

// ---------------------------------------------------------------------------
// Erase (brighten) along a line — same math as Python _erase_line
// ---------------------------------------------------------------------------
void SketchLinesPFM::eraseLine(cv::Mat& img,
                                double x0, double y0, double x1, double y1,
                                double eraseMin, double eraseMax,
                                double radiusMin, double radiusMax,
                                double tone, int w, int h) {
    double dist = std::hypot(x1 - x0, y1 - y0);
    int steps = std::max(1, (int)dist);
    float* data = img.ptr<float>(0);

    for (int s = 0; s <= steps; ++s) {
        double t = (double)s / steps;
        double toneT = (tone > 0.0) ? std::pow(t, 1.0 / std::max(0.01, tone)) : 0.5;
        double eraseVal = eraseMin + (eraseMax - eraseMin) * toneT;
        double radius   = radiusMin + (radiusMax - radiusMin) * toneT;

        double px = x0 + (x1 - x0) * t;
        double py = y0 + (y1 - y0) * t;
        int pxi = (int)px, pyi = (int)py;

        int rInt = std::max(0, (int)radius);
        if (rInt == 0) {
            if (pxi >= 0 && pxi < w && pyi >= 0 && pyi < h)
                data[pyi * w + pxi] = std::min(255.0f, data[pyi * w + pxi] + (float)eraseVal);
        } else {
            int yLo = std::max(0, pyi - rInt), yHi = std::min(h, pyi + rInt + 1);
            int xLo = std::max(0, pxi - rInt), xHi = std::min(w, pxi + rInt + 1);
            for (int ey = yLo; ey < yHi; ++ey) {
                for (int ex = xLo; ex < xHi; ++ex) {
                    double d = std::hypot(ex - px, ey - py);
                    if (d <= radius)
                        data[ey * w + ex] = std::min(255.0f, data[ey * w + ex] + (float)eraseVal);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Main processing
// ---------------------------------------------------------------------------
QVector<DrawingGeometry> SketchLinesPFM::_process(const cv::Mat& image) {
    // ---- apply plotting resolution ----
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (plotRes > 0.0 && std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;

    // Working float buffer (mutable brightness)
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    // ---- Optional edge/sobel/direction maps ----
    const double edgePower   = get("edge_power").toDouble();
    const double sobelPower  = get("sobel_power").toDouble();
    const double directional = get("directionality").toDouble();

    cv::Mat edgeMap, sobelMap, directionMap;

    if (edgePower > 0) {
        cv::Mat edges;
        cv::Canny(workImg, edges, 50, 150);
        edges.convertTo(edgeMap, CV_32F, 1.0 / 255.0);
    }
    if (sobelPower > 0) {
        cv::Mat sx, sy, mag;
        cv::Sobel(workImg, sx, CV_64F, 1, 0, 3);
        cv::Sobel(workImg, sy, CV_64F, 0, 1, 3);
        cv::magnitude(sx, sy, mag);
        double mn, mx;
        cv::minMaxLoc(mag, &mn, &mx);
        mag.convertTo(sobelMap, CV_32F, 1.0 / (mx + 1e-10));
    }
    if (directional > 0) {
        cv::Mat sx, sy;
        cv::Sobel(workImg, sx, CV_64F, 1, 0, 3);
        cv::Sobel(workImg, sy, CV_64F, 0, 1, 3);
        // directionMap[y,x] = atan2(sy, sx)
        directionMap.create(H, W, CV_32F);
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                directionMap.at<float>(y, x) = (float)std::atan2(
                    sy.at<double>(y, x), sx.at<double>(y, x));
    }

    // ---- Read settings ----
    const double lineDensity     = get("line_density").toDouble() / 100.0 * 100.0; // keep as %
    const int    lineMinLen      = std::max(1, (int)get("line_min_length").toDouble());
    const int    lineMaxLen      = std::max(1, (int)get("line_max_length").toDouble());
    const int    lineMaxLimit    = get("line_max_limit").toInt();
    const int    angleTests      = std::max(1, get("angle_tests").toInt());
    const double luminancePower  = get("luminance_power").toDouble() / 100.0;
    const double distortion      = get("distortion").toDouble() / 100.0;
    const double angularityVal   = get("angularity").toDouble() / 100.0;
    const double deltaAngle      = get("drawing_delta_angle").toDouble();

    const int    sqgMin          = std::max(0, (int)get("squiggle_min_length").toDouble());
    const int    sqgMax          = std::max(1, (int)get("squiggle_max_length").toDouble());
    const double sqgDeviation    = get("squiggle_max_deviation").toDouble() / 100.0;

    const double eraseMin        = get("erase_min").toDouble();
    const double eraseMax        = get("erase_max").toDouble();
    const double eraseRMin       = get("erase_radius_min").toDouble();
    const double eraseRMax       = get("erase_radius_max").toDouble();
    const double tone            = get("tone").toDouble() / 100.0;

    const bool   shadingOn       = get("shading").toBool();
    const double shadeThresh     = get("shading_threshold").toDouble() / 100.0;
    const double shadeAngleMin   = get("shading_start_angle_min").toDouble() * M_PI / 180.0;
    const double shadeAngleMax   = get("shading_start_angle_max").toDouble() * M_PI / 180.0;
    const double shadeDelta      = get("shading_delta_angle").toDouble() * M_PI / 180.0;

    // ---- Scale factors to map back to original coords ----
    const double sx = (double)image.cols / W;
    const double sy = (double)image.rows / H;

    // ---- Target brightness ----
    cv::Scalar meanScalar = cv::mean(lightened);
    const double initialBrightness = meanScalar[0];
    const double targetBrightness  = initialBrightness
        + (255.0 - initialBrightness) * (lineDensity / 100.0);

    // Pre-compute delta_rad
    const double deltaRad  = (std::abs(deltaAngle) < 360.0)
                             ? (std::abs(deltaAngle) * M_PI / 180.0)
                             : (2.0 * M_PI);

    float* lightenedPtr = lightened.ptr<float>(0);
    const float* edgePtr    = edgeMap.empty()    ? nullptr : edgeMap.ptr<float>(0);
    const float* sobelPtr   = sobelMap.empty()   ? nullptr : sobelMap.ptr<float>(0);
    const float* dirPtr     = directionMap.empty()? nullptr : directionMap.ptr<float>(0);

    QVector<DrawingGeometry> geometries;
    int totalSegments = 0;
    int iteration     = 0;
    const int maxIterations = W * H;

    // Darkness weights vector (reused)
    std::vector<double> weights(W * H);

    while (iteration < maxIterations && !isCancelled()) {
        // ---- Check stopping conditions ----
        double curBrightness = 0.0;
        for (int i = 0; i < W * H; ++i) curBrightness += lightenedPtr[i];
        curBrightness /= (W * H);

        double densityProgress = (curBrightness - initialBrightness)
                                 / std::max(1.0, targetBrightness - initialBrightness);
        densityProgress = std::clamp(densityProgress, 0.0, 1.0);

        if (densityProgress >= 1.0) break;
        if (lineMaxLimit > 0 && totalSegments >= lineMaxLimit) break;

        bool isShading = shadingOn && (densityProgress >= shadeThresh);

        // ---- Step 1: Weighted random start position ----
        double darknessSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lightenedPtr[i]);
            weights[i] = d;
            darknessSum += d;
        }
        if (darknessSum < 1.0) break;

        int startIdx = weightedChoice(weights);
        int startY   = startIdx / W;
        int startX   = startIdx % W;

        // ---- Step 2-4: Build a squiggle ----
        Path squiggle;
        squiggle.push_back({ (double)startX, (double)startY });
        double cx0 = (double)startX, cy0 = (double)startY;
        double startBrightness = lightenedPtr[startY * W + startX];
        int segmentCount = 0;
        double prevAngle = randUniform(0.0, 2.0 * M_PI);

        for (int segI = 0; segI < sqgMax && !isCancelled(); ++segI) {
            // Step 3: find darkest direction
            double bestScore = -std::numeric_limits<double>::infinity();
            double bestX = cx0, bestY = cy0, bestAngle = prevAngle;

            double baseAngle, testDelta;
            if (isShading) {
                baseAngle = randUniform(shadeAngleMin, shadeAngleMax);
                testDelta = shadeDelta;
            } else {
                baseAngle = prevAngle - deltaRad / 2.0;
                testDelta = deltaRad;
            }

            double segLen = randUniform(lineMinLen, lineMaxLen);

            for (int t = 0; t < angleTests; ++t) {
                double testAngle = baseAngle + t * (testDelta / std::max(1, angleTests));

                // Angularity penalty
                double angularPenalty = 0.0;
                if (angularityVal > 0 && !isShading) {
                    double diff = std::abs(testAngle - prevAngle);
                    if (diff > M_PI) diff = 2.0 * M_PI - diff;
                    angularPenalty = diff / M_PI * angularityVal;
                }

                // Distortion
                if (distortion > 0)
                    testAngle += randGauss(0.0, distortion * 0.5);

                double ex = cx0 + std::cos(testAngle) * segLen;
                double ey = cy0 + std::sin(testAngle) * segLen;
                ex = std::clamp(ex, 0.0, (double)(W - 1));
                ey = std::clamp(ey, 0.0, (double)(H - 1));

                double mx = (cx0 + ex) / 2.0, my = (cy0 + ey) / 2.0;
                int mxi = (int)mx, myi = (int)my;
                int exi = (int)ex, eyi = (int)ey;

                if (mxi >= 0 && mxi < W && myi >= 0 && myi < H &&
                    exi >= 0 && exi < W && eyi >= 0 && eyi < H) {
                    double bMid = lightenedPtr[myi * W + mxi];
                    double bEnd = lightenedPtr[eyi * W + exi];
                    double score = -(bMid + bEnd) / 2.0 * luminancePower;

                    if (edgePtr) {
                        double edgeScore = ((double)edgePtr[myi*W+mxi] + edgePtr[eyi*W+exi]) / 2.0;
                        score += edgeScore * edgePower;
                    }
                    if (sobelPtr) {
                        double sobelScore = ((double)sobelPtr[myi*W+mxi] + sobelPtr[eyi*W+exi]) / 2.0;
                        score += sobelScore * sobelPower;
                    }
                    if (dirPtr && directional > 0) {
                        double localDir = dirPtr[myi * W + mxi];
                        double diff = std::abs(testAngle - localDir);
                        if (diff > M_PI) diff = 2.0 * M_PI - diff;
                        double dirScore = 1.0 - diff / M_PI;
                        score += dirScore * directional * 0.5;
                    }
                    score -= angularPenalty * 50.0;

                    if (score > bestScore) {
                        bestScore = score;
                        bestX = ex; bestY = ey;
                        bestAngle = testAngle;
                    }
                }
            }

            // Check squiggle deviation
            if (segI > 0 && sqgDeviation < 1.0) {
                int bxi = (int)bestX, byi = (int)bestY;
                if (bxi >= 0 && bxi < W && byi >= 0 && byi < H) {
                    double curB = lightenedPtr[byi * W + bxi];
                    double dev = std::abs(curB - startBrightness) / 255.0;
                    if (dev > sqgDeviation && segmentCount >= sqgMin)
                        break;
                }
            }

            squiggle.push_back({ bestX, bestY });
            ++segmentCount;
            ++totalSegments;

            eraseLine(lightened, cx0, cy0, bestX, bestY,
                      eraseMin, eraseMax, eraseRMin, eraseRMax, tone, W, H);

            cx0 = bestX; cy0 = bestY;
            prevAngle = bestAngle;
            if (segmentCount >= sqgMax) break;
        }

        // Scale back to original coords
        if (squiggle.size() >= 2) {
            Path scaled;
            scaled.reserve(squiggle.size());
            for (auto& p : squiggle)
                scaled.push_back({ p.first * sx, p.second * sy });
            geometries.append(DrawingGeometry{ scaled, 0 });
        }

        ++iteration;
        if (iteration % 50 == 0) {
            emitProgress((float)densityProgress, geometries.size(),
                         QString("Drawing... %1 shapes, %2% density")
                         .arg(geometries.size())
                         .arg((int)(densityProgress * 100)));
        }
    }
    return geometries;
}


// Original File: ../cpp/pfm\sketch_quad_beziers2_pfm.cpp

// -------------------------------------------------------------------------

QVector<DrawingGeometry> SketchQuadBeziers2PFM::_process(const cv::Mat& image) {
    return SketchCubicBeziers2PFM::_process(image);
}


// Original File: ../cpp/pfm\sketch_quad_beziers_pfm.cpp

// ---------------------------------------------------------------------------
SketchQuadBeziersPFM::SketchQuadBeziersPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchQuadBeziersPFM::defineSettings() const {
    auto s = makeSketchCommonSettings();
    s.append({ "curve_smoothness", "Curve Smoothness", SettingType::Integer, 20, {}, 1,100,1,60,1,{},"Curves" });
    return s;
}

QVector<DrawingGeometry> SketchQuadBeziersPFM::_process(const cv::Mat& image) {
    return runSketchLoop(this, image, CurveOutputMode::QuadBezier,
                         std::max(1, get("curve_smoothness").toInt()));
}


// Original File: ../cpp/pfm\sketch_radial_pfm.cpp

SketchRadialPFM::SketchRadialPFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchRadialPFM::defineSettings() const {
    return { {"lines", "Lines", SettingType::Integer, 1000, QVariant(), 10, 5000, 10, 5000, 10} };
}

QVector<DrawingGeometry> SketchRadialPFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    int w = image.cols;
    int h = image.rows;
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Radial...");
        float theta = randUniform(0.0, 2.0 * M_PI);
        Path path;
        float r = 0.0f;
        while (r < std::hypot(cx, cy)) {
            float x = cx + r * std::cos(theta);
            float y = cy + r * std::sin(theta);
            if (x >= 0 && x < w && y >= 0 && y < h) {
                if (image.at<float>(int(y), int(x)) < 200.0f) {
                    path.push_back({x, y});
                } else {
                    if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
                    path.clear();
                }
            }
            r += 2.0f;
        }
        if (path.size() > 1) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// Original File: ../cpp/pfm\sketch_scribble_pfm.cpp

SketchScribblePFM::SketchScribblePFM(QObject* parent) : PathFindingModule(parent) { initSettings(); }

QVector<PFMSetting> SketchScribblePFM::defineSettings() const {
    return {
        {"lines", "Lines", SettingType::Integer, 1000, QVariant(), 10, 5000, 10, 5000, 10},
        {"length", "Length", SettingType::Number, 50.0, QVariant(), 5.0, 200.0, 5.0, 200.0, 1.0}
    };
}

QVector<DrawingGeometry> SketchScribblePFM::_process(const cv::Mat& image) {
    int lines = m_settings["lines"].toInt();
    float length = m_settings["length"].toDouble();
    int w = image.cols;
    int h = image.rows;
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - image.at<float>(y, x);
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < lines; ++i) {
        if (isCancelled()) break;
        if (i % 50 == 0) emitProgress(float(i) / lines, geoms.size(), "Sketch Scribble...");
        int idx = weightedChoice(probs);
        float cx = idx % w;
        float cy = idx / w;
        Path path;
        int pts = int(randUniform(3, 8));
        for (int j = 0; j < pts; ++j) {
            path.push_back({cx + randUniform(-length, length), cy + randUniform(-length, length)});
        }
        DrawingGeometry dg; dg.path = catmull_rom_chain(path); geoms.push_back(dg);
    }
    return geoms;
}


// Original File: ../cpp/pfm\sketch_shapes.cpp

SketchShapesPFM::SketchShapesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchShapesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,0.1,1.0,0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,999999,0,999999,1 },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "shape_min_size",      "Shape Min Size",      SettingType::Integer,    2,    {}, 1,100,1,50,1,{},"Shapes" },
        { "shape_max_size",      "Shape Max Size",      SettingType::Integer,    40,   {}, 2,500,2,200,1,{},"Shapes" },
        { "size_tests",          "Size Tests",          SettingType::Integer,    10,   {}, 1,100,1,50,1,{},"Shapes" },
        { "enable_circles",      "Enable Circles",      SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "enable_squares",      "Enable Squares",      SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "enable_triangles",    "Enable Triangles",    SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "enable_stars",        "Enable Stars",        SettingType::Boolean,   true,  {}, 0,1,0,1,1,{},"Shapes" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

QVector<DrawingGeometry> SketchShapesPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    auto edgeMaps = sketch_utils::buildEdgeMaps(workImg, get("edge_power").toDouble(), 0.0, 0.0);

    const double edgePower   = get("edge_power").toDouble();
    const double lumPower    = get("luminance_power").toDouble() / 100.0;
    const double lineDensity = get("line_density").toDouble();
    const int    maxLimit    = get("line_max_limit").toInt();
    const int    shMin       = std::max(1, get("shape_min_size").toInt());
    const int    shMax       = std::max(2, get("shape_max_size").toInt());
    const int    sizeTests   = std::max(1, get("size_tests").toInt());
    const double eraseMin    = get("erase_min").toDouble();
    const double eraseMax    = get("erase_max").toDouble();
    const double tone        = get("tone").toDouble() / 100.0;

    // Build enabled shape list
    QStringList shapes;
    if (get("enable_circles").toBool())   shapes << "circle";
    if (get("enable_squares").toBool())   shapes << "square";
    if (get("enable_triangles").toBool()) shapes << "triangle";
    if (get("enable_stars").toBool())     shapes << "star";
    if (shapes.isEmpty()) shapes << "circle";

    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    std::vector<double> weights(W * H);
    QVector<DrawingGeometry> geometries;
    int iteration = 0;

    while (iteration < W * H && !isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (maxLimit > 0 && (int)geometries.size() >= maxLimit) break;

        float* lp = lightened.ptr<float>(0);
        double darkSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int idx = weightedChoice(weights);
        int cy = idx / W, cx = idx % W;

        // Pick shape type and rotation randomly
        QString shapeType = shapes[randInt(0, shapes.size() - 1)];
        double rotation   = randUniform(0.0, 360.0);

        double bestScore = -std::numeric_limits<double>::infinity();
        double bestSize  = shMin;
        for (int t = 0; t < sizeTests; ++t) {
            double testSize = randUniform(shMin, shMax);
            double hs = testSize / 2.0;
            int x0 = std::max(0, (int)(cx - hs)), x1 = std::min(W - 1, (int)(cx + hs));
            int y0 = std::max(0, (int)(cy - hs)), y1 = std::min(H - 1, (int)(cy + hs));
            if (x1 <= x0 || y1 <= y0) continue;
            cv::Rect roi(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
            double score = -cv::mean(lightened(roi))[0] * lumPower;
            if (!edgeMaps.edge.empty() && edgePower > 0)
                score += cv::mean(edgeMaps.edge(roi))[0] * edgePower * 255.0;
            if (score > bestScore) { bestScore = score; bestSize = testSize; }
        }

        double r = bestSize / 2.0;
        Path rawPath;
        if      (shapeType == "circle")   rawPath = generate_circle(cx, cy, r, std::max(12, (int)(r * 2)));
        else if (shapeType == "square")   rawPath = generate_rectangle(cx, cy, bestSize, bestSize, rotation);
        else if (shapeType == "triangle") rawPath = generate_polygon(cx, cy, r, 3, rotation);
        else if (shapeType == "star")     rawPath = generate_star(cx, cy, r, r * 0.4, 5, rotation);
        else                               rawPath = generate_circle(cx, cy, r, 12);

        Path scaledPath;
        scaledPath.reserve(rawPath.size());
        for (auto& p : rawPath) scaledPath.push_back({ p.first * sx, p.second * sy });
        geometries.append(DrawingGeometry{ scaledPath, 0 });

        double eraseVal = eraseMin + (eraseMax - eraseMin) * tone;
        sketch_utils::eraseRect(lightened, (int)(cx - r), (int)(cy - r),
                                (int)(cx + r), (int)(cy + r), eraseVal, W, H);

        if (++iteration % 50 == 0)
            emitProgress((float)dp, geometries.size(),
                         QString("Shapes %1 %2%").arg(geometries.size()).arg((int)(dp*100)));
    }
    return geometries;
}


// Original File: ../cpp/pfm\sketch_sobel_edges_pfm.cpp

SketchSobelEdgesPFM::SketchSobelEdgesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchSobelEdgesPFM::defineSettings() const {
    return SketchLinesPFM().settingsList();
}

QVector<DrawingGeometry> SketchSobelEdgesPFM::_process(const cv::Mat& image) {
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::Mat mag;
    cv::magnitude(gx, gy, mag);
    double maxVal;
    cv::minMaxLoc(mag, nullptr, &maxVal);
    cv::Mat edge_img;
    mag.convertTo(edge_img, CV_32F, 255.0 / (maxVal + 1e-6));
    edge_img = 255.0 - edge_img;

    SketchLinesPFM sketch;
    for (auto it = m_settings.constBegin(); it != m_settings.constEnd(); ++it) {
        sketch.set(it.key(), it.value().currentValue());
    }
    connect(&sketch, &PathFindingModule::progressUpdate, this, &PathFindingModule::progressUpdate);
    return sketch.process(edge_img);
}


// Original File: ../cpp/pfm\sketch_spirals.cpp

SketchSpiralsPFM::SketchSpiralsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchSpiralsPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,   {}, 0.05, 2.0,   0.1,  1.0,  0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,    {}, 0,    999999, 0,    999999, 1 },
        { "max_radius",          "Max Radius",          SettingType::Number,  500.0, {}, 10.0, 5000.0, 10.0, 1000.0, 10.0 },
        { "density",             "Density",             SettingType::Number,  1.0,   {}, 0.1,  5.0,    0.1,  3.0,    0.1 },
    };
}

QVector<DrawingGeometry> SketchSpiralsPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const double maxR   = get("max_radius").toDouble();
    const double density = get("density").toDouble();

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    const double cx = w / 2.0, cy = h / 2.0;
    Path path;
    double angle = 0.0, r = 0.0;

    while (r < maxR && !isCancelled()) {
        double x = cx + std::cos(angle) * r;
        double y = cy + std::sin(angle) * r;

        double dr;
        if (x >= 0 && x < w && y >= 0 && y < h) {
            float b = imgF.at<float>((int)y, (int)x) / 255.0f;
            path.push_back({ x, y });
            dr = (0.1 + (double)b * 2.0) * density;
        } else {
            dr = 1.0 * density;
        }
        r     += dr;
        angle += 0.1;

        if (path.size() % 1000 == 0)
            emitProgress((float)(r / maxR), 1, QString("Spiral r=%1").arg(r, 0, 'f', 1));
    }

    return { DrawingGeometry{ path, 0 } };
}


// Original File: ../cpp/pfm\sketch_squares.cpp

SketchSquaresPFM::SketchSquaresPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchSquaresPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,     1.0,  {}, 0.05,2.0,   0.1, 1.0, 0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer,    42,   {}, 0,   999999, 0,   999999,1 },
        { "edge_power",          "Edge Power",          SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "sobel_power",         "Sobel Power",         SettingType::Number,     0.0,  {}, 0,200,0,100,1,{},"Style" },
        { "luminance_power",     "Luminance Power",     SettingType::Number,   100.0,  {}, 0,200,0,200,1,{},"Style" },
        { "line_density",        "Line Density",        SettingType::Percentage, 75.0, {}, 0,100,0,100,1,{},"Segments" },
        { "line_max_limit",      "Line Max Limit",      SettingType::Integer,   -1,    {}, -1,1000000,-1,1000000,1,{},"Segments" },
        { "square_min_size",     "Square Min Size",     SettingType::Integer,    1,    {}, 1,100,1,50,1,{},"Squares" },
        { "square_max_size",     "Square Max Size",     SettingType::Integer,    40,   {}, 2,500,2,200,1,{},"Squares" },
        { "size_tests",          "Size Tests",          SettingType::Integer,    10,   {}, 1,100,1,50,1,{},"Squares" },
        { "erase_min",           "Erase Min",           SettingType::Integer,    50,   {}, 0,255,0,255,1,{},"Erasing" },
        { "erase_max",           "Erase Max",           SettingType::Integer,    125,  {}, 0,255,0,255,1,{},"Erasing" },
        { "tone",                "Tone",                SettingType::Percentage, 50.0, {}, 0,100,0,100,1,{},"Erasing" },
    };
}

QVector<DrawingGeometry> SketchSquaresPFM::_process(const cv::Mat& image) {
    // Apply plotting resolution
    cv::Mat workImg;
    double plotRes = get("plotting_resolution").toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }
    const int W = workImg.cols, H = workImg.rows;
    cv::Mat lightened;
    workImg.convertTo(lightened, CV_32F);

    auto edgeMaps = sketch_utils::buildEdgeMaps(workImg,
        get("edge_power").toDouble(), 0.0, 0.0);

    const double edgePower   = get("edge_power").toDouble();
    const double lumPower    = get("luminance_power").toDouble() / 100.0;
    const double lineDensity = get("line_density").toDouble();
    const int    maxLimit    = get("line_max_limit").toInt();
    const int    sqMin       = std::max(1, get("square_min_size").toInt());
    const int    sqMax       = std::max(2, get("square_max_size").toInt());
    const int    sizeTests   = std::max(1, get("size_tests").toInt());
    const double eraseMin    = get("erase_min").toDouble();
    const double eraseMax    = get("erase_max").toDouble();
    const double tone        = get("tone").toDouble() / 100.0;

    const double initBright   = cv::mean(lightened)[0];
    const double targetBright = initBright + (255.0 - initBright) * (lineDensity / 100.0);
    const double sx = (double)image.cols / W, sy = (double)image.rows / H;

    std::vector<double> weights(W * H);
    QVector<DrawingGeometry> geometries;
    int iteration = 0;

    while (iteration < W * H && !isCancelled()) {
        double dp = sketch_utils::densityProgress(lightened, initBright, targetBright);
        if (dp >= 1.0) break;
        if (maxLimit > 0 && (int)geometries.size() >= maxLimit) break;

        // Weighted random sample
        float* lp = lightened.ptr<float>(0);
        double darkSum = 0.0;
        for (int i = 0; i < W * H; ++i) {
            double d = std::max(0.0, 255.0 - (double)lp[i]);
            weights[i] = d; darkSum += d;
        }
        if (darkSum < 1.0) break;

        int idx = weightedChoice(weights);
        int cy = idx / W, cx = idx % W;

        // Test different sizes
        double bestScore = -std::numeric_limits<double>::infinity();
        double bestSize  = sqMin;

        for (int t = 0; t < sizeTests; ++t) {
            double testSize = randUniform(sqMin, sqMax);
            double hs = testSize / 2.0;
            int x0 = std::max(0, (int)(cx - hs)), x1 = std::min(W - 1, (int)(cx + hs));
            int y0 = std::max(0, (int)(cy - hs)), y1 = std::min(H - 1, (int)(cy + hs));
            if (x1 <= x0 || y1 <= y0) continue;

            // Mean brightness in region
            cv::Rect roi(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
            double regionMean = cv::mean(lightened(roi))[0];
            double score = -regionMean * lumPower;

            if (!edgeMaps.edge.empty() && edgePower > 0) {
                double edgeMean = cv::mean(edgeMaps.edge(roi))[0];
                score += edgeMean * edgePower * 255.0;
            }
            if (score > bestScore) { bestScore = score; bestSize = testSize; }
        }

        // Draw square
        double hs = bestSize / 2.0;
        Path path = {
            { (cx - hs) * sx, (cy - hs) * sy },
            { (cx + hs) * sx, (cy - hs) * sy },
            { (cx + hs) * sx, (cy + hs) * sy },
            { (cx - hs) * sx, (cy + hs) * sy },
            { (cx - hs) * sx, (cy - hs) * sy }
        };
        geometries.append(DrawingGeometry{ path, 0 });

        // Erase
        double eraseVal = eraseMin + (eraseMax - eraseMin) * tone;
        sketch_utils::eraseRect(lightened, (int)(cx - hs), (int)(cy - hs),
                                (int)(cx + hs), (int)(cy + hs), eraseVal, W, H);

        if (++iteration % 50 == 0)
            emitProgress((float)dp, geometries.size(),
                         QString("Squares %1 shapes %2%").arg(geometries.size()).arg((int)(dp*100)));
    }
    return geometries;
}


// Original File: ../cpp/pfm\sketch_superformula_pfm.cpp

// -------------------------------------------------------------------------

SketchSuperformulaPFM::SketchSuperformulaPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchSuperformulaPFM::defineSettings() const {
    return {
        {"shapes", "Shapes", SettingType::Integer, 100, QVariant(), 10, 1000, 10, 1000, 10},
        {"size", "Size", SettingType::Number, 20.0, QVariant(), 5.0, 100.0, 5.0, 100.0, 1.0},
        {"m", "M (Symmetry)", SettingType::Number, 6.0, QVariant(), 0.0, 20.0, 0.0, 20.0, 0.1},
        {"n1", "N1", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1},
        {"n2", "N2", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1},
        {"n3", "N3", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1}
    };
}

QVector<DrawingGeometry> SketchSuperformulaPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int shapes = m_settings["shapes"].toInt();
    double size = m_settings["size"].toDouble();
    double m = m_settings["m"].toDouble();
    double n1 = m_settings["n1"].toDouble();
    double n2 = m_settings["n2"].toDouble();
    double n3 = m_settings["n3"].toDouble();

    // Darkness weights
    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }

    if (sum < 1.0) return {};
    for (auto& p : probs) p /= sum;

    QVector<DrawingGeometry> geoms;
    int steps = 100;
    
    for (int i = 0; i < shapes; ++i) {
        if (isCancelled()) break;
        emitProgress(float(i) / shapes, geoms.size(), "Generating Superformulas...");

        int idx = weightedChoice(probs);
        int cx = idx % w;
        int cy = idx / w;

        Path path;
        double local_size = size * (probs[idx] * sum / 255.0); // Recover approximate darkness 0-1

        for (int step = 0; step <= steps; ++step) {
            double theta = (double(step) / steps) * 2.0 * M_PI * 2.0;

            double t1 = std::pow(std::abs(std::cos(m * theta / 4.0)), n2);
            double t2 = std::pow(std::abs(std::sin(m * theta / 4.0)), n3);
            double r = 0.0;
            if ((t1 + t2) != 0.0) {
                r = std::pow(t1 + t2, -1.0 / n1);
            }

            double x = cx + r * std::cos(theta) * local_size;
            double y = cy + r * std::sin(theta) * local_size;
            path.push_back({x, y});
        }
        
        DrawingGeometry dg;
        dg.path = path;
        geoms.push_back(dg);
    }

    return geoms;
}


// Original File: ../cpp/pfm\sketch_sweeping_curves_pfm.cpp

// -------------------------------------------------------------------------
SketchSweepingCurvesPFM::SketchSweepingCurvesPFM(QObject* parent) : SketchCubicBeziers2PFM(parent) {}

QString SketchSweepingCurvesPFM::name() const { return "Sketch Sweeping Curves"; }


// Original File: ../cpp/pfm\sketch_voronoi_pfm.cpp

// -------------------------------------------------------------------------
SketchVoronoiPFM::SketchVoronoiPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchVoronoiPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 500, QVariant(), 10, 5000, 10, 5000, 10}
    };
}

QVector<DrawingGeometry> SketchVoronoiPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : pts) subdiv.insert(p);

    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);

    QVector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        for (size_t i = 0; i < facet.size() - 1; ++i) {
            DrawingGeometry dg;
            dg.path = {{facet[i].x, facet[i].y}, {facet[i+1].x, facet[i+1].y}};
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\sketch_waves.cpp

SketchWavesPFM::SketchWavesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SketchWavesPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05, 2.0,  0.1, 1.0,  0.05, {}, "Default", "Scale factor applied to the image before processing." },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,    999999,0,   999999,1,   {}, "Default" },
        { "lines_count",         "Lines Count",         SettingType::Integer, 100,  {}, 10,   1000,  10,  500,   10 },
        { "wave_freq",           "Frequency",           SettingType::Number,  10.0, {}, 1.0,  100.0, 1.0, 50.0,  1.0 },
        { "wave_amp",            "Max Amplitude",       SettingType::Number,  20.0, {}, 1.0,  100.0, 1.0, 50.0,  1.0 },
    };
}

QVector<DrawingGeometry> SketchWavesPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const int   linesCount = get("lines_count").toInt();
    const double freq      = get("wave_freq").toDouble();
    const double maxAmp    = get("wave_amp").toDouble();

    QVector<DrawingGeometry> geometries;
    geometries.reserve(linesCount);

    const float* imgData = reinterpret_cast<const float*>(image.data);
    // Convert once to float for fast access
    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    for (int i = 0; i < linesCount && !isCancelled(); ++i) {
        double yBase = (double)i / std::max(1, linesCount - 1) * h;
        Path path;
        path.reserve(w);

        for (int x = 0; x < w; ++x) {
            int yi = (int)std::min((double)(h - 1), yBase);
            float brightness = imgF.at<float>(yi, x) / 255.0f;
            double amp = maxAmp * (1.0 - brightness);
            double y = yBase + std::sin((double)x / w * M_PI * 2.0 * freq) * amp;
            path.push_back({ (double)x, y });
        }

        geometries.append(DrawingGeometry{ path, 0 });
        if ((i + 1) % 10 == 0)
            emitProgress((float)(i + 1) / linesCount, geometries.size(),
                         QString("Waves %1/%2").arg(i + 1).arg(linesCount));
    }
    return geometries;
}


// Original File: ../cpp/pfm\spiral_circular_scribbles_pfm.cpp

SpiralCircularScribblesPFM::SpiralCircularScribblesPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SpiralCircularScribblesPFM::defineSettings() const {
    return {
        {"pitch", "Pitch", SettingType::Number, 3.0, QVariant(), 1.0, 20.0, 1.0, 20.0, 0.5},
        {"wobble", "Wobble", SettingType::Number, 2.0, QVariant(), 0.0, 15.0, 0.0, 15.0, 0.5}
    };
}

QVector<DrawingGeometry> SpiralCircularScribblesPFM::_process(const cv::Mat& image) {
    float pitch = m_settings["pitch"].toDouble();
    float wobble = m_settings["wobble"].toDouble();
    int w = image.cols;
    int h = image.rows;
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float max_r = std::hypot(cx, cy);
    
    Path path;
    float theta = 0.0f;
    float r = 0.0f;
    
    QVector<DrawingGeometry> geoms;
    while (r < max_r) {
        if (isCancelled()) break;
        if (int(theta * 100) % 500 == 0) emitProgress(r / max_r, 0, "Spiral Circular Scribbles...");
        
        int xi = std::clamp(int(cx + r * std::cos(theta)), 0, w - 1);
        int yi = std::clamp(int(cy + r * std::sin(theta)), 0, h - 1);
        float dark = (255.0f - image.at<float>(yi, xi)) / 255.0f;
        
        float disp = std::sin(theta * 5.0f) * wobble * dark;
        float x = cx + (r + disp) * std::cos(theta);
        float y = cy + (r + disp) * std::sin(theta);
        
        path.push_back({x, y});
        theta += 0.12f;
        r = theta * pitch / (2.0f * M_PI);
    }
    
    if (path.size() > 1) {
        DrawingGeometry dg; dg.path = path;
        geoms.push_back(dg);
    }
    return geoms;
}


// Original File: ../cpp/pfm\spiral_sawtooth_pfm.cpp


SpiralSawtoothPFM::SpiralSawtoothPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> SpiralSawtoothPFM::defineSettings() const {
    return {
        {"pitch", "Pitch", SettingType::Number, 4.0, QVariant(), 1.0, 30.0, 1.0, 30.0, 0.5},
        {"teeth", "Teeth", SettingType::Integer, 8, QVariant(), 2, 64, 2, 64, 1}
    };
}

QVector<DrawingGeometry> SpiralSawtoothPFM::_process(const cv::Mat& image) {
    float pitch = m_settings["pitch"].toDouble();
    int teeth = m_settings["teeth"].toInt();
    int w = image.cols;
    int h = image.rows;
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float max_r = std::hypot(cx, cy);
    
    Path path;
    float theta = 0.0f;
    float r = 0.0f;
    
    QVector<DrawingGeometry> geoms;
    while (r < max_r) {
        if (isCancelled()) break;
        if (int(theta * 100) % 500 == 0) emitProgress(r / max_r, 0, "Spiral Sawtooth...");
        
        float tooth = std::sin(theta * teeth) * pitch * 0.3f;
        float x = cx + (r + tooth) * std::cos(theta);
        float y = cy + (r + tooth) * std::sin(theta);
        
        path.push_back({x, y});
        theta += 0.15f;
        r = theta * pitch / (2.0f * M_PI);
    }
    
    if (path.size() > 1) {
        DrawingGeometry dg; dg.path = path;
        geoms.push_back(dg);
    }
    return geoms;
}


// Original File: ../cpp/pfm\stipple_chaos_pfm.cpp

Path StippleChaosPFM::generateShape(float cx, float cy, float r) {
    int pts = int(randUniform(0, 4)) + 3;
    return generate_polygon(cx, cy, r, pts, randUniform(0, 360));
}


// Original File: ../cpp/pfm\stipple_circles_pfm.cpp

Path StippleCirclesPFM::generateShape(float cx, float cy, float r) { return generate_circle(cx, cy, r, 12); }


// Original File: ../cpp/pfm\stipple_crosses_pfm.cpp

Path StippleCrossesPFM::generateShape(float cx, float cy, float r) {
    return {{cx-r, cy}, {cx+r, cy}, {cx, cy}, {cx, cy-r}, {cx, cy+r}};
}


// Original File: ../cpp/pfm\stipple_dots.cpp

StippleDotsPFM::StippleDotsPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> StippleDotsPFM::defineSettings() const {
    return {
        { "plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  {}, 0.05, 2.0,   0.1,  1.0,   0.05 },
        { "random_seed",         "Random Seed",         SettingType::Integer, 42,   {}, 0,    999999, 0,    999999, 1 },
        { "num_dots",            "Number of Dots",      SettingType::Integer, 5000, {}, 100,  50000,  100,  20000,  100 },
        { "dot_size",            "Dot Radius",          SettingType::Number,  1.0,  {}, 0.1,  10.0,   0.1,  5.0,    0.1 },
    };
}

QVector<DrawingGeometry> StippleDotsPFM::_process(const cv::Mat& image) {
    const int h = image.rows, w = image.cols;
    const int    numDots = get("num_dots").toInt();
    const double dotR    = get("dot_size").toDouble();

    cv::Mat imgF;
    image.convertTo(imgF, CV_32F);

    // Build darkness weights
    std::vector<double> weights(h * w);
    const float* fp = imgF.ptr<float>(0);
    double total = 0.0;
    for (int i = 0; i < h * w; ++i) {
        double d = std::max(0.0, 255.0 - (double)fp[i]);
        weights[i] = d;
        total += d;
    }
    if (total < 1.0) return {};

    // Place dots via weighted sampling
    QVector<DrawingGeometry> geometries;
    geometries.reserve(numDots);

    for (int i = 0; i < numDots && !isCancelled(); ++i) {
        int idx = weightedChoice(weights);
        int cy = idx / w, cx = idx % w;

        Path circle = generate_circle((double)cx, (double)cy, dotR, 6);
        geometries.append(DrawingGeometry{ circle, 0 });

        if (i % 500 == 0)
            emitProgress((float)i / numDots, geometries.size(), "Stippling...");
    }
    return geometries;
}


// Original File: ../cpp/pfm\stipple_hexagons_pfm.cpp

Path StippleHexagonsPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 6, 0); }


// Original File: ../cpp/pfm\stipple_layers_pfm.cpp

// ---------------------------------------------------------------------------

StippleLayersPFM::StippleLayersPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> StippleLayersPFM::defineSettings() const {
    return {
        {"plotting_resolution", "Plotting Resolution", SettingType::Number,  1.0,  QVariant(), 0.1, 2.0,   0.1,   2.0,   0.05},
        {"random_seed",         "Random Seed",         SettingType::Integer, 42,   QVariant(), 0,   999999, 0,     999999, 1},
        {"layers",              "Layers",              SettingType::Integer, 4,    QVariant(), 2,   8,      2,     8,      1},
        {"dots_per_layer",      "Dots per Layer",      SettingType::Integer, 1500, QVariant(), 100, 20000,  100,   20000,  100},
    };
}

QVector<DrawingGeometry> StippleLayersPFM::_process(const cv::Mat& image) {
    cv::Mat workImg;
    double plotRes = m_settings["plotting_resolution"].toDouble();
    if (std::abs(plotRes - 1.0) > 1e-4) {
        int nw = std::max(1, (int)(image.cols * plotRes));
        int nh = std::max(1, (int)(image.rows * plotRes));
        cv::resize(image, workImg, cv::Size(nw, nh), 0, 0, cv::INTER_AREA);
    } else {
        workImg = image.clone();
    }

    int n = m_settings["layers"].toInt();
    int dotsPerLayer = m_settings["dots_per_layer"].toInt();
    QVector<DrawingGeometry> geoms;

    float step = 255.0f / n;
    for (int layer = 0; layer < n && !isCancelled(); ++layer) {
        float lo = layer * step;
        float hi = (layer + 1) * step;

        cv::Mat layerImg = cv::Mat::ones(workImg.size(), CV_8UC1) * 255;
        for (int y = 0; y < workImg.rows; ++y) {
            const uchar* src = workImg.ptr<uchar>(y);
            uchar* dst = layerImg.ptr<uchar>(y);
            for (int x = 0; x < workImg.cols; ++x) {
                float v = (float)src[x];
                if (v >= lo && v < hi) dst[x] = src[x];
            }
        }

        StippleDotsPFM stipple;
        stipple.set("num_dots", dotsPerLayer);
        stipple.set("dot_size", 1.0 + layer * 0.3);

        auto layerGeoms = stipple.process(layerImg);
        for (auto& g : layerGeoms) {
            g.penIndex = layer;
            geoms.append(g);
        }
        emitProgress(float(layer + 1) / n, geoms.size(),
                     QString("Stipple layer %1/%2...").arg(layer + 1).arg(n));
    }
    return geoms;
}


// Original File: ../cpp/pfm\stipple_lines_pfm.cpp

Path StippleLinesPFM::generateShape(float cx, float cy, float r) {
    return {{cx-r, cy-r}, {cx+r, cy+r}};
}


// Original File: ../cpp/pfm\stipple_squares_pfm.cpp

Path StippleSquaresPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 4, 45); }


// Original File: ../cpp/pfm\stipple_stars_pfm.cpp

Path StippleStarsPFM::generateShape(float cx, float cy, float r) { 
    Path p1 = generate_polygon(cx, cy, r, 5, 0);
    Path p2 = generate_polygon(cx, cy, r*0.5f, 5, 36);
    p1.insert(p1.end(), p2.begin(), p2.end());
    return p1;
}


// Original File: ../cpp/pfm\stipple_triangles_pfm.cpp

Path StippleTrianglesPFM::generateShape(float cx, float cy, float r) { return generate_polygon(cx, cy, r, 3, 30); }


// Original File: ../cpp/pfm\stipple_variable_circles_pfm.cpp

Path StippleVariableCirclesPFM::generateShape(float cx, float cy, float r) {
    return generate_circle(cx, cy, r * (0.5 + randUniform(0, 1)), 8);
}


// Original File: ../cpp/pfm\stipple_variable_squares_pfm.cpp

Path StippleVariableSquaresPFM::generateShape(float cx, float cy, float r) {
    return generate_polygon(cx, cy, r * (0.5 + randUniform(0, 1)), 4, randUniform(0, 90));
}


// Original File: ../cpp/pfm\streamlines_edge_field_pfm.cpp

QVector<DrawingGeometry> StreamlinesEdgeFieldPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(7, 7), 0);
    cv::GaussianBlur(gy, gy, cv::Size(7, 7), 0);
    
    // For Edge field, flow is perpendicular to gradient
    cv::Mat fx = -gy;
    cv::Mat fy = gx;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Edge Field Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// Original File: ../cpp/pfm\streamlines_flow_field_pfm.cpp

QVector<DrawingGeometry> StreamlinesFlowFieldPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(5, 5), 0);
    cv::GaussianBlur(gy, gy, cv::Size(5, 5), 0);
    
    // For Flow field, flow is along gradient
    cv::Mat fx = gx;
    cv::Mat fy = gy;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Flow Field Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// Original File: ../cpp/pfm\streamlines_superformula_pfm.cpp

QVector<PFMSetting> StreamlinesSuperformulaPFM::defineSettings() const {
    auto s = BaseStreamlinePFM::defineSettings();
    s.push_back({"wave_amp", "Wave Amplitude", SettingType::Number, 2.0, QVariant(), 0.0, 20.0, 0.0, 20.0, 0.5});
    return s;
}

QVector<DrawingGeometry> StreamlinesSuperformulaPFM::_process(const cv::Mat& image) {
    int count = m_settings["line_count"].toInt();
    int max_len = m_settings["max_length"].toInt();
    float step = m_settings["step_size"].toDouble();
    float amp = m_settings["wave_amp"].toDouble();
    
    cv::Mat gx, gy;
    cv::Sobel(image, gx, CV_32F, 1, 0, 3);
    cv::Sobel(image, gy, CV_32F, 0, 1, 3);
    cv::GaussianBlur(gx, gx, cv::Size(3, 3), 0);
    cv::GaussianBlur(gy, gy, cv::Size(3, 3), 0);
    
    cv::Mat fx = gx;
    cv::Mat fy = gy;
    
    QVector<DrawingGeometry> geoms;
    for (int i = 0; i < count; ++i) {
        if (isCancelled()) break;
        if (i % 20 == 0) emitProgress(float(i) / count, geoms.size(), "Superformula Streamlines...");
        float x = randUniform(0, image.cols);
        float y = randUniform(0, image.rows);
        Path path = trace_streamline(fx, fy, x, y, max_len, step);
        if (path.size() >= 2 && amp > 0) {
            Path waved;
            for (size_t j = 0; j < path.size(); ++j) {
                float wx = path[j].first + std::sin(j * 0.2f) * amp;
                float wy = path[j].second + std::cos(j * 0.15f) * amp;
                waved.push_back({wx, wy});
            }
            path = waved;
        }
        if (path.size() >= 2) { DrawingGeometry dg; dg.path = path; geoms.push_back(dg); }
    }
    return geoms;
}


// Original File: ../cpp/pfm\tspmstpfm.cpp

// -------------------------------------------------------------------------
TSPMSTPFM::TSPMSTPFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> TSPMSTPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 3000, QVariant(), 100, 20000, 100, 20000, 100}
    };
}

QVector<DrawingGeometry> TSPMSTPFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int nodes = m_settings["nodes"].toInt();

    std::vector<double> probs(w * h, 0.0);
    double sum = 0.0;
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            if (d > 0) {
                probs[y * w + x] = d;
                sum += d;
            }
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> points;
    for (int i = 0; i < nodes; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        points.push_back(cv::Point2f(idx % w, idx / w));
    }

    emitProgress(0.2f, 0, "Delaunay Triangulation...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, w, h));
    for (const auto& p : points) subdiv.insert(p);
    
    std::vector<cv::Vec4f> edgeList;
    subdiv.getEdgeList(edgeList);

    // Map exact coordinates to indices
    std::map<std::pair<float, float>, int> pt2idx;
    for (size_t i = 0; i < points.size(); ++i) {
        pt2idx[{points[i].x, points[i].y}] = i;
    }

    std::vector<std::vector<std::pair<int, float>>> adj(points.size());
    for(const auto& e : edgeList) {
        auto it1 = pt2idx.find({e[0], e[1]});
        auto it2 = pt2idx.find({e[2], e[3]});
        if (it1 != pt2idx.end() && it2 != pt2idx.end()) {
            int u = it1->second;
            int v = it2->second;
            float d = std::hypot(e[0]-e[2], e[1]-e[3]);
            adj[u].push_back({v, d});
            adj[v].push_back({u, d});
        }
    }

    emitProgress(0.6f, 0, "Solving Minimum Spanning Tree...");
    
    std::vector<bool> in_mst(points.size(), false);
    std::vector<float> min_weight(points.size(), 1e12f);
    std::vector<int> parent(points.size(), -1);
    
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
    pq.push({0.0f, 0});
    min_weight[0] = 0.0f;
    
    QVector<DrawingGeometry> geoms;
    
    while(!pq.empty()) {
        if (isCancelled()) return {};
        int u = pq.top().second;
        pq.pop();
        
        if (in_mst[u]) continue;
        in_mst[u] = true;
        
        if (parent[u] != -1) {
            DrawingGeometry dg;
            dg.path = {{points[parent[u]].x, points[parent[u]].y}, {points[u].x, points[u].y}};
            geoms.push_back(dg);
        }
        
        for (const auto& edge : adj[u]) {
            int v = edge.first;
            float weight = edge.second;
            if (!in_mst[v] && weight < min_weight[v]) {
                min_weight[v] = weight;
                parent[v] = u;
                pq.push({weight, v});
            }
        }
    }
    
    return geoms;
}


// Original File: ../cpp/pfm\tsp_classic_pfm.cpp

// -------------------------------------------------------------------------
std::vector<double> TSPClassicPFM::getProbabilities(const cv::Mat& image) {
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            double d = 255.0 - row[x];
            if (d > 0) probs[y * image.cols + x] = d;
        }
    }
    return probs;
}


// Original File: ../cpp/pfm\tsp_outline_pfm.cpp

// -------------------------------------------------------------------------
QVector<PFMSetting> TSPOutlinePFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, QVariant(), 100, 20000, 100, 20000, 100},
        {"edge_thresh1", "Canny Thresh 1", SettingType::Integer, 100, QVariant(), 0, 255, 0, 255, 10},
        {"edge_thresh2", "Canny Thresh 2", SettingType::Integer, 200, QVariant(), 0, 255, 0, 255, 10}
    };
}

std::vector<double> TSPOutlinePFM::getProbabilities(const cv::Mat& image) {
    cv::Mat u8, edges;
    image.convertTo(u8, CV_8U);
    cv::Canny(u8, edges, m_settings["edge_thresh1"].toInt(), m_settings["edge_thresh2"].toInt());

    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = edges.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            if (row[x] > 0) probs[y * image.cols + x] = row[x];
        }
    }
    return probs;
}


// Original File: ../cpp/pfm\tsp_shading_pfm.cpp

// -------------------------------------------------------------------------
QVector<PFMSetting> TSPShadingPFM::defineSettings() const {
    return {
        {"nodes", "Nodes", SettingType::Integer, 2000, QVariant(), 100, 20000, 100, 20000, 100},
        {"threshold", "Darkness Cutoff", SettingType::Number, 25.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0}
    };
}

std::vector<double> TSPShadingPFM::getProbabilities(const cv::Mat& image) {
    float thresh = (m_settings["threshold"].toDouble() / 100.0) * 255.0;
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            float d = 255.0 - row[x];
            if (d >= thresh) probs[y * image.cols + x] = d;
        }
    }
    return probs;
}


// Original File: ../cpp/pfm\tsp_stipple_pfm.cpp

// -------------------------------------------------------------------------
std::vector<double> TSPStipplePFM::getProbabilities(const cv::Mat& image) {
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            double d = 255.0 - row[x];
            // Only use pixels that are darker than 128 (stipple effect)
            if (d > 128.0) probs[y * image.cols + x] = d;
        }
    }
    return probs;
}


// Original File: ../cpp/pfm\tsp_voronoi_pfm.cpp

// -------------------------------------------------------------------------
TSPVoronoiPFM::TSPVoronoiPFM(QObject* parent) : BaseTSPPFM(parent) {}

QVector<PFMSetting> TSPVoronoiPFM::defineSettings() const {
    auto base = BaseTSPPFM::defineSettings();
    base.append({"voronoi_thresh", "Edge Threshold", SettingType::Integer, 80, QVariant(), 10, 255, 10, 255, 10});
    return base;
}

std::vector<double> TSPVoronoiPFM::getProbabilities(const cv::Mat& image) {
    int thresh = m_settings.contains("voronoi_thresh") ?
                 m_settings["voronoi_thresh"].toInt() : 80;
    cv::Mat edges;
    cv::Canny(image, edges, thresh, thresh * 2);
    std::vector<double> probs(image.cols * image.rows, 0.0);
    for (int y = 0; y < image.rows; ++y) {
        const uchar* row = edges.ptr<uchar>(y);
        for (int x = 0; x < image.cols; ++x) {
            if (row[x] > 0) probs[y * image.cols + x] = row[x];
        }
    }
    return probs;
}


// Original File: ../cpp/pfm\voronoi_circles_pfm.cpp

QVector<DrawingGeometry> VoronoiCirclesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 40 == 0) emitProgress(float(i) / pts.size(), geoms.size(), "Voronoi Circles...");
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        DrawingGeometry dg; dg.path = generate_circle(pts[i].x, pts[i].y, r, 16);
        geoms.push_back(dg);
    }
    return geoms;
}


// Original File: ../cpp/pfm\voronoi_dashes_pfm.cpp

QVector<DrawingGeometry> VoronoiDashesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    QVector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() < 2) continue;
        for (size_t i = 0; i < facet.size() - 1; ++i) {
            float x0 = facet[i].x, y0 = facet[i].y;
            float x1 = facet[i+1].x, y1 = facet[i+1].y;
            float mx = (x0 + x1) / 2.0f;
            float my = (y0 + y1) / 2.0f;
            float length = std::hypot(x1 - x0, y1 - y0) * 0.4f;
            float angle = std::atan2(y1 - y0, x1 - x0);
            DrawingGeometry dg;
            dg.path = {
                {mx - std::cos(angle) * length / 2.0f, my - std::sin(angle) * length / 2.0f},
                {mx + std::cos(angle) * length / 2.0f, my + std::sin(angle) * length / 2.0f}
            };
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\voronoi_diagram_pfm.cpp

QVector<DrawingGeometry> VoronoiDiagramPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<std::vector<cv::Point2f>> facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);
    QVector<DrawingGeometry> geoms;
    for (const auto& facet : facets) {
        if (facet.size() >= 2) {
            DrawingGeometry dg;
            for (const auto& pt : facet) dg.path.push_back({pt.x, pt.y});
            dg.path.push_back({facet[0].x, facet[0].y});
            geoms.push_back(dg);
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\voronoi_letters_pfm.cpp

// -------------------------------------------------------------------------
int VoronoiLettersPFM::getLloydIters() const {
    return m_settings["lloyd_iterations"].toInt();
}


// Original File: ../cpp/pfm\voronoi_shapes_pfm.cpp

QVector<DrawingGeometry> VoronoiShapesPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        float r = nearest_seed_radius(pts[i].x, pts[i].y, pts);
        int sides = 3 + (i % 5);
        DrawingGeometry dg; dg.path = generate_polygon(pts[i].x, pts[i].y, r, sides);
        geoms.push_back(dg);
    }
    return geoms;
}


// Original File: ../cpp/pfm\voronoi_stippling_pfm.cpp

QVector<DrawingGeometry> VoronoiStipplingPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < pts.size(); ++i) {
        if (isCancelled()) break;
        int xi = std::clamp(int(pts[i].x), 0, image.cols - 1);
        int yi = std::clamp(int(pts[i].y), 0, image.rows - 1);
        float dark = (255.0f - image.at<float>(yi, xi)) / 255.0f;
        float r = std::max(0.4f, nearest_seed_radius(pts[i].x, pts[i].y, pts) * 0.15f * dark);
        DrawingGeometry dg; dg.path = generate_circle(pts[i].x, pts[i].y, r, 8);
        geoms.push_back(dg);
    }
    return geoms;
}


// Original File: ../cpp/pfm\voronoi_tree_pfm.cpp

QVector<DrawingGeometry> VoronoiTreePFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    
    std::map<std::pair<float, float>, int> pt2idx;
    for (size_t i = 0; i < pts.size(); ++i) pt2idx[{pts[i].x, pts[i].y}] = i;
    
    std::vector<std::vector<std::pair<int, float>>> adj(pts.size());
    for (const auto& e : edges) {
        auto it1 = pt2idx.find({e[0], e[1]});
        auto it2 = pt2idx.find({e[2], e[3]});
        if (it1 != pt2idx.end() && it2 != pt2idx.end()) {
            int u = it1->second;
            int v = it2->second;
            float d = std::hypot(e[0]-e[2], e[1]-e[3]);
            adj[u].push_back({v, d});
            adj[v].push_back({u, d});
        }
    }
    
    std::vector<bool> in_mst(pts.size(), false);
    std::vector<float> min_weight(pts.size(), 1e12f);
    std::vector<int> parent(pts.size(), -1);
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<std::pair<float, int>>> pq;
    pq.push({0.0f, 0});
    min_weight[0] = 0.0f;
    
    QVector<DrawingGeometry> geoms;
    while(!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (in_mst[u]) continue;
        in_mst[u] = true;
        if (parent[u] != -1) {
            DrawingGeometry dg; dg.path = {{pts[parent[u]].x, pts[parent[u]].y}, {pts[u].x, pts[u].y}};
            geoms.push_back(dg);
        }
        for (const auto& edge : adj[u]) {
            int v = edge.first;
            float weight = edge.second;
            if (!in_mst[v] && weight < min_weight[v]) {
                min_weight[v] = weight;
                parent[v] = u;
                pq.push({weight, v});
            }
        }
    }
    return geoms;
}


// Original File: ../cpp/pfm\voronoi_triangulation_pfm.cpp

QVector<DrawingGeometry> VoronoiTriangulationPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    emitProgress(0.5, 0, "Delaunay...");
    cv::Subdiv2D subdiv(cv::Rect(0, 0, image.cols, image.rows));
    for (const auto& p : pts) subdiv.insert(p);
    std::vector<cv::Vec4f> edges;
    subdiv.getEdgeList(edges);
    QVector<DrawingGeometry> geoms;
    for (const auto& e : edges) {
        DrawingGeometry dg; dg.path = {{e[0], e[1]}, {e[2], e[3]}};
        geoms.push_back(dg);
    }
    return geoms;
}


// Original File: ../cpp/pfm\voronoi_tsppfm.cpp

QVector<DrawingGeometry> VoronoiTSPPFM::_process(const cv::Mat& image) {
    auto pts = getSeeds(image);
    if (pts.size() < 2) return {};
    Path path = solve_tsp_nn(pts, [this](){ return isCancelled(); });
    DrawingGeometry dg; dg.path = path;
    return {dg};
}


// Original File: ../cpp/pfm\_letters_base_pfm.cpp
static std::map<char, std::vector<std::pair<float, float>>> LETTER_GLYPHS = {
    {'A', {{0.1, 1}, {0.5, 0}, {0.9, 1}, {0.7, 0.55}, {0.3, 0.55}}},
    {'B', {{0.2, 0}, {0.2, 1}, {0.6, 1}, {0.8, 0.85}, {0.6, 0.5}, {0.8, 0.15}, {0.6, 0}, {0.2, 0}}},
    {'C', {{0.9, 0.15}, {0.6, 0}, {0.3, 0}, {0.1, 0.3}, {0.1, 0.7}, {0.3, 1}, {0.6, 1}, {0.9, 0.85}}},
    {'D', {{0.2, 0}, {0.2, 1}, {0.55, 1}, {0.85, 0.75}, {0.85, 0.25}, {0.55, 0}, {0.2, 0}}},
    {'E', {{0.8, 0}, {0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.65, 0.5}}},
    {'F', {{0.2, 0}, {0.2, 1}, {0.8, 1}, {0.2, 0.5}, {0.7, 0.5}}},
    {'G', {{0.9, 0.2}, {0.6, 0}, {0.3, 0.05}, {0.1, 0.4}, {0.1, 0.7}, {0.35, 1}, {0.75, 0.9}, {0.75, 0.55}, {0.5, 0.55}}},
    {'H', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.8, 0.5}, {0.8, 1}, {0.8, 0}}},
    {'I', {{0.35, 0}, {0.65, 0}, {0.5, 0}, {0.5, 1}, {0.35, 1}, {0.65, 1}}},
    {'J', {{0.7, 0}, {0.3, 0}, {0.3, 0.8}, {0.5, 1}, {0.75, 0.85}}},
    {'K', {{0.2, 0}, {0.2, 1}, {0.2, 0.5}, {0.85, 1}, {0.25, 0.5}, {0.85, 0}}},
    {'L', {{0.2, 1}, {0.2, 0}, {0.85, 0}}},
    {'M', {{0.1, 0}, {0.1, 1}, {0.5, 0.5}, {0.9, 1}, {0.9, 0}}},
    {'N', {{0.15, 0}, {0.15, 1}, {0.85, 0}, {0.85, 1}}},
    {'O', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.15, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}}},
    {'P', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}}},
    {'Q', {{0.5, 0}, {0.15, 0.15}, {0, 0.5}, {0.2, 0.85}, {0.5, 1}, {0.85, 0.85}, {1, 0.5}, {0.85, 0.15}, {0.5, 0}, {0.75, 0.25}, {1, 0}}},
    {'R', {{0.2, 0}, {0.2, 1}, {0.65, 1}, {0.85, 0.8}, {0.65, 0.5}, {0.2, 0.5}, {0.85, 0}}},
    {'S', {{0.85, 0.85}, {0.55, 1}, {0.25, 0.9}, {0.15, 0.65}, {0.45, 0.5}, {0.75, 0.35}, {0.85, 0.15}, {0.55, 0}, {0.25, 0.1}}},
    {'T', {{0, 1}, {1, 1}, {0.5, 1}, {0.5, 0}}},
    {'U', {{0.15, 1}, {0.15, 0.25}, {0.35, 0}, {0.65, 0}, {0.85, 0.25}, {0.85, 1}}},
    {'V', {{0, 1}, {0.5, 0}, {1, 1}}},
    {'W', {{0, 1}, {0.25, 0}, {0.5, 0.6}, {0.75, 0}, {1, 1}}},
    {'X', {{0, 0}, {1, 1}, {0, 1}, {1, 0}}},
    {'Y', {{0, 1}, {0.5, 0.45}, {1, 1}, {0.5, 0.45}, {0.5, 0}}},
    {'Z', {{0.1, 1}, {0.9, 1}, {0.1, 0}, {0.9, 0}}}
};

static Path letter_glyph_path(char letter, float cx, float cy, float size) {
    if (LETTER_GLYPHS.find(letter) == LETTER_GLYPHS.end()) letter = 'O';
    Path p;
    for (const auto& pt : LETTER_GLYPHS[letter]) {
        p.push_back({cx + (pt.first - 0.5f) * size, cy + (0.5f - pt.second) * size});
    }
    return p;
}

static char letter_for_brightness(float darkness) {
    int idx = int(std::clamp(darkness / 255.0f, 0.0f, 0.999f) * 26.0f);
    const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return letters[idx];
}

static float nearest_seed_radius(float cx, float cy, const std::vector<cv::Point2f>& points) {
    float best = 1e12f;
    for (const auto& pt : points) {
        float dx = pt.x - cx;
        float dy = pt.y - cy;
        if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) continue;
        float d = std::hypot(dx, dy);
        if (d < best) best = d;
    }
    return best < 1e11f ? best * 0.45f : 8.0f;
}

// -------------------------------------------------------------------------
_LettersBasePFM::_LettersBasePFM(QObject* parent) : PathFindingModule(parent) {
    initSettings();
}

QVector<PFMSetting> _LettersBasePFM::defineSettings() const {
    return {
        {"cell_count", "Cell Count", SettingType::Integer, 800, QVariant(), 50, 20000, 50, 20000, 100},
        {"lloyd_iterations", "Lloyd Iterations", SettingType::Integer, 0, QVariant(), 0, 20, 0, 20, 1},
        {"min_brightness", "Min Brightness", SettingType::Number, 0.0, QVariant(), 0.0, 255.0, 0.0, 255.0, 1.0},
        {"letter_scale", "Letter Scale", SettingType::Number, 0.85, QVariant(), 0.2, 1.5, 0.2, 1.5, 0.05}
    };
}

QVector<DrawingGeometry> _LettersBasePFM::_process(const cv::Mat& image) {
    int w = image.cols;
    int h = image.rows;
    int cell_count = m_settings["cell_count"].toInt();
    int lloyd_iters = getLloydIters();
    float min_brightness = m_settings["min_brightness"].toDouble();
    float letter_scale = m_settings["letter_scale"].toDouble();

    std::vector<double> probs(w * h, 0.0);
    std::vector<double> original_dark(w * h, 0.0);
    double sum = 0.0;
    
    for (int y = 0; y < h; ++y) {
        const uchar* row = image.ptr<uchar>(y);
        for (int x = 0; x < w; ++x) {
            double d = 255.0 - row[x];
            original_dark[y * w + x] = d;
            if (d < 0) d = 0;
            probs[y * w + x] = d;
            sum += d;
        }
    }
    if (sum < 1e-6) return {};
    for (auto& p : probs) p /= sum;

    std::vector<cv::Point2f> pts;
    for (int i = 0; i < cell_count; ++i) {
        if (isCancelled()) return {};
        int idx = weightedChoice(probs);
        pts.push_back(cv::Point2f(idx % w, idx / w));
    }

    if (lloyd_iters > 0) {
        int step = std::max(1, int(std::sqrt(h * w / 20000)));
        std::vector<cv::Point2f> sub_coords;
        std::vector<double> sub_w;
        for (int y = 0; y < h; y += step) {
            for (int x = 0; x < w; x += step) {
                sub_coords.push_back(cv::Point2f(x, y));
                sub_w.push_back(probs[y * w + x]);
            }
        }
        
        for (int iter = 0; iter < lloyd_iters; ++iter) {
            if (isCancelled()) return {};
            emitProgress(float(iter) / lloyd_iters, 0, "Lloyd Relaxation...");
            
            std::vector<cv::Point2f> new_pts(pts.size(), cv::Point2f(0,0));
            std::vector<double> weight_sums(pts.size(), 0.0);

            for (size_t i = 0; i < sub_coords.size(); ++i) {
                float best_d = 1e12f;
                int best_c = -1;
                for (size_t c = 0; c < pts.size(); ++c) {
                    float dx = sub_coords[i].x - pts[c].x;
                    float dy = sub_coords[i].y - pts[c].y;
                    float d = dx*dx + dy*dy;
                    if (d < best_d) { best_d = d; best_c = c; }
                }
                if (best_c != -1) {
                    new_pts[best_c].x += sub_coords[i].x * sub_w[i];
                    new_pts[best_c].y += sub_coords[i].y * sub_w[i];
                    weight_sums[best_c] += sub_w[i];
                }
            }

            for (size_t c = 0; c < pts.size(); ++c) {
                if (weight_sums[c] > 1e-9) {
                    pts[c].x = new_pts[c].x / weight_sums[c];
                    pts[c].y = new_pts[c].y / weight_sums[c];
                }
            }
        }
    }

    std::vector<cv::Point2f> filtered_pts;
    for (const auto& p : pts) {
        int xi = std::clamp(int(p.x), 0, w - 1);
        int yi = std::clamp(int(p.y), 0, h - 1);
        if (original_dark[yi * w + xi] >= min_brightness) {
            filtered_pts.push_back(p);
        }
    }

    QVector<DrawingGeometry> geoms;
    for (size_t i = 0; i < filtered_pts.size(); ++i) {
        if (isCancelled()) break;
        if (i % 40 == 0) emitProgress(float(i) / std::max<size_t>(1, filtered_pts.size()), geoms.size(), "Placing letters...");
        
        float cx = filtered_pts[i].x;
        float cy = filtered_pts[i].y;
        
        int xi = std::clamp(int(cx), 0, w - 1);
        int yi = std::clamp(int(cy), 0, h - 1);
        float dark = original_dark[yi * w + xi];
        
        char letter = letter_for_brightness(dark);
        float r = nearest_seed_radius(cx, cy, filtered_pts) * 2.0f * letter_scale;
        
        Path path = letter_glyph_path(letter, cx, cy, std::max(4.0f, r));
        if (path.size() >= 2) {
            DrawingGeometry dg;
            dg.path = path;
            geoms.push_back(dg);
        }
    }

    return geoms;
}


// Original File: ../cpp/filters\artistic_distort_filters.cpp

// ---------------------------------------------------------------------------
// Artistic
// ---------------------------------------------------------------------------
QVector<PFMSetting> PosterizeFilter::defineSettings() const {
    return {
        { "levels", "Levels", SettingType::Integer, 4, QVariant(), 2, 256, 2, 256, 1 }
    };
}
cv::Mat PosterizeFilter::process(const cv::Mat& image) {
    int levels = get("levels").toInt();
    if (levels >= 256) return image.clone();

    double factor1 = 256.0 / levels;
    double factor2 = 255.0 / (levels - 1);
    
    cv::Mat img32;
    image.convertTo(img32, CV_32F);
    
    cv::Mat out;
    img32 = (img32 / factor1);
    
    // Equivalent to np.floor in C++ matrix operations
    for (int r = 0; r < img32.rows; ++r) {
        float* ptr = img32.ptr<float>(r);
        for (int c = 0; c < img32.cols * img32.channels(); ++c) {
            ptr[c] = std::floor(ptr[c]) * factor2;
        }
    }
    
    img32.convertTo(out, CV_8U);
    return out;
}

cv::Mat EdgePreserveFilter::process(const cv::Mat& image) {
    cv::Mat out;
    if (image.channels() == 3) {
        cv::edgePreservingFilter(image, out, 1, 60.0f, 0.4f);
    } else {
        cv::Mat bgr;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::Mat res;
        cv::edgePreservingFilter(bgr, res, 1, 60.0f, 0.4f);
        cv::cvtColor(res, out, cv::COLOR_BGR2GRAY);
    }
    return out;
}

cv::Mat StylizationFilter::process(const cv::Mat& image) {
    cv::Mat out;
    if (image.channels() == 3) {
        cv::stylization(image, out, 60.0f, 0.45f);
    } else {
        cv::Mat bgr;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::Mat res;
        cv::stylization(bgr, res, 60.0f, 0.45f);
        cv::cvtColor(res, out, cv::COLOR_BGR2GRAY);
    }
    return out;
}

QVector<PFMSetting> OilPaintingFilter::defineSettings() const {
    return {
        { "size", "Size", SettingType::Integer, 5, QVariant(), 1, 20, 1, 20, 1 },
        { "dynRatio", "Dynamic Ratio", SettingType::Integer, 1, QVariant(), 1, 10, 1, 10, 1 }
    };
}

cv::Mat OilPaintingFilter::process(const cv::Mat& image) {
    if (image.empty()) return {};

    bool wasGray = image.channels() == 1;
    cv::Mat bgr;
    if (wasGray) cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
    else if (image.channels() == 4) cv::cvtColor(image, bgr, cv::COLOR_BGRA2BGR);
    else bgr = image.clone();

    int size = std::max(1, get("size").toInt());
    if (size % 2 == 0) ++size;
    int radius = size / 2;
    int dynRatio = std::max(1, get("dynRatio").toInt());

    cv::Mat gray;
    cv::cvtColor(bgr, gray, cv::COLOR_BGR2GRAY);
    cv::Mat out(bgr.size(), bgr.type());

    for (int y = 0; y < bgr.rows; ++y) {
        for (int x = 0; x < bgr.cols; ++x) {
            std::vector<int> counts(dynRatio, 0);
            std::vector<cv::Vec3i> sums(dynRatio, cv::Vec3i(0, 0, 0));

            for (int dy = -radius; dy <= radius; ++dy) {
                int yy = std::clamp(y + dy, 0, bgr.rows - 1);
                for (int dx = -radius; dx <= radius; ++dx) {
                    int xx = std::clamp(x + dx, 0, bgr.cols - 1);
                    int bin = std::clamp((int)(gray.at<uchar>(yy, xx) * dynRatio / 256.0), 0, dynRatio - 1);
                    cv::Vec3b pix = bgr.at<cv::Vec3b>(yy, xx);
                    counts[bin]++;
                    sums[bin] += cv::Vec3i(pix[0], pix[1], pix[2]);
                }
            }

            int best = 0;
            for (int i = 1; i < dynRatio; ++i)
                if (counts[i] > counts[best]) best = i;
            int n = std::max(1, counts[best]);
            out.at<cv::Vec3b>(y, x) = cv::Vec3b(
                cv::saturate_cast<uchar>(sums[best][0] / n),
                cv::saturate_cast<uchar>(sums[best][1] / n),
                cv::saturate_cast<uchar>(sums[best][2] / n));
        }
    }

    if (wasGray) {
        cv::Mat grayOut;
        cv::cvtColor(out, grayOut, cv::COLOR_BGR2GRAY);
        return grayOut;
    }
    return out;
}

QVector<PFMSetting> DetailEnhanceFilter::defineSettings() const {
    return {
        { "sigma_s", "Sigma S", SettingType::Number, 10.0, QVariant(), 1.0, 200.0, 1.0, 200.0, 1.0 },
        { "sigma_r", "Sigma R", SettingType::Number, 0.15, QVariant(), 0.01, 1.0, 0.01, 1.0, 0.01 }
    };
}

cv::Mat DetailEnhanceFilter::process(const cv::Mat& image) {
    cv::Mat out;
    float sigmaS = (float)get("sigma_s").toDouble();
    float sigmaR = (float)get("sigma_r").toDouble();
    if (image.channels() == 3) {
        cv::detailEnhance(image, out, sigmaS, sigmaR);
    } else {
        cv::Mat bgr, res;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::detailEnhance(bgr, res, sigmaS, sigmaR);
        cv::cvtColor(res, out, cv::COLOR_BGR2GRAY);
    }
    return out;
}

QVector<PFMSetting> PencilSketchFilter::defineSettings() const {
    return {
        { "sigma_s", "Sigma S", SettingType::Number, 60.0, QVariant(), 1.0, 200.0, 1.0, 200.0, 1.0 },
        { "sigma_r", "Sigma R", SettingType::Number, 0.07, QVariant(), 0.01, 1.0, 0.01, 1.0, 0.01 }
    };
}

cv::Mat PencilSketchFilter::process(const cv::Mat& image) {
    cv::Mat gray, color;
    float sigmaS = (float)get("sigma_s").toDouble();
    float sigmaR = (float)get("sigma_r").toDouble();
    if (image.channels() == 3) {
        cv::pencilSketch(image, gray, color, sigmaS, sigmaR, 0.05f);
    } else {
        cv::Mat bgr;
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
        cv::pencilSketch(bgr, gray, color, sigmaS, sigmaR, 0.05f);
    }
    return gray;
}

cv::Mat EmbossFilter::process(const cv::Mat& image) {
    cv::Mat kernel = (cv::Mat_<float>(3, 3) << -2, -1, 0, -1, 1, 1, 0, 1, 2);
    cv::Mat out;
    cv::filter2D(image, out, -1, kernel);
    return out;
}

cv::Mat QuantizeFilter::process(const cv::Mat& image) {
    cv::Mat img32;
    image.convertTo(img32, CV_32F);
    
    for (int r = 0; r < img32.rows; ++r) {
        float* ptr = img32.ptr<float>(r);
        for (int c = 0; c < img32.cols * img32.channels(); ++c) {
            ptr[c] = std::round(ptr[c] / 32.0f) * 32.0f;
        }
    }
    
    cv::Mat out;
    img32.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// Distort
// ---------------------------------------------------------------------------
QVector<PFMSetting> VignetteFilter::defineSettings() const {
    return {
        { "intensity", "Intensity", SettingType::Number, 150.0, QVariant(), 10.0, 300.0, 10.0, 300.0, 5.0 }
    };
}
cv::Mat VignetteFilter::process(const cv::Mat& image) {
    double intensity = get("intensity").toDouble();
    int h = image.rows;
    int w = image.cols;
    
    cv::Mat kernelX = cv::getGaussianKernel(w, intensity);
    cv::Mat kernelY = cv::getGaussianKernel(h, intensity);
    cv::Mat kernel = kernelY * kernelX.t();
    
    cv::Mat mask;
    cv::normalize(kernel, mask, 0.0, 1.0, cv::NORM_MINMAX);
    mask.convertTo(mask, CV_32F);
    
    if (image.channels() == 3) {
        cv::cvtColor(mask, mask, cv::COLOR_GRAY2BGR);
    }
    
    cv::Mat img32;
    image.convertTo(img32, CV_32F);
    img32 = img32.mul(mask);
    
    cv::Mat out;
    img32.convertTo(out, CV_8U);
    return out;
}

QVector<PFMSetting> PixelateFilter::defineSettings() const {
    return {
        { "size", "Pixel Size", SettingType::Integer, 10, QVariant(), 2, 100, 2, 100, 1 }
    };
}
cv::Mat PixelateFilter::process(const cv::Mat& image) {
    int size = get("size").toInt();
    int h = image.rows;
    int w = image.cols;
    
    cv::Mat small, out;
    cv::resize(image, small, cv::Size(w / size, h / size), 0, 0, cv::INTER_LINEAR);
    cv::resize(small, out, cv::Size(w, h), 0, 0, cv::INTER_NEAREST);
    return out;
}

QVector<PFMSetting> WaveFilter::defineSettings() const {
    return {
        { "amplitude", "Amplitude", SettingType::Number, 10.0, QVariant(), 1.0, 100.0, 1.0, 100.0, 1.0 },
        { "frequency", "Frequency", SettingType::Number, 0.05, QVariant(), 0.01, 0.5, 0.01, 0.5, 0.01 }
    };
}
cv::Mat WaveFilter::process(const cv::Mat& image) {
    int h = image.rows;
    int w = image.cols;
    double amp = get("amplitude").toDouble();
    double freq = get("frequency").toDouble();
    
    cv::Mat mapX(h, w, CV_32FC1);
    cv::Mat mapY(h, w, CV_32FC1);
    
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            mapY.at<float>(y, x) = static_cast<float>(y);
            mapX.at<float>(y, x) = static_cast<float>(x + amp * std::sin(y * freq));
        }
    }
    
    cv::Mat out;
    cv::remap(image, out, mapX, mapY, cv::INTER_LINEAR);
    return out;
}


// Original File: ../cpp/filters\base_filter.cpp

ImageFilter::ImageFilter(QObject* parent) : QObject(parent) {
    // Note: Subclasses must call initSettings() inside their constructors!
}

void ImageFilter::initSettings() {
    m_settings.clear();
    auto sList = defineSettings();
    for (const auto& s : sList) {
        m_settings.insert(s.key, s);
    }
}

QVector<PFMSetting> ImageFilter::settingsList() const {
    QVector<PFMSetting> list;
    // We fetch them dynamically to ensure any GUI updates to m_settings are reflected
    auto defined = defineSettings();
    for (const auto& s : defined) {
        if (m_settings.contains(s.key)) {
            list.append(m_settings[s.key]);
        } else {
            list.append(s);
        }
    }
    return list;
}

QVariant ImageFilter::get(const QString& key) const {
    if (m_settings.contains(key)) {
        return m_settings[key].currentValue();
    }
    return QVariant();
}

void ImageFilter::set(const QString& key, const QVariant& value) {
    if (m_settings.contains(key)) {
        m_settings[key].value = value;
    }
}

void ImageFilter::resetAll() {
    for (auto it = m_settings.begin(); it != m_settings.end(); ++it) {
        it->value = QVariant(); // clear value so currentValue() falls back to defaultVal
    }
}


// Original File: ../cpp/filters\basic_filters.cpp

// ---------------------------------------------------------------------------
// BrightnessFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> BrightnessFilter::defineSettings() const {
    return {
        { "brightness", "Brightness", SettingType::Number, 0.0, QVariant(), -255.0, 255.0, -255.0, 255.0, 1.0 }
    };
}

cv::Mat BrightnessFilter::process(const cv::Mat& image) {
    double b = get("brightness").toDouble();
    if (b == 0.0) return image.clone();

    cv::Mat img;
    image.convertTo(img, CV_32F);
    img += b;
    
    cv::Mat out;
    img.convertTo(out, CV_8U, 1.0, 0.0); // automatically clips 0-255
    return out;
}

// ---------------------------------------------------------------------------
// ContrastFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> ContrastFilter::defineSettings() const {
    return {
        { "contrast", "Contrast", SettingType::Number, 1.0, QVariant(), 0.0, 5.0, 0.0, 5.0, 0.1 }
    };
}

cv::Mat ContrastFilter::process(const cv::Mat& image) {
    double c = get("contrast").toDouble();
    if (std::abs(c - 1.0) < 1e-4) return image.clone();

    cv::Mat img;
    image.convertTo(img, CV_32F);
    img = (img - 127.5) * c + 127.5;

    cv::Mat out;
    img.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// InvertFilter
// ---------------------------------------------------------------------------
cv::Mat InvertFilter::process(const cv::Mat& image) {
    cv::Mat out;
    cv::bitwise_not(image, out);
    return out;
}

// ---------------------------------------------------------------------------
// ThresholdFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> ThresholdFilter::defineSettings() const {
    return {
        { "threshold", "Threshold", SettingType::Integer, 127, QVariant(), 0, 255, 0, 255, 1 }
    };
}

cv::Mat ThresholdFilter::process(const cv::Mat& image) {
    int t = get("threshold").toInt();
    cv::Mat out;
    cv::threshold(image, out, t, 255, cv::THRESH_BINARY);
    return out;
}

// ---------------------------------------------------------------------------
// UnsharpMaskFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> UnsharpMaskFilter::defineSettings() const {
    return {
        { "radius", "Radius", SettingType::Integer, 5, QVariant(), 1, 50, 1, 50, 1 },
        { "amount", "Amount", SettingType::Number,  1.5, QVariant(), 0.0, 5.0, 0.0, 5.0, 0.1 }
    };
}

cv::Mat UnsharpMaskFilter::process(const cv::Mat& image) {
    int r = get("radius").toInt();
    double a = get("amount").toDouble();

    int ksize = r * 2 + 1;
    cv::Mat blurred;
    cv::GaussianBlur(image, blurred, cv::Size(ksize, ksize), 0);

    cv::Mat imgFloat, blurredFloat;
    image.convertTo(imgFloat, CV_32F);
    blurred.convertTo(blurredFloat, CV_32F);

    cv::Mat sharpened = imgFloat + (imgFloat - blurredFloat) * a;
    
    cv::Mat out;
    sharpened.convertTo(out, CV_8U);
    return out;
}


// Original File: ../cpp/filters\color_filters.cpp

// ---------------------------------------------------------------------------
// GrayscaleFilter
// ---------------------------------------------------------------------------
cv::Mat GrayscaleFilter::process(const cv::Mat& image) {
    if (image.channels() == 3) {
        cv::Mat out;
        cv::cvtColor(image, out, cv::COLOR_BGR2GRAY);
        return out;
    } else if (image.channels() == 4) {
        cv::Mat out;
        cv::cvtColor(image, out, cv::COLOR_BGRA2GRAY);
        return out;
    }
    return image.clone();
}

cv::Mat DesaturateFilter::process(const cv::Mat& image) {
    if (image.channels() == 3) {
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        cv::Mat out;
        cv::cvtColor(gray, out, cv::COLOR_GRAY2BGR);
        return out;
    } else if (image.channels() == 4) {
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
        cv::Mat out;
        cv::cvtColor(gray, out, cv::COLOR_GRAY2BGRA);
        return out;
    }
    return image.clone();
}

// ---------------------------------------------------------------------------
// SaturationFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> SaturationFilter::defineSettings() const {
    return {
        { "saturation", "Saturation", SettingType::Number, 1.0, QVariant(), 0.0, 3.0, 0.0, 3.0, 0.1 }
    };
}

cv::Mat SaturationFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    double sat = get("saturation").toDouble();
    if (std::abs(sat - 1.0) < 1e-4) return image.clone();

    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    
    // Scale saturation (channel 1)
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    channels[1].convertTo(channels[1], CV_32F);
    channels[1] *= sat;
    channels[1].convertTo(channels[1], CV_8U);
    cv::merge(channels, hsv);

    cv::Mat out;
    cv::cvtColor(hsv, out, cv::COLOR_HSV2BGR);
    return out;
}

// ---------------------------------------------------------------------------
// HueFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> HueFilter::defineSettings() const {
    return {
        { "hue", "Hue Shift", SettingType::Integer, 0, QVariant(), -180, 180, -180, 180, 1 }
    };
}

cv::Mat HueFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    int hue = get("hue").toInt();
    if (hue == 0) return image.clone();

    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    
    // Shift Hue (channel 0)
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    
    channels[0].convertTo(channels[0], CV_16S);
    channels[0] += hue;
    
    // Wrap around 180
    for(int r = 0; r < channels[0].rows; r++) {
        short* row = channels[0].ptr<short>(r);
        for(int c = 0; c < channels[0].cols; c++) {
            if (row[c] < 0) row[c] += 180;
            if (row[c] >= 180) row[c] -= 180;
        }
    }
    
    channels[0].convertTo(channels[0], CV_8U);
    cv::merge(channels, hsv);

    cv::Mat out;
    cv::cvtColor(hsv, out, cv::COLOR_HSV2BGR);
    return out;
}

// ---------------------------------------------------------------------------
// GammaFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> GammaFilter::defineSettings() const {
    return {
        { "gamma", "Gamma", SettingType::Number, 1.0, QVariant(), 0.1, 5.0, 0.1, 5.0, 0.1 }
    };
}

cv::Mat GammaFilter::process(const cv::Mat& image) {
    double gamma = get("gamma").toDouble();
    if (std::abs(gamma - 1.0) < 1e-4) return image.clone();

    double invGamma = 1.0 / gamma;
    cv::Mat lut(1, 256, CV_8U);
    uchar* p = lut.ptr();
    for(int i = 0; i < 256; ++i) {
        p[i] = cv::saturate_cast<uchar>(std::pow(i / 255.0, invGamma) * 255.0);
    }
    
    cv::Mat out;
    cv::LUT(image, lut, out);
    return out;
}

// ---------------------------------------------------------------------------
// ExposureFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> ExposureFilter::defineSettings() const {
    return {
        { "exposure", "Exposure (EV)", SettingType::Number, 0.0, QVariant(), -5.0, 5.0, -5.0, 5.0, 0.1 }
    };
}

cv::Mat ExposureFilter::process(const cv::Mat& image) {
    double ev = get("exposure").toDouble();
    if (std::abs(ev) < 1e-4) return image.clone();

    double multiplier = std::pow(2.0, ev);
    cv::Mat imgFloat;
    image.convertTo(imgFloat, CV_32F);
    imgFloat *= multiplier;

    cv::Mat out;
    imgFloat.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// SepiaFilter
// ---------------------------------------------------------------------------
cv::Mat SepiaFilter::process(const cv::Mat& image) {
    cv::Mat img3;
    if (image.channels() == 1) {
        cv::cvtColor(image, img3, cv::COLOR_GRAY2BGR);
    } else {
        img3 = image.clone();
    }

    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        0.272f, 0.534f, 0.131f,
        0.349f, 0.686f, 0.168f,
        0.393f, 0.769f, 0.189f);
    
    cv::Mat out;
    cv::transform(img3, out, kernel);
    out.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// CLAHEFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> CLAHEFilter::defineSettings() const {
    return {
        { "clip_limit", "Clip Limit", SettingType::Number, 2.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1 },
        { "grid_size", "Grid Size", SettingType::Integer, 8, QVariant(), 2, 32, 2, 32, 2 }
    };
}

cv::Mat CLAHEFilter::process(const cv::Mat& image) {
    double clip = get("clip_limit").toDouble();
    int grid = get("grid_size").toInt();

    auto clahe = cv::createCLAHE(clip, cv::Size(grid, grid));

    if (image.channels() == 3) {
        cv::Mat lab;
        cv::cvtColor(image, lab, cv::COLOR_BGR2Lab);
        
        std::vector<cv::Mat> channels;
        cv::split(lab, channels);
        clahe->apply(channels[0], channels[0]); // Apply to L channel
        cv::merge(channels, lab);
        
        cv::Mat out;
        cv::cvtColor(lab, out, cv::COLOR_Lab2BGR);
        return out;
    } else if (image.channels() == 1) {
        cv::Mat out;
        clahe->apply(image, out);
        return out;
    }
    return image.clone();
}

// ---------------------------------------------------------------------------
// TemperatureFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> TemperatureFilter::defineSettings() const {
    return {
        { "temperature", "Temp", SettingType::Number, 0.0, QVariant(), -100.0, 100.0, -100.0, 100.0, 1.0 }
    };
}

cv::Mat TemperatureFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    double temp = get("temperature").toDouble();
    if (std::abs(temp) < 1e-4) return image.clone();

    cv::Mat img16;
    image.convertTo(img16, CV_16S);
    
    // Simple heuristic: OpenCV is BGR
    // B = 0, G = 1, R = 2
    // Positive temp adds red, removes blue
    std::vector<cv::Mat> channels;
    cv::split(img16, channels);
    channels[2] += temp; // Red
    channels[0] -= temp; // Blue
    cv::merge(channels, img16);

    cv::Mat out;
    img16.convertTo(out, CV_8U);
    return out;
}


// Original File: ../cpp/filters\edge_blur_filters.cpp

// ---------------------------------------------------------------------------
// Edge Filters
// ---------------------------------------------------------------------------
QVector<PFMSetting> CannyFilter::defineSettings() const {
    return {
        { "threshold1", "Threshold 1", SettingType::Integer, 100, QVariant(), 0, 255, 0, 255, 1 },
        { "threshold2", "Threshold 2", SettingType::Integer, 200, QVariant(), 0, 255, 0, 255, 1 }
    };
}

cv::Mat CannyFilter::process(const cv::Mat& image) {
    int t1 = get("threshold1").toInt();
    int t2 = get("threshold2").toInt();

    cv::Mat gray;
    if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else if (image.channels() == 4) cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
    else gray = image;

    cv::Mat out;
    cv::Canny(gray, out, t1, t2);
    // Return a 3-channel image if input was 3-channel
    if (image.channels() == 3) {
        cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
    }
    return out;
}


QVector<PFMSetting> SobelFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 7, 1, 7, 2 },
        { "scale", "Scale", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1 }
    };
}

cv::Mat SobelFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    if (ksize % 2 == 0) ksize += 1;
    double scale = get("scale").toDouble();

    cv::Mat gray;
    if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else if (image.channels() == 4) cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
    else gray = image;

    cv::Mat sobelx, sobely;
    cv::Sobel(gray, sobelx, CV_64F, 1, 0, ksize, scale);
    cv::Sobel(gray, sobely, CV_64F, 0, 1, ksize, scale);

    cv::Mat mag;
    cv::magnitude(sobelx, sobely, mag);
    
    cv::Mat out;
    mag.convertTo(out, CV_8U);
    if (image.channels() == 3) {
        cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
    }
    return out;
}


QVector<PFMSetting> LaplacianFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 7, 1, 7, 2 }
    };
}

cv::Mat LaplacianFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    if (ksize % 2 == 0) ksize += 1;

    cv::Mat gray;
    if (image.channels() == 3) cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else if (image.channels() == 4) cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
    else gray = image;

    cv::Mat laplacian;
    cv::Laplacian(gray, laplacian, CV_64F, ksize);
    laplacian = cv::abs(laplacian);

    cv::Mat out;
    laplacian.convertTo(out, CV_8U);
    if (image.channels() == 3) {
        cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
    }
    return out;
}

cv::Mat PrewittFilter::process(const cv::Mat& image) {
    cv::Mat kernelx = (cv::Mat_<float>(3, 3) << 1, 1, 1, 0, 0, 0, -1, -1, -1);
    cv::Mat kernely = (cv::Mat_<float>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
    
    cv::Mat imgx, imgy;
    cv::filter2D(image, imgx, CV_32F, kernelx);
    cv::filter2D(image, imgy, CV_32F, kernely);
    
    cv::Mat mag;
    cv::magnitude(imgx, imgy, mag);
    
    cv::Mat out;
    mag.convertTo(out, CV_8U);
    return out;
}

cv::Mat ScharrFilter::process(const cv::Mat& image) {
    cv::Mat scharrx, scharry;
    cv::Scharr(image, scharrx, CV_32F, 1, 0);
    cv::Scharr(image, scharry, CV_32F, 0, 1);
    
    cv::Mat mag;
    cv::magnitude(scharrx, scharry, mag);
    
    cv::Mat out;
    mag.convertTo(out, CV_8U);
    return out;
}

QVector<PFMSetting> DoGFilter::defineSettings() const {
    return {
        { "sigma1", "Sigma 1", SettingType::Number, 1.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1 },
        { "sigma2", "Sigma 2", SettingType::Number, 2.0, QVariant(), 0.1, 10.0, 0.1, 10.0, 0.1 }
    };
}

cv::Mat DoGFilter::process(const cv::Mat& image) {
    double s1 = get("sigma1").toDouble();
    double s2 = get("sigma2").toDouble();
    
    cv::Mat blur1, blur2;
    cv::GaussianBlur(image, blur1, cv::Size(0,0), s1);
    cv::GaussianBlur(image, blur2, cv::Size(0,0), s2);
    
    cv::Mat dog;
    cv::absdiff(blur1, blur2, dog);
    dog *= 2;
    
    return dog;
}

cv::Mat RidgeDetectionFilter::process(const cv::Mat& image) {
    cv::Mat img;
    if (image.channels() == 3) cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);
    else img = image;

    cv::Mat dx, dy, dxy;
    cv::Sobel(img, dx, CV_32F, 2, 0);
    cv::Sobel(img, dy, CV_32F, 0, 2);
    cv::Sobel(img, dxy, CV_32F, 1, 1);
    
    // Trace and Det
    cv::Mat trace = dx + dy;
    cv::Mat det = dx.mul(dy) - dxy.mul(dxy);
    
    // lambda1
    cv::Mat discriminant;
    cv::sqrt(cv::max(trace.mul(trace) - 4*det, 0), discriminant);
    cv::Mat lambda1 = 0.5 * (trace + discriminant);
    lambda1 = cv::abs(lambda1);
    
    double minVal, maxVal;
    cv::minMaxLoc(lambda1, &minVal, &maxVal);
    if (maxVal > 0) {
        lambda1 = (lambda1 / maxVal) * 255.0;
    }
    
    cv::Mat out;
    lambda1.convertTo(out, CV_8U);
    if (image.channels() == 3) {
        cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);
    }
    return out;
}

cv::Mat HighPassFilter::process(const cv::Mat& image) {
    cv::Mat blur;
    cv::GaussianBlur(image, blur, cv::Size(11, 11), 0);
    
    cv::Mat img32, blur32;
    image.convertTo(img32, CV_32F);
    blur.convertTo(blur32, CV_32F);
    
    cv::Mat hp = img32 - blur32 + 127.0;
    
    cv::Mat out;
    hp.convertTo(out, CV_8U);
    return out;
}


// ---------------------------------------------------------------------------
// Blur Filters
// ---------------------------------------------------------------------------
QVector<PFMSetting> GaussianBlurFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, QVariant(), 1, 51, 1, 51, 2 },
        { "sigma", "Sigma", SettingType::Number, 0.0, QVariant(), 0.0, 10.0, 0.0, 10.0, 0.5 }
    };
}

cv::Mat GaussianBlurFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    if (ksize % 2 == 0) ksize += 1;
    double sigma = get("sigma").toDouble();

    cv::Mat out;
    cv::GaussianBlur(image, out, cv::Size(ksize, ksize), sigma);
    return out;
}


QVector<PFMSetting> MedianBlurFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, QVariant(), 3, 51, 3, 51, 2 }
    };
}

cv::Mat MedianBlurFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    if (ksize % 2 == 0) ksize += 1;

    cv::Mat out;
    cv::medianBlur(image, out, ksize);
    return out;
}


QVector<PFMSetting> BoxBlurFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 5, QVariant(), 1, 51, 1, 51, 1 }
    };
}

cv::Mat BoxBlurFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();

    cv::Mat out;
    cv::blur(image, out, cv::Size(ksize, ksize));
    return out;
}

QVector<PFMSetting> MotionBlurFilter::defineSettings() const {
    return {
        { "size", "Size", SettingType::Integer, 15, QVariant(), 3, 101, 3, 101, 2 }
    };
}

cv::Mat MotionBlurFilter::process(const cv::Mat& image) {
    int size = get("size").toInt();
    if (size % 2 == 0) size += 1;
    
    cv::Mat kernel = cv::Mat::zeros(size, size, CV_32F);
    int mid = size / 2;
    for (int i = 0; i < size; ++i) {
        kernel.at<float>(mid, i) = 1.0f / size;
    }
    
    cv::Mat out;
    cv::filter2D(image, out, -1, kernel);
    return out;
}

cv::Mat BilateralFilter::process(const cv::Mat& image) {
    cv::Mat out;
    cv::bilateralFilter(image, out, 9, 75, 75);
    return out;
}

cv::Mat LowPassFilter::process(const cv::Mat& image) {
    cv::Mat out;
    cv::GaussianBlur(image, out, cv::Size(21, 21), 0);
    return out;
}

cv::Mat SharpenMoreFilter::process(const cv::Mat& image) {
    cv::Mat kernel = (cv::Mat_<float>(3,3) << -1, -1, -1, -1, 9, -1, -1, -1, -1);
    cv::Mat out;
    cv::filter2D(image, out, -1, kernel);
    return out;
}



// Original File: ../cpp/filters\morph_filters.cpp

// ---------------------------------------------------------------------------
// Dilation & Erosion
// ---------------------------------------------------------------------------
QVector<PFMSetting> DilationFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 31, 1, 31, 2 },
        { "iterations", "Iterations", SettingType::Integer, 1, QVariant(), 1, 10, 1, 10, 1 }
    };
}
cv::Mat DilationFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    int iters = get("iterations").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::dilate(image, out, kernel, cv::Point(-1,-1), iters);
    return out;
}

QVector<PFMSetting> ErosionFilter::defineSettings() const {
    return {
        { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 31, 1, 31, 2 },
        { "iterations", "Iterations", SettingType::Integer, 1, QVariant(), 1, 10, 1, 10, 1 }
    };
}
cv::Mat ErosionFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    int iters = get("iterations").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::erode(image, out, kernel, cv::Point(-1,-1), iters);
    return out;
}

// ---------------------------------------------------------------------------
// Advanced Morph
// ---------------------------------------------------------------------------
QVector<PFMSetting> OpeningFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 31, 1, 31, 2 } };
}
cv::Mat OpeningFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_OPEN, kernel);
    return out;
}

QVector<PFMSetting> ClosingFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 31, 1, 31, 2 } };
}
cv::Mat ClosingFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_CLOSE, kernel);
    return out;
}

QVector<PFMSetting> MorphGradientFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 3, QVariant(), 1, 31, 1, 31, 2 } };
}
cv::Mat MorphGradientFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_GRADIENT, kernel);
    return out;
}

QVector<PFMSetting> TopHatFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 9, QVariant(), 1, 101, 1, 101, 2 } };
}
cv::Mat TopHatFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_TOPHAT, kernel);
    return out;
}

QVector<PFMSetting> BlackHatFilter::defineSettings() const {
    return { { "ksize", "Kernel Size", SettingType::Integer, 9, QVariant(), 1, 101, 1, 101, 2 } };
}
cv::Mat BlackHatFilter::process(const cv::Mat& image) {
    int ksize = get("ksize").toInt();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::Mat out;
    cv::morphologyEx(image, out, cv::MORPH_BLACKHAT, kernel);
    return out;
}

// ---------------------------------------------------------------------------
// Cross & Ellipse
// ---------------------------------------------------------------------------
cv::Mat DilateCrossFilter::process(const cv::Mat& image) {
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
    cv::Mat out;
    cv::dilate(image, out, kernel, cv::Point(-1,-1), 1);
    return out;
}

cv::Mat ErodeCrossFilter::process(const cv::Mat& image) {
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
    cv::Mat out;
    cv::erode(image, out, kernel, cv::Point(-1,-1), 1);
    return out;
}

cv::Mat DilateEllipseFilter::process(const cv::Mat& image) {
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::Mat out;
    cv::dilate(image, out, kernel, cv::Point(-1,-1), 1);
    return out;
}

cv::Mat ErodeEllipseFilter::process(const cv::Mat& image) {
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::Mat out;
    cv::erode(image, out, kernel, cv::Point(-1,-1), 1);
    return out;
}


// Original File: ../cpp/filters\noise_filters.cpp

// ---------------------------------------------------------------------------
// GaussianNoiseFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> GaussianNoiseFilter::defineSettings() const {
    return {
        { "mean", "Mean", SettingType::Number, 0.0, QVariant(), -100.0, 100.0, -100.0, 100.0, 1.0 },
        { "sigma", "Std Dev", SettingType::Number, 25.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0 }
    };
}

cv::Mat GaussianNoiseFilter::process(const cv::Mat& image) {
    double mean = get("mean").toDouble();
    double sigma = get("sigma").toDouble();

    cv::Mat noise(image.size(), image.type());
    cv::randn(noise, mean, sigma);
    
    cv::Mat img16;
    image.convertTo(img16, CV_16S);
    
    cv::Mat noise16;
    noise.convertTo(noise16, CV_16S);
    
    img16 += noise16;
    
    cv::Mat out;
    img16.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// SaltAndPepperFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> SaltAndPepperFilter::defineSettings() const {
    return {
        { "prob", "Probability", SettingType::Percentage, 5.0, QVariant(), 0.0, 100.0, 0.0, 100.0, 1.0 }
    };
}

cv::Mat SaltAndPepperFilter::process(const cv::Mat& image) {
    double prob = get("prob").toDouble() / 100.0;
    if (prob <= 0.0) return image.clone();

    cv::Mat out = image.clone();
    cv::Mat noise(image.size(), CV_32F);
    cv::randu(noise, 0.0, 1.0);

    // Apply pepper
    out.setTo(cv::Scalar::all(0), noise < (prob / 2.0));
    // Apply salt
    out.setTo(cv::Scalar::all(255), (noise >= (prob / 2.0)) & (noise < prob));
    
    return out;
}

// ---------------------------------------------------------------------------
// DenoiseFilter
// ---------------------------------------------------------------------------
QVector<PFMSetting> DenoiseFilter::defineSettings() const {
    return {
        { "h", "Strength (h)", SettingType::Number, 10.0, QVariant(), 1.0, 50.0, 1.0, 50.0, 1.0 }
    };
}

cv::Mat DenoiseFilter::process(const cv::Mat& image) {
    double h = get("h").toDouble();
    cv::Mat out;
    if (image.channels() == 3) {
        cv::fastNlMeansDenoisingColored(image, out, h, h, 7, 21);
    } else if (image.channels() == 1) {
        cv::fastNlMeansDenoising(image, out, h, 7, 21);
    } else {
        out = image.clone();
    }
    return out;
}

// ---------------------------------------------------------------------------
// GaussianNoise2Filter
// ---------------------------------------------------------------------------
cv::Mat GaussianNoise2Filter::process(const cv::Mat& image) {
    cv::Mat noise(image.size(), image.type());
    cv::randn(noise, 0.0, 10.0);
    
    cv::Mat img16;
    image.convertTo(img16, CV_16S);
    
    cv::Mat noise16;
    noise.convertTo(noise16, CV_16S);
    
    img16 += noise16;
    
    cv::Mat out;
    img16.convertTo(out, CV_8U);
    return out;
}

// ---------------------------------------------------------------------------
// SpeckleNoiseFilter
// ---------------------------------------------------------------------------
cv::Mat SpeckleNoiseFilter::process(const cv::Mat& image) {
    cv::Mat noise(image.size(), CV_32F);
    cv::randn(noise, 0.0, 1.0);
    
    cv::Mat img32;
    image.convertTo(img32, CV_32F);
    
    cv::Mat out = img32 + img32.mul(noise) * 0.1;
    out.convertTo(out, CV_8U);
    return out;
}


// Original File: ../cpp/filters\threshold_extra_filters.cpp

// ---------------------------------------------------------------------------
// Threshold Filters
// ---------------------------------------------------------------------------
cv::Mat OtsuThresholdFilter::process(const cv::Mat& image) {
    cv::Mat img;
    if (image.channels() == 3) cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);
    else img = image;
    
    cv::Mat out;
    cv::threshold(img, out, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
    return out;
}

QVector<PFMSetting> AdaptiveThresholdFilter::defineSettings() const {
    return {
        { "block_size", "Block Size", SettingType::Integer, 11, QVariant(), 3, 99, 3, 99, 2 },
        { "c", "C (Constant)", SettingType::Number, 2.0, QVariant(), -10.0, 10.0, -10.0, 10.0, 0.5 }
    };
}
cv::Mat AdaptiveThresholdFilter::process(const cv::Mat& image) {
    cv::Mat img;
    if (image.channels() == 3) cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);
    else img = image;
    
    int block = get("block_size").toInt();
    if (block % 2 == 0) block += 1;
    double c = get("c").toDouble();
    
    cv::Mat out;
    cv::adaptiveThreshold(img, out, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, block, c);
    return out;
}

QVector<PFMSetting> TruncateThresholdFilter::defineSettings() const {
    return {
        { "threshold", "Threshold", SettingType::Integer, 127, QVariant(), 0, 255, 0, 255, 1 }
    };
}
cv::Mat TruncateThresholdFilter::process(const cv::Mat& image) {
    int thresh = get("threshold").toInt();
    cv::Mat out;
    cv::threshold(image, out, thresh, 255, cv::THRESH_TRUNC);
    return out;
}

QVector<PFMSetting> ToZeroThresholdFilter::defineSettings() const {
    return {
        { "threshold", "Threshold", SettingType::Integer, 127, QVariant(), 0, 255, 0, 255, 1 }
    };
}
cv::Mat ToZeroThresholdFilter::process(const cv::Mat& image) {
    int thresh = get("threshold").toInt();
    cv::Mat out;
    cv::threshold(image, out, thresh, 255, cv::THRESH_TOZERO);
    return out;
}

// ---------------------------------------------------------------------------
// Extra/Color Filters
// ---------------------------------------------------------------------------
cv::Mat EqualizeHistFilter::process(const cv::Mat& image) {
    cv::Mat out;
    if (image.channels() == 3) {
        cv::Mat yuv;
        cv::cvtColor(image, yuv, cv::COLOR_BGR2YUV);
        std::vector<cv::Mat> channels;
        cv::split(yuv, channels);
        cv::equalizeHist(channels[0], channels[0]);
        cv::merge(channels, yuv);
        cv::cvtColor(yuv, out, cv::COLOR_YUV2BGR);
    } else {
        cv::equalizeHist(image, out);
    }
    return out;
}

cv::Mat AutoContrastFilter::process(const cv::Mat& image) {
    double minVal, maxVal;
    // Calculate global min max across all channels (or simply use normalize)
    cv::Mat out;
    cv::normalize(image, out, 0, 255, cv::NORM_MINMAX);
    return out;
}

cv::Mat AutoColorFilter::process(const cv::Mat& image) {
    cv::Mat out;
    if (image.channels() == 3) {
        std::vector<cv::Mat> channels;
        cv::split(image, channels);
        for (int i = 0; i < 3; ++i) {
            cv::normalize(channels[i], channels[i], 0, 255, cv::NORM_MINMAX);
        }
        cv::merge(channels, out);
    } else {
        cv::normalize(image, out, 0, 255, cv::NORM_MINMAX);
    }
    return out;
}

cv::Mat ColorizeFilter::process(const cv::Mat& image) {
    cv::Mat bgr;
    if (image.channels() == 1) {
        cv::cvtColor(image, bgr, cv::COLOR_GRAY2BGR);
    } else {
        bgr = image;
    }
    
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    channels[0].setTo(15); // OpenCV hue is 0-179, so 30/2 = 15
    channels[1] += 50; // Add saturation
    cv::merge(channels, hsv);
    
    cv::Mat out;
    cv::cvtColor(hsv, out, cv::COLOR_HSV2BGR);
    return out;
}

cv::Mat InvertHueFilter::process(const cv::Mat& image) {
    if (image.channels() != 3) return image.clone();
    
    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    
    cv::Mat h16;
    channels[0].convertTo(h16, CV_16S);
    h16 += 90;
    
    // In opencv modulo
    for (int r = 0; r < h16.rows; ++r) {
        short* ptr = h16.ptr<short>(r);
        for (int c = 0; c < h16.cols; ++c) {
            ptr[c] = ptr[c] % 180;
        }
    }
    
    h16.convertTo(channels[0], CV_8U);
    cv::merge(channels, hsv);
    
    cv::Mat out;
    cv::cvtColor(hsv, out, cv::COLOR_HSV2BGR);
    return out;
}


// Original File: ../cpp/export\gcode_exporter.cpp

bool GCodeExporter::exportGCode(const QString& filepath,
                                const QVector<DrawingGeometry>& geometries,
                                const DrawingAreaConfig& drawingArea,
                                int imageWidth,
                                int imageHeight,
                                const GCodeSettings& settings) {
    if (geometries.isEmpty()) return false;

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // Convert pixel coordinates to physical mm
    QVector<DrawingGeometry> mmGeoms;
    mmGeoms.reserve(geometries.size());
    for (const auto& geom : geometries) {
        Path mmPath = pixel_path_to_mm(geom.path, imageWidth, imageHeight, drawingArea);
        if (!mmPath.empty()) {
            mmGeoms.append({mmPath, geom.penIndex, geom.groupIndex});
        }
    }

    // Prepare, filter and optimize paths
    QMap<int, QVector<Path>> penPaths = PathOptimizer::preparePenPaths(
        mmGeoms, settings.minPathLength, settings.optimizePaths);

    double h_mm = drawingArea.height_mm;
    double cx = settings.centerZero ? (drawingArea.width_mm / 2.0) : 0.0;
    double cy = settings.centerZero ? (drawingArea.height_mm / 2.0) : 0.0;

    out << "; G-code generated by imgto2d (DrawingBot V3 C++ port)\n";
    if (!settings.startGcode.isEmpty()) out << settings.startGcode << "\n";
    out << settings.penUpCmd << "\n";

    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        int penIdx = it.key();
        QString penName = QString("Pen_%1").arg(penIdx);
        
        if (!settings.startLayerCmd.isEmpty()) {
            QString cmd = settings.startLayerCmd;
            cmd.replace("{pen}", penName);
            out << cmd << "\n";
        } else {
            out << "\n; --- LAYER: " << penName << " ---\n";
            out << "M0 ; PAUSE - Swap to pen '" << penName << "'\n";
        }

        for (const auto& path : it.value()) {
            if (path.size() < 2) continue;

            // Move to start
            double x0 = path[0].first - cx + settings.xOffset;
            double y0 = (h_mm - path[0].second) - cy + settings.yOffset; // Invert Y for CNC
            out << QString("G0 X%1 Y%2\n").arg(x0, 0, 'f', 3).arg(y0, 0, 'f', 3);
            out << settings.penDownCmd << "\n";

            // Draw line
            for (std::size_t i = 1; i < path.size(); ++i) {
                double xf = path[i].first - cx + settings.xOffset;
                double yf = (h_mm - path[i].second) - cy + settings.yOffset;
                out << QString("G1 X%1 Y%2\n").arg(xf, 0, 'f', 3).arg(yf, 0, 'f', 3);
            }
            out << settings.penUpCmd << "\n";
        }

        if (!settings.endLayerCmd.isEmpty()) {
            out << settings.endLayerCmd << "\n";
        }
    }

    if (!settings.endGcode.isEmpty()) out << settings.endGcode << "\n";
    
    return true;
}


// Original File: ../cpp/export\hpgl_exporter.cpp

static std::pair<int, int> mmToHPGL(double xMm, double yMm, const DrawingAreaConfig& da, const HPGLSettings& settings) {
    double wMm = std::max(da.width_mm, 0.001);
    double hMm = std::max(da.height_mm, 0.001);

    int x = static_cast<int>((xMm / wMm) * settings.xMax);
    int y = static_cast<int>((yMm / hMm) * settings.yMax);

    if (settings.xMirror) x = settings.xMax - x;
    if (settings.yMirror) y = settings.yMax - y;

    return {x, y};
}

bool HPGLExporter::exportHPGL(const QString& filepath,
                              const QVector<DrawingGeometry>& geometries,
                              const DrawingAreaConfig& drawingArea,
                              int imageWidth,
                              int imageHeight,
                              const HPGLSettings& settings) {
    if (geometries.isEmpty()) return false;

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // Convert pixel coordinates to physical mm
    QVector<DrawingGeometry> mmGeoms;
    mmGeoms.reserve(geometries.size());
    for (const auto& geom : geometries) {
        Path mmPath = pixel_path_to_mm(geom.path, imageWidth, imageHeight, drawingArea);
        if (!mmPath.empty()) {
            mmGeoms.append({mmPath, geom.penIndex, geom.groupIndex});
        }
    }

    // Prepare, filter and optimize paths
    QMap<int, QVector<Path>> penPaths = PathOptimizer::preparePenPaths(
        mmGeoms, settings.minPathLength, settings.optimizePaths);

    double h_mm = drawingArea.height_mm;

    out << "IN;\n";
    out << QString("SP%1;\n").arg(settings.penNumber);
    out << QString("VS%1;\n").arg(settings.penVelocity);
    out << QString("FS%1;\n").arg(settings.curveFlatness, 0, 'f', 2);

    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        int penIdx = it.key();
        out << "; Layer: Pen " << penIdx << "\n";

        for (const auto& path : it.value()) {
            if (path.size() < 2) continue;

            auto [hx0, hy0] = mmToHPGL(path[0].first, h_mm - path[0].second, drawingArea, settings);
            out << QString("PU%1,%2;\n").arg(hx0).arg(hy0);

            QStringList segments;
            for (std::size_t i = 1; i < path.size(); ++i) {
                auto [hx, hy] = mmToHPGL(path[i].first, h_mm - path[i].second, drawingArea, settings);
                segments.append(QString("%1,%2").arg(hx).arg(hy));
            }

            if (!segments.isEmpty()) {
                out << "PD" << segments.join(",") << ";\n";
            }
        }
    }

    out << "SP0;\n";
    return true;
}


// Original File: ../cpp/export\path_optimizer.cpp

QMap<int, QVector<Path>> PathOptimizer::preparePenPaths(
    const QVector<DrawingGeometry>& geometries,
    double minLength,
    bool optimize) 
{
    QMap<int, QVector<Path>> penPaths;
    
    // Group paths by pen index
    for (const auto& geom : geometries) {
        if (geom.path.size() >= 2) {
            penPaths[geom.penIndex].append(geom.path);
        }
    }
    
    // Process each pen layer
    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        std::vector<Path> stdPaths(it.value().begin(), it.value().end());
        
        // Filter short paths
        if (minLength > 0.0) {
            stdPaths = filter_short_paths(stdPaths, minLength);
        }
        
        // Optimize travel path using nearest neighbor
        if (optimize) {
            stdPaths = sort_paths_nearest(stdPaths);
        }
        
        it.value() = QVector<Path>(stdPaths.begin(), stdPaths.end());
    }
    
    return penPaths;
}


// Original File: ../cpp/export\pdf_exporter.cpp

bool PDFExporter::exportPDF(const QString& filepath,
                            const QVector<DrawingGeometry>& geometries,
                            const DrawingAreaConfig& drawingArea,
                            int imageWidth,
                            int imageHeight,
                            const QVector<QColor>& penColors,
                            double penWidthMm) {
    if (geometries.isEmpty()) return false;

    QPdfWriter pdfWriter(filepath);
    
    // Set up PDF page to match DrawingArea precisely
    QSizeF pageSizeMm(drawingArea.width_mm, drawingArea.height_mm);
    pdfWriter.setPageSize(QPageSize(pageSizeMm, QPageSize::Millimeter));
    pdfWriter.setPageMargins(QMarginsF(0, 0, 0, 0));
    pdfWriter.setResolution(300); // DPI

    QPainter painter(&pdfWriter);
    if (!painter.isActive()) {
        qWarning() << "Failed to open PDF for writing:" << filepath;
        return false;
    }

    painter.setRenderHint(QPainter::Antialiasing);

    // Group paths by pen index
    QMap<int, QVector<Path>> penPaths;
    for (const auto& geom : geometries) {
        Path mmPath = pixel_path_to_mm(geom.path, imageWidth, imageHeight, drawingArea);
        if (!mmPath.empty()) {
            penPaths[geom.penIndex].append(mmPath);
        }
    }

    // Determine scale from mm to printer logical dots (resolution)
    // QPdfWriter uses logical dots. The conversion factor is derived from resolution.
    double dotsPerMm = pdfWriter.resolution() / 25.4;

    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        int penIdx = it.key();
        QColor color = (penIdx >= 0 && penIdx < penColors.size()) ? penColors[penIdx] : QColor(0, 0, 0);
        
        QPen qtPen(color);
        qtPen.setWidthF(penWidthMm * dotsPerMm);
        qtPen.setCapStyle(Qt::RoundCap);
        qtPen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(qtPen);
        
        for (const auto& path : it.value()) {
            if (path.size() < 2) continue;
            
            QPainterPath qpath;
            qpath.moveTo(path[0].first * dotsPerMm, path[0].second * dotsPerMm);
            for (std::size_t i = 1; i < path.size(); ++i) {
                qpath.lineTo(path[i].first * dotsPerMm, path[i].second * dotsPerMm);
            }
            
            painter.drawPath(qpath);
        }
    }

    painter.end();
    return true;
}


// Original File: ../cpp/export\svg_exporter.cpp

bool SVGExporter::exportSVG(const QString& filepath,
                            const QVector<DrawingGeometry>& geometries,
                            const DrawingAreaConfig& drawingArea,
                            int imageWidth,
                            int imageHeight,
                            const QVector<QColor>& penColors,
                            double penWidthMm) {
    if (geometries.isEmpty()) return false;

    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // Write SVG header
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << QString("<svg xmlns=\"http://www.w3.org/2000/svg\" "
                   "xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\" "
                   "width=\"%1mm\" height=\"%2mm\" "
                   "viewBox=\"0 0 %1 %2\">\n")
               .arg(drawingArea.width_mm).arg(drawingArea.height_mm);

    // Group paths by pen index
    QMap<int, QVector<Path>> penPaths;
    for (const auto& geom : geometries) {
        Path mmPath = pixel_path_to_mm(geom.path, imageWidth, imageHeight, drawingArea);
        if (!mmPath.empty()) {
            penPaths[geom.penIndex].append(mmPath);
        }
    }

    // Write groups
    for (auto it = penPaths.begin(); it != penPaths.end(); ++it) {
        int penIdx = it.key();
        QColor color = (penIdx >= 0 && penIdx < penColors.size()) ? penColors[penIdx] : QColor(0, 0, 0);
        
        out << QString("  <g id=\"pen_%1\" inkscape:label=\"Pen %1\">\n").arg(penIdx);
        
        for (const auto& path : it.value()) {
            if (path.size() < 2) continue;
            
            QString d = QString("M%1,%2").arg(path[0].first, 0, 'f', 3).arg(path[0].second, 0, 'f', 3);
            for (std::size_t i = 1; i < path.size(); ++i) {
                d += QString(" L%1,%2").arg(path[i].first, 0, 'f', 3).arg(path[i].second, 0, 'f', 3);
            }
            
            out << QString("    <path d=\"%1\" fill=\"none\" stroke=\"%2\" "
                           "stroke-width=\"%3\" stroke-linecap=\"round\" "
                           "stroke-linejoin=\"round\"/>\n")
                       .arg(d).arg(color.name()).arg(penWidthMm, 0, 'f', 4);
        }
        
        out << "  </g>\n";
    }

    out << "</svg>\n";
    return true;
}



// ===========================================================================
// Main Entry Point
// ===========================================================================
int main(int argc, char* argv[]) {
    std::cout << "Amalgamated CLI Ready." << std::endl;
    return 0;
}
