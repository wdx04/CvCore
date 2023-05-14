#pragma once

#include "mbed.h"
#include "cvcore.h"
#include "cvfonts.h"
#include "dma2d.h"
#include <vector>

namespace cv
{
    constexpr int FILLED = -1;

    constexpr uint16_t RGB565_BLACK = 0x0000;
    constexpr uint16_t RGB565_BLUE = 0x001F;
    constexpr uint16_t RGB565_RED = 0xF800;
    constexpr uint16_t RGB565_GREEN = 0x07E0;
    constexpr uint16_t RGB565_CYAN = 0x07FF;
    constexpr uint16_t RGB565_MAGENTA = 0xF81F;
    constexpr uint16_t RGB565_YELLOW = 0xFFE0;
    constexpr uint16_t RGB565_WHITE = 0xFFFF;

    constexpr uint8_t RGB332_BLACK = 0x00;
    constexpr uint8_t RGB332_BLUE = 0x03;
    constexpr uint8_t RGB332_RED = 0xE0;
    constexpr uint8_t RGB332_GREEN = 0x1C;
    constexpr uint8_t RGB332_CYAN = 0x1F;
    constexpr uint8_t RGB332_MAGENTA = 0xE3;
    constexpr uint8_t RGB332_YELLOW = 0xFC;
    constexpr uint8_t RGB332_WHITE = 0xFF;

    class Painter
    {
    public:
        Painter(const Mat& _mat);

        void fill(uint16_t color);

        void rectangle(Point pt1, Point pt2, uint16_t color, int thickness=1);

        void line(Point pt1, Point pt2, uint16_t color, int thickness=1);

        void circle(Point center, int radius, uint16_t color, int thickness=1);

        void ellipse(const RotatedRect& box, uint16_t color, int thickness=1);

        void ellipse(Point center, Size axes, float angle, float startAngle, float endAngle, uint16_t color, int thickness=1);

        void polyline(const std::vector<Point>& contour, uint16_t color, int thickness=1);

        void putText(std::string_view text, Point org, FontBase& font, uint16_t text_color, uint16_t bg_color, bool word_wrap = false, int *consumed_chars = nullptr);

        void putText(std::wstring_view text, Point org, UnicodeFont& font, uint16_t text_color, uint16_t bg_color, bool word_wrap = false, int *consumed_chars = nullptr);

        void drawBitmap(const Mat& bitmap, Point org);

        Mat get_mat() const;

        Size get_mat_size() const;

        Rect get_dirty_rect() const;

        void update_dirty_rect(Rect rc);

        void reset_dirty_rect();

    private:
        Mat mat;
        Rect dirty_rect { 0, 0, 0, 0 };
    };

    extern const uint16_t RGB332to565LUT[256];

    Rect2f boundingRect(const std::vector<cv::Point2f>& pts);

    Rect boundingRect(const std::vector<cv::Point>& pts);

    inline uint16_t rgb332_to_rgb565(uint8_t src)
    {
        return RGB332to565LUT[src];
    }

    inline uint16_t swap_bytes(uint16_t src)
    {
        return (src >> 8) | (src << 8);
    }

    inline uint16_t rgb332_to_rgb565_swapped(uint8_t src)
    {
        return swap_bytes(RGB332to565LUT[src]);
    }
}
