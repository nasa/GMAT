'''
	GMAT: General Mission Analysis Tool
	
	Copyright (c) 2002-2026 United States Government as represented by the
	Administrator of the National Aeronautics and Space Administration.
	All Rights Reserved.
	
	Licensed under the Apache License, Version 2.0 (the "License"); 
	You may not use this file except in compliance with the License. 
	You may obtain a copy of the License at:
	http://www.apache.org/licenses/LICENSE-2.0 
	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
	express or implied.   See the License for the specific language
	governing permissions and limitations under the License.
'''

import math
import os.path
import sys
import difflib
import re

def ochCompare(test, truthFile, dataFile, outFile):
	#Select the file containing test output data and the file containing truth data based on
	#the test being compared.
	#dataFile = "./" + test + ".och"
	#truthFile = "./" + test + ".truth"

	#Error out if either file doesn't exist.
	if not os.path.isfile(dataFile):
		print("Invalid Data file: " + dataFile)
		sys.exit()
	if not os.path.isfile(truthFile):
		print("Invalid Data file: " + truthFile)
		sys.exit()

	#Write the name of the test to the file, and initialize flags.
	#headerFlag: indicates whether or not the Segment header has been written to the file. Only
	#	gets written out for segments that contain discrepancies in the data
	#matchFlag: indicates whether or not everything in the data file matches everything in the
	#	truth file
	outFile.write(test + "\n")
	headerFlag = False
	matchFlag = True

	numSegment = 0
	with open(dataFile, 'r') as f1, open(truthFile, 'r') as f2:
		diff = difflib.ndiff(f1.readlines(), f2.readlines())
				
		#Indicates whether or not the data section has started
		dataflag = False
				
		for line in diff:
			#At a DATA_START line, set the flag so that processing will happen
			if "DATA_START" in line:
				headerFlag = False
				dataflag = True
				numSegment = numSegment + 1

				continue
				
			#If not within a DATA_START - DATA_STOP segment, skip the line
			if not dataflag:
				continue
				
			#At a DATA_STOP line, set the flag so that processing will stop until the next segment
			if "DATA_STOP" in line:
				dataflag = False
				continue
				
			#A line that starts with " " has no discrepancies, and a line that starts with "?"
			#is an indicator of where the discrepancy lies, but we don't care about either. We
			#only need the "-" and "+" lines.
			if line.startswith(" ") or line.startswith('?'):
				continue
				
			if line.startswith('-'):
				dataFileLine = line.rstrip(" \r\n")
				matchFlag = False
			elif line.startswith('+'):
				truthFileLine = line.rstrip(" \r\n")
				
				#Make lists of the non-space content (the numbers)
				dataFileNums = re.findall(r'[^ \t\n]+', dataFileLine)
				truthFileNums = re.findall(r'[^ \t\n]+', truthFileLine)

				#Delete the +/- from the beginning of each list
				if len(dataFileNums) > 0:
					del dataFileNums[0]
				if len(truthFileNums) > 0:
					del truthFileNums[0]
				
				#Calculate the difference in each number in the nonmatching lines
				i = 0
				diffString = "DIFFERENCE:   "
	
				#Calculate the differences between the numbers in the nonmatching lines
				for num1 in dataFileNums:
					num2 = truthFileNums[i]
					diff = float(num1) - float(num2)
					diffString = diffString + str(diff) + "   "
				
					i = i + 1
				
				#Generate the file output
				dataFileLine = "Data:\t" + dataFileLine.lstrip("- \t")
				truthFileLine = "Truth:\t" + truthFileLine.lstrip("+ \t")
				diffString = diffString + "\n"

				if not headerFlag:
					outFile.write("SEGMENT " +str(numSegment) + "\n")
					outFile.write("-----------" + "\n")
					headerFlag = True

				outFile.write(dataFileLine + "\n")
				outFile.write(truthFileLine + "\n")
				outFile.write(diffString + "\n")
				
				dataFileLine = ""
				truthFileLine = ""

	return matchFlag

def snoptCompare(test, target, dataFile, outFile, tolerance):
	#Error out if the test data file doesn't exist.
	if not os.path.isfile(dataFile):
		print("Invalid Data file: " + dataFile)
		sys.exit()

	#Error out if the target value for this test is not in the Comparison file.
	if target == "":
		print("Target for " + test + " not found in Comparison file")
		sys.exit()
	
	#Find & pull the objective value from the data file. Error out if it doesn't exist.
	with open(dataFile) as data:
		for line in data:
			if "No. of iterations" in line and "Objective" in line:
				value = line[(line.index("Objective") + 9):].strip()
	
	if value == "":
		print("Value for " + test + " not found in Data file")
		sys.exit()

	#Set the flag for whether or not this test is a match.
	matchFlag = True
	
	valueFloat = float(value)
	targetFloat = float(target)
	diffString = str(abs(valueFloat - targetFloat))

	#Compare the value and target floats, and determine whether or not they match
	#within the given tolerance.
	if abs(valueFloat - targetFloat) <= tolerance:
		matchString = "Value matches"
	else:
		matchString = "Value doesn't match"
		matchFlag = False
	
	#Write out the file data.
	outFile.write(test + "\n")
	outFile.write("   Test Value: " + value + "\n")
	outFile.write("   Target Value: " + target + "\n")
	outFile.write("   Difference: " + diffString + "\n")
	outFile.write("   " + matchString + "\n")
	outFile.write("\n")

	return matchFlag


#END OF FUNCTION DEFINITIONS
#---------------------------

rootpath = "./"
datapath = rootpath + "run/"
truepath = rootpath + "truth/"

#If there are command line arguments, operate based on those (recommended)
if len(sys.argv) > 1:
	if sys.argv[1] == "OCH":
		test = sys.argv[2]
		compFile = rootpath + "Comparison.txt"

	elif sys.argv[1] == "SNOPT":
		dataDir = datapath
		compFile = rootpath + "Comparison.txt"
		test = sys.argv[2]

	filetype = sys.argv[1]
#If there are no command line arguments, prompt for the file type and test
else:
	filetype = input("Enter the file type (SNOPT, OCH, quit): ")
	test = input("Enter the test to compare, or \"All\": ")

#If an invalid filetype is entered, prompt for a valid one
while filetype != "SNOPT" and filetype != "OCH" and filetype != "quit":
	print("Invalid file type \"" + filetype + "\". Valid options are SNOPT, OCH, or quit.\n")
	filetype = input("Enter the file type (SNOPT, OCH, quit): ")

if filetype == "quit":
	sys.exit()

if filetype == "OCH":
	#Open the output file for OCH comparisons
	outPath = "./DataFileReader_Output_OCH.txt"
	outFile = open(outPath, 'w')
	outFile.truncate()

	dataFile = datapath + test + ".och"
	truthFile = truthpath + test + ".truth"
	
	print("Debug")
	print(datapath)

	#If the test is "All", go through every test listed in the comparison file
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
				if line[0] != "#":
					test = line[:line.index("\t")].strip()
					compCount = compCount + 1

					matchFlag = ochCompare(test, truthFile, dataFile, outFile)

					if matchFlag:
						outFile.write("Files Match\n\n")
						matchCount = matchCount + 1
					outFile.write("\n")

		print(str(compCount) + " tests compared.")
		print(str(matchCount) + " matches found.")
		print(str(compCount - matchCount) + " nonmatching tests found.")

	else:
		matchFlag = ochCompare(test, outFile)

		if matchFlag:
			outFile.write("Files Match\n")
			print("Files Match")
		else:
			print("Files Don't Match")
		outFile.write("\n")

	outFile.close()

		
else:
	#SNOPT
	#The tolerance for discrepancies between the result and the target
	tolerance = 1e-5

	#Open & truncate the SNOPT output file.
	outPath = "./DataFileReader_Output_SNOPT.txt"
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
					dataFile = dataDir + test + "Data.txt"

					#Run the comparison
					matchFlag = snoptCompare(test, target, dataFile, outFile, tolerance)
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
		dataFile = dataDir + test + "Data.txt"

		#Run the comparison
		matchFlag = snoptCompare(test, target, dataFile, outFile, tolerance)

		if matchFlag:
			print("Values Match")
		else:
			print("Values Don't Match")
		outFile.write("\n")

	outFile.close()