#pragma once
#include "qnx_screen_display_button.hpp"
class qnx_screen_exit_button : public qnx_screen_display_button {
public:
    static qnx_screen_exit_button& get_instance() {
        static qnx_screen_exit_button instance;
        return instance;
    }
private:
    virtual bool do_action() override {
        printf("Now to Exit!\n");
    } 
private:
    qnx_screen_exit_button() = default;
    virtual ~qnx_screen_exit_button() = default;
};
#define QNX_SCREEN_EXIT_BUTTON qnx_screen_exit_button::get_instance()