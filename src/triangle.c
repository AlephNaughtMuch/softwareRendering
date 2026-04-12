#include "triangle.h"
#include "display.h"
#include "swap.h"

void fill_flat_bottom_triangle (
    int x0, int y0, 
    int x1, int y1, 
    int mx, int my, 
    uint32_t color
) 
{
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

void fill_flat_top_triangle (
    int x1, int y1, 
    int mx, int my, 
    int x2, int y2, 
    uint32_t color
) 
{
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

void draw_filled_triangle (
    int x0, int y0, 
    int x1, int y1, 
    int x2, int y2, 
    uint32_t color
) 
{
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

    if (y1 == y2) {
        // Draw only the flat bottom
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    } else if (y0 == y1) {
        // Draw only the flat top
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    } else {
        // Calculate the new vertex (mx, my)
        int my = y1;
        int mx = ( (float) ((x2 - x0) * (y1 - y0)) / (y2 - y0) ) + x0;
    
        // Draw flat-bottom triangle
        fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, color);
        // Draw flat-top triangle
        fill_flat_top_triangle(x1, y1, mx, my, x2, y2, color);
    }


}

void draw_textured_triangle (
    int x0, int y0, float u0, float v0,
    int x1, int y1, float u1, float v1,
    int x2, int y2, float u2, float v2,
    uint32_t* texture 
) 
{
    //////////////////////////////////////////////////////////
    // Sort the vertices (y0 < y1 <y2) ///////////////////////
    //////////////////////////////////////////////////////////

    // Check first and second
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);

        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    // Check second and third
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);

        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    // Check possible new first and new second
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);

        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    //////////////////////////////////////////////////////////
    // Render the upper part of the triangle (flat-bottom) ///
    //////////////////////////////////////////////////////////

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) { inv_slope_1 = (float) (x1 - x0) / abs(y1 - y0); }
    if (y2 - y0 != 0) { inv_slope_2 = (float) (x2 - x0) / abs(y2 - y0); }
    
    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
    
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;
    
            if (x_end < x_start) { int_swap(&x_start, &x_end); }
    
            for (int x = x_start; x < x_end; x++) {
                draw_pixel(x, y, (x % 2 == 0 && y % 2 == 0 ? 0xFFFF00FF : 0xFF000000));
            }
    
        }
    }

    //////////////////////////////////////////////////////////
    // Render the bottom part of the triangle (flat-top) /////
    //////////////////////////////////////////////////////////
    inv_slope_1 = 0;
    // inv_slope_2 = 0;

    if (y2 - y1 != 0) { inv_slope_1 = (float) (x2 - x1) / abs(y2 - y1); }
    // if (y2 - y0 != 0) { inv_slope_2 = (float) (x2 - x0) / abs(y2 - y0); }
    
    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
    
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;
    
            if (x_end < x_start) { int_swap(&x_start, &x_end); }
    
            for (int x = x_start; x < x_end; x++) {
                draw_pixel(x, y, (x % 2 == 0 && y % 2 == 0 ? 0xFFFF00FF : 0xFF000000));
            }
    
        }
    }

}