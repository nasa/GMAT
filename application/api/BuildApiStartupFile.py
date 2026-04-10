#-------------------------------------------------------------------------------
# API Startup File Editor
#-------------------------------------------------------------------------------
# Replaces relative paths in the GMAT startup file with absolute paths for the
# API startup file, allowing the API to berun from any folder. This script is
# placed in the application/api directory of the GMAT installation folder. It
# generates two startup files: 
# - api_startup_file.txt, generated from the startup file in the bin directory
# - api_startup_file_d.txt, generated from the startup file in the debug directory
#
# To use:
#    python3 BuildApiStartupFile.py
#===============================================================================
from os.path import dirname, abspath, basename, isdir

gmatdir = dirname(dirname(abspath(__file__))) 	#GMAT directory

#Generate the BIN startup file
binfilename = gmatdir + "/bin/gmat_startup_file.txt"
binfileout = open(gmatdir + "/bin/api_startup_file.txt", "w")

replacePattern = ".."
replaceVal = gmatdir

with open(binfilename) as binfile:
	line = binfile.readline()
	while line:
		outline = line.replace(replacePattern, replaceVal)
		binfileout.write(outline)

		line = binfile.readline()

binfileout.close()

# If a debug build exsits, generate the DEBUG startup file
if isdir(gmatdir + "/debug") :
	debugfilename = gmatdir + "/debug/gmat_startup_file.txt"
	debugfileout = open("api_startup_file_d.txt", "w")

	replacePattern = "./"
	replaceVal = gmatdir + "/"

	with open(debugfilename) as debugfile:
		line = debugfile.readline()
		while line:
			outline = line.replace(replacePattern, replaceVal)
			debugfileout.write(outline)

			line = debugfile.readline()

	debugfileout.close()
	