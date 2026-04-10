#ifndef _SGP4DC_
#define _SGP4DC_
/* -----------------------------------------------------------------------------
*
*                               sgp4DC.h
*
*    this file contains the differential corrections routines using the sgp4
*    analytical propagation code. a detailed discussion of the theory and history
*    may be found in the 2008 aiaa paper by vallado and crawford.
*
*                            companion code for
*               fundamentals of astrodynamics and applications
*                                    2013
*                              by david vallado
*
*       (w) 719-573-2600, email dvallado@agi.com
*
*    current :
*              30 sep 15  david vallado
*                           fix jd, jdfrac
*    changes :
*               3 nov 14  david vallado
*                           update to msvs2013 c++
*               5 nov 14 alek lidtke
*                          changed the size input obsrecarr arrays to leastsquares and findatwaatwb to 1000 to avoid stack overflow when passing arguments by value to these functions.
*                          added #define WGS72MU constant and added it to ast2Body::rv2coe function call in state2satrec
*               3 nov 14  david vallado
*                           update to msvs 2013
*               8 aug 14 alek lidtke
*                          change the constructor of std::vector< std::vector<double> > nut (lines 206 and 377), also other vectors in lines 802, 1267, and 2121
*                          started using fopen_s in line 1345, fscanf_s in line 1356 with MSVS compiler
*               6 aug 08  david vallado
*                           add operationmode for afspc (a) or improved (i)
*              18 jun 08  david vallado
*                           original version
* --------------------------------------------------------------------------- */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <assert.h>

#include "D:/Codes/LIBRARY/CPP/Libraries/astMath/astMath/astMath.h"  // pi, infinite, undefined
#include "D:/Codes/LIBRARY/CPP/Libraries/astTime/astTime/astTime.h"  // pi, twopi, edirection
#include "D:/Codes/LIBRARY/CPP/Libraries/ast2Body/ast2Body/ast2Body.h"
#include "D:/Codes/LIBRARY/CPP/Libraries/astIOD/astIOD/astIOD.h"                                                       
#include "D:/Codes/LIBRARY/CPP/Libraries/astPert/astPert/astPert.h"
#include "D:/Codes/LIBRARY/CPP/Libraries/coordFK5/coordFK5/coordFK5.h"  // eOpt, iau80data
#include "D:/Codes/LIBRARY/CPP/Libraries/eopspw/eopspw/eopspw.h"  // eopdata, spwdata, eopsize, spwsize
#include "D:/Codes/LIBRARY/CPP/Libraries/SGP4/SGP4/SGP4.h"  // gravconsttype, elsetrec, SGP4Version

// note that although a version is given here, a baseline version will be output
// when the obs case difficulties are resolved.
#define SGP4DCVersion      "SGP4DC Version 2014-11-03"

#define WGS72MU 3.986008e5
//#define statesize 6   // 7 if solve for bstar, else 6


// Individual obs types are as follows:
//                     0 Rng
//                     1 Az, El
//                     2 Rng, Az, El
//                     3 TRtAsc, TDecl
//                     4 x y z xdot ydot zdot bstar

typedef struct obsrec
{
	int    sennum;
	long   satnum;
	int    year, mon, day, hr, min;
	double jd, jdf, sec, dtmin, lst;
	int    error;
	char   init, method;
	double rsecef[3], vsecef[3];
	int    obstype;
	double x, y, z, xdot, ydot, zdot, bstar,
		rng, az, el, drng, daz, del,
		rtasc, decl, trtasc, tdecl;
} obsrec;

typedef struct senrec
{
	int sennum;
	char senname[10];
	double senlat, senlon, senalt,
		rngmin, rngmax, azmin, azmax, elmin, elmax,
		biasrng, biasaz, biasel, biasdrng, biasdaz, biasdel,
		biastrtasc, biastdecl,
		noisex, noisey, noisez, noisexdot, noiseydot, noisezdot, noisebstar,
		noiserng, noiseaz, noiseel, noisedrng, noisedaz, noisedel,
		noisetrtasc, noisetdecl;
} senrec;


#pragma once


namespace SGP4DC 
{

	void dsvbksb
		(
		std::vector< std::vector<double> > u,
		std::vector< std::vector<double> > w,
		std::vector< std::vector<double> > v,
		int m, int n,
		std::vector< std::vector<double> > b,
		std::vector< std::vector<double> > &dx
		);


	void printtle(elsetrec);

	int dsvdcmp
		(
		std::vector< std::vector<double> > &a, int m, int n,
		std::vector< std::vector<double> > &w,
		std::vector< std::vector<double> > &v
		);
	//      std::vector< std::vector<double> > &u,

	void getsensorparams
		(
		int sennum,
		senrec& currsenrec
		);

	void state2satrec
		(
		std::vector<double> &xnom,
		char statetype, int statesize,
		edirection direct,
		elsetrec& satrec
		);

	void findatwaatwb
		(
		int firstob, int lastob, int statesize,
		double percentchg, double deltaamtchg, gravconsttype whichconst, char interp, double jdeopstart,
		elsetrec satrec,
		const std::vector<obsrec> &obsrecarr,
		char statetype,
		char proptype,
		std::vector<double> &xnom,
		int derivType, double cdam, double cram,
		char opt,
		const std::vector<jpldedata> &jpldearr,
		double jdjpldestart,
		iau80data iau80rec,
		const std::vector<eopdata> &eoparr,
		double& drng2, double& daz2, double& del2,
		double& ddrng2, double& ddaz2, double& ddel2,
		double& dtrtasc2, double& dtdecl2,
		double& dx2, double& dy2, double& dz2,
		double& dxdot2, double& dydot2, double& dzdot2,
		std::vector< std::vector<double> > &atwa,
		std::vector< std::vector<double> > &atwb,
		std::vector< std::vector<double> > &atw,
		std::vector< std::vector<double> > &b
		);

	void finitediff
		(
		gravconsttype whichconst,
		int pertelem, double percentchg, double deltaamtchg, char statetype, int statesize, char proptype,
		elsetrec& satrec,
		std::vector<double> xnom,
		std::vector< std::vector<double> > &xpert,
		double &deltaamt
		);

	void leastsquares
		(
		double percentchg, double deltaamtchg, double epsilon, gravconsttype whichconst,
		char interp, double jdeopstart,
		elsetrec &satrec, elsetrec satrecx,
		char typeans, char statetype, char proptype,
		int firstob, int lastob, int statesize,
		const std::vector<obsrec> &obsrecarr, int &loops,
		std::vector<double> &xnom,
		int derivType, double cdam, double cram, 
		char opt,
		const std::vector<jpldedata> &jpldearr,
		double jdjpldestart,
		iau80data iau80rec,
		const std::vector<eopdata> &eoparr, 
		std::vector< std::vector<double> > &x,
		std::vector< std::vector<double> > &dx,
		std::vector< std::vector<double> > &atwai,
		std::vector< std::vector<double> > &atwa,
		std::vector< std::vector<double> > &atwb,
		FILE *outfile, FILE *outfile1
		);

}  // namespace


#endif




