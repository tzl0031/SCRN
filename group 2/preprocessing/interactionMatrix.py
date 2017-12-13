"""
Author:Yue Cui
Date:11/6/2017
This program will read a csv file and turn into a adjacency matrix
"""
import csv
import pandas as pd
from itertools import combinations
counterFile = csv.reader(open('counter.csv', 'rU'))
writeMatrix = csv.writer(open('matrix.csv','wb'))
conf = csv.reader(open('final.csv','rU'))
confList = []
authorList= []
authorTimes = []
existedConf = []
for item in conf:
    confList.extend(item)

for author in counterFile:
    authorList.extend(author[:2])
    authorTimes.append(author)
newlist =[]
s = combinations(authorList,2)
for item in s:
    for author in authorTimes:
        if item in author:
           newlist.append(author)
           print "Y"
           break
        else:
            newlist.append([item,0])
            break

for list in newlist:
    writeMatrix.writerow(list)

