#Generate the machinefile
echo "Generating Machinefile"
./gen_machinefile.sh

rm -f plotdata
for N in  1024 2048 4096 8192 16384
do
    for P in  32 64 128 256
    do 
        echo "Running APSP of $N nodes on $P processes"       
        mpiexec -n $P -f machinefile ./BlockedParallel.x ./inputGraphs/graph_$N $N >> plotdata
        mkdir ./profile/profile_$N.$P
        cp profile.* ./profile/profile_$N.$P
        rm profile.* 
    done
done
