#include "utils.h"
#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

float smoothstep(float edge0, float edge1, float x) {
    float t = MAX(0.0f, MIN(1.0f, (x - edge0) / (edge1 - edge0)));
    return t * t * (3.0f - 2.0f * t);
}
