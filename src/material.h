#ifndef MATERIAL_H
#define MATERIAL_H


typedef struct {
    float ka, kd, ks, shininess;
} material_t;

material_t material_new(float ka, float kd, float ks, float shininess);
#endif
