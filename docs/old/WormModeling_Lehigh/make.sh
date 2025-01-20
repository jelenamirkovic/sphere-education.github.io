cd ~/paws; make clean; make; rm log*
./slammer &
sleep 300
./stop_paws
ls

