#pragma once
#include "qnx_screen_display_text.hpp"
#include "qnx_screen_touch.hpp"
class qnx_screen_display_button : public qnx_screen_display_text {
protected:
    int button_id_;
    const wchar_t* button_text_ = nullptr;
    int button_text_size_ = 36;
    int button_position_x_ = 0;
    int button_position_y_ = 0;
    int button_width_ = 1920;
    int button_height_ = 1080;
public:
    qnx_screen_display_button() {
        QNX_SCREEN_TOUCH.add_observer( { this, on_touch_event } );
    }
    virtual ~qnx_screen_display_button() = default;
    int init() {
        int error = qnx_screen_display_text::init();
        if (error) {
            return error;
        }
        set_font_size(button_text_size_);
        error = set_display_position(button_position_x_, button_position_y_, button_width_, button_height_);
        if (error) {
            return error;
        }
        error = display_text(button_text_);
        if (error) {
            return error;
        }
        return 0;     
    }
    inline void set_button_id(int id) {
        button_id_ = id;
    }
    inline void set_button_text(const wchar_t* text) {
        button_text_ = text;
    }
    inline void set_button_text_size(int size) {
        button_text_size_ = size;
    }
    inline void set_button_position(int x, int y) {
        button_position_x_ = x;
        button_position_y_ = y;
    }
    inline void set_button_scale(int width, int height) {
        button_width_ = width;
        button_height_ = height;
    }
protected:
    virtual bool do_action() = 0; 
private:
    static bool on_touch_event(void *observer, qnx_screen_touch_status status, int x, int y) {
        auto *self_ptr = static_cast<qnx_screen_display_button *>(observer);
        if (nullptr == self_ptr) {
            SLOG_E("qnx_screen_display_button observer is null!");
            return false;
        }
        if ((TOUCH_STATTUS_PRESS == status) && self_ptr->is_in_rect(x, y)) {
            return self_ptr->do_action();
        }
        return false;
    }
};