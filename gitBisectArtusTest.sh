# use this script to run a git bisect to find
# commits which break the tests
#
# 1) start the bisect process
#
#	git bisect start
#
# 1) mark a known bad commit
#
#	git bisect bad <commit>
#
# 2) mark a know good commit
# 
#	git bisect good <commit>
#
# 3) run the automated process
#
#	git bisect run ./gitBisectArtusTest.sh
#
# 4) finish with the bisect process, once the error
#    has been found
#
#	git bisect reset
#
rm -rf CMakeFiles/
rm CMakeCache.txt
cmake .
if [ $? -ne 0 ]; then
        echo "cmake failed"
        exit 1
fi
make clean 
make
if [ $? -ne 0 ]; then
        echo "make"
        exit 1
fi
./artus_core_test
# git bisect needs error codes in this range
if [ $? -gt 126 ]; then
	echo "test failed"
	exit 1
fi 


