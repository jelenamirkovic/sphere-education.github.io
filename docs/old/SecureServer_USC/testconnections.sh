i=1
while [ $i -le 6 ] ; do
    ssh blueserver.uscbank1$i.usc430 "hostname; ssh 1.1.1.2 'hostname'"
    i=$(($i+1))
    echo
    echo
done
