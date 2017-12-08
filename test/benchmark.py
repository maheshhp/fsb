import os

currString = "./fsb "
for i in xrange(10000):
    currString = currString + "try/dir"+str(i)+" "

print currString
print os.system(currString)
