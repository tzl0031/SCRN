"""
Author:Yue Cui
Date:11/5/2017
"""
import csv

nameFile = csv.reader(open('author.csv', 'rU'))
resultFile = csv.reader(open('all.csv', 'rU'))
newFile = csv.writer(open("final.csv", 'wb'))
name = []

for row in nameFile:
    name.extend(row)

for row in resultFile:
    for item in row:
         if item in name:
            newFile.writerow(row)
            break