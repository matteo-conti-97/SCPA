#include<mpi.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#define N 10

int main(int argc, char *argv[]){
    int rank, size,tag=0, n=1000000;
    int a[N], b[N], c[N];
    bool is_master = false;
    MPI_Comm comm;
    MPI_Init(&argc, &argv);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Generate vectors
    for(int i=0; i<N; i++){
        a[i] = i;
        b[i] = i;
    }
    
    // Determine the number of elements to send to each process
        //printf("Comm size: %d\n", size);
        int send_counts[size];
        int displacements[size];
        int elem_per_process = (int) N/ (size);
        int rem = N % (size);
        int *a_part;
        int *b_part;

        //Split data among processes
        for (int i = 0; i < size; i++) {
            send_counts[i] = elem_per_process + (i < rem ? 1 : 0);
            displacements[i] = (i > 0) ? displacements[i - 1] + send_counts[i - 1] : 0;
            //printf("Process %d: get an array of %d elements\n", i, send_counts[i]);
            /*for(int j=0; j<send_counts[i]; j++){
                printf("Process %d: gets value a[%d] %d\n", i, i, a[j+displacements[i]]);
            }*/
        }
        a_part = (int *) malloc(send_counts[rank] * sizeof(int));
        b_part = (int *) malloc(send_counts[rank] * sizeof(int));
        //printf("Process %d: a_part size %ld\n", rank, sizeof(a_part) / sizeof(int));
        MPI_Scatterv(a, send_counts, displacements, MPI_INT, a_part, send_counts[rank], MPI_INT, 0, comm);
        MPI_Scatterv(b, send_counts, displacements, MPI_INT, b_part, send_counts[rank], MPI_INT, 0, comm);

    if(rank == 0){        
        //printf("Hi im process slave %d: get an array of %d elements\n", rank, send_counts[rank]); 
        //MPI_Scatterv(b, send_counts, displacements, MPI_INT, b_part, send_counts[rank], MPI_INT, 0, comm);
    }
    else{
        //printf("Hi im process slave %d: get an array of %d elements\n", rank, send_counts[rank]); 
        for(int i=0; i<send_counts[rank]; i++){
            printf("Process slave sum %d: %d\n", rank, a_part[i] + b_part[i]);
        }
    }
    

    MPI_Finalize();
    return 0;
}

//TODO escludere p0 dallo scatter
