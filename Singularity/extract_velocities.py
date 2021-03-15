#!/usr/bin/env python3

import csv
import sys


if len(sys.argv) == 3:
    evolution = int(sys.argv[2])-1
    outputfile = open('Parameters_File.csv', mode='a')
    csvwriter = csv.writer(outputfile)

    with open(sys.argv[1],'r') as csvfile:
        data = csv.reader(csvfile, delimiter=',')
        for row in data:
            if row[0].startswith("best"):
                evolution = evolution + 1
                csvwriter.writerow(["Evolution {:d}".format(evolution),row[0]])
                row = next(data)
                csvwriter.writerow(row)

else:
    print("Invalid command.\n./extract_velocities.py filename start_number")
