sudo apparmor_parser -R /etc/apparmor.d/usr.sbin.tcpdump
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
hostname=`hostname`
dev=`ip route get 10.1.5.2 | awk '{print $3}'| head -1`
if [[ $hostname =~ client ]] ; then
    dev=`ip route get 10.1.5.2 | awk '{print $5}'| head -1`
fi
opid=0
sudo tcpdump -i $dev -w /var/log/baseline.dump -nn &
sleep 100
opid=`ps axuw | awk '{print $2 " " $11}' | grep tcpdump | awk '{print $1}'`
echo "killing $opid"
sudo kill -9 $opid
sudo chmod a+rx /var/log/baseline.dump
python /share/education/ResilientServerGamified_USC/gameon/flow_stats/monitor.py -i 10 /var/log/baseline.dump --db /tmp/baseline.fs
opid=0
while :
do
    sudo tcpdump -i $dev -w /var/log/current.dump -nn &
    sleep 60
    opid=`ps axuw | awk '{print $2 " " $11}' | grep tcpdump | awk '{print $1}'`
    echo "Killing $opid"
    sudo kill -9 $opid
    sudo chmod a+rx /var/log/current.dump
    python /share/education/ResilientServerGamified_USC/gameon/flow_stats/monitor.py -i 10 /var/log/current.dump --db /tmp/current.fs
    cd /share/education/ResilientServerGamified_USC/gameon/flow_stats/; python detect.py > /tmp/detect.working
    mv /tmp/detect.working /tmp/detect.log
done
