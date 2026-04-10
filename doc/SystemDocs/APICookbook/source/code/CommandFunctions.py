#!/usr/bin/python3

'''
	Functions used to access Mission Control Sequence commands
'''

from load_gmat import gmat

def GetMissionSequence():
	'''
		Access method for the mission control sequence of a loaded script

		returns:
			A reference to the first scripted command of the MCS
	'''

	mod = gmat.Moderator.Instance()
	mcs = mod.GetFirstCommand().GetNext()

	if mcs is None:
		print("Please load a script before accessing the MCS")

	return mcs

def FindSolverCommand(node, number = 1):
	'''
		Function used to find a Solver branch command.

		inputs:
			node The starting node in a control sequence
			number	A counter allowing access to later nodes.  Set to 1 for the 
			        first SolverBranchCommand, 2 for the second, etc.

		returns:
			The requested node, or None if the node was not found
	'''
	counter = 0
	retnode = None

	while node != None:
		if node.IsOfType("SolverBranchCommand"):
			counter = counter + 1
			if counter == number:
				retnode = node
				break
		node = node.GetNext()

	return retnode

def FindCommand(node, type, number = 1):
	'''
		Function used to find a child command of a set type.

		inputs:
			parent  A branch command node in a control sequence
			type 	The type of node requested
			number	A counter allowing access to later nodes.  Set to 1 for the 
			        first instance, 2 for the second, etc.

		returns:
			The requested node, or None if the node was not found
	'''
	counter = 0
	retnode = None

	while node != None:
		if node.IsOfType(type):
			counter = counter + 1
			if counter == number:
				retnode = node
				break
		node = node.GetNext()

	return retnode

def FindCommandByName(node, name):
	'''
		Function used to find a Solver branch command.

		inputs:
			node	The starting node in a control sequence
			name	The name set on the command

		returns:
			The requested node, or None if the node was not found
	'''
	retnode = None

	while node != None:
		if node.GetName() == name:
			retnode = node
			break
		node = node.GetNext()

	return retnode

def FindChild(parent, type, number = 1):
	'''
		Function used to find a child command of a set type.

		inputs:
			parent  A branch command node in a control sequence
			type 	The type of node requested
			number	A counter allowing access to later nodes.  Set to 1 for the 
			        first instance, 2 for the second, etc.

		returns:
			The requested node, or None if the node was not found
	'''
	counter = 0
	retnode = None
	node = parent.GetChildCommand()

	while node != None:
		if node.IsOfType(type):
			counter = counter + 1
			if counter == number:
				retnode = node
				break
		node = node.GetNext()

	return retnode


def FindChildByName(parent, name, number = 1):
	'''
		Function used to find a child command with a specific name.

		inputs:
			parent  A branch command node in a control sequence
			name 	The name of node requested
			number	A counter allowing access to later nodes.  Set to 1 for the 
			        first instance, 2 for the second, etc.

		returns:
			The requested node, or None if the node was not found
	'''
	counter = 0
	retnode = None
	node = parent.GetChildCommand()

	while node != None:
		if node.GetName() == name:
			retnode = node
			break
		node = node.GetNext()

	return retnode
