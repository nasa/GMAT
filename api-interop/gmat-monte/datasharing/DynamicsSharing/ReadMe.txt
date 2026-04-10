Monte to Gmat to Monte Dynamic Sharing example. 
Written by Alex Campbell (alex.campbell@nasa.gov) July 2022, 

This demo is a proof of concept showing what can be done when it comes to sharing dynamics and using the new external model feature in GMAT. There may be mistakes in this such as calculations being off or using the wrong fields. This example is really only focused on the big picture. 

For setup make sure to edit load_gmat.py so that GmatInstall on line 12 correctly points to the main folder of your gmat build. Refer to  API_README.txt in the api folder for more details.

To run call "mpython BasicStart.mpy" 
This will print out the velocity and accelration calculated from monte at each interation to the consel. As well as great a report file called SimpleReport that has the position and velocity from gmat at each propagation.

Explanation of included files 
BasicStart.mpy
	This is the start point. All it does here is set a script in gmat and then runs that script. When trying to share Dynamics between gmat and monte, it has to start with a monte file, that calls gmat, and gmat then calls another monte file. For a better example of starting with a monte script that then calls gmat look at the other gmat to monte sharing examples located in the api-interop folder. 

SimpleGmatScript.script
	A Gmat Geosynchronous Orbit script. 
	MyForce.External defines what python script to get dynamics data from
	MyForce.External.ExcludeOtherForces when true causes gmat to ignore all other forces it would normally calcuate and just use the data from the python script
	MyForce.External.EntryPoint sets the entry point for the python script. defaults to getForces().

getExternalMonteForces.py
	The monte script that Gmat is calling to get dynamics data.
	It will be given 3 varibles:
		- state vector(x,y,z, vx, vy, vz) km, km/sec
		- The current time interval from epoch 05-JAN-1941 12:00:00 TAI
		- order of derivavte
	and is expected to return a 6 element derivatives vector

SimpleExternalForceModel.py
	Example of a non-monte python script that can also be used to generate dynamics data. 


