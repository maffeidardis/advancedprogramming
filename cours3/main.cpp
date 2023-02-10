//
//  main.cpp
//  cours1
//
//  Created by Thiago Petrilli Maffei Dardis on 27/01/2023.
//  Copyright © 2023 Thiago Petrilli Maffei Dardis. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <cstdlib>
#include <algorithm>
#include <pthread.h>

double ** alloc(int M, int N=1){
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


void printMatrix(double **p, int M, int N){
    int m, n;
    
    for (m=0; m < M; m++){
        for (n=0; n<N; n++){
            std::cout << p[m][n] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

}

int main(void) {
    std::cout<< "Insert M,N,P" <<std::endl;
    int M,N, P;
    std::cin>>M>>N>>P;
    double **A = alloc(M,N);
    printMatrix(A,M,N);
    double **B = alloc(M,P);
    printMatrix(B,M,P);
    double **C = alloc(M,P);
    printMatrix(C,M,P);
    mul(A,B,C,M,N,P);
    return 0;
}
