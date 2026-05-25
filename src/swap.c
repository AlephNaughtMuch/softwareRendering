#include "swap.h"

void int_swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void float_swap(float* a, float* b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

void vec3_swap(vec3_t* a, vec3_t* b) {
    vec3_t temp = *a;
    *a = *b;
    *b = temp;
}
