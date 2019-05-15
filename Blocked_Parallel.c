/***********************************************************************************************
 *   File: Blocked_Parallel.c                                                                  *
 *   Description: Blocked Parallel Version of APSP							                   *
 *                                                                                             *
 *   Author: Group-10 (Jatin and Swapnil)                                                      *
 *                                                                                             *
 ***********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include<time.h>
#include<string.h>
#define INF 99999

int min(int a, int B)
{
  if (a < B)
    return a;
  else
    return B;
}
int floyd(int **C, int **A, int **B, int block_size)
{
  int i, j, k;
  for (k = 1; k <= block_size; k++)
    for (j = 1; j <= block_size; j++)
      for (i = 1; i <= block_size; i++)
      {
        C[i][j] = min(C[i][j], A[i][k] + B[k][j]);
      }
}
int main(int argc, char *argv[])
{

  MPI_Init(&argc, &argv);
  MPI_Status status;
  int myrank, size;
  if (argc != 3)
  {
    printf("Invalid Arguments !! \n");
    printf("Usage: %s Filename VerticesCount \n", argv[0]);
    return -1;
  }
  
  char *inputfile = argv[1];
  int V = atoi(argv[2]);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  double proc_time, start_time, total_time, sum_time, max_time;

  //getting mpi comm size
  int B = V / size;
  int numbytes = ((V * B) * sizeof(int));
  int *sdbuf;
  sdbuf = (int *)malloc(numbytes);

  // Reading the file input
  MPI_File fileHandle;
  int mode = MPI_MODE_RDWR;
  start_time = MPI_Wtime();

  MPI_File_open(MPI_COMM_WORLD, inputfile, mode, MPI_INFO_NULL, &fileHandle);
  MPI_Offset disp = (MPI_Offset)myrank * numbytes;

  MPI_File_set_view(fileHandle, disp, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
  
  int result = MPI_File_read(fileHandle, sdbuf,V*B, MPI_INT, &status);
  //printf("result=%d\n",result);
  MPI_File_close(&fileHandle);
  int *myarr = sdbuf;
  //temp, 2-d array  to store computed pivot
  int *temp = (int *)malloc(((B * B)) * sizeof(int));

  // for (int i = 0; i < size; i++)
  //  printf("%d %d \n", myrank, myarr[i]);
  int *recvarr = (int *)malloc(((V * V) / size) * sizeof(int));
  int k, j;
//   printf("B= %d\n",B);
  
//   if (myrank == 0)
//    {
//      for (int i = 0; i < V*B; i++)
//      {
//       printf("%d\t", sdbuf[i]);
//       if (i % V == V - 1)
//         printf("\n");
//      }
//    }
// // // 

  for (k = 1; k <= size; k++)
  {
   printf("%d iteration\n", k);
    
     if (myrank == k-1)
     {
      for (int k_i = 0; k_i < B; k_i++)
        {
          for (int j = 0; j < B; j++)
          for (int i = 0; i <B; i++)
          {

            *((myarr + (V * i) + (k - 1) * B + j)) = min(*(myarr + (V * i) + (k - 1) * B + j), *(myarr + (V * i) + (k - 1) * B + k_i) + *(myarr + (V * k_i) + (k - 1) * B + j));
                //A[i][j]=min(A[i][j],A[i][k]+A[k][j])
                //store to temp array
                temp[i * B + j] = *((myarr + (V * i) + (k - 1) * B + j));
          }
        }   

   
  }
  //  2.broadcast this  pivot block(temp) to all processes
   // MPI_Bcast(temp, (B * B), MPI_INT, k - 1, MPI_COMM_WORLD);

    //3.computing row ,column pivots blocks
    //computing row if myrank is k-1
    //4.for the row pivots take the current block and itself to compute the block   floyd(KK,Kj)
    if (myrank == k - 1)
    {
      for (int m = 1; m <=size; m++)
      {
        if (m != k)
        {
          for (int k_i = 0; k_i < B; k_i++)
            for (int j = 0; j < B; j++)
              for (int i =0; i < B; i++)
              {

                *((myarr + (V * i) + (m - 1) * B + j)) = min(*(myarr + (V * i) + (m - 1) * B + j), *(myarr + (V * k_i) + (m - 1) * B + j) + temp[i * B + k_i]);
                //A[i][j]=min(A[i][j],A[i][k]+A[k][j])
              }
        }
      }
    }
  //  5.for the column pivots take the temp block and itself to compute the block  (ik,kk)
   // computing column
   //copy myarr to recvarr
    if(myrank==k-1)
    {
    for (int i = 0; i < V * B; i++)
      recvarr[i] = myarr[i];
    }
    //6.broadcast columns pivots,and calculate.for the row pivots don't send.compute.
    MPI_Bcast(recvarr, (V * B), MPI_INT, k - 1, MPI_COMM_WORLD);

    int k_i, i, m;
    if (myrank != k - 1)
    {
      for (k_i = 0; k_i < B; k_i++)
        for (j = 0; j <B; j++)
          for (i = 0; i < B; i++)
          {

            *((myarr + (V * i) + (k - 1) * B+j) )= min(*(myarr + (V * i) + (k - 1) * B + j), *(myarr + (V * i) + (k - 1) * B + k_i) + *(recvarr + (V * k_i) + (k - 1) * B + j));
            //A[i][j]=min(A[i][j],A[i][k]+A[k][j])

          }
      for (m = 1; m <= size; m++)
      {
        if (m != k)
        {
          for (k_i = 0; k_i < B; k_i++)
            for (j = 0; j < B; j++)
              for (i = 0; i < B; i++)
              {

                *((myarr + (V * i) + (m - 1) * B + j)) = min(*(myarr + (V * i) + (m - 1) * B + j), *(myarr + (V * i) + (k- 1) * B + k_i) + *(recvarr + (V * k_i) + (m - 1) * B + j));
                //A[i][j]=min(A[i][j],A[i][k]+A[k][j])
  
              }
        }
      }
    }
    
//    MPI_Barrier(MPI_COMM_WORLD);
   }

  // printf("after iteration ---%d--value is--%d\n", myrank, temp);
  int *rbuf = NULL;
 if (myrank == 0)
  rbuf = (int *)malloc((V*V) * sizeof(int));
  MPI_Gather(myarr, V*B, MPI_INT, rbuf, V*B, MPI_INT, 0, MPI_COMM_WORLD);
 /* if (myrank == 0)
  {
    for (int i = 0; i < (V*V); i++) 
    {
      printf("%d\t", rbuf[i]);
      if (i % V == V - 1)
        printf("\n");
    }
  }
  */
  double end_time=MPI_Wtime();
  proc_time=end_time-start_time;

  MPI_Reduce(&proc_time, &max_time, 1,MPI_DOUBLE, MPI_MAX, 0,MPI_COMM_WORLD);

 // Writing the output to file
 /*   MPI_File fh;
    mode = MPI_MODE_CREATE | MPI_MODE_RDWR;
    MPI_File_open(MPI_COMM_WORLD, "output.txt", mode, MPI_INFO_NULL, &fh);
    disp = (MPI_Offset)myrank * numbytes;
  MPI_File_set_view(fh, disp, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
// int result = MPI_File_read(fileHandle, sdbuf,V*B, MPI_INT, &status);
    MPI_File_write_at(fh, disp, myarr, V*B, MPI_INT, &status);
    MPI_File_close(&fh);
*/
  if(myrank==0)
        {
            printf(" %d, %d ,%f \n", V, size, max_time); 
                 
        }

  MPI_Finalize();
  return 0;
} 
