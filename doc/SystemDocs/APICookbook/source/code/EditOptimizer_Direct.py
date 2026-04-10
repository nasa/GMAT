#!/usr/bin/python3

'''
	Example showing how to manipulate variables in a solver control sequence 
	using the GMAT API.  

	This example uses the Hohmann transfer sample script supplied with GMAT.  
	Create a folder one level up from the run folder, name it scripts, and copy
	Ex_HohmannTransfer.script from the GMAT samples folder into that folder.
'''

from load_gmat import gmat
import CommandFunctions as cf

# For reporting, set up a local log file
gmat.UseLogFile("./VarExampleLog.txt")
gmat.EchoLogFile()

# Load the lunar optimal fuel transfer script into GMAT.
retval = gmat.LoadScript("../scripts/Ex_MinFuelLunarTransfer.script")

if retval == False:
	print("The script failed to load.")
	exit()

# Connect to the GMAT engine and access the MCS
mcs = cf.GetMissionSequence()

# Locate the Target command
solverCmd = cf.FindCommandByName(mcs, "Optimal Transfer")

solverCmd.SetField("ShowProgressWindow", False)

optName = solverCmd.GetField("OptimizerName")
opt = gmat.GetObject(optName)

minCommand = cf.FindChild(solverCmd, "Minimize")

constraints = []

# Collect the constraint list
index = 1
node = cf.FindChild(solverCmd, "NonlinearConstraint", index)

while node:
	constraints.append(node)
	index = index + 1
	node = cf.FindChild(solverCmd, "NonlinearConstraint", index)

# Run with the scripted settings
gmat.RunScript()

print("\n\nObjective function:  ", minCommand.GetField("ObjectiveName"),
     "=" , minCommand.GetNumber("Cost"), "\n")

if len(constraints) > 0:
	print("Constraints:")
	for i in range(len(constraints)):
		print("   ", constraints[i].GetField("ConstraintArg1"), 
			constraints[i].GetField("Operator"),
      		constraints[i].GetField("ConstraintArg2"), " ==> ", 
      		constraints[i].GetNumber("LHSValue"), 
      		constraints[i].GetField("Operator"),
      		constraints[i].GetNumber("RHSValue"))

input("\nPress Enter to continue...\n")

nlc =  cf.FindChildByName(solverCmd, "SMA = 2300")
if nlc is None:
	print("The command named 'SMA = 2300' was not found")
	exit()
nlc.SetField("RHSValue", 2100.0)

nlc =  cf.FindChildByName(solverCmd, "Inc = 65")
if nlc is None:
	print("The command named 'Inc = 65' was not found")
	exit()
nlc.SetField("RHSValue", 85)

nlc =  cf.FindChildByName(solverCmd, "ECC = 0.01")
if nlc is None:
	print("The command named 'ECC = 0.01' was not found")
	exit()
nlc.SetField("RHSValue", 0.005)

if len(constraints) > 0:
	print("\nConstraints reset:")
	for i in range(len(constraints)):
		print(constraints[i].GetName(), "now set to ", constraints[i].GetNumber("RHSValue")) 

input("\nPress Enter to continue...\n")

# Run with the scripted settings
gmat.RunScript()

print("\n\nObjective function:  ", minCommand.GetField("ObjectiveName"), "=" , minCommand.GetNumber("Cost"), "\n")

if len(constraints) > 0:
	print("Constraints:")
	for i in range(len(constraints)):
		print("   ", constraints[i].GetField("ConstraintArg1"), 
			constraints[i].GetField("Operator"),
      		constraints[i].GetField("ConstraintArg2"), " ==> ", 
      		constraints[i].GetNumber("LHSValue"), 
      		constraints[i].GetField("Operator"), 
      		constraints[i].GetNumber("RHSValue"))
print("\nTa da!\n\n")
