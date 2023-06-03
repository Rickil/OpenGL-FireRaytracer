#ifndef TP1_MATRIX4_HH
#define TP1_MATRIX4_HH

#include "Vector3.hh"

#include <iostream>
#include <cmath>

class matrix4 {
public:
    float m[4][4] = {{0,0,0,0},
                   {0,0,0,0},
                   {0,0,0,0},
                   {0,0,0,0}};
    matrix4(){};
    void applyMatrix(float matrix[4][4]){
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                this->m[i][j] = matrix[i][j];
            }
        }
    }

    void transpose(){
        for (int i = 0; i < 4; i++) {
            for (int j = i+1; j < 4; j++) {
                // Swap matrix[i][j] and matrix[j][i]
                float temp = m[i][j];
                m[i][j] = m[j][i];
                m[j][i] = temp;
            }
        }
    }
    void operator*=(const matrix4& rhs){
        matrix4 tmp;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                tmp.m[i][j] = 0;
                for (int k = 0; k < 4; ++k) {
                    tmp.m[i][j] += m[i][k] * rhs.m[k][j];
                }
            }
        }
        *this = tmp;
    }

    Vector3 operator*(Vector3 v){
        float* vector = v.toFloatArray();
        float tmp[3];
        for (int i = 0; i < 3; ++i) {
            tmp[i] = 0;
            for (int j = 0; j < 3; ++j) {
                tmp[i] += m[i][j] * vector[j];
            }
        }
        for (int i = 0; i < 3; ++i) {
            vector[i] = tmp[i];
        }
        v = v.fromFloatArray(vector);
        delete[] vector;
        return v;
    }

    static matrix4 identity(){
        matrix4 tmp;
        for (int i = 0; i < 4; ++i) {
            tmp.m[i][i] = 1;
        }
        return tmp;
    }

    static matrix4 translation(float x, float y, float z){
        matrix4 tmp;
        for (int i = 0; i < 4; ++i) {
            tmp.m[i][i] = 1;
        }

        //set translation
        tmp.m[0][3] = x;
        tmp.m[1][3] = y;
        tmp.m[2][3] = z;
        return tmp;
    }

    static matrix4 rotationX(float angle){
        matrix4 tmp;
        for (int i = 0; i < 4; ++i) {
            tmp.m[i][i] = 1;
        }

        //set rotation
        tmp.m[1][1] = cos(angle);
        tmp.m[1][2] = -sin(angle);
        tmp.m[2][1] = sin(angle);
        tmp.m[2][2] = cos(angle);
        return tmp;
    }

    static matrix4 rotationY(float angle){
        matrix4 tmp;
        for (int i = 0; i < 4; ++i) {
            tmp.m[i][i] = 1;
        }

        //set rotation
        tmp.m[0][0] = cos(angle);
        tmp.m[0][2] = sin(angle);
        tmp.m[2][0] = -sin(angle);
        tmp.m[2][2] = cos(angle);
        return tmp;
    }

    static matrix4 rotationZ(float angle){
        matrix4 tmp;
        for (int i = 0; i < 4; ++i) {
            tmp.m[i][i] = 1;
        }

        //set rotation
        tmp.m[0][0] = cos(angle);
        tmp.m[0][1] = -sin(angle);
        tmp.m[1][0] = sin(angle);
        tmp.m[1][1] = cos(angle);
        return tmp;
    }
};

std::ostream& operator<<(std::ostream &out, const matrix4 &m){
    out << "[";
    for (int i = 0; i < 4; ++i) {
        out << "[";
        for (int j = 0; j < 4; ++j) {
            out << m.m[i][j];
            if (j < 3)
                out << ",";
        }
        out << "]";
        if (i < 3)
            out << '\n';
    }
    return out << "]";
}




#endif //TP1_MATRIX4_HH
