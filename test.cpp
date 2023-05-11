#include "qnx_text_draw.hpp"
#include "qnx_screen_display_zone.hpp"
#include "qnx_screen_display_image.hpp"
#include "qnx_screen_display_text.hpp"
#include "qnx_screen_exit_button.hpp"
#include <fcntl.h>
int main(int argc, const char **argv) {
    if (false == SLOG_INIT()) {
        return -1;
    }
    int error = QNX_SCREEN_CTX.init();
    if (error) {
        SLOG_E("qnx screen context init failed:%d", error);
        return -2;
    }
    error = QNX_SCREEN_DISPLAY_ZONE.init();
    if (error) {
        SLOG_E("qnx screen display zone init failed:%d", error);
        return -3;
    }
    printf("now blacking the whole screen!\n");
    // test black the screen
    QNX_SCREEN_DISPLAY_ZONE.set_display_margin(0, 0, 0, 0);
    sleep(10);
    error = QNX_SCREEN_DISPLAY_IMAGE.init();
    if (error) {
        SLOG_E("qnx screen display image init failed:%d", error);
        return -4;
    }
    // test display image
    printf("now show the picture!\n");
    char pic_path[128] = { 0 };
    printf("input picture path:");
    scanf("%s", pic_path);
    FILE *fp = fopen(pic_path, "rb");
    if (!fp) {
        printf("can not open %s!\n", pic_path);
        return -5;
    }
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *uyvy_content = (char *)malloc(sizeof(char) * file_size);
    assert(uyvy_content != nullptr);
    fread(uyvy_content, 1, file_size, fp);
    fclose(fp);
    QNX_SCREEN_DISPLAY_IMAGE.set_display_position(0, 0);
    QNX_SCREEN_DISPLAY_IMAGE.set_image_size(1920, 1080);
    QNX_SCREEN_DISPLAY_IMAGE.set_display_size(1920, 1080);
    QNX_SCREEN_DISPLAY_IMAGE.display_image(uyvy_content);
    sleep(10);
    free(uyvy_content);
    // test display text
    error = QNX_TEXT_DRAW.init();
    if (error) {
        printf("QNX_TEXT_DRAW init failed!\n");
        return -6;
    }
    error = QNX_SCREEN_DISPLAY_TEXT.init();
    if (error) {
        SLOG_E("qnx screen display text init failed:%d", error);
        return -7;
    }
    QNX_SCREEN_DISPLAY_TEXT.set_font_size(36);
    error = QNX_SCREEN_DISPLAY_TEXT.set_display_position(100, 100, 1920, 1080);
    if (error) {
        printf("set_display_position failed:%d\n", error);
        return -8;
    }
    printf("now show the word!\n");
    const wchar_t* str = L"Hello World 我们是中国人! 123456";
    error = QNX_SCREEN_DISPLAY_TEXT.display_text(str);
    if (error) {
        printf("display_text failed:%d\n", error);
        return -9; 
    }
    sleep(10);
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
    const wchar_t* exit_str = L"退出";
    QNX_SCREEN_EXIT_BUTTON.set_button_text(exit_str);
    error = QNX_SCREEN_EXIT_BUTTON.init();
    if (error) {
        printf("exit button init failed:%d\n", error);
    }
    sleep(10);
    QNX_SCREEN_TOUCH.stop_listen();
    
    return 0;
}