#!bash
while true; do
    sleep 1
    ./bst -t 10 --lower 0 --upper 10000 --incrementor 1 --contention 1 --sync hoh --test no
    ./bst -t 10 --lower 0 --upper 10000 --incrementor 1 --contention 1 --sync rw --test no
done