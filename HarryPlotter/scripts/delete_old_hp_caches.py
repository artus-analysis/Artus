import subprocess

agecutoff = 90

total = int(subprocess.check_output("ls $HP_WORK_BASE_COMMON/caches -1 | wc -l", shell=True))
print "Total files in cache folder:",total

## I guessed that checking the age of the oldest file would be faster than going through all files with "find", but this didn't seem to be the case
#oldestfile = subprocess.check_output("ls $HP_WORK_BASE_COMMON/caches -tr | sed -n '1p'", shell=True)
#oldestfiletime = subprocess.check_output("stat -c %Z $HP_WORK_BASE_COMMON/caches/"+oldestfile, shell=True)
#currenttime = subprocess.check_output("date +%s", shell=True)
#oldestfileage = (int(currenttime)-int(oldestfiletime))/60/60/24 #Output in seconds, calculate to days
#old = 0
#if oldestfileage > agecutoff:
old = int(subprocess.check_output("find $HP_WORK_BASE_COMMON/caches -mtime +"+str(agecutoff)+" -user ${USER} -type f -name '*.root' | wc -l", shell=True))
print "Files older than",agecutoff,"days:",old

if old > total/2:
  midfileage = agecutoff
  print "There are plenty of old files. Remove these?"
else:
  midfile = subprocess.check_output("ls $HP_WORK_BASE_COMMON/caches -t | sed -n '"+str(total/2)+"p'", shell=True)
  midfiletime = subprocess.check_output("stat -c %Z $HP_WORK_BASE_COMMON/caches/"+midfile, shell=True)
  currenttime = subprocess.check_output("date +%s", shell=True)
  midfileage = (int(currenttime)-int(midfiletime))/60/60/24 #Output in seconds, calculate to days
  print "There are many files that aren't too old. Remove the older half? (Files older than",midfileage,"days)"

answer = raw_input("[y|N]: ")
if answer in ["y","Y"]:
  print "Removing old files..."
  delete = subprocess.check_output("find $HP_WORK_BASE_COMMON/caches -mtime +"+str(midfileage)+" -user ${USER} -type f -name '*.root' -delete", shell=True)
  print "Done!"
exit()
