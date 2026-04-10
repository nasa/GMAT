-----------------------------------------------------------------
Prerequisites
-----------------------------------------------------------------
MATLAB.  Only tested on R2015b, but will probably work on 
earlier versions but be slower as R2015b has JIT compilation
that dramatically improves performance of OO MATLAB code.

-----------------------------------------------------------------
Usage
-----------------------------------------------------------------
Configuration:
    1. Edit the script named SystemConfigurationScript.m.  Modify
	the variable named "installDir" to point to the location of this
	README file.  Then, run the script SystemConfigurationScript.m

Execution:
    1. Run TATC_APITestDriver_Minimal.m (located in the test 
    folder) to see a basic API example and run a simple test case
	2. Run TATC_APITestDriver_Analysis.m (located in the test 
    folder)to run a test problem and process the O-C data output
	to determine coverage statistics

