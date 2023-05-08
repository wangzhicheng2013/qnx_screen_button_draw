#pragma once
struct rectangle {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    inline void set(int l, int t, int r, int b) {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }
    inline bool is_in(int x, int y) {
        return (x > left) && (x < right) && (y > top) && (y < bottom);
    }
};
