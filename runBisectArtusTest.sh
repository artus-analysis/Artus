# run like this: ./runBisectArtusTest <known bad commit> <known good commit>
echo "Starting bisect from bad rev. $1 to good rev. $2"
git bisect start
git bisect bad $1
git bisect good $2
git bisect run ./gitBisectArtusTest.sh
git bisect reset
echo "Bisect done"
