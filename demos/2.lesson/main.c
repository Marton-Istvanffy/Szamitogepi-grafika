#include "matrix.h"
#include <stdio.h>

int main() {
    float m1[3][3], m2[3][3], res[3][3];
    float pont[3] = {2.0f, 3.0f, 1.0f}; 
    float eredmeny_pont[3];

    printf("Egysegmatrizs szorzasa 10-zel:\n");
    init_identity_matrix(m1);
    scalar_multiply(m1, 10.0f);
    print_matrix(m1);

    init_identity_matrix(m1);
    m1[0][2] = 5.0f; // Eltolás x=5
    init_identity_matrix(m2);
    m2[1][1] = 2.0f; // Skálázás y=2
    multiply_matrices(m1, m2, res);
    printf("Matrix szorzas eredmenye:\n");
    print_matrix(res);

    float transz[3][3];
    init_identity_matrix(transz);
    
    printf("Eredeti pont: (%.1f, %.1f)\n", pont[0], pont[1]);
    
    shift(transz, 10.0f, 5.0f);
    scale(transz, 2.0f, 2.0f);    
    rotate(transz, 90.0f);        
    
    transform_point(transz, pont, eredmeny_pont);
    
    printf("Transzformalt pont koordinatai: (%.2f, %.2f)\n", eredmeny_pont[0], eredmeny_pont[1]);
    printf("Az alkalmazott transzformacios matrix:\n");
    print_matrix(transz);

    return 0;
}