#pragma once

#include "mbed.h"

// basic types for image processing

namespace cv
{
    class Size;
    class Rect;
    class Point
    {
    public:
        Point() = default;
        Point(const Point& r) = default;
        Point(int _x, int _y);
        Point(const Size& sz);
        Point& operator = (const Point& r) = default;

        bool inside(const Rect& r) const;

        int x = 0; //!< x coordinate of the point
        int y = 0; //!< y coordinate of the point
    };

    class Size
    {
    public:
        Size() = default;
        Size(const Size& r) = default;
        Size(int _width, int _height);
        Size(const Point& pt);
        Size& operator = (const Size& r) = default;

        int area() const;
        float aspectRatio() const;
        bool empty() const;

        int width = 0; //!< the width
        int height = 0; //!< the height
    };

    class Rect
    {
    public:
        Rect() = default;
        Rect(const Rect& r) = default;
        Rect(int _x, int _y, int _width, int _height);
        Rect(const Point& org, const Size& sz);
        Rect(const Point& pt1, const Point& pt2);
        Rect& operator = (const Rect& r) = default;

        Point tl() const;
        Point br() const;
        Size size() const;
        int area() const;
        bool empty() const;
        bool contains(const Point& pt) const;

        int x = 0; //!< x coordinate of the top-left corner
        int y = 0; //!< y coordinate of the top-left corner
        int width = 0; //!< width of the rectangle
        int height = 0; //!< height of the rectangle
    };

    class Size2f;
    class Rect2f;
    class Point2f
    {
    public:
        Point2f() = default;
        Point2f(const Point2f& r) = default;
        Point2f(float _x, float _y);
        Point2f(const Size2f& sz);
        Point2f& operator = (const Point2f& r) = default;

        bool inside(const Rect2f& r) const;

        float x = 0.0f; //!< x coordinate of the point
        float y = 0.0f; //!< y coordinate of the point
    };

    class Size2f
    {
    public:
        Size2f() = default;
        Size2f(const Size2f& r) = default;
        Size2f(float _width, float _height);
        Size2f(const Point2f& pt);
        Size2f& operator = (const Size2f& r) = default;

        float area() const;
        float aspectRatio() const;
        bool empty() const;

        float width = 0; //!< the width
        float height = 0; //!< the height
    };

    class Rect2f
    {
    public:
        Rect2f() = default;
        Rect2f(const Rect2f& r) = default;
        Rect2f(float _x, float _y, float _width, float _height);
        Rect2f(const Point2f& org, const Size2f& sz);
        Rect2f(const Point2f& pt1, const Point2f& pt2);
        Rect2f& operator = (const Rect2f& r) = default;

        Point2f tl() const;
        Point2f br() const;
        Size2f size() const;
        float area() const;
        bool empty() const;
        bool contains(const Point2f& pt) const;

        float x = 0; //!< x coordinate of the top-left corner
        float y = 0; //!< y coordinate of the top-left corner
        float width = 0; //!< width of the rectangle
        float height = 0; //!< height of the rectangle
    };

    class RotatedRect
    {
    public:
        RotatedRect() = default;
        RotatedRect(const Point2f& center, const Size2f& size, float angle);
        RotatedRect(const Point2f& point1, const Point2f& point2, const Point2f& point3);
        void points(Point2f pts[]) const;
        Rect boundingRect() const;
        Rect2f boundingRect2f() const;

        Point2f center;
        Size2f size;
        float angle = 0;
    };

    class Range
    {
    public:
        Range() = default;
        Range(const Range& r) = default;
        Range(int _start, int _end);
        Range& operator = (const Range& r) = default;
        int size() const;
        bool empty() const;
        static Range all();

        int start = 0, end = 0;
    };

    enum MatType { MONO8 = 0, RGB565 = 1 };
    constexpr size_t AUTO_STEP = 0;

    class Mat
    {
    public:
        Mat() = default;
        Mat(const Mat& m) = default;
        Mat(int _rows, int _cols, int _type, void* _data, size_t _step=AUTO_STEP);
        Mat(Size size, int _type, void* _data, size_t _step=AUTO_STEP);
        Mat(const Mat& m, const Rect& roi);
        Mat(const Mat& m, const Range& rowRange, const Range& colRange=Range::all());
        Mat& operator = (const Mat& m) = default;
        Mat row(int y) const;
        Mat col(int x) const;
        Mat rowRange(int startrow, int endrow) const;
        Mat colRange(int startcol, int endcol) const;
        Mat& operator = (const uint16_t& s);
        Mat operator()( const Rect& roi ) const;
        Size size() const;
        bool isContinuous() const;
        size_t elemSize() const;
        bool empty() const;
        size_t total() const;
        bool copyTo(Mat arr) const;
    
        template<typename _Tp> _Tp* ptr(int row = 0)
        {
                return reinterpret_cast<_Tp*>(data + step[0] * row);
        }

        template<typename _Tp> const _Tp* ptr(int row = 0) const
        {
                return reinterpret_cast<const _Tp*>(data + step[0] * row);
        }

        template<typename _Tp> _Tp* ptr(int row, int col)
        {
            return reinterpret_cast<_Tp*>(data + row * step[0] + col * step[1]);
        }

        template<typename _Tp> const _Tp* ptr(int row, int col) const
        {
            return reinterpret_cast<const _Tp*>(data + row * step[0] + col * step[1]);
        }

        template<typename _Tp> _Tp& at(int row, int col)
        {
            return *reinterpret_cast<_Tp*>(data + row * step[0] + col * step[1]);
        }

        template<typename _Tp> const _Tp& at(int row, int col) const
        {
            return *reinterpret_cast<const _Tp*>(data + row * step[0] + col * step[1]);
        }

        int type = 0;
        int rows = 0, cols = 0;
        size_t step[2] = { 0, 0 };
        uint8_t* data = nullptr;
    };

    inline bool operator == (const Point& a, const Point& b)
    {
        return a.x == b.x && a.y == b.y;
    }

    inline bool operator != (const Point& a, const Point& b)
    {
        return !(a == b);
    }

    inline Point& operator += (Point& a, const Point& b)
    {
        a.x += b.x;
        a.y += b.y;
        return a;
    }

    inline Point& operator -= (Point& a, const Point& b)
    {
        a.x -= b.x;
        a.y -= b.y;
        return a;
    }

    inline Point operator + (const Point& a, const Point& b)
    {
        return Point(a.x + b.x, a.y + b.y);
    }

    inline Point operator - (const Point& a, const Point& b)
    {
        return Point( a.x - b.x, a.y - b.y );
    }

    inline float norm(const Point& pt)
    {
        return std::sqrtf((float)pt.x*pt.x + (float)pt.y*pt.y);
    }

    inline bool operator == (const Size& a, const Size& b)
    {
        return a.width == b.width && a.height == b.height;
    }

    inline bool operator != (const Size& a, const Size& b)
    {
        return !(a == b);
    }

    inline bool operator == (const Rect& a, const Rect& b)
    {
        return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
    }

    inline bool operator != (const Rect& a, const Rect& b)
    {
        return !(a == b);
    }

    inline Rect& operator &= (Rect& a, const Rect& b)
    {
        if (a.empty() || b.empty()) {
            a = Rect();
            return a;
        }
        const Rect& Rx_min = (a.x < b.x) ? a : b;
        const Rect& Rx_max = (a.x < b.x) ? b : a;
        const Rect& Ry_min = (a.y < b.y) ? a : b;
        const Rect& Ry_max = (a.y < b.y) ? b : a;
        if ((Rx_min.x < 0 && Rx_min.x + Rx_min.width < Rx_max.x) ||
            (Ry_min.y < 0 && Ry_min.y + Ry_min.height < Ry_max.y)) {
            a = Rect();
            return a;
        }
        a.width = std::min(Rx_min.width - (Rx_max.x - Rx_min.x), Rx_max.width);
        a.height = std::min(Ry_min.height - (Ry_max.y - Ry_min.y), Ry_max.height);
        a.x = Rx_max.x;
        a.y = Ry_max.y;
        if (a.empty())
            a = Rect();
        return a;
    }

    inline Rect& operator |= ( Rect& a, const Rect& b )
    {
        if (a.empty()) {
            a = b;
        }
        else if (!b.empty()) {
            int x1 = std::min(a.x, b.x);
            int y1 = std::min(a.y, b.y);
            a.width = std::max(a.x + a.width, b.x + b.width) - x1;
            a.height = std::max(a.y + a.height, b.y + b.height) - y1;
            a.x = x1;
            a.y = y1;
        }
        return a;
    }

    inline Rect operator & (const Rect& a, const Rect& b)
    {
        Rect c = a;
        return c &= b;
    }

    inline Rect operator | (const Rect& a, const Rect& b)
    {
        Rect c = a;
        return c |= b;
    }

    inline Point2f& operator += (Point2f& a, const Point2f& b)
    {
        a.x += b.x;
        a.y += b.y;
        return a;
    }

    inline Point2f& operator -= (Point2f& a, const Point2f& b)
    {
        a.x -= b.x;
        a.y -= b.y;
        return a;
    }

    inline Point2f operator + (const Point2f& a, const Point2f& b)
    {
        return Point2f(a.x + b.x, a.y + b.y);
    }

    inline Point2f operator - (const Point2f& a, const Point2f& b)
    {
        return Point2f( a.x - b.x, a.y - b.y );
    }

    inline float norm(const Point2f& pt)
    {
        return std::sqrtf((float)pt.x*pt.x + (float)pt.y*pt.y);
    }

    inline Rect2f& operator &= (Rect2f& a, const Rect2f& b)
    {
        if (a.empty() || b.empty()) {
            a = Rect2f();
            return a;
        }
        const Rect2f& Rx_min = (a.x < b.x) ? a : b;
        const Rect2f& Rx_max = (a.x < b.x) ? b : a;
        const Rect2f& Ry_min = (a.y < b.y) ? a : b;
        const Rect2f& Ry_max = (a.y < b.y) ? b : a;
        if ((Rx_min.x < 0 && Rx_min.x + Rx_min.width < Rx_max.x) ||
            (Ry_min.y < 0 && Ry_min.y + Ry_min.height < Ry_max.y)) {
            a = Rect2f();
            return a;
        }
        a.width = std::min(Rx_min.width - (Rx_max.x - Rx_min.x), Rx_max.width);
        a.height = std::min(Ry_min.height - (Ry_max.y - Ry_min.y), Ry_max.height);
        a.x = Rx_max.x;
        a.y = Ry_max.y;
        if (a.empty())
            a = Rect2f();
        return a;
    }

    inline Rect2f& operator |= ( Rect2f& a, const Rect2f& b )
    {
        if (a.empty()) {
            a = b;
        }
        else if (!b.empty()) {
            int x1 = std::min(a.x, b.x);
            int y1 = std::min(a.y, b.y);
            a.width = std::max(a.x + a.width, b.x + b.width) - x1;
            a.height = std::max(a.y + a.height, b.y + b.height) - y1;
            a.x = x1;
            a.y = y1;
        }
        return a;
    }

    inline Rect2f operator & (const Rect2f& a, const Rect2f& b)
    {
        Rect2f c = a;
        return c &= b;
    }

    inline Rect2f operator | (const Rect2f& a, const Rect2f& b)
    {
        Rect2f c = a;
        return c |= b;
    }

    inline bool operator == (const Range& a, const Range& b)
    {
        return a.start == b.start && a.end == b.end;
    }

    inline bool operator != (const Range& a, const Range& b)
    {
        return !(a == b);
    }

}

constexpr float CV_PI = 3.1415926f;

inline int cvRound(float value)
{
    return int(std::lround(value));
}

inline int cvCeil(float value)
{
    int i = (int)value;
    return i + (i < value);
}

inline int cvFloor(float value)
{
    int i = (int)value;
    return i - (i > value);
}
