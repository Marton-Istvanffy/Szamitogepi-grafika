#include "matrix.h"
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void init_zero_matrix(float matrix[3][3]) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            matrix[i][j] = 0.0f;
        }
    }
}

void init_identity_matrix(float matrix[3][3]) {
    init_zero_matrix(matrix);
    for (int i = 0; i < 3; ++i) matrix[i][i] = 1.0f;
}

void print_matrix(const float matrix[3][3]) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%8.4f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void add_matrices(const float a[3][3], const float b[3][3], float c[3][3]) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}

void scalar_multiply(float matrix[3][3], float scalar) {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            matrix[i][j] *= scalar;
}

void multiply_matrices(const float a[3][3], const float b[3][3], float result[3][3]) {
    float temp[3][3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                temp[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            result[i][j] = temp[i][j];
}

void transform_point(const float m[3][3], const float p[3], float res[3]) {
    float temp[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp[i] += m[i][j] * p[j];
        }
    }
    for (int i = 0; i < 3; i++) res[i] = temp[i];
}

void scale(float m[3][3], float sx, float sy) {
    float s[3][3];
    init_identity_matrix(s);
    s[0][0] = sx;
    s[1][1] = sy;
    multiply_matrices(m, s, m);
}

void shift(float m[3][3], float tx, float ty) {
    float t[3][3];
    init_identity_matrix(t);
    t[0][2] = tx; // x irányú eltolás
    t[1][2] = ty; // y irányú eltolás
    multiply_matrices(m, t, m);
}

void rotate(float m[3][3], float angle) {
    float r[3][3];
    init_identity_matrix(r);
    float rad = angle * (M_PI / 180.0f);
    r[0][0] = cos(rad);  r[0][1] = -sin(rad);
    r[1][0] = sin(rad);  r[1][1] = cos(rad);
    multiply_matrices(m, r, m);
}