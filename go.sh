make clean
make arm && cat ./test_m.ko | nc -l -p 1111 -q 1
