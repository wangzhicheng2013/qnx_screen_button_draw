#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ft2build.h>
#include <wchar.h>
#include <math.h>
#include <assert.h>
#include FT_FREETYPE_H
class qnx_text_draw {
private:
    FT_Library library_;
    FT_Face face_;
    const char *font_path_ = "./FZLTH.ttf";
    unsigned char* mask_ = nullptr;
    const wchar_t* text_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    int font_size_ = 0;
public:
    static qnx_text_draw& get_instance() {
        static qnx_text_draw instance;
        return instance;
    }
    inline void set_font_path(const char *path) {
        font_path_ = path;
    }
    int init() {
        int error = FT_Init_FreeType(&library_);
        if (error) {
            return error;
        }
        error = FT_New_Face(library_, font_path_, 0, &face_);
        if (error) {
            return error;
        }
        return 0;
    }
    int draw(unsigned char* font_mask, 
             const wchar_t* str, 
             int width, 
             int height, 
             int font_size) {
        mask_ = font_mask;
        text_ = str;
        width_ = width;
        height_ = height;
        font_size_ = font_size;
        return draw_string_mask();
    }
private:
    qnx_text_draw() = default;
    virtual ~qnx_text_draw() {
        uninit();
    }
    void uninit() {
        FT_Done_Face(face_);
        FT_Done_FreeType(library_);
    }
    void draw_char_mask(FT_Bitmap *bitmap, FT_Int x, FT_Int y) {
        FT_Int i = 0, j = 0, p = 0, q = 0;
        FT_Int x_max = x + bitmap->width;
        FT_Int y_max = y + bitmap->rows;
        for (i = x, p = 0;i < x_max;i++, p++) {
            for (j = y, q = 0;j < y_max;j++, q++) {
                if (i < 0 || j < 0 || i >= width_ || j >= height_) {
                    continue;
                }
                mask_[width_ * j + i] |= bitmap->buffer[q * bitmap->width + p];
            }
        }
    }
    int draw_string_mask() {
        FT_GlyphSlot  slot;
        FT_Matrix     matrix;
        FT_Vector     pen;
        FT_Error      error;

        double        angle = 0;
        int           target_height = 0;
        int           i = 0, num_chars = 0;
        num_chars     = wcslen(text_);
        angle         = (0.0 / 360) * 3.14159 * 2;
        target_height = height_;
        FT_Set_Pixel_Sizes(face_, font_size_, font_size_);
        matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
        matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
        matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
        matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

        memset(mask_, 0, width_ * height_);
        pen.x = 0 * 64;
        pen.y = (target_height - (font_size_ * 7) / 10) * 32;
        for (i = 0; i < num_chars; i++) {
            FT_Set_Transform(face_, &matrix, &pen);
            error = FT_Load_Char(face_, text_[i], FT_LOAD_RENDER);
            if (error) {
                continue;
            }
            slot = face_->glyph;
            FT_Int left = slot->bitmap_left;
            FT_Int top = target_height - slot->bitmap_top;
            if (top < 0) {
                top = 0;
            }
            draw_char_mask(&slot->bitmap, left, top);
            pen.x += slot->advance.x;
            pen.y += slot->advance.y;
        }
        return error;
    }
};
#define QNX_TEXT_DRAW qnx_text_draw::get_instance()