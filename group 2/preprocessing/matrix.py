import csv


counterFile = csv.reader(open('counter.csv', 'rU'))
writeMatrix = csv.writer(open('matrix.csv','wb'))
conf = csv.reader(open('final.csv','rU'))
confList = []
authorList= []
existedConf = []
for item in conf:
    confList.append(item)

for author in counterFile:
    authorList.append(author)

print "done"
for author in authorList:

    print "author"
    for item in confList:
        print "conf"
        if author[:2] in item[1:]:
            existedConf.append(item[0])
            writeMatrix.writerow(author + [existedConf])
            existedConf = []
        else:
            print "NO??????"
