//
//  main.cpp
//  cours1
//
//  Created by Thiago Petrilli Maffei Dardis on 27/01/2023.
//  Copyright © 2023 Thiago Petrilli Maffei Dardis. All rights reserved.
//

#include <iostream>

double ** alloc(int M, int N=0){
    double **p = new double*[M];
    for(int i = 0; i<M; i++){
        p[i] = new double [N];
        memset(p[i], 0, N*sizeof(double));
        for (int j = 0; j<N; j++){
            p[i][j] = 2*drand48()-1;
        }
    }
    return p;
}

void free(double **p, int M){
    for (int i = 0; i<M; i++){
        delete []p[i];
    }
    delete []p;
}

void mul(double **A, double **B, double **C, int M, int N, int P){
    for (int i = 0; i<M; i++){
        for (int j = 0; j<P; j++){
            C[i][j] = 0;
            for (int k = 0; k<N; k++){
                C[i][j] += A[i][k]*B[k][j];
            }
        }
    }
}

int main(void) {
    int M,N, P;
    std::cin>>M>>N>>P;
    double **A = alloc(M,N);
    double **B = alloc(M,P);
    double **C = alloc(M,P);
    mul(A,B,C,M,N,P);
    return 0;
}
