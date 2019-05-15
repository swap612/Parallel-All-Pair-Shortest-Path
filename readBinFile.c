/*************************************************************************************************
 *   File: readBinFile.c                                    		                             *
 *   Description: Read Binary input file for testing purpose                                     *
 *               																				 *
 *   Authors:   Group-10 (Jatin, Swapnil)                                                        *
 *                                                                                               *
 ************************************************************************************************/

#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
    if(argc != 3 ){
	    printf("Invalid Arguments !! \n");
    	    printf("Usage: %s filename NodeCount \n", argv[0]);
	    return -1;
    }
    char *infile = argv[1];
    int V = atoi(argv[2]);
     printf("V is %d\n", V);
     
    FILE *fp = fopen(infile, "rb");

    int value;
    int rc;
    int i =0;
    while (rc = fread(&value, sizeof(int), 1, fp) > 0)
    {
      
        printf("%d\t", value);
        i++;
          if(i == V){
            printf("\n");
            i =0;
        }
    }
}
