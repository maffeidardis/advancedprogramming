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

int main (int argc, char * argv[])

{
    struct sockaddr_in saddr;
    struct hostent * server;
    int s, ret;
    
    if (argc == 1) {
        std::cerr << "usage: " << argv[0]
        << " [ adresse IP/nom du serveur ]" << std::endl;
        return 0;
    }
    
    s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0) {
        std::cerr << "cannot create socket" << std::endl;
        return 0;
    }
    
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(DEFAULT_PORT);
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        std::cerr << "gethostbyname() failed" << std::endl;
        close(s);
        return 0;
    }
    else {
        memcpy(&saddr.sin_addr, server->h_addr, 4);
    }
    
    ret = connect(s, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) {
        std::cerr << "cannot connect to remote server" << std::endl;
        close(s);
        return 0;
    }
    
    //Sending the Matrix dimension to the server
    int M,N,P; NUM_SERVERS;
    std::cout<<"M, N, P"<<std::endl;
    std::cin>>M>>N>>P;

    write(s, &N, sizeof(int));
    write(s, &P, sizeof(int));
    
    double **A = alloc(M,N);
    double **B = alloc(N,P);
    double **C = alloc(M,P);

    int r = M % NUM_SERVERS;
    int l1 = 0, L;
    int l2 = M / NUM_SERVERS + r;

    //Sending the Matrix A
    for (int i = 0; i< NUM_SERVERS; i++) {
        L = l2-l1;
        write(s, &(L), sizeof(int));
        
        for (int j = l1; j<l2; j++){
            for (int k = 0; k<N; k++){
                write(s, &A[j][k], sizeof(double));
            }
        }

        l1 = l2;
        l2 += M / NUM_SERVERS;
    }

    //Sending the Matrix B
    for (int i = 0; i<N; i++){
        for (int k = 0; k<P; k++){
            write(s, &B[i][k], sizeof(double));
        }
    }

    //Receiving matrix C
    for (int i = 0; i< NUM_SERVERS; i++) {
        L = l2-l1;        
        for (int j = 0; j<l2; j++){
            for (int k = 0; k<N; k++){
                read(s, &C[j][k], sizeof(double));
            }
        }

        l1 = l2;
        l2 += M / NUM_SERVERS;
    }

    printMatrix(C,L,P);


    close(s);
    return 0;
}
