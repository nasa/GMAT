Monte To GMAT covariance script to demonstrate a covariance array being shared between MONTE and GMAT 

Explanation of script
1) In Monte, set up a spacecraft and populate initial fields
2) Save the covariance array 
	CovArray = sol.cov().toArray()
3) Copy that array to an rmatrix for gmat
	rmat[r,c] = CovArray[r,c]
4) Add the rmatrix to gmat 
	sat.SetRmatrixParameter("OrbitErrorCovariance",rmat )
5) Run the gmat script 
	gmat.RunScript()
6) Save the resulting covariance array and convert it to monte format
	c2 = M.Covariance( UnitsType, gmatLowerLeft, Covariance.LOWER_LEFT )
7) Add the array to Monte
	config2.insert( Params, [0,0,0,0,0,0], c2 )
