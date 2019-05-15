/***********************************************************************************************
 *   File: readBenchmark.c                                 	                               *
 *   Description: Read the SNAP Benchmark and generate file				       *
 *                                                                                             *
 *   Author: Group-10 (Jatin and Swapnil)                                                      *
 *                                                                                             *
 ***********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INF 99999

int getIndex(int i, int j, int cols)
{
	return (i * cols + j);
}

int main(int argc, char **argv)
{

	FILE *inputfile, *cmd;
	char buff[800];
	char *token;
	int total_lines;
	int i = 0;
	size_t len = 0;
	if(argc != 4 ){
	    printf("Invalid Arguments !! \n");
    	    printf("Usage: %s Inputfilename NodeCount Outputfile\n", argv[0]);
	    return -1;
    }
    
    char *infile = argv[1];

    int V = atoi(argv[2]);
	inputfile = fopen(infile, "r");

	int *arr = (int *)(malloc(sizeof(int) * V * V));

	// Initialize the arr with INF and diagonal elements as 0
	for (int i = 0; i < V * V; i++)
	{
		arr[i] = INF;
	}

	int ind_i = 0, ind_j = 0, weight;
	while (fgets(buff, sizeof buff, inputfile) != NULL)
	{

		int num;
		token = strtok(buff, ",");
		ind_i = atoi(token);
		printf("%d\t", ind_i);

		if (token != NULL)
			token = strtok(NULL, ",");
		ind_j = atoi(token);
		printf("%d\t", ind_j);

		token = strtok(NULL, ",");
		weight = atoi(token);
		printf("%d\t", weight);

		arr[getIndex(ind_i, ind_j, V)] = weight;
		while (token != NULL)
		{

			token = strtok(NULL, ",");
		}

		printf("\n");
	}

	fclose(inputfile);
	
	FILE *outputfile;
	char *outfile = argv[3];
	outputfile = fopen(outfile, "wb");


	for (int i = 0; i < V * V; i++)
	{
		fprintf(outputfile,"%d", arr[i]);
	}
}

