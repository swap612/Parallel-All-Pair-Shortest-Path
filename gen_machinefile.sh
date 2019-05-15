######## script to generate machinefile ######

#remove prev available machine file
rm -f machinefile

ppn=8         #set ppn

# add running ip to machinefile
myIP=$(ifconfig | grep '172.27.19.'| awk '{print $2}')      #extracting current ip
echo $myIP":8" >> machinefile

count=1       # counter to store hosts count
ip=1          # value for IP address last place

# loop through IP 1 to 30 to check the connectivity 
while [ $ip -lt 61 ]; do

  host=172.27.19.$ip          # set ip address for host
  
  if [ $host == $myIP ]; then   #if host is same as myIP skip as it was already added
     ip=`expr $ip + 1`
     continue;
  fi                   

  ping -c 1 $host > /dev/null           # checking connectivity by pinging the host
  
  #if above cmd result in 0 means host is connected
  if [ $? -eq 0 ]; then                 # $? store the output of prev cmd
    echo $host:$ppn >> machinefile      # add working host:ppn to machinefile
    count=`expr $count + 1`             # increment the count
  fi

  if [ $count -eq 16 ]; then            #If count becomes 16 stop
    break
  fi
  
  ip=`expr $ip + 1`        #incrementing the ip

done
