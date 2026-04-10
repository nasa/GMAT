@echo off

cd C:/GMAT/gmat-main/build/BuildSystem/windows-32

rem configuration
set server=ndc-relay.ndc.nasa.gov
set fromaddress=peter.j.candell@nasa.gov
set toaddress=peter.j.candell@nasa.gov

blat start.txt -to %toaddress% -f %fromaddress% -s "GMAT Build Started "%time% -server %server% -timestamp -debug -log blat.log
call GmatBuild.cmd %1 > results.txt
blat results.txt -to %toaddress% -f %fromaddress% -s "GMAT Build "%time% -server %server% -timestamp -debug -log blat.log
