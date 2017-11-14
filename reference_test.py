import os
i = 1, 10, 20, 30
k = 0
for m in i:
    print 'Creation of ' + str(m) + ' directories'
    cmd = './a.out '
    for j in xrange(0, m):
        cmd += 'dir' + str(k) + ' '
        k = k + 1
    os.system(cmd)
