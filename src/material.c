#include "material.h"

material_t material_new(float ka, float kd, float ks, float shininess) {
    material_t mat;
    mat.ka = ka;
    mat.kd = kd;
    mat.ks = ks;
    mat.shininess = shininess;
    return mat;
}
