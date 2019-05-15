"""
    FileName: genAdjacencyMatrix.py
	Generates a random undirected graph adjacency matrix with positive weights
    Authors: Group-10 (Jatin, Swapnil)	
"""

import numpy as np
import sys

# check for the number of arguments
if len(sys.argv)!=2:
	print ("Wrong number of arguments!")
	print ("Usage: python "+ sys.argv[0] +" NodeCount")
	sys.exit()

# set the number of nodes in graph by reading the argument
NodeCount = int(sys.argv[1])

# set the weight parameter
MIN_WEIGHT = 1
MAX_WEIGHT = 10 

# Generates the random integers from 1 to 10 and create an matrix array of NodeCount* NodeCount dimension
b = np.random.random_integers(MIN_WEIGHT, MAX_WEIGHT/2, size=(NodeCount, NodeCount)) 

# making the matrix symmetric as the undirected graph has symmetric matrix
b_symm1 = (b + b.T)

# Making the diagonal elements as '0' as distance to same node is 0 
b_symm2 =  (b_symm1)-(np.diag(b_symm1.diagonal()))

#printing the matrix
print(b_symm2)

# saves the graph in binary format
b_symm2.astype('int32').tofile('../inputGraphs/graph_'+str(NodeCount))

