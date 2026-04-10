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

filetype = input("Enter the file type (SNOPT, OCH, quit): ")

while filetype != "SNOPT" and filetype != "OCH" and filetype != "quit":
	print("Invalid file type \"" + filetype + "\". Valid options are SNOPT, OCH, or quit.\n")
	filetype = input("Enter the file type (SNOPT, OCH, quit): ")

if filetype == "quit":
	sys.exit()

if filetype == "OCH":
	#file1 = input("Input 1st OCH File: ")
	#file2 = input("Input 2nd OCH File: ")
	file1 = "./Hypersensitive_Radau.och"
	file2 = "./Hypersensitive_Radau_2.och"

	if not os.path.isfile(file1):
		print("Invalid Data file: " + file1)
		sys.exit()
	if not os.path.isfile(file2):
		print("Invalid Data file: " + file2)
		sys.exit()

	numSegment = 1
	with open(file1, 'r') as f1, open(file2, 'r') as f2:
		diff = difflib.ndiff(f1.readlines(), f2.readlines())

		#Indicates whether or not the data section has started
		dataflag = False

		for line in diff:
			#At a DATA_START line, set the flag so that processing will happen
			if "DATA_START" in line:
				print("SEGMENT " +str(numSegment))
				print("-----------")
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
				file1line = line
			elif line.startswith('+'):
				file2line = line

				#Make lists of the non-space content (the numbers)
				file1nums = re.findall(r'[^ \t\n]+', file1line)
				file2nums = re.findall(r'[^ \t\n]+', file2line)
				#Delete the +/- from the beginning of each list
				del file1nums[0]
				del file2nums[0]

				#Calculate the difference in each number in the nonmatching lines
				i = 0
				diffString = "DIFFERENCE:   "

				for num1 in file1nums:
					num2 = file2nums[i]
					diff = float(num1) - float(num2)
					diffString = diffString + str(diff) + "   "

					i = i + 1

				diffString = diffString + "\n\n"

				sys.stdout.write(file1line)
				sys.stdout.write(file2line)
				sys.stdout.write(diffString)

				file1line = ""
				file2line = ""
else:
	#SNOPT

	#The directory that data output files are stored in
	dataDir = "/home/crc/lowthrusttsi/CPlusPlus/test/bin/"
	#The path to the comparison file
	compFile = "/home/crc/lowthrusttsi/CPlusPlus/test/TestOptCtrl/Comparison.txt"
	#The tolerance for discrepancies between the result and the target
	tolerance = 1e-5

	#The test data being compared
	test = input("Enter the test to compare, or \"All\": ")
	
	if test == "All":
		if not os.path.isfile(compFile):
			print("Invalid Comparison file: " + compFile)
			sys.exit()
	
		with open(compFile) as comp:
			for line in comp:
				if line[0] != "#":
					test = line[:line.index("\t")].strip()
					target = line[(line.index("\t") + 1):].strip()
					dataFile = dataDir + test + "Data.txt"
	
					if not os.path.isfile(dataFile):
						print("Invalid Data file: " + dataFile)
						sys.exit()
	
					if target == "":
						print("Target for " + test + " not found in Comparison file")
						sys.exit()
	
					with open(dataFile) as data:
						for line in data:
							if "No. of iterations" in line and "Objective" in line:
								value = line[(line.index("Objective") + 9):].strip()
	
					if value == "":
						print("Value for " + test + " not found in Data file")
						sys.exit()
	
					valueFloat = float(value)
					targetFloat = float(target)
					diffString = str(abs(valueFloat - targetFloat))
					if abs(valueFloat - targetFloat) <= tolerance:
						matchString = "Value matches"
					else:
						matchString = "Value doesn't match"
	
					print
					print(test)
					print("   Test Value: " + value)
					print("   Target Value: " + target)
					print("   Difference: " + diffString)
					print("   " + matchString)
	else:
		dataFile = dataDir + test + "Data.txt"
		
		if not os.path.isfile(dataFile):
			print("Invalid Data file: " + dataFile)
			sys.exit()
		if not os.path.isfile(compFile):
			print("Invalid Comparison file: " + compFile)
			sys.exit()
		
		value = ""
		target = ""
		
		with open(dataFile) as data:
			for line in data:
				if "No. of iterations" in line and "Objective" in line:
					value = line[(line.index("Objective") + 9):].strip()

		with open(compFile) as comp:
			for line in comp:
				if test in line:
					target = line[(line.index("\t") + 1):].strip()
		
		if value == "":
			print("Value for " + test + " not found in Data file")
			sys.exit()
		if target == "":
			print("Target for " + test + " not found in Comparison file")
			sys.exit()
		
		valueFloat = float(value)
		targetFloat = float(target)
		diffString = str(abs(valueFloat - targetFloat))
		#math.isclose(valueFloat, targetFloat, rel_tol=1e-5)
		print()
		print("Test Value: " + value)
		print("Target Value: " + target)
		print("Difference: " + diffString)
		if abs(valueFloat - targetFloat) <= tolerance:
			print("Value matches")
		else:
			print("Value doesn't match")