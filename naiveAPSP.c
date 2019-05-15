/**************************************************************************************************
 *   File: naiveAPSP.c                                                                 			  *
 *   Description: Naive APSP Algorithm                                                 			  *
 *                Assign row is assigned to 1 process so for V vertices graph we need V processes *
 *   Authors:   Group-10 (Jatin, Swapnil)                                                         *
 *                                                                                                *
 *************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define INF 99999

int main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);
    MPI_Status status;
    int myrank, size;
     if(argc != 3 ){
	    printf("Invalid Arguments !! \n");
    	    printf("Usage: %s Filename VerticesCount \n", argv[0]);
	    return -1;
    }

    char *inputfile = argv[1];
    int V = atoi(argv[2]);
   
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double proc_time, start_time, total_time, sum_time, max_time;

    int *sdbuf;

    int numbytes = V * sizeof(int);
    sdbuf = (int *)malloc(numbytes);

    // Reading the file input 
    MPI_File fileHandle;
    int mode = MPI_MODE_RDWR;
    start_time = MPI_Wtime();
    
    MPI_File_open(MPI_COMM_WORLD, inputfile, mode, MPI_INFO_NULL, &fileHandle);
    MPI_Offset disp = (MPI_Offset)myrank * numbytes;

    MPI_File_set_view(fileHandle, disp, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
    int result = MPI_File_read(fileHandle, sdbuf, size, MPI_INT, &status);
    MPI_File_close(&fileHandle);

    int *myarr = sdbuf;

    int *recvarr = (int *)(malloc(sizeof(int) * V));
    int k, j;
    
    for (k = 0; k < V; k++)
    {
 
        //Mpi broadcast
        if (myrank == k)
        {
            for (j = 0; j < V; j++)
            {
                recvarr[j] = myarr[j];
            }
        }
        MPI_Bcast(recvarr, V, MPI_INT, k, MPI_COMM_WORLD);
        for (j = 0; j < V; j++)
        {
            if (myarr[j] > myarr[k] + recvarr[j])
            {
                myarr[j] = myarr[k] + recvarr[j];
            }
        }
        //   printf("barrier %d iteration of %d rank\n", k, myrank);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // printf("after iteration ---%d--value is--%d\n",myrank,temp);
    int *rbuf = NULL;
    if (myrank == 0)
        rbuf = (int *)malloc(size * size * sizeof(int));
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Gather(myarr, V, MPI_INT, rbuf, V, MPI_INT, 0, MPI_COMM_WORLD);

    // Writing the output to file
    MPI_File fh;
    mode = MPI_MODE_CREATE | MPI_MODE_RDWR;
    MPI_File_open(MPI_COMM_WORLD, "output.txt", mode, MPI_INFO_NULL, &fh);
    disp = (MPI_Offset)myrank * numbytes;
    MPI_File_write_at(fh, disp, myarr, size, MPI_INT, &status);
    MPI_File_close(&fh);

    proc_time = MPI_Wtime() - start_time;

    // Calculating Max_time at rank 0
    MPI_Reduce(&proc_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // Printing the max time reduced at rank 0
    if (myrank == 0)
        printf("%d  %lf\n",size, max_time);

    MPI_Finalize();
    return 0;
}
