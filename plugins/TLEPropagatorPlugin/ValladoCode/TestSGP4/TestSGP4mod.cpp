/* ---------------------------------------------------------------------
*
*                              testSGP4.cpp
*
*  this program tests the sgp4 propagator. an stk ephemeris file is generated
*  along with the test output. the code for this is left justified for easy
*  location.
*
*                          companion code for
*             fundamentals of astrodynamics and applications
*                                  2013
*                            by david vallado
*
*     (w) 719-573-2600, email dvallado@agi.com, davallado@gmail.com
*     *****************************************************************
*    current :
*               7 dec 15  david vallado
*                           fix jd, jdfrac
*    changes :
*               3 nov 14  david vallado
*                           update to msvs2013 c++
*              11 nov 13  david vallado
*                           conversion to msvs c++
*                           misc fixes to constants and options
*                           add singly averaged state elements to be exported
*               3 sep 08  david vallado
*                           add switch for afspc compatibility and improved operation
*              14 may 08  david vallado
*                           fixes for linux suggested by brian micek
*                           misc fixes noted by the community - manual operation,
*                           formats, char lengths
*              14 aug 06  david vallado
*                           update mfe for verification time steps, constants
*              20 jul 05  david vallado
*                           fixes for paper, corrections from paul crawford
*               7 jul 04  david vallado
*                           fix record file and get working
*              14 may 01  david vallado
*                           2nd edition baseline
*                     80  norad
*                           original baseline
*       ----------------------------------------------------------------      */


#include "stdafx.h"
#include "astTime.h"
#include "coordFK5.h"
#include "ast2Body.h"
#include "astMath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <time.h>

extern char help;
extern FILE *dbgfile;

#include "SGP4.h"

//using namespace System;
//using namespace SGP4Funcs;

#define pi 3.14159265358979323846

char typerun, typeinput, opsmode;
gravconsttype  whichconst;
int whichcon;


int _tmain(int argc, _TCHAR* argv[])
//int main(array<System::String ^> ^args)
//int main()
{

	char str[2];
	char infilename[75];
	double ro[3];
	double vo[3];
	FILE *infile, *outfile, *outfilee, *outoutfile; 
	errno_t err;

	// ----------------------------  locals  -------------------------------
	double p, a, ecc, incl, node, argp, nu, m, arglat, truelon, lonper;
	double sec, jd, jdFrac, rad, tsince, startmfe, stopmfe, deltamin;
	int  year; int mon; int day; int hr; int min;
	char longstr1[130];
	typedef char str3[4];
	str3 monstr[13];
	char outname[64];
	char longstr2[130];
	elsetrec satrec;

	rad = 180.0 / pi;
	// ------------------------  implementation   --------------------------
	strcpy_s(monstr[1], "Jan");
	strcpy_s(monstr[2], "Feb");
	strcpy_s(monstr[3], "Mar");
	strcpy_s(monstr[4], "Apr");
	strcpy_s(monstr[5], "May");
	strcpy_s(monstr[6], "Jun");
	strcpy_s(monstr[7], "Jul");
	strcpy_s(monstr[8], "Aug");
	strcpy_s(monstr[9], "Sep");
	strcpy_s(monstr[10], "Oct");
	strcpy_s(monstr[11], "Nov");
	strcpy_s(monstr[12], "Dec");

	printf("%s\n", SGP4Version);

	//opsmode = 'a' best understanding of how afspc code works
	//opsmode = 'i' improved sgp4 resulting in smoother behavior
	
	//printf("input operation mode (a), i \n\n");

	/* 
	opsmode = getchar();
	fflush(stdin);
	getchar();
	*/
	
	opsmode = 'a';
		

	/*
	printf("typerun = c compare 1 year of full satcat data \n",
		"typerun = v verification run, requires modified elm file with \n",
		"              start, stop, and delta times \n",
		"typerun = m manual operation- either mfe, epoch, or day of yr \n");
	printf("input type of run c, v, m \n\n"); 

	

	
	typerun = getchar();
	fflush(stdin);
	getchar();
	*/
	
	typerun = 'm';



	//typeinput = 'm' input start stop mfe
	//typeinput = 'e' input start stop ymd hms
	//typeinput = 'd' input start stop yr dayofyr
	/*
	if ((typerun != 'v') && (typerun != 'c'))
	{
		printf("input mfe, epoch (YMDHMS), or dayofyr approach, m,e,d \n\n");
		typeinput = getchar();	
	}
	else
		typeinput = 'e';
		
	*/
	
	typeinput = 'e';

	//printf("input which constants 721 (72) 84 \n");
	//scanf_s("%i", &whichcon);
	whichcon = 72;
	if (whichcon == 721) whichconst = wgs72old;
	if (whichcon == 72) whichconst = wgs72;
	if (whichcon == 84) whichconst = wgs84;

	// sgp4fix no longer needed. done once in sgp4init
	// getgravconst( whichconst, tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2 );

	// ---------------- setup files for operation ------------------
	// input 2-line element set file
	printf("input elset filename: \n");
	//infilename = "ISSlatest.txt";
	scanf_s("%s", infilename, sizeof(infilename));
	err = fopen_s(&infile, infilename, "r");
	if (infile == NULL)
	{
		printf("Failed to open file: %s\n", infilename);
		return 1;
	}

	if (typerun == 'c')
		err = fopen_s(&outfile, "tcppall.out", "w");
	else
	{
		if (typerun == 'v')
			err = fopen_s(&outfile, "tcppver.out", "w");
		else
			err = fopen_s(&outfile, "tcpp.out", "w");
	}
	err = fopen_s(&outoutfile, "plotInMatlab.txt", "w");

	fopen_s(&dbgfile, "sgp4test.dbg", "w");
	fprintf(dbgfile, "this is the debug output\n\n");

	// ----------------- test simple propagation -------------------
	while (feof(infile) == 0)
	{

		// sgp4fix addiional parameters to store from the TLE
		satrec.classification = 'U';
		strncpy_s(satrec.intldesg, "          ", 11 * sizeof(char));
		satrec.ephtype = 0;
		satrec.elnum = 0;
		satrec.revnum = 0;

		do
		{
			fgets(longstr1, 130, infile);
			strncpy_s(str, &longstr1[0], 1);
			str[1] = '\0';
		} while ((strcmp(str, "#") == 0) && (feof(infile) == 0));

		if (feof(infile) == 0)
		{
			fgets(longstr2, 130, infile);
			// convert the char string to sgp4 elements
			// includes initialization of sgp4 and jd, jdfrac and sgp4init
			SGP4Funcs::twoline2rv(longstr1, longstr2, typerun, typeinput, opsmode, whichconst,
				startmfe, stopmfe, deltamin, satrec);
			fprintf(outfile, "%ld xx\n", satrec.satnum);

			printf(" %ld\n", satrec.satnum);
			// call the propagator to get the initial state vector value
			// no longer need gravconst since it is assigned in sgp4init
			SGP4Funcs::sgp4(satrec, 0.0, ro, vo);
			

			double rteme[3];
			double vteme[3];
			double ateme[3];

			double rpef[3];
			double vpef[3];
			double apef[3];


			for (int i = 0; i < 3; i++) {
				rteme[i] = ro[i];
				vteme[i] = vo[i];
			}

			// calculate ateme
			double ateme_unit_vector[3];
			double magnitude_of_rteme = sqrt(pow(rteme[0],2) + pow(rteme[1],2) + pow(rteme[2],2));
			for (int i = 0; i < 3; i++) {
				ateme_unit_vector[i] = -1*rteme[i]/magnitude_of_rteme;
				ateme[i] = 9.81*ateme_unit_vector[i];
			}

			

			// generate .e files for stk
			jd = satrec.jdsatepoch;
			jdFrac = satrec.jdsatepochF;
			strncpy_s(outname, &longstr1[2], 5);
			outname[5] = '.';
			outname[6] = 'e';
			outname[7] = '\0';
			SGP4Funcs::invjday(jd, jdFrac, year, mon, day, hr, min, sec);
			// delta UT1 = UT1 - UTC; on 9/12/2016 delta UT1 = -0.3811866 seconds
			// UTC = UT1 - delta UT1 => UT1 = UTC + delta UT1
			double day_jdut1 = (jd+jdFrac + (-0.3811899))/86400;
			double jdut1 =  (day_jdut1-2451545)/36525;



			// convert from TEME to PEF
			coordFK5::teme_pef(rteme, vteme, ateme, eTo, rpef, vpef, apef, jdut1);

			
			double recef[3];
			for (int i = 0; i < 3; i++) {
				recef[i] = rpef[i];
			}
			
			double latgc, latgd, lon, hellp;
			// PEF to LAT-LON-AL
			ast2Body::ijk2ll(recef, jdut1, latgc, latgd, lon, hellp);
			double latgc_in_degree = latgc*(180/pi);
			double lon_in_degree = lon*(180/pi);
			//fprintf(outfile, "1st LAT is %lf \n", latgc);
			//fprintf(outoutfile,"Latitude\n");
			//fprintf(outoutfile,"Longitude\n");
			
			//fprintf(outoutfile, "%.15lf\t%.15lf\n\n\n",day_jdut1,latgc_in_degree);
			//fprintf(outoutfile, "%.15lf\t%.15lf\n\n\n",day_jdut1,lon_in_degree);

			double magnitude_of_vteme = sqrt(pow(vteme[0],2) + pow(vteme[1],2) + pow(vteme[2],2));









			err = fopen_s(&outfilee, outname, "w");
			fprintf(outfilee, "stk.v.4.3 \n"); // must use 4.3...
			fprintf(outfilee, "\n");
			fprintf(outfilee, "BEGIN Ephemeris \n");
			fprintf(outfilee, " \n");
			fprintf(outfilee, "NumberOfEphemerisPoints		146 \n");
			fprintf(outfilee, "ScenarioEpoch	  %3i %3s%5i%3i:%2i:%12.9f \n", day, monstr[mon],
				year, hr, min, sec);
			fprintf(outfilee, "InterpolationMethod		Lagrange \n");
			fprintf(outfilee, "InterpolationOrder		5 \n");
			fprintf(outfilee, "CentralBody				Earth \n");
			fprintf(outfilee, "CoordinateSystem			TEME \n");
			fprintf(outfilee, "CoordinateSystemEpoch	%3i %3s%5i%3i:%2i:%12.9f \n", day,
				monstr[mon], year, hr, min, sec);
			fprintf(outfilee, "DistanceUnit			Kilometers \n");
			fprintf(outfilee, " \n");
			fprintf(outfilee, "EphemerisTimePosVel \n");
			fprintf(outfilee, " \n");
			fprintf(outfilee, " %16.8f %16.8f %16.8f %16.8f %12.9f %12.9f %12.9f\n",
				satrec.t, ro[0], ro[1], ro[2], vo[0], vo[1], vo[2]);

			fprintf(outfile, " %16.8f %16.8f %16.8f %16.8f %12.9f %12.9f %12.9f\n",
				satrec.t, ro[0], ro[1], ro[2], vo[0], vo[1], vo[2]);
			//fprintf(outfile, "1st LAT number is %lf \n\n\n", latgc_in_degree);
			fprintf(outfile, "1st LON degree is %lf \n\n\n", lon_in_degree);
			//fprintf(outfile, "1st LON is %lf \n\n\n", lon);
			fprintf(outfile, "1st LAT is %lf \n\n\n", latgc_in_degree);
			fprintf(outfile, "1st altitude is %lf \n\n\n", hellp);
			fprintf(outfile, "1st velocity (teme) is %lf \n\n\n", magnitude_of_vteme);
			tsince = startmfe;
			// check so the first value isn't written twice
			if (fabs(tsince) > 1.0e-8)
				tsince = tsince - deltamin;

			// ----------------- loop to perform the propagation ----------------
			while ((tsince < stopmfe) && (satrec.error == 0))
			{
				tsince = tsince + deltamin;

				if (tsince > stopmfe)
					tsince = stopmfe;

				
				SGP4Funcs::sgp4(satrec, tsince, ro, vo);


				for (int i = 0; i < 3; i++) {
					rteme[i] = ro[i];
					vteme[i] = vo[i];
				}


				magnitude_of_rteme = sqrt(pow(rteme[0], 2) + pow(rteme[1], 2) + pow(rteme[2], 2));
				for (int i = 0; i < 3; i++) {
					ateme_unit_vector[i] = -1 * (rteme[i] / magnitude_of_rteme);
					ateme[i] = 9.81*ateme_unit_vector[i];
				}

				jd = satrec.jdsatepoch;
				jdFrac = satrec.jdsatepochF;
				//strncpy_s(outname, &longstr1[2], 5);
				//outname[5] = '.';
				//outname[6] = 'e';
				//outname[7] = '\0';
				SGP4Funcs::invjday(jd, jdFrac, year, mon, day, hr, min, sec);
				// delta UT1 = UT1 - UTC; on 9/12/2016 delta UT1 = -0.3811866 seconds
				// UTC = UT1 - delta UT1 => UT1 = UTC + delta UT1
				day_jdut1 = (jd+jdFrac + (-0.3811899))/86400;
				jdut1 =  (day_jdut1-2451545)/36525;




				// convert TEME to PEF
				coordFK5::teme_pef(rteme, vteme, ateme, eTo, rpef, vpef, apef, jdut1);

				for (int i = 0; i < 3; i++) {
					recef[i] = rpef[i];
				}

				// convert from PEF to LLA
				ast2Body::ijk2ll(recef, jdut1, latgc, latgd, lon, hellp);
				latgc_in_degree = latgc*(180 / pi);
				lon_in_degree= lon*(180/pi);

				magnitude_of_vteme = sqrt(pow(vteme[0],2) + pow(vteme[1],2) + pow(vteme[2],2));



				if (satrec.error > 0)
					printf("# *** error: t:= %f *** code = %3d\n",
					satrec.t, satrec.error);

				if (satrec.error == 0)
				{
					if ((typerun != 'v') && (typerun != 'c'))
					{
						double jd = satrec.jdsatepoch;
						double jdFrac = satrec.jdsatepochF + tsince / 1440.0;
						if (jdFrac < 0.0)
						{
							jd = jd - 1.0;
							jdFrac = jdFrac + 1.0;
						}
						SGP4Funcs::invjday(jd, jdFrac, year, mon, day, hr, min, sec);
						/*day_jdut1 = (jd+jdFrac + (-0.3811899))/86400;
						jdut1 =  (day_jdut1-2451545)/36525;*/

						fprintf(outfile,
							" %16.8f %16.8f %16.8f %16.8f %12.9f %12.9f %12.9f %5i%3i%3i %2i:%2i:%9.6f\n",
							tsince, ro[0], ro[1], ro[2], vo[0], vo[1], vo[2], year, mon, day, hr, min, sec);
						//                            fprintf(outfile, " %16.8f %16.8f %16.8f %16.8f %12.9f %12.9f %12.9f\n",
						//                                           tsince,ro[0],ro[1],ro[2],vo[0],vo[1],vo[2]);
						fprintf(outfile, "please LONG degree be %lf at %d\n", lon_in_degree, min);
						//fprintf(outfile, "LON is %lf \n",lon1);
						fprintf(outfile, "LAT degree is %lf at minute %d \n", latgc_in_degree,min);
						fprintf(outfile, "altitude is %lf at minute %d \n", hellp, min);
						fprintf(outfile, "velocity (teme) is %lf at minute %d \n", magnitude_of_vteme, min);
						//fprintf(outoutfile, "%.15lf\t%.15lf\n\n",day_jdut2,latgc_in_degree1);
						//fprintf(outoutfile, "%.8ld\t%.8ld\n\n",day_jdut2,latgc_in_degree1);
						//fprintf(outoutfile, "%.15lf\t%.15lf\n\n",day_jdut1,lon_in_degree);

					}
					else
					{
						jd = satrec.jdsatepoch;
						jdFrac = satrec.jdsatepochF + tsince / 1440.0;
						if (jdFrac < 0.0)
						{
							jd = jd - 1.0;
							jdFrac = jdFrac + 1.0;
						}
						SGP4Funcs::invjday(jd, jdFrac, year, mon, day, hr, min, sec);
						/*day_jdut1 = (jd+jdFrac + (-0.3811899))/86400;
						jdut1 =  (day_jdut1-2451545)/36525;*/


						fprintf(outfilee, " %16.6f %16.8f %16.8f %16.8f %12.9f %12.9f %12.9f \n",
							tsince*60.0, ro[0], ro[1], ro[2], vo[0], vo[1], vo[2]);

						fprintf(outfile, " %16.8f %16.8f %16.8f %16.8f %12.9f %12.9f %12.9f",   // \n
							tsince, ro[0], ro[1], ro[2], vo[0], vo[1], vo[2]);

						SGP4Funcs::rv2coe(ro, vo, satrec.mu, p, a, ecc, incl, node, argp, nu, m, arglat, truelon, lonper);
						fprintf(outfile, " %14.6f %8.6f %10.5f %10.5f %10.5f %10.5f %10.5f %5i%3i%3i %2i:%2i:%9.6f\n",
							a, ecc, incl*rad, node*rad, argp*rad, nu*rad,
							m*rad, year, mon, day, hr, min, sec);
					}
				} // if satrec.error == 0

			} // while propagating the orbit

			fprintf(outfilee, " END Ephemeris \n");
			fclose(outfilee);

		} // if not eof

	} // while through the input file

	// sgp4fix demonstrate method of running SGP4 directly from orbital element values
	//1 08195U 75081A   06176.33215444  .00000099  00000-0  11873-3 0   813
	//2 08195  64.1586 279.0717 6877146 264.7651  20.2257  2.00491383225656
	const double deg2rad = pi / 180.0;         //   0.0174532925199433
	const double xpdotp = 1440.0 / (2.0 *pi);  // 229.1831180523293

	whichconst = wgs72;
	opsmode = 'a';
	satrec.satnum = 8195;
	satrec.jdsatepoch = 2453911.0;
	satrec.jdsatepochF = 0.8321544402;
	satrec.no_kozai = 2.00491383;
	satrec.ecco = 0.6877146;
	satrec.inclo = 64.1586;
	satrec.nodeo = 279.0717;
	satrec.argpo = 264.7651;
	satrec.mo = 20.2257;
	satrec.nddot = 0.00000e0;
	satrec.bstar = 0.11873e-3;
	satrec.ndot = 0.00000099;
	satrec.elnum = 813;
	satrec.revnum = 22565;
	satrec.classification = 'U';
	strncpy_s(satrec.intldesg, "          ", 11 * sizeof(char));
	satrec.ephtype = 0;

	// convert units and initialize
	satrec.no_kozai = satrec.no_kozai / xpdotp; //* rad/min
	satrec.ndot = satrec.ndot / (xpdotp*1440.0);  //* ? * minperday
	satrec.nddot = satrec.nddot / (xpdotp*1440.0 * 1440);
	satrec.inclo = satrec.inclo  * deg2rad;
	satrec.nodeo = satrec.nodeo  * deg2rad;
	satrec.argpo = satrec.argpo  * deg2rad;
	satrec.mo = satrec.mo     * deg2rad;

	// set start/stop times for propagation
	startmfe = 0.0;
	stopmfe = 2880.0;
	deltamin = 120.0;

	SGP4Funcs::sgp4init(whichconst, opsmode, satrec.satnum, satrec.jdsatepoch + satrec.jdsatepochF - 2433281.5, satrec.bstar,
		satrec.ndot, satrec.nddot, satrec.ecco, satrec.argpo, satrec.inclo, satrec.mo, satrec.no_kozai,
		satrec.nodeo, satrec);

	tsince = startmfe;
	while ((tsince < stopmfe) && (satrec.error == 0))
	{
		tsince = tsince + deltamin;

		if (tsince > stopmfe)
			tsince = stopmfe;

		SGP4Funcs::sgp4(satrec, tsince, ro, vo);

		jd = satrec.jdsatepoch;
		jdFrac = satrec.jdsatepochF + tsince / 1440.0;
		if (jdFrac < 0.0)
		{
			jd = jd - 1.0;
			jdFrac = jdFrac - 1.0;
		}
		SGP4Funcs::invjday(jd, jdFrac, year, mon, day, hr, min, sec);

		fprintf(outfile, " %16.8f %16.8f %16.8f %16.8f %12.9f %12.9f %12.9f",
			tsince, ro[0], ro[1], ro[2], vo[0], vo[1], vo[2]);
	} // while propagating the orbit


	return 0;
}  // testSGP4

