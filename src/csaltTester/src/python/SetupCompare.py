#!/usr/bin/env python3

# Python3 script used to setup truth data for the CSALT test system
# We may want to add some features here... (ID specific file/folder on command 
# line, etc)
#
# Written by Darrel Conway, Thinking Systems
# Date:      July 5, 2017
import os
import StringUtil as su

# These are the strings seen on Linux.  Mac?  Win?
searchStrings = ["No. of iterations", "Objective"]

# Location of the truth data goes here:
todir = "./truth"

# Function that builds a comparison file from the truth folder
#
# @param todir The folder containign the truth data
def ParseSnoptFiles(todir):
   compareFileName = "NewCompare.txt"
   outFile = open(compareFileName, 'w')
   outFile.truncate()

   for root, dirs, files in os.walk(todir):
      for filename in files:
         base, extension = os.path.splitext(filename)
         if (extension == ".snopt"):
            testName = base[:-4]
            	
            with open(todir + "/" + filename) as data:
               for line in data:
                  text = su.TextAfter(line, searchStrings)
                  if text != "":
                     value = text
            testline = testName + "\t" + value
            outFile.write(testline)
            
   outFile.close()
   
# Core functionality: moves files into place

ParseSnoptFiles(todir)
