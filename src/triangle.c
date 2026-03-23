#include "triangle.h"
#include "display.h"

void int_swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int mx, int my, uint32_t color) {
    // Find the slopes for the two triangle legs
    float inv_slope_1 = ((float) (x1 - x0)) / (y1 - y0);
    float inv_slope_2 = ((float) (mx - x0)) / (my - y0);

    // Start x_start and x_end from the top vertex
    float x_start = x0;
    float x_end = x0;

    // Loop the scanlines 
    for (int y = y0; y <= my; y++) {
        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;

    }

}

void fill_flat_top_triangle(int x1, int y1, int mx, int my, int x2, int y2, uint32_t color) {
    // Find the slopes for the two triangle legs
    float inv_slope_1 = ((float) (x2 - x1)) / (y2 - y1);
    float inv_slope_2 = ((float) (x2 - mx)) / (y2 - my);

    // Start x_start and x_end from the bottom vertex
    float x_start = x2;
    float x_end = x2;

    // Loop the scanlines from bottom to top now
    for (int y = y2; y >= y1; y--) {
        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // Sort the vertices (y0 < y1 <y2)
    // Check first and second
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }
    // Check second and third
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }
    // Check possible new first and new second
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    // Calculate the new vertex (mx, my)
    int my = y1;
    int mx = ( (float) ((x2 - x0) * (y1 - y0)) / (y2 - y0) ) + x0;

    // Draw flat-bottom triangle
    fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, color);
    // Draw flat-top triangle
    fill_flat_top_triangle(x1, y1, mx, my, x2, y2, color);

}