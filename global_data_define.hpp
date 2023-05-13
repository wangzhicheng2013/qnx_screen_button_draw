#pragma once
enum global_error_code {
    QNX_SCREEN_DISPLAY_COUNT_LACK = -1000,
    QNX_SCREEN_DISPLAY_NOT_FOUND = -1001,
    DISPLAY_MARGIN_COORDINATE_INVALID = -1002,
    DISPLAY_MARGIN_COORDINATE_EXCEED = -1003,
    IMAGE_FORMAT_INVALID = -1004,
    DATA_PTR_IS_NULL = -1005,
};

enum qnx_screen_default_zorder {
    QNX_SCREEN_DEFAULT_ZORDER_OF_ZONE = 999,
    QNX_SCREEN_DEFAULT_ZORDER_OF_IMAGE,
    QNX_SCREEN_DEFAULT_ZORDER_OF_TEXT,
};
// display camera frame
const static int MIN_DISPLAY_WIDTH = 500;
const static int MIN_DISPLAY_HEIGHT = 500;
const static int DEFAULT_FONT_SIZE = 36;
enum qnx_screen_visibility {
    SCREEN_GONE,
    SCREEN_VISIBLE,
};