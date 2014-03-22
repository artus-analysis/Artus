# run like this: ./runBisectArtusTest <good commit> <known bad commit>
echo "Starting bisect from good rev. $1 to bad rev. $2"
git bisect start
git bisect bad $1
git bisect good $2
git bisect run ./gitBisectArtusTest.sh
git bisect reset
echo "Bisect done"
