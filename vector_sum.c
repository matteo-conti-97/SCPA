#include<mpi.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#define N 10000000

bool checkResult(int *a, int *b, int *c);

int main(int argc, char *argv[]){
    int rank, size,tag=0;
    int *a, *b, *c;
    bool is_master = false;
    MPI_Comm comm;
    MPI_Init(&argc, &argv);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank==0){
        a= (int *) malloc(N * sizeof(int));
        b= (int *) malloc(N * sizeof(int));
        c= (int *) malloc(N * sizeof(int));
        //Generate vectors
        for(int i=0; i<N; i++){
            a[i] = i;
            b[i] = i;
        }
    }
    
    // Determine the number of elements to send to each process
        //printf("Comm size: %d\n", size);
        int send_counts[size];
        int displacements[size];
        int elem_per_process = (int) N/ (size-1);
        int rem = N % (size-1);
        int *a_part;
        int *b_part;
        int *ret;
        send_counts[0] = 0; //Send count will receive no data
        displacements[0] = 0; //Displacement is 0 for process 0
        
        MPI_Barrier(comm);//Sync processes
        double t1; 
        t1 = MPI_Wtime();
        //Split data among processes except process 0
        for (int i = 1; i < size; i++) {
            send_counts[i] = elem_per_process + (i <= rem ? 1 : 0);
            displacements[i] = (i > 0) ? displacements[i - 1] + send_counts[i - 1] : 0;
            //printf("Process %d: send count %d\n", i, send_counts[i]);
        }

        //Allocate memory for the scattered data to receive
        a_part = rank == 0 ? (int *)( MPI_IN_PLACE ) : (int *) malloc(send_counts[rank] * sizeof(int));
        b_part = rank == 0 ? (int *)( MPI_IN_PLACE ) : (int *) malloc(send_counts[rank] * sizeof(int));
        MPI_Scatterv(a, send_counts, displacements, MPI_INT, a_part, send_counts[rank], MPI_INT, 0, comm);
        MPI_Scatterv(b, send_counts, displacements, MPI_INT, b_part, send_counts[rank], MPI_INT, 0, comm);

    if(rank == 0){        
    }
    else{
        ret = (int *) malloc(send_counts[rank] * sizeof(int));
        //printf("Hi im process slave %d: get an array of %d elements\n", rank, send_counts[rank]); 
        for(int i=0; i<send_counts[rank]; i++){
            ret[i] = a_part[i] + b_part[i];
            //printf("Process slave sum %d: %d\n", rank, ret[i]);
        }
    }
    
    MPI_Gatherv(ret, send_counts[rank], MPI_INT, c, send_counts, displacements, MPI_INT, 0, comm);
    if(rank==0){
        
        /*printf("Result at process %d is:\n[", rank);
        for(int i=0; i<N; i++){
            if(i==N-1)
                printf("%d", c[i]);
            else
                printf("%d,", c[i]);
        }
        printf("]\n");*/
        if(checkResult(a, b, c))
            printf("Result is correct\n");
        else
            printf("Result is wrong\n");
        printf("Elapsed %lf seconds\n", MPI_Wtime()-t1);
    }

    MPI_Finalize();
    return 0;
}

bool checkResult(int *a, int *b, int *c){
    for(int i=0; i<N; i++){
        if(c[i] != a[i] + b[i])
            return false;
    }
    return true;
}

//TODO escludere p0 dallo scatter
