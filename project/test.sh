#!/bin/bash

echo "1. High contention hand over hand lock testing"
echo "2. High contention reader writer lock testing"
echo "3. Uniform access hand over hand lock testing"
echo "4. Uniform access reader writer lock testing"
echo "5. Assertion test for put, get and range query for hand over hand locking"
echo "6. Assertion test for put, get and range query for reader writer locking"

read -p "Enter choice from the above options:" option
# option=$1

case $option in
    1) ./bst -t 10 --lower 0 --upper 10000 --test high --sync hoh
    ;;
    2) ./bst -t 10 --lower 0 --upper 10000 --test high --sync rw
    ;;
    3) ./bst -t 50 --lower 0 --upper 10000 --test random --sync hoh
    ;;
    4) ./bst -t 50 --lower 0 --upper 10000 --test random --sync rw
    ;;
    5) ./bst  --lower 0 --upper 50000 --test simple --sync hoh --incrementor 10
    ;;
    6) ./bst  --lower 0 --upper 50000 --test simple --sync rw --incrementor 10
    ;;
esac
# while true; do
#     sleep 1
#     ./bst -t 10 --lower 0 --upper 10000 --test high --sync hoh
# done