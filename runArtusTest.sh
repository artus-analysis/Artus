rm -rf CMakeFiles/
rm CMakeCache.txt
cmake .
make clean 
make -j4
./artus_core_test


