/*************************************************************************************************
 *   File: naiveCheckerboard.c                                                                   *
 *   Description: Naive Checkerboard Algorithm                                                   *
 *                Assign each element to 1 process so for V vertices graph we need V^2 processes *
 *   Authors:   Group-10 (Jatin, Swapnil)                                                        *
 *                                                                                               *
 ************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define INF 99999
#define V 5
int main(int argc, char *argv[])
{
  int myrank, size;

  MPI_Init(&argc, &argv);
  MPI_Status status;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  double proc_time, start_time, total_time, sum_time, max_time;

  int graph[V][V] = {{0, INF, 6, 3, INF},
                     {3, 0, INF, INF, INF},
                     {INF, INF, 0, 2, INF},
                     {INF, 1, 1, 0, INF},
                     {INF, 4, INF, 2, 0}

  };
  
  start_time = MPI_Wtime();
  int row_index = (myrank / V);
  int column_index = (myrank % V);
  // printf("myrank is %d\n",myrank);
  int temp = graph[row_index][column_index];
  int m = 5, l, k;
  for (l = 1; l <= V; l++)
  {
    k = l;
    // printf("%d iteration\n",k);
    //sending (k-1)th column elements to its respective row.
    if (((myrank % V) == (k - 1)))
    {
      //  printf("%d rank ,sending row, %d iteration \n",myrank,k);
      for (int j = 0; j < V; j++)
      {
        int recv_rank = (myrank / V) * (V) + j;
        if (recv_rank != myrank)
        {

          MPI_Send(&temp, 1, MPI_INT, recv_rank, 99, MPI_COMM_WORLD);
        }
      }
    }
    //sending (k-1)th row elements to its respective columns.
    if (myrank / V == (k - 1))
    {
      // printf("%d rank ,sending column, %d iteration \n",myrank,k);
      for (int j = 0; j < V; j++)
      {
        int recv_rank = (myrank + j * V) % (V * V);
        if (recv_rank != myrank)
        {

          int row_index = (myrank / V);
          int column_index = (myrank % V);
          MPI_Send(&temp, 1, MPI_INT, recv_rank, 99, MPI_COMM_WORLD);
        }
      }
    }
    int recv1, recv2;
    if (!((myrank % V == (k - 1)) && (myrank / V == (k - 1))))
    {
      if ((myrank % V == (k - 1)) || (myrank / V == (k - 1)))
      {

        MPI_Recv(&recv1, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);
        // printf("%d rank,receiving once,%d iteration \n",myrank,k);
        if (temp > temp + recv1)
          temp = temp + recv1;
      }
      else
      {

        // printf("%d rank is receiving twice,%d iteration\n",myrank,k);

        MPI_Recv(&recv1, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);

        MPI_Recv(&recv2, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);
        if (temp > recv1 + recv2)
          temp = recv1 + recv2;
      }
    }
    //  printf("barrier %d iteration of %d rank\n",k,myrank);
    MPI_Barrier(MPI_COMM_WORLD);
  }

  // printf("after iteration ---%d--value is--%d\n",myrank,temp);
  int *rbuf = NULL;

  proc_time = MPI_Wtime() - start_time;

  if (myrank == 0)
    rbuf = (int *)malloc(size * sizeof(int));
  MPI_Gather(&temp, 1, MPI_INT, rbuf, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (myrank == 0)
  {
    for (int i = 0; i < size; i++)
    {
      printf("%d\t", rbuf[i]);
      if (i % V == V - 1)
        printf("\n");
    }
  }

  // Calculating Max_time at rank 0
  MPI_Reduce(&proc_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  // Printing the max time reduced at rank 0
  if (myrank == 0)
    printf("%d  %lf\n", size, max_time);

  MPI_Finalize();
  return 0;
}
