Raise Apogee script to demonstrate Finite Burn being shared between MONTE and GMAT 

Explanation of script
1) In Monte, set up a spacecraft and populate initial fields
2) Save it to a spice 
	cristo.convert( boa, EphemName1 )
3) Read that spice into GMAT 
	sat.SetField("OrbitSpiceKernelName", EphemName1Full)
4) Run a finite burn propagation 
	gmat.RunScript()
5) Save result to a new spice
	ephem.SetField("Filename", EphemName2)
6) Load the new spice into Monte 
	boa.load( EphemName2 )
7) Display the results
	ShowTrajectory(boa, theSats, beginTime, maneuverTime, True)
