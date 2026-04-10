import math
import os.path
import sys
import re
import OCHComparator
import SNOPTComparator
import LagrangeInterpolator

def ShowUsage():
   print("\n------------------\nCSALT Test Program\n------------------\n" + 
      "Usage:\n\n   python3 TestCSALT.py <compareType> <TestCase> [options]\n\n" + 
      "where\n\n   <CompareType> is OCH, SNOPT, or All\n" + 
      "   <TestCase>    is a named test case, or \"All\" for all test cases in the Comparison.txt file." + 
      "\n\nAdditional options:\n\n" +
      "   -compFile     Name of the comparison file (default: Comparison.txt)\n" +
      "   -runpath      Path to the folder containing run data (default: ./run)\n" +
      "   -truepath     Path to the truth data used for comparisons (default: ./truth)\n" + 
      "   -tolerance    The tolerance to use in comparisons (default: 0.0005)\n\n")
   sys.exit()


#The rootpath should be the current directory.
#Datapath and truepath are paths to the folders containing data files and truth files.
rootpath = "./"
datapath = rootpath + "run/"
truepath = rootpath + "truth/"

if len(sys.argv) < 3:
   ShowUsage()
else:
   filetype = sys.argv[1]
   test = sys.argv[2]   

if filetype not in ["OCH", "SNOPT", "All"]:
   ShowUsage()

compFile = rootpath + "Comparison.txt"

#Set the default tolerance
tolerance = 0.0005

#Account for any optional command line arguments beyond the first two. Any optional command line arguments should
#  come in pairs, taking the form "-argument Value"
if len(sys.argv) > 3:
   if (len(sys.argv) % 2) != 1:
      ShowUsage()
   else:
      argNum = 3

      while argNum < len(sys.argv):
         argName = sys.argv[argNum]
         argVal = sys.argv[argNum + 1]

         if argName == "-compFile":
            compFile = argVal

         elif argName == "-runpath":
            datapath = argVal + "/"

         elif argName == "-truepath":
            truepath = argVal + "/"

         elif argName == "-tolerance":
            tolerance = float(argVal)

         else:
            print("Invalid Command Line arguments.\n")
            ShowUsage()

         argNum = argNum + 2

if filetype == "All":
   #OCH
   outPath = rootpath + "CSALTComparator_Output_OCH.txt"
   outFile = open(outPath, 'w')
   outFile.truncate()

   #Compare all of the available tests
   if test == "All":
      #Initialize the counts for tests compared and matches found.
      compCount = 0
      matchCount = 0

      if not os.path.isfile(compFile):
         print("Invalid Comparison file: " + compFile)
         outFile.close()
         sys.exit()
   
      with open(compFile) as comp:
         for line in comp:
            if line.strip() == "":
               continue
            if line[0] != "#":
               test = line[:line.index("\t")].strip()
               compCount = compCount + 1

               dataFile = datapath + test + ".och"
               truthFile = truepath + test + ".truth"

               matchFlag = True      # Assuming success!

               try:
                  matchFlag = OCHComparator.ochComparator(test, dataFile, truthFile, outFile, tolerance)
               except Exception as ex:
                  s = str(ex)
                  print("   ", test, " failed: ", s)
                  outFile.write("   Exception in OCH comparison: " + s + "\n\n");
                  matchFlag = False

               if matchFlag:
                  matchCount = matchCount + 1

      print("------------------")
      print("OCH")
      print("------------------")
      print(str(compCount) + " tests compared.")
      print(str(matchCount) + " matches found.")
      print(str(compCount - matchCount) + " nonmatching tests found.")
      print("------------------")

      test = "All"

   #Compare a single specified test
   else:
      matchFlag = True      # Assuming success!

      try:
         matchFlag = OCHComparator.ochComparator(test, datapath + test + ".och", truepath + test + ".truth", outFile, tolerance)
      except Exception as ex:
         s = str(ex)
         print("   ", test, " failed: ", s)
         outFile.write("   Exception in OCH comparison: " + s + "\n\n");
         matchFlag = False

      if matchFlag:
         print("Files Match.")
      else:
         print("Files do not match. See output file.")

   #Close the output file
   outFile.close()

   #SNOPT
   outPath = "./CSALTComparator_Output_SNOPT.txt"
   outFile = open(outPath, 'w')
   outFile.truncate()

   #Initialize the counts for tests compared and matches found.
   compCount = 0
   matchCount = 0

   #Ensure the comparison file exists
   if not os.path.isfile(compFile):
      print("Invalid Comparison file: " + compFile)
      outFile.close()
      sys.exit()
   
   #Compare all tests listed in the comparison file
   if test == "All":
      #Iterate through each test in the comparison file
      with open(compFile) as comp:
         for line in comp:
            if line[0] != "#":
               #Pull out the test name and target value, and generate the name of the
               #file containing that test's output data
               test = line[:line.index("\t")].strip()
               target = line[(line.index("\t") + 1):].strip()
               dataFile = datapath + test + "Data.txt"

               #Run the comparison
               matchFlag = SNOPTComparator.snoptCompare(test, target, dataFile, outFile, tolerance)
               compCount = compCount + 1

               if matchFlag:
                  matchCount = matchCount + 1

      #Print the comparison summary
      print("SNOPT")
      print("------------------")
      print(str(compCount) + " tests compared.")
      print(str(matchCount) + " matches found.")
      print(str(compCount - matchCount) + " nonmatching tests found.")
      print("------------------")

   #Compare a single specified test
   else:
      #Pull the target value from the comparison file, and generate the name of the
      #file containing the test's output data
      with open(compFile) as comp:
         for line in comp:
            if test in line:
               target = line[(line.index("\t") + 1):].strip()
      dataFile = datapath + test + "Data.txt"

      #Run the comparison
      matchFlag = SNOPTComparator.snoptCompare(test, target, dataFile, outFile, tolerance)

      if matchFlag:
         print("Values Match")
      else:
         print("Values Don't Match")
      outFile.write("\n")

   outFile.close()

#Do an OCH Comparison
elif filetype == "OCH":
   outPath = rootpath + "CSALTComparator_Output_OCH.txt"
   outFile = open(outPath, 'w')
   outFile.truncate()

   #Compare all of the available tests
   if test == "All":
      #Initialize the counts for tests compared and matches found.
      compCount = 0
      matchCount = 0

      if not os.path.isfile(compFile):
         print("Invalid Comparison file: " + compFile)
         outFile.close()
         sys.exit()
   
      with open(compFile) as comp:
         for line in comp:
            if line.strip() == "":
               continue
            if line[0] != "#":
               test = line[:line.index("\t")].strip()
               compCount = compCount + 1

               dataFile = datapath + test + ".och"
               truthFile = truepath + test + ".truth"

               matchFlag = True

               try:
                  matchFlag = OCHComparator.ochComparator(test, dataFile, truthFile, outFile, tolerance)
               except Exception as ex:
                  s = str(ex)
                  print("   ", test, " failed: ", s)
                  outFile.write("   Exception in OCH comparison: " + s + "\n\n");
                  matchFlag = False

               if matchFlag:
                  matchCount = matchCount + 1

      print(str(compCount) + " tests compared.")
      print(str(matchCount) + " matches found.")
      print(str(compCount - matchCount) + " nonmatching tests found.")

   #Compare a single specified test
   else:
      matchFlag = True

      try:
         matchFlag = OCHComparator.ochComparator(test, datapath + test + ".och", truepath + test + ".truth", outFile, tolerance)
      except Exception as ex:
         s = str(ex)
         print("   ", test, " failed: ", s)
         outFile.write("   Exception in OCH comparison: " + s + "\n\n");
         matchFlag = False

      if matchFlag:
         print("Files Match.")
      else:
         print("Files do not match. See output file.")

   #Close the output file
   outFile.close()

#Do a SNOPT comparison
elif filetype == "SNOPT":
   outPath = "./CSALTComparator_Output_SNOPT.txt"
   outFile = open(outPath, 'w')
   outFile.truncate()

   #Initialize the counts for tests compared and matches found.
   compCount = 0
   matchCount = 0

   #Ensure the comparison file exists
   if not os.path.isfile(compFile):
      print("Invalid Comparison file: " + compFile)
      outFile.close()
      sys.exit()
   
   #Compare all tests listed in the comparison file
   if test == "All":
      #Iterate through each test in the comparison file
      with open(compFile) as comp:
         for line in comp:
            if line[0] != "#":
               #Pull out the test name and target value, and generate the name of the
               #file containing that test's output data
               test = line[:line.index("\t")].strip()
               target = line[(line.index("\t") + 1):].strip()
               dataFile = datapath + test + "Data.txt"

               #Run the comparison
               matchFlag = SNOPTComparator.snoptCompare(test, target, dataFile, outFile, tolerance)
               compCount = compCount + 1

               if matchFlag:
                  matchCount = matchCount + 1

      #Print the comparison summary
      print(str(compCount) + " tests compared.")
      print(str(matchCount) + " matches found.")
      print(str(compCount - matchCount) + " nonmatching tests found.")

   #Compare a single specified test
   else:
      #Pull the target value from the comparison file, and generate the name of the
      #file containing the test's output data
      with open(compFile) as comp:
         for line in comp:
            if test in line:
               target = line[(line.index("\t") + 1):].strip()
      dataFile = datapath + test + "Data.txt"

      #Run the comparison
      matchFlag = SNOPTComparator.snoptCompare(test, target, dataFile, outFile, tolerance)

      if matchFlag:
         print("Values Match")
      else:
         print("Values Don't Match")
      outFile.write("\n")

   outFile.close()
