import math
import os.path
import sys
import re
import LagrangeInterpolator as li

def ochCompare(data1, data2, tolerance):
	#Data1 and Data2 are both 2d lists. Each Tuple represents a point in a segment, and should contain the same number of floats.
	#returnList will be a list of tuples, each containing 3 items: a list from Data1, a list from Data2, and a list of differences.
	data1Index = 0
	data2Index = 0
	returnList = []

	continueFlag = True
	dataMatch = True
	#Catch if everything in the segment is a mismatch.
	allWrongFlag = True

	#Iterate through the lists of lists until one list runs out of values.
	while continueFlag:
		data1Item = data1[data1Index]
		data2Item = data2[data2Index]
		diffList = []

		itemMatch = True

		#If the number of floats in each list do not match, or if either is blank, there is something wrong with the input data.
		if len(data1Item) != len(data2Item):
			print("Error: Lines do not have a matching number of values.")
			print("Line 1: " + str(data1Index + 1) + "   Line2: " + str(data2Index + 1))
			print("DEBUG   Data1: " + str(data1Item) + "   Data2: " + str(data2Item))
			return [False, "Value Count Error"]

		if len(data1Item) == 0 or len(data2Item) == 0:
			print("Error: One or both items are blank.")
			print("Line 1: " + str(data1Index + 1) + "   Line2: " + str(data2Index + 1))
			return [False, "Blank Value Error"]

		#The first float in each list represents time. Tuples will be matched together based on time.
		data1Time = data1Item[0]
		data2Time = data2Item[0]

		diffList.append(data1Time - data2Time)


		#If the times are within the tolerance's difference from each other, line up the two tuples, and compare their values. If any
		#	differences are found, add them to the difference list.
		if abs(data1Time - data2Time) <= tolerance:
			i = 1

			while i < len(data1Item):
				data1Val = data1Item[i]
				data2Val = data2Item[i]
				valDiff = data1Val - data2Val
				diffList.append(valDiff)

				if abs(valDiff) > tolerance:
					dataMatch = False
					itemMatch = False
				i += 1

			#We only want to return the lines and difference lists for tuples with matching times, but nonmatching values.
			if not itemMatch:
				returnList.append([data1Item, data2Item, diffList, data1Index + 1, data2Index + 1])
			#Iterate to the next item of both lists.
			data1Index += 1
			data2Index += 1

		#If the times do not match, then count it as a line missing from either the first or the second set of data.
		elif data1Time < data2Time:
			returnItem = [data1Item, [], [], data1Index + 1, -1]
			returnList.append(returnItem)
			dataMatch = False
			itemMatch = False
			data1Index += 1

		elif data1Time > data2Time:
			returnItem = [[], data2Item, [], -1, data2Index + 1]
			returnList.append(returnItem)
			dataMatch = False
			itemMatch = False
			data2Index += 1

		#If a line matches, set allWrongFlag to False.
		if itemMatch and allWrongFlag and (data1Time != 0):
			allWrongFlag = False

		#When one list runs out, end the loop.
		if data1Index == len(data1) or data2Index == len(data2):
			continueFlag = False

	#If either index is less than the total length of that data set, then there were lines at the end missing from the other data set.
	if data1Index < len(data1):
		while data1Index < len(data1):
			returnItem = [data1[data1Index], [], [], data1Index + 1, -1]
			returnList.append(returnItem)
			dataMatch = False
			data1Index += 1

	if data2Index < len(data2):
		while data2Index < len(data2):
			returnItem = [[], data2[data2Index], [], -1, data2Index + 1]
			returnList.append(returnItem)
			dataMatch = False
			data2Index += 1	

	#If allWrongFlag is true, then no matching items were found. Instead of returning the full list, simply return a string, as
	#	with the errors.
	if allWrongFlag:
		return [dataMatch, "      No values matched in this segment."]

	#Return True or False for whether or not the overall comparison matched, as well as any difference information.
	return [dataMatch, returnList]

def ochCompareInterpolator(data1, data2, tolerance):
	interpPoints = []
	#dataValues = []
	funcValues = []
	indVar = []

	for i in data1:
		#dataValues.append(i[1:])
		interpPoints.append(i[0])

	for i in data2:
		funcValues.append(i[1:])
		indVar.append(i[0])

	interpData = li.InterpolateArray(interpPoints, funcValues, indVar)

	#print(type(interpData))
	#interpData = numpy.tolist()

	i = 0
	while i < len(interpData):
		interpData[i] = [interpPoints[i]] + interpData[i]
		i += 1

	compareResult = ochCompare(data1, interpData, tolerance)
	return compareResult



def ochRead(dataFile, truthFile):
	#dataFile is the path to a file containing OCH data output from a test.
	#truthFile is the path to a file containing OCH truth data for a test.
	with open(dataFile, 'r') as f1, open(truthFile, 'r') as f2:
		dataList = f1.readlines()
		truthList = f2.readlines()

	dataSegments = []
	truthSegments = []
	segment = []
	dataFlag = False

	#Iterate through each line in the data file.
	for line in dataList:
		#If the line is "DATA_START", it is the beginning of a segment. Turn on the data flag.
		if "DATA_START" in line:
			dataFlag = True
			segment = []
			continue

		#If the line is "DATA_STOP", it is the end of a segment. Turn off the data flag and add the full segment list to dataSegments.
		if "DATA_STOP" in line:
			dataFlag = False
			dataSegments.append(segment)
			continue

		#If the line is between a DATA_START and a DATA_STOP, convert it into a list of floats and add it to the segment list.
		if dataFlag:
			line = line.rstrip(" \r\n")
			lineVals = re.findall(r'[^ \t\n]+', line)
			lineNums = [float(i) for i in lineVals]

			if len(lineNums) != 0:
				segment.append(lineNums)

	segment = []
	dataFlag = False

	#Iterate through each line in the truth file.
	for line in truthList:
		#If the line is "DATA_START", it is the beginning of a segment. Turn on the data flag.
		if "DATA_START" in line:
			dataFlag = True
			segment = []
			continue

		#If the line is "DATA_STOP", it is the end of a segment. Turn off the data flag and add the full segment list to dataSegments.
		if "DATA_STOP" in line:
			dataFlag = False
			truthSegments.append(segment)
			continue

		#If the line is between a DATA_START and a DATA_STOP, convert it into a list of floats and add it to the segment list.
		if dataFlag:
			line = line.rstrip(" \r\n")
			lineVals = re.findall(r'[^ \t\n]+', line)
			lineNums = [float(i) for i in lineVals]

			if len(lineNums) != 0:
				segment.append(lineNums)

	#Return the lists of data segments and truth segments.
	return [dataSegments, truthSegments]

def ochComparator(test, dataFile, truthFile, outFile, tolerance):
	#Read the OCH data and truth files into lists of lists of tuples (list = line of data, inner list = data segment, outer list = file)
	dataSegments, truthSegments = ochRead(dataFile, truthFile)

	#Write out this test's header
	outFile.write(test + "\n")
	
	segmentNum = 0
	testMatch = True

	#Iterate through each data segment
	while segmentNum < len(dataSegments):
		#Compare matching numbered segments from the data file and the truth file
		matchFlag, compareList = ochCompareInterpolator(dataSegments[segmentNum], truthSegments[segmentNum], tolerance)

		#Check for errors or entire match failures
		if isinstance(compareList, str):
			outFile.write("   SEGMENT " + str(segmentNum + 1) + "\n")
			outFile.write(compareList + "\n")
			testMatch = False
			segmentNum += 1
			continue
		
		#If the segments match, nothing else needs to happen. If they don't match, the segment difference information needs to be written out.
		if not matchFlag:
			testMatch = False
			missingDataLines = []
			missingTruthLines = []
			outFile.write("   SEGMENT " + str(segmentNum + 1) + "\n")
	
			for [dataLine, truthLine, diffList, dataLineIndex, truthLineIndex] in compareList:
				#Format the tuples into one-line strings, with the same amount of space padding for each value (currently 27 characters)
				#If one of the tuples is empty, it indicates a missing line from either the data file or truth file, depending on which is missing.
				if len(dataLine) > 0:
					dataLine = ''.join(["{:<27}".format(str(i)) for i in dataLine])
				else:
					dataLine = ""
					missingDataLines.append(truthLineIndex)
		
				if len(truthLine) > 0:
					truthLine = ''.join(["{:<27}".format(str(i)) for i in truthLine])
				else:
					truthLine = ""
					missingTruthLines.append(dataLineIndex)
		
				if len(diffList) > 0:
					diffList = ''.join(["{:<27}".format(str(i)) for i in diffList])
				else:
					diffList = ""
				
				#Output each line, and the differences
				if len(dataLine) > 0 and len(truthLine) > 0:
					outFile.write("      DATA LINE " + str(dataLineIndex) + ":\t" + dataLine + "\n")
					outFile.write("      TRUTH LINE " + str(truthLineIndex) + ":\t" + truthLine + "\n")
					outFile.write("      DIFF:\t" + diffList + "\n\n")
	
			#Output the missing lines
			if len(missingDataLines) > 0:
				outFile.write("   The following Truth lines are missing from the Data file: " + ", ".join(str(i) for i in missingDataLines) + "\n")
	
			if len(missingTruthLines) > 0:
				outFile.write("   The following Data lines are missing from the Truth file: " + ", ".join(str(i) for i in missingTruthLines) + "\n")
	
		segmentNum += 1

	#If all the segments in this test match and no difference data needed to be written, write out "Files Match."
	if testMatch:
		outFile.write("   Files Match.\n")
	outFile.write("\n")
	return testMatch


#rootpath = "./"
#datapath = rootpath + "run/"
#truepath = rootpath + "truth/"
#
##If there are command line arguments, operate based on those (recommended)
#if sys.argv[1] == "OCH":
#	test = sys.argv[2]
#	compFile = rootpath + "Comparison.txt"
#
#elif sys.argv[1] == "SNOPT":
#	test = sys.argv[2]
#	compFile = rootpath + "Comparison.txt"
#	dataDir = datapath
#
#else:
#	print("Invalid Command Line Arguments. First argument must be a valid run type (OCH or SNOPT).")
#	sys.exit()
#
#filetype = sys.argv[1]
#
#tolerance = 0.0001
#
#if len(sys.argv) > 3:
#	if (len(sys.argv) % 2) != 1:
#		print("Invalid number of Command Line arguments.\n")
#		sys.exit()
#	else:
#		argNum = 3
#
#		while argNum < len(sys.argv):
#			argName = sys.argv[argNum]
#			argVal = sys.argv[argNum + 1]
#
#			if argName == "-compFile":
#				compFile = argVal
#
#			elif argName == "-runpath":
#				datapath = argVal
#
#			elif argName == "-truepath":
#				truepath = argVal
#
#			elif argName == "-tolerance":
#				tolerance = float(argVal)
#
#			else:
#				print("Invalid Command Line arguments.\n")
#				sys.exit()
#
#			argNum = argNum + 2
#
#if filetype == "OCH":
#	outPath = rootpath + "OCHComparator_Output.txt"
#	outFile = open(outPath, 'w')
#	outFile.truncate()
#
#	if test == "All":
#		#Initialize the counts for tests compared and matches found.
#		compCount = 0
#		matchCount = 0
#
#		if not os.path.isfile(compFile):
#			print("Invalid Comparison file: " + compFile)
#			outFile.close()
#			sys.exit()
#	
#		with open(compFile) as comp:
#			for line in comp:
#				if line.strip() == "":
#					continue
#				if line[0] != "#":
#					test = line[:line.index("\t")].strip()
#					compCount = compCount + 1
#
#					dataFile = datapath + test + ".och"
#					truthFile = truepath + test + ".truth"
#
#					matchFlag = ochComparator(test, dataFile, truthFile, outFile, tolerance)
#
#					if matchFlag:
#						matchCount = matchCount + 1
#
#		print(str(compCount) + " tests compared.")
#		print(str(matchCount) + " matches found.")
#		print(str(compCount - matchCount) + " nonmatching tests found.")
#
#	else:
#		OCHComparator(test, datapath + test + ".och", truepath + test + ".truth", outFile, tolerance)
#
#	
#	outFile.close()