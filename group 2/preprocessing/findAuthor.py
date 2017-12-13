"""
2
Author:Yue Cui
Date:11/5/2017
This program will create an author.csv file which contains all the authors who has published more than 1 paper.
"""
import csv
from collections import defaultdict

readAll = csv.reader(open('all.csv', 'rU'))
writer = csv.writer(open("all_noConf.csv", 'wb'))
for row in readAll:
    del row[0]
    writer.writerow(row)
readNoConf = csv.reader(open("all_noConf.csv", 'rU'))
writeAuthor = csv.writer(open('author.csv', 'wb'))
name = defaultdict(int)
for row in readNoConf:
    for item in row:
        name[item] += 1
for row in name.items():
    if row[1] > 1 and row[0] != "":
        writeAuthor.writerow([row[0]])
