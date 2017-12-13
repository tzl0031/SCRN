"""
 Author:Yue Cui
 Date:11/5/2017
 This program will read all the sub-trees in file dblp.xml to a .csv file
 with the constrain of 15 required conference from 2000 to 2010.
 The first row in the .csv file is the conference name and the following rows are the authors.
"""
import unicodecsv as csv
from lxml import etree

parser = etree.XMLParser (load_dtd=True)

tree = etree.parse ("dblp.xml", parser)
root = tree.getroot ()
count = 1
with open ('all.csv', 'a') as csvFile:
    writer = csv.writer (csvFile, encoding='utf-8')

    for child in root:
        type = child.tag
        typeList = ['article', 'inproceedings', 'proceedings', 'book', 'incollection']
        isConf = child.attrib['key'].split ('/')[0]
        confName = child.attrib['key'].split ('/')[1]
        confList = ['icde', 'vldb', 'pods', 'edbt', 'kdd', 'icdm', 'sdm', 'pakdd', 'ijcai', 'aaai', 'sigir', 'ecir',
                    'cvpr', 'icml', 'ecml']
        row = [confName]
        if type in typeList:
            if isConf == "conf":
                if confName in confList:
                    if 2000 <= int (child.find ('year').text) <= 2010:
                        for author in child.iter ('author'):
                            row.append (author.text)
                        try:
                            writer.writerow (row)
                            count += 1

                        except KeyboardInterrupt:
                            writer.close ()
