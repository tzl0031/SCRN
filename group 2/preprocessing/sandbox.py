from itertools import combinations
from collections import Counter
from itertools import izip, islice
import csv
countList = Counter()
count = Counter()
authorList=[]
newList = []
testlist = [('icde','a','b','d'),('icde','b','c'),('icdm','d','a'),('icdm','a','b')]
for item in testlist:
    authorList.extend(list(combinations(tuple(item[1:]),2)))
print authorList
for item in authorList:
    newList.append(tuple(sorted(item)))
print newList

countList = Counter(newList)
for item in countList:
    print item, countList[item]

writer = csv.writer(open("test.csv",'wb'))
newList = [['A','a','b','c'],['B','d','e','f'],['C','a','b','f']]
test = [['a','b','4'],['a','c','2']]
conf = []
for item in test:
    print tuple(item[:2])
    for row in newList:
        if set(item[:2]).issubset(row[1:]):
            conf.append(row[0])
    writer.writerow(item+[conf])
    conf = []


