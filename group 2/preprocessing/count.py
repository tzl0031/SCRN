"""
Author:Yue Cui
Date:11/5/2017
This program will read a csv file and count all the co-authors in the file and return to a new csv file
"""
from itertools import izip, islice
from collections import Counter
from itertools import combinations
import csv

authorList = []
newList = []
countList = Counter ()
with open ('final.csv', 'rU') as File:
    reader = csv.reader (File)
    for row in reader:
            authorList.extend(list(combinations(tuple(row[1:]),2)))

for author in authorList:

    newList.append(tuple(sorted(author)))


countList = Counter(newList)
with open ('counter.csv', 'wb') as outFile:
    writer = csv.writer (outFile)
    for item in countList:

        row = item + (countList[item],)

        writer.writerow(row)
        #print countList[item]