//
//  client.cpp
//  cours2
//
//  Created by Thiago Petrilli Maffei Dardis on 03/02/2023.
//  Copyright © 2023 Thiago Petrilli Maffei Dardis. All rights reserved.
//

#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include "base.h"
#include <pthread.h>
#include <unistd.h>

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

struct mult_args {
    int l1;
    int l2;
    int M;
    int N;
    int P;
    double **A;
    double **B;
    double **C;
    char *ip;
};

void * request (void* args){
    mult_args * p = (mult_args *)args;
    struct sockaddr_in saddr;
    struct hostent * server;
    int s, ret;
    
    s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0) {
        std::cerr << "cannot create socket" << std::endl;
        return NULL;
    }
    
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(DEFAULT_PORT);
    server = gethostbyname(p->ip);
    if (server == NULL) {
        std::cerr << "gethostbyname() failed" << std::endl;
        close(s);
        return NULL;
    }
    else {
        memcpy(&saddr.sin_addr, server->h_addr, 4);
    }
    
    ret = connect(s, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) {
        std::cerr << "cannot connect to remote server" << std::endl;
        close(s);
        return NULL;
    }
    
    
    write(s, &p->N, sizeof(int));
    write(s, &p->P, sizeof(int));

    //Sending the Matrix A
    write(s, &(p->l1), sizeof(int));
    write(s, &(p->l2), sizeof(int));

    for (int j = p->l1; j < p->l2; j++) {
        write(s, &(p->A[j][0]), p->N * sizeof(double));
    }


    //int L = p->l2 - p->l1;

    //Sending the Matrix B
    for (int i = 0; i<p->N; i++){
        for (int k = 0; k<p->P; k++){
            write(s, &(p->B[i][k]), sizeof(double));
        }
    }

    //Receiving matrix C   
    for (int j = p->l1; j < p->l2; j++) {
        for (int k = 0; k < p->N; k++) {
            read(s, &(p->C[j][k]), sizeof(double));
        }
    }
    close(s);
    return NULL;

}

int main (int argc, char * argv[])
{
    //Sending the Matrix dimension to the server

    if (argc != NUM_SERVERS + 1) {
        std::cerr << "usage: " << argv[0]
        << " [ adresse IP/nom du serveur ] ..." << std::endl;
        return 0;
    }
    
    int M,N,P;
    struct mult_args args[NUM_SERVERS];
    std::cout<<"M, N, P"<<std::endl;
    std::cin>>M>>N>>P;
    
    double **A = alloc(M,N);
    double **B = alloc(N,P);
    double **C = alloc(M,P);
    
    int r = M % NUM_SERVERS;
    int l1 = 0, L;
    int l2 = M / NUM_SERVERS + r;
    pthread_t threads[NUM_SERVERS];

    for (int i = 0; i< NUM_SERVERS; i++) {
        L = l2-l1;

        args[i].l1 = l1;
        args[i].l2 = l2;
        args[i].M = M;
        args[i].N = N;
        args[i].P = P;
        args[i].A = A;
        args[i].B = B;
        args[i].C = C;
        args[i].ip = argv[i + 1];

        int rc = pthread_create(&threads[i], NULL, request, (void*) &args[i]);
        if (rc != 0) {
            std::cerr << "err creating pthread" << std::endl;
        }

        //sleep(2); 
        // sleep for 2 seconds before creating next thread
        l1 = l2;
        l2 += M / NUM_SERVERS;

    }

    for (int i = 0; i< NUM_SERVERS; i++) {
        pthread_join(threads[i], NULL);
    }

    printMatrix(C,M,P);

    return 0;
}