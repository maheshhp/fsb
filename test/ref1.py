import os

currString = "./fsb3 "
for i in xrange(500):
    currString = currString + "dir"+str(i)+" "

print os.system(currString)
