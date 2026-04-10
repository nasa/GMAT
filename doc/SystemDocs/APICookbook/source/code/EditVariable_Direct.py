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


# Load the Hohmann transfer script into GMAT.
retval = gmat.LoadScript("../scripts/Ex_HohmannTransfer.script")

if retval == False:
	print("The script failed to load.")
	exit()

# Connect to the GMAT engine and access the MCS
mcs = cf.GetMissionSequence()

# Locate the Target command
solverCmd = cf.FindSolverCommand(mcs)

# Find the 1st Vary command
vary1 = cf.FindChild(solverCmd, "Vary")
# Find the 2nd Vary command
vary2 = cf.FindChild(solverCmd, "Vary", 2)

print("Variable 1:  Initial Value:", vary1.GetNumber("InitialValue"), 
	"Perturbation:", vary1.GetNumber("Perturbation"),
	"Max Step:", vary1.GetNumber("MaxStep"),)
print("Variable 2:  Initial Value:", vary2.GetNumber("InitialValue"), 
	"Perturbation:", vary2.GetNumber("Perturbation"),
	"Max Step:", vary2.GetNumber("MaxStep"))

print("The targeter", solverCmd.GetField("Targeter"), 
	"entered its run with convergence status", 
	solverCmd.GetField("TargeterConverged"))

input("\nPress Enter to continue...\n")

# Run with the scripted settings
gmat.RunScript()

print("The targeter", solverCmd.GetField("Targeter"), 
	"completed its run with convergence status", 
	solverCmd.GetField("TargeterConverged"))

input("\nPress Enter to continue...\n")

val = vary1.GetNumber("CurrentValue")

# Now change the settings
vary1.SetField("InitialValue", val)
vary1.SetField("Perturbation", 0.0005)
vary2.SetField("InitialValue", 1.4)
vary2.SetField("MaxStep", 0.02)

achieve2 = cf.FindChild(solverCmd, "Achieve", 2)
achieve2.SetField("Tolerance", 1e-7)

print("Variable 1:  Initial Value:", vary1.GetNumber("InitialValue"), 
	"Perturbation:", vary1.GetNumber("Perturbation"),
	"Max Step:", vary1.GetNumber("MaxStep"),)
print("Variable 2:  Initial Value:", vary2.GetNumber("InitialValue"), 
	"Perturbation:", vary2.GetNumber("Perturbation"),
	"Max Step:", vary2.GetNumber("MaxStep"))

input("\nPress Enter to continue...\n")

gmat.RunScript()

print("The targeter", solverCmd.GetField("Targeter"), 
	"completed its run with convergence status", 
	solverCmd.GetField("TargeterConverged"))

print("The goal", achieve2.GetField("Goal"), "=", 
	achieve2.GetField("GoalValue"), "has a tolerance of",
	achieve2.GetField("Tolerance"), "and a final value",
	achieve2.GetNumber("AchievedValue"))
