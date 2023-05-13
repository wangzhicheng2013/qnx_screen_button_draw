#pragma once
#include "qnx_screen_display.hpp"
#include "qnx_text_draw.hpp"
class qnx_screen_display_text : public qnx_screen_display {
private:
    int font_size_ = DEFAULT_FONT_SIZE;
    unsigned char* font_mask_ = nullptr;
protected:
    rectangle valid_rect_;  // a reginal for a button
public:
    qnx_screen_display_text() = default;
    virtual ~qnx_screen_display_text() {
        unit_win();
        free_font_mask();
    }
    int init() {
        return init_win();
    }
    inline void set_font_size(int size) {
        font_size_ = size;
    }
    inline void set_valid_rectangle(int x, int y, int width, int height) {
        valid_rect_.set(x, y, x + width, y + height);
        SLOG_I("valid rect = [%d, %d, %d, %d]", valid_rect_.left, valid_rect_.top, valid_rect_.right, valid_rect_.bottom);
    }
    int set_display_position(int x, int y, int width, int height) {
        set_valid_rectangle(x, y, width, height);
        int error = qnx_screen_display::set_display_position(x, y);
        if (error) {
            return error;
        }
        error = set_display_size(width, height);
        if (error) {
            return error;
        }
        error = set_screen_buffer_size(display_size_);
        if (error) {
            return error;
        }
        set_font_mask();
        return 0;
    }
    bool is_in_rect(int x, int y) {
        return (1 == visible_) && (true == valid_rect_.is_in(x, y));
    }
    int display_text(const wchar_t* str) {
        if (nullptr == str) {
            SLOG_E("display text receive null string!");
            return DATA_PTR_IS_NULL;
        }
        screen_buffer_t screen_buf = { 0 };
        int error = screen_get_window_property_pv(win_, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&screen_buf);
        if (error) {
            SLOG_E("screen_set_window_property_pv for SCREEN_PROPERTY_RENDER_BUFFERS failed, error:%s", strerror(errno));
            return error;
        }
        char *image_ptr = nullptr;
        error = screen_get_buffer_property_pv(screen_buf, SCREEN_PROPERTY_POINTER, (void **)&image_ptr);
        if (error) {
            SLOG_E("screen_set_window_property_pv for SCREEN_PROPERTY_POINTER failed, error:%s", strerror(errno));
            return error;
        }
        int stride = 0;
        error = screen_get_buffer_property_iv(screen_buf, SCREEN_PROPERTY_STRIDE, &stride);
        if (error) {
            SLOG_E("screen_get_buffer_property_iv for SCREEN_PROPERTY_STRIDE failed, error:%s", strerror(errno));
            return error;  
        }
        error = QNX_TEXT_DRAW.draw(font_mask_, str, display_size_[0], display_size_[1], font_size_);
        if (error) {
            SLOG_E("qnx text draw failed!");
            return error;
        }
        draw_text_image(image_ptr, stride);
        screen_post_window(win_, screen_buf, 0, nullptr, 0);
        return 0;
    }
protected:
    int init_win() {
        int error = qnx_screen_display::init_win();
        if (error) {
            return error;
        }
        set_display_zorder(QNX_SCREEN_DEFAULT_ZORDER_OF_TEXT);
        error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_ZORDER, &display_zorder_);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_ZORDER failed, error:%s", strerror(errno));
            return error;
        }
        static const int format = SCREEN_FORMAT_RGBA8888;
        error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_FORMAT, &format);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_FORMAT failed, error:%s", strerror(errno));
            return error;
        }
        static const int transparency = SCREEN_TRANSPARENCY_SOURCE_OVER;
        error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_TRANSPARENCY, &transparency);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_TRANSPARENCY failed, error:%s", strerror(errno));
            return error;
        }
        return 0;
    }
    inline void draw_pix(int x, int y, char *ptr, int stride, int color) {
        if (nullptr == ptr) {
            SLOG_E("draw pix get null ptr!");
            return;
        }
        ptr += (stride * y);
        ptr[x * 4] = ((0xff000000 & color) >> 24);
        ptr[x * 4 + 1] = ((0x00ff0000 & color) >> 16);
        ptr[x * 4 + 2] = ((0x0000ff00 & color) >> 8);
        ptr[x * 4 + 3] = (0x000000ff & color);
    }
    void set_font_mask() {
        free_font_mask();
        font_mask_ = (unsigned char*)malloc(display_size_[0] * display_size_[1]);
        assert(font_mask_ != nullptr);
    }
    void free_font_mask() {
        if (font_mask_ != nullptr) {
            free(font_mask_);
            font_mask_ = nullptr;
        }
    }
    void draw_text_image(char* image, int stride) {
        int i = 0, j = 0;
        for (;i < display_size_[1];i++) {
            for (j = 0;j < display_size_[0];j++) {
                if (font_mask_[display_size_[0] * i + j]) {
                    draw_pix(j, i, image, stride, 0x0000ffff);
                }
                else {
                    // bg alpha set to 0x01 to receive touch event
                    // bg alpha set to 0, sometimes can not receive touch event
                    draw_pix(j, i, image, stride, 0xffffff01);
                }
            }
        }
    }
};