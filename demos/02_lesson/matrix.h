#ifndef MATRIX_H
#define MATRIX_H

void init_zero_matrix(float matrix[3][3]);
void init_identity_matrix(float matrix[3][3]);
void print_matrix(const float matrix[3][3]);
void add_matrices(const float a[3][3], const float b[3][3], float c[3][3]);
void scalar_multiply(float matrix[3][3], float scalar);
void multiply_matrices(const float a[3][3], const float b[3][3], float result[3][3]);
void transform_point(const float m[3][3], const float p[3], float res[3]);
void scale(float m[3][3], float sx, float sy);
void shift(float m[3][3], float tx, float ty);
void rotate(float m[3][3], float angle);

#endif 