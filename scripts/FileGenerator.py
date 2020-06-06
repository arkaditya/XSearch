import os
import sys
from os.path import join

path = sys.argv[1]
f = open("FileList.txt","w+")

for file in os.listdir(path):
	line = join(path,file)
	f.write("%s\n" % (line))


f.close()
