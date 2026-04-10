#!/usr/bin/env python3

# Python3 script used to setup truth data for the CSALT test system
# We may want to add some features here... (ID specific file/folder on command 
# line, etc)
#
# Written by Darrel Conway, Thinking Systems
# Date:      July 5, 2017
import os
import shutil
import StringUtil as su

# These are the strings seen on Linux.  Mac?  Win?
searchStrings = ["No. of iterations", "Objective"]

# Directories to use go here; from is data from a run, to is truth location
fromdir = r"./run"
todir = r"./truth"

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

# Walk through the files in the fromdir
for root, dirs, files in os.walk(fromdir):
   for filename in files:
      path = os.path.join(root, filename)

      # Separate file name from extension
      base, extension = os.path.splitext(filename)

      if (extension == ".txt"):
         # SNOPT output file -- make more robust!
         newExtension = ".snopt"
      else:
         newExtension = ".truth"

      if (base != "CsaltLog"):
         shutil.copy(fromdir + "/" + filename, todir + "/" + base + newExtension)
         
      ParseSnoptFiles(todir)

