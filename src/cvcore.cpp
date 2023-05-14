#include "cvcore.h"
#include <climits>
#include <utility>
#include <algorithm>
#include <cmath>

namespace cv
{
    Point::Point(int _x, int _y)
        : x(_x), y(_y)
    {
    }

    Point::Point(const Size& sz)
        : x(sz.width), y(sz.height)
    {
    }

    bool Point::inside(const Rect& r) const
    {
        return r.contains(*this);
    }

    Size::Size(int _width, int _height)
        : width(_width), height(_height)
    {
    }

    Size::Size(const Point& pt)
        : width(pt.x), height(pt.y)
    {
    }

    int Size::area() const
    {
        return width * height;
    }

    float Size::aspectRatio() const
    {
        return static_cast<float>(width) / static_cast<float>(height);
    }

    bool Size::empty() const
    {
        return width <= 0 || height <= 0;
    }

    Rect::Rect(int _x, int _y, int _width, int _height)
        : x(_x), y(_y), width(_width), height(_height)
    {
    }

    Rect::Rect(const Point& org, const Size& sz)
        : x(org.x), y(org.y), width(sz.width), height(sz.height)
    {
    }

    Rect::Rect(const Point& pt1, const Point& pt2)
    {
        x = std::min(pt1.x, pt2.x);
        y = std::min(pt1.y, pt2.y);
        width = std::max(pt1.x, pt2.x) - x;
        height = std::max(pt1.y, pt2.y) - y;
    }

    Point Rect::tl() const
    {
        return Point(x, y);
    }

    Point Rect::br() const
    {
        return Point(x + width, y + height);
    }

    Size Rect::size() const
    {
        return Size(width, height);
    }

    int Rect::area() const
    {
        return width * height;
    }

    bool Rect::empty() const
    {
        return width <= 0 || height <= 0;
    }

    bool Rect::contains(const Point& pt) const
    {
        return x <= pt.x && pt.x < x + width && y <= pt.y && pt.y < y + height;
    }

    Point2f::Point2f(float _x, float _y)
        : x(_x), y(_y)
    {
    }

    Point2f::Point2f(const Size2f& sz)
        : x(sz.width), y(sz.height)
    {
    }

    bool Point2f::inside(const Rect2f& r) const
    {
        return r.contains(*this);
    }

    Size2f::Size2f(float _width, float _height)
        : width(_width), height(_height)
    {
    }

    Size2f::Size2f(const Point2f& pt)
        : width(pt.x), height(pt.y)
    {
    }

    float Size2f::area() const
    {
        return width * height;
    }

    float Size2f::aspectRatio() const
    {
        return width / height;
    }

    bool Size2f::empty() const
    {
        return width <= 0.0f || height <= 0.0f;
    }

    Rect2f::Rect2f(float _x, float _y, float _width, float _height)
        : x(_x), y(_y), width(_width), height(_height)
    {
    }

    Rect2f::Rect2f(const Point2f& org, const Size2f& sz)
        : x(org.x), y(org.y), width(sz.width), height(sz.height)
    {
    }

    Rect2f::Rect2f(const Point2f& pt1, const Point2f& pt2)
    {
        x = std::min(pt1.x, pt2.x);
        y = std::min(pt1.y, pt2.y);
        width = std::max(pt1.x, pt2.x) - x;
        height = std::max(pt1.y, pt2.y) - y;
    }

    Point2f Rect2f::tl() const
    {
        return Point2f(x, y);
    }

    Point2f Rect2f::br() const
    {
        return Point2f(x + width, y + height);
    }

    Size2f Rect2f::size() const
    {
        return Size2f(width, height);
    }

    float Rect2f::area() const
    {
        return width * height;
    }

    bool Rect2f::empty() const
    {
        return width <= 0.0f || height <= 0.0f;
    }

    bool Rect2f::contains(const Point2f& pt) const
    {
        return x <= pt.x && pt.x < x + width && y <= pt.y && pt.y < y + height;
    }

    RotatedRect::RotatedRect(const Point2f& _center, const Size2f& _size, float _angle)
        : center(_center), size(_size), angle(_angle)
    {
    }

    RotatedRect::RotatedRect(const Point2f& _point1, const Point2f& _point2, const Point2f& _point3)
    {
        Point2f _center = _point1 + _point3;
        _center.x *= 0.5f;
        _center.y *= 0.5f;
        Point2f vecs[2];
        vecs[0] = _point1 - _point2;
        vecs[1] = _point2 - _point3;
        float x = std::max(norm(_point1), std::max(norm(_point2), norm(_point3)));
        float a = std::min(norm(vecs[0]), norm(vecs[1]));

        // wd_i stores which vector (0,1) or (1,2) will make the width
        // One of them will definitely have slope within -1 to 1
        int wd_i = 0;
        if( std::fabsf(vecs[1].y) < std::fabsf(vecs[1].x) ) wd_i = 1;
        int ht_i = (wd_i + 1) % 2;

        float _angle = std::atanf(vecs[wd_i].y / vecs[wd_i].x) * 180.0f / (float) CV_PI;
        float _width = (float) norm(vecs[wd_i]);
        float _height = (float) norm(vecs[ht_i]);

        center = _center;
        size = Size2f(_width, _height);
        angle = _angle;
    }

    void RotatedRect::points(Point2f pt[]) const
    {
        float _angle = angle * CV_PI / 180.0f;
        float b = (float)cos(_angle)*0.5f;
        float a = (float)sin(_angle)*0.5f;
        pt[0].x = center.x - a*size.height - b*size.width;
        pt[0].y = center.y + b*size.height - a*size.width;
        pt[1].x = center.x + a*size.height - b*size.width;
        pt[1].y = center.y - b*size.height - a*size.width;
        pt[2].x = 2*center.x - pt[0].x;
        pt[2].y = 2*center.y - pt[0].y;
        pt[3].x = 2*center.x - pt[1].x;
        pt[3].y = 2*center.y - pt[1].y;
    }

    Rect RotatedRect::boundingRect() const
    {
        Point2f pt[4];
        points(pt);
        Rect r(cvFloor(std::min(std::min(std::min(pt[0].x, pt[1].x), pt[2].x), pt[3].x)),
            cvFloor(std::min(std::min(std::min(pt[0].y, pt[1].y), pt[2].y), pt[3].y)),
            cvCeil(std::max(std::max(std::max(pt[0].x, pt[1].x), pt[2].x), pt[3].x)),
            cvCeil(std::max(std::max(std::max(pt[0].y, pt[1].y), pt[2].y), pt[3].y)));
        r.width -= r.x - 1;
        r.height -= r.y - 1;
        return r;
    }


    Rect2f RotatedRect::boundingRect2f() const
    {
        Point2f pt[4];
        points(pt);
        Rect2f r(Point2f(min(min(min(pt[0].x, pt[1].x), pt[2].x), pt[3].x), min(min(min(pt[0].y, pt[1].y), pt[2].y), pt[3].y)),
                    Point2f(max(max(max(pt[0].x, pt[1].x), pt[2].x), pt[3].x), max(max(max(pt[0].y, pt[1].y), pt[2].y), pt[3].y)));
        return r;
    }

    Range::Range(int _start, int _end)
        : start(_start), end(_end)
    {
    }

    int Range::size() const
    {
        return end - start;
    }

    bool Range::empty() const
    {
        return end == start;
    }

    Range Range::all()
    {
        return Range(INT_MIN, INT_MAX);
    }

    Mat::Mat(int _rows, int _cols, int _type, void* _data, size_t _step)
        : rows(_rows), cols(_cols), type(_type), data(reinterpret_cast<uint8_t*>(_data))
    {
        step[0] = _step;
        step[1] = elemSize();
        if(_step == 0)
        {
            step[0] = _cols * step[1];
        }
    }

    Mat::Mat(Size size, int _type, void* _data, size_t _step)
        : rows(size.height), cols(size.width), type(_type), data(reinterpret_cast<uint8_t*>(_data))
    {
        step[0] = _step;
        step[1] = elemSize();
        if(_step == 0)
        {
            step[0] = size.width * step[1];
        }
    }

    Mat::Mat(const Mat& m, const Rect& roi)
        : type(m.type), rows(roi.height), cols(roi.width), data(m.data + roi.y*m.step[0])
    {
        step[0] = m.step[0];
        step[1] = m.step[1];
        data += roi.x * m.step[1];
    }

    Mat::Mat(const Mat& m, const Range& _rowRange, const Range& _colRange)
    {
        *this = m;
        if( _rowRange != Range::all() && _rowRange != Range(0,rows) )
        {
            rows = _rowRange.size();
            data += step[0]*_rowRange.start;
        }
        if( _colRange != Range::all() && _colRange != Range(0,cols) )
        {
            cols = _colRange.size();
            data += step[1]*_colRange.start;
        }
    }

    Mat Mat::row(int y) const
    {
        return Mat(*this, Range(y, y + 1), Range::all());
    }

    Mat Mat::col(int x) const
    {
        return Mat(*this, Range::all(), Range(x, x + 1));
    }

    Mat Mat::rowRange(int startrow, int endrow) const
    {
        return Mat(*this, Range(startrow, endrow), Range::all());
    }

    Mat Mat::colRange(int startcol, int endcol) const
    {
        return Mat(*this, Range::all(), Range(startcol, endcol));
    }

    Mat& Mat::operator =(const uint16_t& s)
    {
        switch (type)
        {
        case MONO8:
        {
            uint8_t v = static_cast<uint8_t>(s);
            if (isContinuous())
            {
                memset(ptr<uint8_t>(), v, total());
            }
            else
            {
                for (int y = 0; y < rows; y++)
                {
                    memset(ptr<uint8_t>(y), static_cast<size_t>(cols), v);
                }
            }
            break;
        }
        case RGB565:
        {
            if (isContinuous())
            {
                std::fill_n(ptr<uint16_t>(), total(), s);
            }
            else
            {
                for (int y = 0; y < rows; y++)
                {
                    std::fill_n(ptr<uint16_t>(y), static_cast<size_t>(cols), s);
                }
            }
            break;
        }
        }
        return *this;
    }

    Mat Mat::operator()( const Rect& roi ) const
    {
        return Mat(*this, roi);
    }

    bool Mat:: isContinuous() const
    {
        return step[0] == step[1] * cols;
    }

    Size Mat:: size() const
    {
        return Size(cols, rows);
    }

    size_t Mat::elemSize() const
    {
        switch(type)
        {
            case MONO8:
                return 1;
            case RGB565:
                return 2;
        }
        return 0;
    }

    bool Mat::empty() const
    {
        return cols <= 0 || rows <= 0;
    }

    size_t Mat::total() const
    {
        return cols * rows;
    }

    bool Mat::copyTo(Mat arr) const
    {
        if (type != arr.type)
        {
            return false;
        }
        int rows_to_copy = std::min(rows, arr.rows);
        int cols_to_copy = std::min(cols, arr.cols);
        if (rows_to_copy > 0 && cols_to_copy > 0)
        {
            switch (type)
            {
            case MONO8:
            {
                for (int y = 0; y < rows_to_copy; y++)
                {
                    std::copy_n(ptr<uint8_t>(), cols_to_copy, arr.ptr<uint8_t>());
                }
                break;
            }
            case RGB565:
            {
                for (int y = 0; y < rows_to_copy; y++)
                {
                    std::copy_n(ptr<uint16_t>(), cols_to_copy, arr.ptr<uint16_t>());
                }
                break;
            }
            }
        }
        return true;
    }
}
