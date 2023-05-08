#pragma once
#include "qnx_screen_display.hpp"
class qnx_screen_display_image : public qnx_screen_display {
private:
    int image_format_ = SCREEN_FORMAT_UYVY;
    int image_size_ = 0;
    int image_scale_[2] = { 0 };
public:
    static qnx_screen_display_image& get_instance() {
        static qnx_screen_display_image instance;
        return instance;
    }
    int init() {
        return init_win();
    }
    inline void set_image_format(int format) {
        image_format_ = format;
    }
    int set_image_size(int width, int height) {
        image_scale_[0] = width;
        image_scale_[1] = height;
        if (SCREEN_FORMAT_UYVY == image_format_) {
            image_size_ = 2 * width * height;
        }
        else {
            image_size_ = 0;
            SLOG_E("only support UYVY format!");
            return IMAGE_FORMAT_INVALID;
        }
        SLOG_I("set image scale: %dx%d, image size:%d", image_scale_[0], image_scale_[1], image_size_);
        return set_screen_buffer_size(image_scale_);
    }
    int display_image(const char *image_ptr) {
        if (nullptr == image_ptr) {
            SLOG_E("display null image!");
            return DATA_PTR_IS_NULL;
        }
        screen_buffer_t render_buf[2] = { 0 };
        int error = screen_get_window_property_pv(win_, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)render_buf);
        if (error) {
            SLOG_E("screen_get_window_property_pv for SCREEN_PROPERTY_RENDER_BUFFERS failed, error:%s", strerror(errno));
            return error;
        }
        int size[2] = {0, 0};	// image size
        error = screen_get_buffer_property_iv(render_buf[0], SCREEN_PROPERTY_BUFFER_SIZE, size);    // is the same with display size
        if (error) {
            SLOG_E("screen_get_buffer_property_iv for SCREEN_PROPERTY_BUFFER_SIZE failed, error:%s", strerror(errno));
            return error;
        }
        char* buf_ptr = nullptr;
        // Associative buffer and data pointer
        error = screen_get_buffer_property_pv(render_buf[0], SCREEN_PROPERTY_POINTER, (void **)&buf_ptr);
        if (error) {
            SLOG_E("screen_get_buffer_property_pv for SCREEN_PROPERTY_POINTER failed, error:%s", strerror(errno));
            return error;
        }
        memcpy(buf_ptr, image_ptr, image_size_);
        screen_post_window(win_, render_buf[0], 0, nullptr, 0);
        return 0;
    }
private:
    qnx_screen_display_image() = default;
    virtual ~qnx_screen_display_image() {
        unit_win();
    }
    int init_win() {
        int error = qnx_screen_display::init_win();
        if (error) {
            return error;
        }
        set_display_zorder(QNX_SCREEN_DEFAULT_ZORDER_OF_IMAGE);
        error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_ZORDER, &display_zorder_);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_ZORDER failed, error:%s", strerror(errno));
            return error;
        }
        error = screen_set_window_property_iv(win_, SCREEN_PROPERTY_FORMAT, &image_format_);
        if (error) {
            SLOG_E("screen_set_window_property_iv for SCREEN_PROPERTY_FORMAT failed, error:%s", strerror(errno));
            return error;
        }
        return 0;
    }
};
#define QNX_SCREEN_DISPLAY_IMAGE qnx_screen_display_image::get_instance()