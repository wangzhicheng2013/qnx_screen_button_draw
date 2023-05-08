#pragma once
#include "qnx_slog2.hpp"
#include "qnx_screen_context.hpp"
#include <vector>
#include <functional>
using on_touch_function_type = std::function<bool(int,      // touch status
                                                  int,      // touch x corrdinate
                                                  int)>;    // touch y corrdinate
enum qnx_screen_touch_status {
    TOUCH_STATTUS_NONE,
    TOUCH_STATTUS_PRESS,
    TOUCH_STATTUS_MOVE,
    TOUCH_STATTUS_RELEASE,
};
class qnx_screen_touch {
private:
    std::vector<on_touch_function_type>touch_functions_;
    pthread_t listen_thread_ = 0;
    bool stop_listen_ = false;
    screen_event_t screen_ev_ = { 0 };
public:
    static qnx_screen_touch& get_instance() {
        static qnx_screen_touch instance;
        return instance;
    }
    int init() {
        int error = screen_create_event(&screen_ev_);
        if (error) {
            SLOG_E("screen_create_event failed, error:%s", strerror(errno));
            return error;
        }
        static const int val = 101;
        error = screen_set_event_property_iv(screen_ev_, SCREEN_PROPERTY_TYPE, &val);
        if (error) {
            SLOG_E("screen_set_event_property_iv for SCREEN_PROPERTY_TYPE failed, error:%s", strerror(errno));
            return error;
        }
        return 0;
    }
    void start_listen() {
        int rc = pthread_create(&listen_thread_, nullptr, capture_touch, this);
        assert(0 == rc);
        SLOG_I("qnx screen on touch listen thread start ok!");
    }
    void stop_listen() {
        stop_listen_ = true;
    }
    void add_on_touch_function(on_touch_function_type fun) {
        touch_functions_.emplace_back(fun);
    }
private:
    qnx_screen_touch() = default;
    virtual ~qnx_screen_touch() {
        pthread_join(listen_thread_, nullptr);
        stop_listen_ = true;
        screen_destroy_event(screen_ev_);
    }
    static void* capture_touch(void *arg) {
        qnx_screen_touch *touch = static_cast<qnx_screen_touch *>(arg);
        if (nullptr == touch) {
            SLOG_E("qnx_screen_touch is null!");
            return nullptr;
        }
        int status = 0;
        int error = 0;
        qnx_screen_touch_status touch_status = TOUCH_STATTUS_NONE;
        while(false == touch->stop_listen_) {
            error = screen_get_event(QNX_SCREEN_CTX.screen_ctx, touch->screen_ev_, -1);
            if (error) {
                SLOG_E("screen_get_event failed, error:%s", strerror(errno));
                return nullptr;
            }
            error = screen_get_event_property_iv(touch->screen_ev_, SCREEN_PROPERTY_TYPE, &status);
            if (error) {
                SLOG_E("screen_get_event_property_iv for SCREEN_PROPERTY_TYPE failed, error:%s", strerror(errno));
                return nullptr;
            }
            touch_status = get_touch_status(status);
            if (TOUCH_STATTUS_NONE == touch_status) {
                continue;
            }
            int cur_pos[2] = { 0 };
            error = screen_get_event_property_iv(touch->screen_ev_, SCREEN_PROPERTY_POSITION, (int *)&cur_pos);
            if (error) {
                SLOG_E("screen_get_event_property_iv for SCREEN_PROPERTY_POSITION failed, error:%s", strerror(errno));
                continue;
            }
            SLOG_I("qnx screen capture touch event:%d, position(%d,%d)", touch_status, cur_pos[0], cur_pos[1]);
            for (auto &func : touch->touch_functions_) {
                func(touch_status, cur_pos[0], cur_pos[1]);
            }
        }
        SLOG_W("capture_touch thread break!");
        return nullptr;
    }
    inline static qnx_screen_touch_status get_touch_status(int status) {
        switch (status)
        {
        case SCREEN_EVENT_MTOUCH_TOUCH:
            return TOUCH_STATTUS_PRESS;
            break;
        case SCREEN_EVENT_MTOUCH_RELEASE:
            return TOUCH_STATTUS_RELEASE;
            break;
        case SCREEN_EVENT_MTOUCH_MOVE:
            return TOUCH_STATTUS_MOVE;
            break;
        default:
           break;
        }
        return TOUCH_STATTUS_NONE;
    }
};
#define QNX_SCREEN_TOUCH qnx_screen_touch::get_instance()