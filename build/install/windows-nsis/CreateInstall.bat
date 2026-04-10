#	Using a mingw shell window, Run from the windows-nsis directory as shown below
#	cd "C:\Repos\gmat\build\install\windows-nsis"
#   rm outfile     #delete old log file
#	CreateInstall.bat 2>&1 | tee outfile

#	Clean up directory as needed
make clean


make assemble VERSION="R2022a"
echo   
echo Assemble complete
echo  


#	Use sejda-console to amend the PDF files (e.g., add a cover sheet)

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-A4-Trimmed.pdf gmat-internal/GMAT/docs/help/help-a4.pdf -o gmat-internal/GMAT/docs/help/help-a4-new.pdf -s all:all:
mv gmat-internal/GMAT/docs/help/help-a4-new.pdf gmat-internal/GMAT/docs/help/help-a4.pdf

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-Letter-Trimmed.pdf gmat-internal/GMAT/docs/help/help-letter.pdf -o gmat-internal/GMAT/docs/help/help-letter-new.pdf -s all:all:
mv gmat-internal/GMAT/docs/help/help-letter-new.pdf gmat-internal/GMAT/docs/help/help-letter.pdf

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-A4-Trimmed.pdf gmat-public/GMAT/docs/help/help-a4.pdf -o gmat-public/GMAT/docs/help/help-a4-new.pdf -s all:all:
mv gmat-public/GMAT/docs/help/help-a4-new.pdf gmat-public/GMAT/docs/help/help-a4.pdf

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-Letter-Trimmed.pdf gmat-public/GMAT/docs/help/help-letter.pdf -o gmat-public/GMAT/docs/help/help-letter-new.pdf -s all:all:
mv gmat-public/GMAT/docs/help/help-letter-new.pdf gmat-public/GMAT/docs/help/help-letter.pdf
 
 
echo   
echo sejda-console PDF merging complete
echo 
 
 
#	Run 'make VERSION="R2022a-rc#"', where "#" is the number of the RC you're creating. 
#	This will create four packages in the current directory: A .zip and a .exe file for both 
#	the internal and public versions. 
#	Note: To create only an internal version, run 'make internal VERSION="R2022a-rc#"'.

#  Remove propietary plug-ins and folders from Public Version
rm gmat-public/GMAT/bin/snopt7.dll
rm gmat-public/GMAT/bin/snopt7_cpp.dll
rm -r gmat-public/GMAT/data/atmosphere/Msise86_Data

make VERSION="R2022a-rc3"

echo   
echo Creation of Install packages complete.  Check for errors in log file.  
echo

#	Copy the four package files to the network: 
#			\\mesa-file\595\GMAT\Builds\windows\VS2017_build_64\R2022a\RC# where "#" is number of the RC.  	  


#	To clean everything up afterwards, run "make clean".

#	Note: To make the final release bundles, you can't just rename the files to take off the "-rc#" portion. 
#		  You need to recreate the bundles using this command: make VERSION="R2022a" 
