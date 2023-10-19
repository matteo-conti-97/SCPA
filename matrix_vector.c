#include<mpi.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#define N 4 //Number of rows of matrix
#define M 3 //Number of columns of matrix and vector size


int main(int argc, char *argv[]){
    int rank, size,tag=0;
    int *mat, *v, *c;
    MPI_Comm comm;
    MPI_Init(&argc, &argv);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    v= (int *) malloc(M * sizeof(int));
    for(int i=0; i<M; i++){
        v[i] = i;
    } 

    if(rank==0){
        mat= (int *) malloc(N * M *sizeof(int)); //Cause scatter needs contiguous memory
        c= (int *) malloc(N * sizeof(int));
        int cnt=0;

        //Generate matrix and vector
        for(int i=0; i<N; i++){
            for(int j=0; j<M; j++){
                mat[i*M+j] = cnt;
                cnt++;
            }
        }
    }
    
    // Determine the number of elements to send to each process
        //printf("Comm size: %d\n", size);
        int send_counts[size];
        int displacements[size];
        int elem_per_process = (int) (N/ (size-1))*M; //Ad ogni processo va una riga da M elementi
        int rem = N % (size-1);
        int *mat_part;
        int *local_v;
        int ret;
        send_counts[0] = 0; //Send count will receive no data
        displacements[0] = 0; //Displacement is 0 for process 0
        
        MPI_Barrier(comm);//Sync processes
        double t1; 
        t1 = MPI_Wtime();
        //Split data among processes except process 0
        for (int i = 1; i < size; i++) {
            send_counts[i] = elem_per_process + (i <= rem ? M : 0); //Add the remaining rows to the first processes, so M ints 
            displacements[i] = (i > 0) ? displacements[i - 1] + send_counts[i - 1] : 0;
            //printf("Process %d: send count %d\n", i, send_counts[i]);
        }

        //Allocate memory for the scattered data to receive and do scatter
        mat_part = rank == 0 ? (int *)( MPI_IN_PLACE ) : (int *) malloc(send_counts[rank] * sizeof(int));
        MPI_Scatterv(mat, send_counts, displacements, MPI_INT, mat_part, send_counts[rank], MPI_INT, 0, comm);
        //Broadcast vector
        MPI_Bcast(v, M, MPI_INT, 0, comm);

    if(rank != 0){        
        ret = 0;

        printf("Process %d received array:\n[", rank);
        for(int i=0; i<send_counts[rank]; i++)
            printf("%d,", mat_part[i]);
        printf("]\n");
        printf("Process %d received vector:\n[", rank);
        for(int i=0; i<M; i++)
            printf("%d,", v[i]);
        printf("]\n");
        for(int i=0; i<M; i++)
            ret += mat_part[i] * v[i];
        printf("Process slave  %d sum: %d\n", rank, ret);
    }
    
    /*MPI_Gatherv(ret, send_counts[rank], MPI_INT, c, send_counts, displacements, MPI_INT, 0, comm);
    if(rank==0){
        
        /*printf("Result at process %d is:\n[", rank);
        for(int i=0; i<N; i++){
            if(i==N-1)
                printf("%d", c[i]);
            else
                printf("%d,", c[i]);
        }
        printf("]\n");
            printf("Result is correct\n");
        else
            printf("Result is wrong\n");
        printf("Elapsed %lf seconds\n", MPI_Wtime()-t1);
    }*/

    MPI_Finalize();
    return 0;
}