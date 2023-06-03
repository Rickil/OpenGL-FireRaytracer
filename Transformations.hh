#ifndef TP1_TRANSFORMATIONS_HH
#define TP1_TRANSFORMATIONS_HH
#define PI 3.14159265359

#include "matrix4.hh"
#include <cmath>

void vectorialProduct(float *a, float *b, float *c){
    c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
}

Vector3 vectorialProduct(Vector3 a, Vector3 b){
    float x = a.y*b.z - a.z*b.y;
    float y = a.z*b.x - a.x*b.z;
    float z = a.x*b.y - a.y*b.x;

    return {x,y,z};
}

float divide(float x, float y){
    return (y == 0 && x == 0) ? 0 : x/y;
}

float rad(float deg){
    return deg*PI/180.0;
}

void normalize(float v[3]){
    float norm = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    v[0] = divide(v[0], norm);
    v[1] = divide(v[1], norm);
    v[2] = divide(v[2], norm);
}

void frustum(matrix4 &mat,
                const float &left, const float &right,
                const float &bottom, const float &top,
                const float &z_near, const float &z_far){
    float A = (right + left) / (right - left);
    float B = (top + bottom) / (top - bottom);
    float C = - (z_far+z_near) / (z_far-z_near);
    float D = - (2*z_far*z_near) / (z_far-z_near);
    float topLeft = (2*z_near)/(right-left);
    float midLeft = (2*z_near)/(top-bottom);
    float m[4][4] = {{topLeft, 0,A,0},
                     {0,midLeft,B,0},
                     {0,0,C,D},
                     {0,0,-1,0}};
    mat.applyMatrix(m);
    mat.transpose();
}

void look_at(matrix4 &mat,
                const float &eyeX, const float &eyeY, const float &eyeZ,
                const float &centerX, const float &centerY, const float &centerZ,
                float upX, float upY, float upZ){
    float f[3] = {centerX-eyeX, centerY-eyeY, centerZ-eyeZ};
    normalize(f);
    float up[3] = {upX, upY, upZ};
    normalize(up);
    float s[3];
    vectorialProduct(f, up, s);
    float sNormalized[3] = {s[0],s[1],s[2]};
    normalize(sNormalized);
    float u[3];
    vectorialProduct(s, f, u);

    float m[4][4] = {{s[0], s[1], s[2],0},
                     {u[0],u[1],u[2],0},
                     {-f[0],-f[1],-f[2],0},
                     {0,0,0,1}};

    //get the translation matrix
    float t[4][4] = {{1,0,0,-eyeX},
                     {0,1,0,-eyeY},
                     {0,0,1,-eyeZ},
                     {0,0,0,1}};
    matrix4 translation;
    translation.applyMatrix(t);

    mat.applyMatrix(m);
    mat *= translation;
    mat.transpose();
}
#endif //TP1_TRANSFORMATIONS_HH