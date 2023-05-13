#include "qnx_text_draw.hpp"
#include "qnx_screen_exit_button.hpp"
int main(int argc, const char **argv) {
    if (false == SLOG_INIT()) {
        return -1;
    }
    int error = QNX_SCREEN_CTX.init();
    if (error) {
        SLOG_E("qnx screen context init failed:%d", error);
        return -2;
    }
    // test display text
    error = QNX_TEXT_DRAW.init();
    if (error) {
        printf("QNX_TEXT_DRAW init failed!\n");
        return -3;
    }
    qnx_screen_display_text text1, text2;
    error = text1.init();
    if (error) {
        SLOG_E("qnx screen display text init failed:%d", error);
        return -4;
    }
    error = text2.init();
    if (error) {
        SLOG_E("qnx screen display text init failed:%d", error);
        return -5;
    }
    error = text1.set_display_position(100, 100, 300, 300);
    if (error) {
        printf("set_display_position failed:%d\n", error);
        return -6;
    }
    error = text2.set_display_position(500, 500, 300, 300);
    if (error) {
        printf("set_display_position failed:%d\n", error);
        return -7;
    }
    const wchar_t* str = L"我1b";
    error = text1.display_text(str);
    if (error) {
        printf("display_text failed:%d\n", error);
        return -8; 
    }
    str = L"你2a";
    error = text2.display_text(str);
    if (error) {
        printf("display_text failed:%d\n", error);
        return -8; 
    }
    sleep(5);
    text1.clear();
    sleep(5);
    if (text1.is_cleared) {
        if (true == text1.init()) {
            text1.set_display_position(100, 100, 300, 300);
            text1.display_text(str);
        }
    }
    error = QNX_SCREEN_TOUCH.init();
    if (error) {
        printf("QNX_SCREEN_TOUCH.init failed:%d\n", error);
        return -10;
    }
    QNX_SCREEN_TOUCH.start_listen();
    // draw a exit button
    QNX_SCREEN_EXIT_BUTTON.set_button_id(100001);
    QNX_SCREEN_EXIT_BUTTON.set_button_position(100, 100);
    QNX_SCREEN_EXIT_BUTTON.set_button_text_size(50);
    QNX_SCREEN_EXIT_BUTTON.set_button_scale(200, 200);
    str = L"退出";
    QNX_SCREEN_EXIT_BUTTON.set_button_text(str);
    error = QNX_SCREEN_EXIT_BUTTON.init();
    if (error) {
        printf("exit button init failed:%d\n", error);
        return -11;
    }
    sleep(5);
    QNX_SCREEN_TOUCH.stop_listen();
    
    return 0;
}