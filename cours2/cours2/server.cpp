//
//  main.cpp
//  cours2
//
//  Created by Thiago Petrilli Maffei Dardis on 03/02/2023.
//  Copyright © 2023 Thiago Petrilli Maffei Dardis. All rights reserved.
//

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include "base.h"

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


void * hconnect (void * fd)

{
    int f = *((int *)fd);
    
    //Reading the matrix dimensions from the client
    int N,P,L;
    read(f, &N, sizeof(int));
    read(f, &P, sizeof(int));
    read(f, &L, sizeof(int));

    //Allocating the matrix A
    double **A = alloc(L,N);
    for (int j =0; j<L; j++){
        for (int k=0; k<N; k++){
            read(f, &A[j][k], sizeof(double));
        }
    }

    //Allocating the matrix B
    double **B = alloc(N,P);
    for (int j =0; j<N; j++){
        for (int k=0; k<P; k++){
            read(f, &B[j][k], sizeof(double));
        }
    }

    //Multiplicate both A and B
    double **C = alloc(L,P);
    mul(A,B,C,L,N,P);

    //Sending matrix C
    for (int j =0; j<L; j++){
        for (int k=0; k<N; k++){
            write(f, &C[j][k], sizeof(double));
        }
    }

    printMatrix(C,L,P);
    
    close(f);
    
    free(fd);
    pthread_detach(pthread_self());
    return NULL;
}

int main (int argc, char ** argv)

{
    const char * server = "0.0.0.0";
    struct sockaddr_in sin;
    int s, f, ret;
    pthread_t tid;
    int optval = 1;
    
    sin.sin_family = AF_INET;
    sin.sin_port = htons(DEFAULT_PORT);
    std::cout << sin.sin_port << std::endl;
    ret = inet_aton(server, &sin.sin_addr);
    if (ret == 0) {
        fprintf(stderr, "address %s invalid\n", server);
        return 0;
    }
    
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0) {
        fprintf(stderr, "cannot create socket\n");
        return 0;
    }
    
    ret = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (ret != 0) {
        fprintf(stderr, "setsockopt() failed\n");
        return 0;
    }
    
    ret = bind(s, (struct sockaddr *)&sin, sizeof(sin));
    if (ret != 0) {
        fprintf(stderr, "bind() failed\n");
        return 0;
    }
    
    ret = listen(s, 10);
    if (ret != 0) {
        fprintf(stderr, "listen() failed\n");
        return 0;
    }
    
    while (1) {
        f = accept(s, NULL, 0);
        if (f == -1) {
            fprintf(stderr, "accept() failed\n");
            return 0;
        }
        
        int * fd = new int;
        *fd = f;
        pthread_create(&tid, NULL, hconnect, (void *)fd);
    }
    
    return 0;
}
