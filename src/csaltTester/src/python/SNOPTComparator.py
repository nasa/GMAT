import math
import os.path
import sys
import re

def snoptCompare(test, target, dataFile, outFile, tolerance):
	#Error out if the test data file doesn't exist.
	if not os.path.isfile(dataFile):
		print("Invalid Data file: " + dataFile)
		return False

	#Error out if the target value for this test is not in the Comparison file.
	if target == "":
		print("Target for " + test + " not found in Comparison file")
		return False
	
	#Find & pull the objective value from the data file. Error out if it doesn't exist.
	with open(dataFile) as data:
		for line in data:
			if "No. of iterations" in line and "Objective" in line:
				value = line[(line.index("Objective") + 9):].strip()
	
	if value == "":
		print("Value for " + test + " not found in Data file")
		return False

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