#!/bin/bash

find $HP_WORK_BASE_COMMON/caches -mtime +100 -user ${USER} -type f -name "*.root"

echo ""
read -p "Delete these files? [y|N] " YN
case $YN in
	[Yy]* ) echo "Deleting...";;
	* ) echo "Quit"; exit;;
esac

find $HP_WORK_BASE_COMMON/caches -mtime +100 -user ${USER} -type f -name "*.root" -delete
echo "Done"
