#pragma once
#include "qnx_screen_display.hpp"
class qnx_screen_display_zone : public qnx_screen_display {
private:
    // full screen display by default
    int margin_left_ = -1;
    int margin_top_ = -1;
    int margin_right_ = -1;
    int margin_bottom_ = -1;
    int display_rect_[4] = { 0 };
    // display camera frame
    int min_display_width_ = MIN_DISPLAY_WIDTH;
    int min_display_height_ = MIN_DISPLAY_HEIGHT;
public:
    static qnx_screen_display_zone& get_instance() {
        static qnx_screen_display_zone instance;
        return instance;
    }
    int init() {
        return init_win();
    }
    inline void set_min_display_scale(int w, int h) {
        min_display_width_ = w;
        min_display_height_ = h;
    }
    // left:left distance from the far left side of the screen
    // top:top distance from the far top side of the screen
    // right distance from the far right side of the screen
    // bottom distance from the far bottom side of the screen
    int set_display_margin(int left,            // left margin
                           int top,             // top margin
                           int right,           // right margin
                           int bottom) {        // bottom margin
        if ((left == margin_left_) && (right == margin_right_) && (top == margin_top_) && (bottom == margin_bottom_)) {
            SLOG_D("not need to set display margin,left:%d, top:%d, right:%d, bottom:%d", left, top, right, bottom);
            return 0;
        }
        if ((left < 0) || (top < 0) || (right < 0) || (bottom < 0)) {
            return DISPLAY_MARGIN_COORDINATE_INVALID;
        }
        if ((left + right + min_display_width_ > QNX_SCREEN_CTX.screen_size[0]) || (top + bottom + min_display_height_> QNX_SCREEN_CTX.screen_size[1])) {
            SLOG_E("invalid layout margin value,left:%d, top:%d, right:%d, bottom:%d", left, top, right, bottom);
            return DISPLAY_MARGIN_COORDINATE_EXCEED;
        }
        display_rect_[0] = left;
        display_rect_[1] = top;
        SLOG_I("display zone position: %dx%d", display_rect_[0], display_rect_[1]);
        int error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_POSITION, display_rect_);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_POSITION failed, error:%s", strerror(errno));
            return error;
        }
        display_rect_[2] = QNX_SCREEN_CTX.screen_size[0] - left - right;
        display_rect_[3] = QNX_SCREEN_CTX.screen_size[1] - top - bottom;
        SLOG_I("display zone size: %dx%d", display_rect_[2], display_rect_[3]);
        error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_SIZE, display_rect_ + 2);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_SIZE failed, error:%s", strerror(errno));
            return error;
        }
        screen_buffer_t render_buf[2] = { 0 };
        error = screen_get_window_property_pv(win_, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)render_buf);
        if (error) {
            SLOG_E("screen_get_window_property_pv for SCREEN_PROPERTY_RENDER_BUFFERS failed, error:%s", strerror(errno));
            return error;
        }
        const static int black_color = 0xff000000;
        const static int bg_color[] = {SCREEN_BLIT_COLOR, black_color, SCREEN_BLIT_END};
        screen_fill(QNX_SCREEN_CTX.screen_ctx, render_buf[0], bg_color);
        screen_post_window(win_, render_buf[0], 0, nullptr, 0);
        margin_left_ = left;
        margin_right_ = right;
        margin_top_ = top;
        margin_bottom_ = bottom;
        return 0;
    }
private:
    qnx_screen_display_zone() = default;
    virtual ~qnx_screen_display_zone() {
        unit_win();
    }
    int init_win() {
        int error = qnx_screen_display::init_win();
        if (error) {
            return error;
        }
        set_display_zorder(QNX_SCREEN_DEFAULT_ZORDER_OF_ZONE);
        error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_ZORDER, &display_zorder_);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_ZORDER failed, error:%s", strerror(errno));
            return error;
        }
        static const int alpha = 255;
        error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_GLOBAL_ALPHA, &alpha);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_GLOBAL_ALPHA failed, error:%s", strerror(errno));
            return error;
        }
        SLOG_I("display win position: %dx%d", display_rect_[0], display_rect_[1]);
        error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_POSITION, display_rect_);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_POSITION failed, error:%s", strerror(errno));
            return error;
        }
        display_rect_[2] = QNX_SCREEN_CTX.screen_size[0];
        display_rect_[3] = QNX_SCREEN_CTX.screen_size[1];
        SLOG_I("display win size: %dx%d", display_rect_[2], display_rect_[3]);
        error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_SIZE, display_rect_ + 2);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_SIZE failed, error:%s", strerror(errno));
            return error;
        }
        return set_display_margin(0, 0, 0, 0);      // black the whole screen zone
    }
};
#define QNX_SCREEN_DISPLAY_ZONE qnx_screen_display_zone::get_instance()