/* -----------------------------------------------------------------------------
*
*                               sgp4DC.cpp
*
*    this file contains the differential corrections routines using the sgp4
*    analytical propagation code.a detailed discussion of the theory and history
*    may be found in the 2008 aiaa paper by vallado and crawford.
*
*                            companion code for
*               fundamentals of astrodynamics and applications
* 2007
* by david vallado
*
*       (w)719 - 573 - 2600, email dvallado@agi.com
*
*    current :
*              30 sep 15  david vallado
*                           fix jd, jdfrac
*    changes :
*               3 nov 14  david vallado
*                           update to msvs2013 c++
*               8 aug 14 alek lidtke
*                          change the constructor of std::vector< std::vector<double> > nut (lines 206 and 377), also other vectors in lines 802, 1267, and 2121
*                          started using fopen_s in line 1345, fscanf_s in line 1356 with MSVS compiler
*               6 aug 08  david vallado
*                           add operationmode for afspc (a) or improved (i)
*              18 jun 08  david vallado
*                           original version
* -------------------------------------------------------------------------- - */

#include "sgp4DC.h"


namespace SGP4DC 
{
	static  double    DMAX(double a, double b) {
		if (a > b) return  a;
		else return b;
	}

	static  float     FMAX(float a, float b)  {
		if (a > b) return  a;
		else return b;
	}

	static  int       IMAX(int   a, int   b)  {
		if (a > b) return  a;
		else return b;
	}

	static  double    DMIN(double a, double b) {
		if (a < b) return  a;
		else return b;
	}

	static  float     FMIN(float a, float b)  {
		if (a < b) return  a;
		else return b;
	}

	static  int       IMIN(int   a, int   b)  {
		if (a < b) return  a;
		else return b;
	}

    static  double    DSQR(double a) { return(a*a); }
    static  float     FSQR(float a)  { return(a*a); }

    static double sum_f(double a, double b) { return a + b; }
#define NO_DIFF(a, b) sum_f(a,b)==(b)
#define SIGN(a, b)  ((b) >= 0 ? fabs(a) : -fabs(a))

	static double dpythag(double a, double b)
	{
		double absa, absb;

		absa = fabs(a);
		absb = fabs(b);

		if (absa == 0.0)
			return absb;
		else if (absb == 0.0)
			return absa;
		else if (absa > absb)
			return absa * sqrt(1.0 + DSQR(absb / absa));
		else
			return absb * sqrt(1.0 + DSQR(absa / absb));
	}

/* ==================================================================
This is zero-offset (C style) version of NR routine for doing the
SVD back-substitution action.

Solve A*x = b for x in least-squares manner by using the SVD
T
decomposition of matrix A = U * w * V

u[0..m-1][0..n-1] these are constants from A by SVD decomposition.
w[0..n-1]
v[0..n-1][0..n-1]

b[0..m-1] is input
x[0..n-1] is output

m & n are dimensions, normally m > n for true (over determined)
solution.
================================================================== */

	void dsvbksb
		(
		std::vector< std::vector<double> > u,
		std::vector< std::vector<double> > w,
		std::vector< std::vector<double> > v,
		int m, int n,
		std::vector< std::vector<double> > b,
		std::vector< std::vector<double> > &dx
		)
	{
		int jj, j, i;
		double s;
		//	std::vector< std::vector<double> > tmp(n, 1);
		std::vector< std::vector<double> > tmp = std::vector< std::vector<double> >(n, std::vector<double>(1));

		for (j = 0; j < n; j++)
		{
			s = 0.0;
			// Here we understand that if w[j] == 0 then treat X/0 as 0, not infinity.
			if (w[j][0] = 0.0)
			{
				for (i = 0; i < m; i++) s += u[i][j] * b[i][0];
				s /= w[j][0];
			}
			tmp[j][0] = s;
		}

		for (j = 0; j < n; j++)
		{
			s = 0.0;
			for (jj = 0; jj < n; jj++) s += v[j][jj] * tmp[jj][0];
			dx[j][0] = s;
		}
	} // dsvbksb


/* -----------------------------------------------------------------------------
*   Perform Singular Value Decomposition of a matrix A to get:
*
*				 T
*	A = U * w * V
*
*   This can then be used to robustly (in a singular-matrix sense) solve
*   the least-squares problem:
*
*   A * x = b
*
*   With known A and b and dim b > dim x for an over-determined system by
*   calling the dsvbksb() function with U, w and V. For near-singular cases
*   you can zero and small w[] terms to prevent cancling near-infinities.
*
*   Here the calling arguments are:
*
*	A[0..m-1][0..n-1]	input as A
*
*	m, n				dimensions of matrix A (and for w & V)
*
*	U[0..m-1][0..n-1]	output
*	w[0..n-1		output
*	V[0..n-1][0..n-1]	output
*
*   Return value is 0 if OK, or -1 if failed (rare).
*
--------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
	int dsvdcmp(
		std::vector< std::vector<double> > &a, int m, int n,
		std::vector< std::vector<double> > &w,
		std::vector< std::vector<double> > &v
		)
	{
		bool flag = false;
		int i, its, j, jj, k, l = 0, nm = 0;
		double anorm, c, f, g, h, s, scale, x, y, z;
		const int SVD_ITMAX = 30;
		//const double SVD_EPS = std::numeric_limits<double>::epsilon();
		const double SVD_EPS = 0.000000001;

		double rv1[7];  // 7 make largest, old was n

		g = scale = anorm = 0.0;
		for (i = 0; i < n; i++)
		{
			l = i + 2;
			rv1[i] = scale * g;
			g = s = scale = 0.0;
			if (i < m)
			{
				for (k = i; k < m; k++) scale += fabs(a[k][i]);
				if (scale != 0.0) {
					for (k = i; k < m; k++)
					{
						a[k][i] /= scale;
						s += a[k][i] * a[k][i];
					}
					f = a[i][i];
					g = -SIGN(sqrt(s), f);
					h = f * g - s;
					a[i][i] = f - g;
					for (j = l - 1; j < n; j++)
					{
						for (s = 0.0, k = i; k < m; k++) s += a[k][i] * a[k][j];
						f = s / h;
						for (k = i; k < m; k++) a[k][j] += f * a[k][i];
					}
					for (k = i; k < m; k++) a[k][i] *= scale;
				}
			}
			w[i][0] = scale * g;
			g = s = scale = 0.0;
			if (i + 1 <= m && i + 1 != n)
			{
				for (k = l - 1; k < n; k++) scale += fabs(a[i][k]);
				if (scale != 0.0) {
					for (k = l - 1; k < n; k++)
					{
						a[i][k] /= scale;
						s += a[i][k] * a[i][k];
					}
					f = a[i][l - 1];
					g = -SIGN(sqrt(s), f);
					h = f * g - s;
					a[i][l - 1] = f - g;
					for (k = l - 1; k < n; k++) rv1[k] = a[i][k] / h;
					for (j = l - 1; j < m; j++) {
						for (s = 0.0, k = l - 1; k < n; k++) s += a[j][k] * a[i][k];
						for (k = l - 1; k < n; k++) a[j][k] += s * rv1[k];
					}
					for (k = l - 1; k < n; k++) a[i][k] *= scale;
				}
			}
			anorm = DMAX(anorm, (fabs(w[i][0]) + fabs(rv1[i])));
		}
		for (i = n - 1; i >= 0; i--)
		{
			if (i < n - 1)
			{
				if (g != 0.0)
				{
					for (j = l; j < n; j++)
						v[j][i] = (a[i][j] / a[i][l]) / g;
					for (j = l; j < n; j++)
					{
						for (s = 0.0, k = l; k < n; k++) s += a[i][k] * v[k][j];
						for (k = l; k < n; k++) v[k][j] += s * v[k][i];
					}
				}
				for (j = l; j < n; j++) v[i][j] = v[j][i] = 0.0;
			}
			v[i][i] = 1.0;
			g = rv1[i];
			l = i;
		}
		for (i = IMIN(m, n) - 1; i >= 0; i--)
		{
			l = i + 1;
			g = w[i][0];
			for (j = l; j < n; j++) a[i][j] = 0.0;
			if (g != 0.0)
			{
				g = 1.0 / g;
				for (j = l; j < n; j++)
				{
					for (s = 0.0, k = l; k < m; k++) s += a[k][i] * a[k][j];
					f = (s / a[i][i]) * g;
					for (k = i; k < m; k++) a[k][j] += f * a[k][i];
				}
				for (j = i; j < m; j++) a[j][i] *= g;
			}
			else for (j = i; j < m; j++) a[j][i] = 0.0;
			++a[i][i];
		}
		for (k = n - 1; k >= 0; k--)
		{
			for (its = 1; its <= SVD_ITMAX; its++)
			{
				flag = true;
				for (l = k; l >= 0; l--) {
					nm = l - 1;
					if (fabs(rv1[l]) <= SVD_EPS * anorm)
					{
						flag = false;
						break;
					}
					if (fabs(w[nm][0]) <= SVD_EPS * anorm) break;
				}
				if (flag)
				{
					c = 0.0;
					s = 1.0;
					for (i = l; i < k + 1; i++)
					{
						f = s * rv1[i];
						rv1[i] = c * rv1[i];
						if (fabs(f) <= SVD_EPS * anorm) break;
						g = w[i][0];
						h = dpythag(f, g);
						w[i][0] = h;
						h = 1.0 / h;
						c = g * h;
						s = -f * h;
						for (j = 0; j < m; j++)
						{
							y = a[j][nm];
							z = a[j][i];
							a[j][nm] = y * c + z * s;
							a[j][i] = z * c - y * s;
						}
					}
				}
				z = w[k][0];
				if (l == k)
				{
					if (z < 0.0)
					{
						w[k][0] = -z;
						for (j = 0; j < n; j++) v[j][k] = -v[j][k];
					}
					break;
				}

				if (its == SVD_ITMAX)
				{
					printf("dsvdcmp: No convergence in %d iterations", SVD_ITMAX);
					return -1;
				}

				x = w[l][0];
				nm = k - 1;
				y = w[nm][0];
				g = rv1[nm];
				h = rv1[k];
				f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
				g = dpythag(f, 1.0);
				f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;
				c = s = 1.0;
				for (j = l; j <= nm; j++)
				{
					i = j + 1;
					g = rv1[i];
					y = w[i][0];
					h = s * g;
					g = c * g;
					z = dpythag(f, h);
					rv1[j] = z;
					c = f / z;
					s = h / z;
					f = x * c + g * s;
					g = g * c - x * s;
					h = y * s;
					y *= c;
					for (jj = 0; jj < n; jj++)
					{
						x = v[jj][j];
						z = v[jj][i];
						v[jj][j] = x * c + z * s;
						v[jj][i] = z * c - x * s;
					}
					z = dpythag(f, h);
					w[j][0] = z;
					if (z) {
						z = 1.0 / z;
						c = f * z;
						s = h * z;
					}
					f = c * g + s * y;
					x = c * y - s * g;
					for (jj = 0; jj < m; jj++)
					{
						y = a[jj][j];
						z = a[jj][i];
						a[jj][j] = y * c + z * s;
						a[jj][i] = z * c - y * s;
					}
				}
				rv1[l] = 0.0;
				rv1[k] = f;
				w[k][0] = x;
			}
		}

		return 0;
	}  // dsvdcmp


/* -----------------------------------------------------------------------------
*                           procedure printtle
*
*  this procedure prints out the tle in a tle format.
*
*  author        : david vallado                  719-573-2600    6 aug 2008
*
*  inputs          description                    range / units
*    satrec      - record of satellite parameters for TLE
*
*  outputs       :
*    none.
*
*  locals        :
*    none.
*
*  coupling      :
*    none.
* --------------------------------------------------------------------------- */

	void printtle
		(
		elsetrec satrec
		)
	{
		char longstr1[130], longstr1a[130], longstr2[130];
		char intldesg[11];  // classification
		char bstarstr[6], bstarexp, *ptr;
		double rad;
		const double xpdotp = 1440.0 / (2.0 *pi);  // 229.1831180523293
		rad = 180.0 / pi;
		strcpy_s(intldesg, "testsat");

		// -------------------- write out tle format of data ----------------------
		// may need to do some of these...
		// added %1c and classification here was N before, 
		// changed intldesg to 7 from 10 characters, added 3 spaces after %7s to align columns, 
		// changed epochdays to %11.8lf and added a leading 0
		sprintf_s(longstr1, "1 %5ldN%10s%02i%12.8lf  .00000000  00000-0 ",
			satrec.satnum, intldesg, satrec.epochyr, satrec.epochdays);

		sprintf_s(longstr1a, "%8.4e 0    1\n", satrec.bstar);
		ptr = strchr(longstr1a, 'e');
		bstarexp = longstr1a[ptr - longstr1a + 3];
		sprintf_s(longstr1a, "%8.4e 0    1\n", satrec.bstar*1000.0);
		strncpy_s(bstarstr, longstr1a, 6);

		// now do the second line
		if (satrec.inclo < 0.0)
			satrec.inclo = 2.0 * pi + satrec.inclo;
		if (satrec.nodeo < 0.0)
			satrec.nodeo = 2.0 * pi + satrec.nodeo;
		if (satrec.argpo < 0.0)
			satrec.argpo = 2.0 * pi + satrec.argpo;
		if (satrec.mo < 0.0)
			satrec.mo = 2.0 * pi + satrec.mo;
		// use the '0' in the format to get left filling zeros
		sprintf_s(longstr2, "2 %5ld %08.4lf%9.4lf %07.0lf %8.4lf %8.4lf %11.8lf 00000\n",
			satrec.satnum, satrec.inclo * rad, satrec.nodeo * rad,
			satrec.ecco * 10000000.0, satrec.argpo * rad, satrec.mo * rad, satrec.no_unkozai * xpdotp);

		//     printf("%s%s%s", longstr1,bstarstr, bstarexp);
		printf("%s%s", longstr1, longstr1a);
		printf("%s", longstr2);
	}  //  printtle


/* -----------------------------------------------------------------------------
*                           procedure getsensorparams
*
*  this procedure gets the sensor parameters. nte that the values in here are
*  arbirtrary at this point, but may be filled in and added as appropriate.
*
*  author        : david vallado                  719-573-2600    1 dec 2007
*
*  inputs          description                    range / units
*    sennum      - sensor number
*
*  outputs       :
*    currsenrec  - structure containing sensor information
*
*  locals        :
*    none.
*
*  coupling      :
*    none.
* --------------------------------------------------------------------------- */

	void getsensorparams
		(
		int sennum,
		senrec& currsenrec
		)
	{
		double rad;
		rad = 180.0 / pi;
		switch (sennum)
		{
		case 1:
		{
				  ///*
				  currsenrec.noisex = 0.01;    // 10 m
				  currsenrec.noisey = 0.01;
				  currsenrec.noisez = 0.01;
				  currsenrec.noisexdot = 0.01;  // 10 cm/s
				  currsenrec.noiseydot = 0.01;
				  currsenrec.noisezdot = 0.01;
				  currsenrec.noisebstar = 0.0001;
				  //*/
				  /*
				  currsenrec.noisex = 1.0;    // 1 km
				  currsenrec.noisey = 1.0;
				  currsenrec.noisez = 1.0;
				  currsenrec.noisexdot = 0.001;  // 1 m/s
				  currsenrec.noiseydot = 0.001;
				  currsenrec.noisezdot = 0.001;
				  currsenrec.noisebstar = 1.0;
				  */
		}
			break;
		case 2:
		{
				  currsenrec.noisex = 0.001;
		}
			break;
		case 3:
		{
				  currsenrec.noisex = 0.001;
		}
			break;
		case 344:
		{
					currsenrec.sennum = 344;
					strcpy_s(currsenrec.senname, "fylingdales");
					currsenrec.senlat = 54.37 / rad;
					currsenrec.senlon = -0.67 / rad;
					currsenrec.senalt = 0.3389;  // km
					currsenrec.noiserng = 0.09;   // km
					currsenrec.noiseaz = 0.02 / rad;  // rad
					currsenrec.noiseel = 0.01 / rad;  // rad
		}
		case 932:
		{
					currsenrec.sennum = 932;
					strcpy_s(currsenrec.senname, "kaena point");
					currsenrec.senlat = 21.572 / rad;
					currsenrec.senlon = 201.733 / rad;
					currsenrec.senalt = 0.3005;  // km

					currsenrec.noiserng = 1.0;   // km   .09
					currsenrec.noiseaz = 1.0 / rad;  // rad .02
					currsenrec.noiseel = 1.0 / rad;  // rad .01

					//           currsenrec.noiserng = 0.09;   // km   .09
					//           currsenrec.noiseaz = 0.02 / rad;  // rad .02
					//           currsenrec.noiseel = 0.01 / rad;  // rad .01
		}
			break;
		}  // case

	} // getsensorparams


/* -----------------------------------------------------------------------------
*                           procedure state2satrec
*
*  this procedure converts the state to the satrec structure, and back.
*
*  author        : david vallado                  719-573-2600   15 jan 2008
*
*  inputs          description                    range / units
*    xnom        - state vector                   varied
*    direct      - direction of conversion        eTo, eFrom
*    satrec      - structure of satellite parameters for TLE
*
*  outputs       :
*    satrec      - structure of satellite parameters for TLE
*    xnom        - state vector                   varied
*
*  locals        :
*    rnom        - nom position vector at epoch   km
*    vnom        - nom velocity vector at epoch   km/s
*
*  coupling      :
*    none.
*
*  references    :
* --------------------------------------------------------------------------- */

	void state2satrec
		(
		std::vector<double> &xnom,
		char statetype, int statesize,
		edirection direct,
		elsetrec& satrec
		)
	{
		double rnom[3], vnom[3];
		double p, a, ecc, incl, raan, argp, nu, m, eccanom, arglat, truelon, lonper;
		elsetrec satrecorig;

		satrecorig = satrec;
		if (direct == eTo)
		{
			switch (statetype)
			{
			case 'v':
			{
						rnom[0] = xnom[0];
						rnom[1] = xnom[1];
						rnom[2] = xnom[2];
						vnom[0] = xnom[3];
						vnom[1] = xnom[4];
						vnom[2] = xnom[5];
						ast2Body::rv2coe(rnom, vnom, 3.986008e5, p, a, ecc, incl, raan, argp, nu, m, eccanom, arglat, truelon, lonper);
						satrec.no_unkozai = 60.0 * sqrt(3.986008e5 / (a * a * a));  // rad per min, sgp4 wgs-72 mu value
						satrec.a = a / 6378.135;  // er
						satrec.ecco = ecc;
						satrec.inclo = incl;
						satrec.nodeo = raan;
						satrec.argpo = argp;
						satrec.mo = m;
			}
				break;
			case 't':  // tle (keplerian) elements
			{
						   //                  satrec.no    = xnom[0];  // rad/min
						   satrec.a = xnom[0];  // er
						   satrec.no_unkozai = 1.0 / 13.446839 * sqrt(1.0 / (satrec.a * satrec.a * satrec.a));  // rad / min
						   satrec.ecco = xnom[1];
						   satrec.inclo = xnom[2];
						   satrec.nodeo = xnom[3];
						   satrec.argpo = xnom[4];
						   satrec.mo = xnom[5];
			}
				break;
			case 'e':  // equinoctial elements
			{
						   //                  satrec.no    = xnom[2];    // rad / min
						   //                  satrec.a     = pow( 1.0 / (satrec.no * satrec.no * 13.446839 * 13.446839) , 1.0 / 3.0 );  // er
						   satrec.a = xnom[2];    // er
						   if (satrec.a < 1.0)  // can't be less than the radius of the earth
						   {
							   printf("changed a %11.7lf ", satrec.a);
							   satrec.a = satrecorig.a * 1.01;   // er   1.05, .9
							   xnom[2] = satrec.a;  // remember to change both!!!
							   printf(" to %11.7lf \n", satrec.a);
						   }
						   satrec.no_unkozai = (1.0 / 13.446839) * sqrt(1.0 / (satrec.a * satrec.a * satrec.a));  // rad / min
						   if (satrec.no_unkozai < 1.0e-5)
						   {
							   printf("changed no %11.7lf ", satrec.no_kozai);
							   satrec.no_unkozai = satrecorig.no_unkozai * 0.9;
							   satrec.a = pow(1.0 / (satrec.no_unkozai * satrec.no_unkozai * 13.446839 * 13.446839), 1.0 / 3.0);  // er
							   xnom[2] = satrec.a;  // remember to change both!!!
							   printf(" to %11.7lf \n", satrec.no_unkozai);
						   }
						   satrec.ecco = (sqrt(xnom[0] * xnom[0] + xnom[1] * xnom[1]));
						   if (satrec.ecco > 1.0)
						   {
							   printf("changed ecco %11.7lf ", satrec.ecco);
							   satrec.ecco = satrecorig.ecco * 0.9;
							   // be careful of order here...
							   // could iterate, but since we're just changing values, it's not needed
							   xnom[0] = (satrec.ecco * cos(satrec.argpo + satrec.nodeo));    // ke, af
							   xnom[1] = (satrec.ecco * sin(satrec.argpo + satrec.nodeo));    // he, ag
							   printf(" to %11.7lf \n", satrec.ecco);
						   }
						   satrec.inclo = 2.0 * atan(sqrt(xnom[4] * xnom[4] + xnom[5] * xnom[5]));
						   satrec.nodeo = atan2(xnom[4], xnom[5]);

						   // make sure nodeo stays within 0  to 2pi as it gets used without trigonmetric functions
						   if (satrec.nodeo < 0.0)
						   {
							   satrec.nodeo = twopi + satrec.nodeo;
							   xnom[4] = (tan(satrec.inclo*0.5) * sin(satrec.nodeo));  // pe
							   xnom[5] = (tan(satrec.inclo*0.5) * cos(satrec.nodeo));  // qe
						   }

						   satrec.argpo = atan2(xnom[1], xnom[0]) - satrec.nodeo;
						   satrec.mo = xnom[3] - satrec.nodeo - satrec.argpo;
						   // fmod doesn't seem to make a diff in the results
						   satrec.mo = fmod(satrec.mo, twopi);
			}
				break;
			} // case
			if (statesize > 6)
				satrec.bstar = xnom[6];
		}
		else  // find xnom from satrec -----------------------------------------
		{
			switch (statetype)
			{
			case 'v':  // vectors
			{
						   // xnom contains the nominal pos and vel vector
			}
				break;
			case 't':  // tle (keplerian) elements
			{
						   //  xnom[0] = satrec.no;
						   xnom[0] = satrec.a;  // er
						   xnom[1] = satrec.ecco;
						   xnom[2] = satrec.inclo;
						   xnom[3] = satrec.nodeo;
						   xnom[4] = satrec.argpo;
						   xnom[5] = satrec.mo;
			}
				break;
			case 'e':  // equinoctial elements
			{
						   xnom[0] = (satrec.ecco * cos(satrec.argpo + satrec.nodeo));    // ke, af
						   xnom[1] = (satrec.ecco * sin(satrec.argpo + satrec.nodeo));    // he, ag
						   xnom[2] = satrec.a;  // a is in er
						   xnom[3] = (fmod(satrec.mo + satrec.argpo + satrec.nodeo, twopi));    // L
						   xnom[4] = (tan(satrec.inclo*0.5) * sin(satrec.nodeo));  // pe
						   xnom[5] = (tan(satrec.inclo*0.5) * cos(satrec.nodeo));  // qe
			}
				break;
			} // case
			if (statesize > 6)
				xnom[6] = satrec.bstar;

		}
	} // state2satrec


/* -----------------------------------------------------------------------------
*
*                           procedure findatwbatwa
*
* this procedure finds the a and b matrices for the differential correction
*   problem.  remember that it isn't critical for the propagations to use
*   the highest fidelity techniques because we're only trying to find the
*   "slope". k is an index that allows us to do multiple rows at once. it's
*   used for both the b and a matrix calculations.
*
*  algorithm     : find the a and b matrices by accumulation to reduce matrix
*                  sizes calculate the matrix combinations
*                  atw is found without matrix operations to avoid large matrices
*
*  author        : david vallado                  719-573-2600    6 aug 2008
*
*  inputs          description                    range / units
*    firstob     - number of observations
*    lastob      - number of observations
*    statesize   - size of state                  6 , 7
*    percentchg  - amount to modify the vectors
*                  by in finite differencing
*    deltaamtchg - tolerance for small value in
*                  finite differencing            0.0000001
*    whichconst  - parameter for sgp4 constants   wgs72, wgs721, wgs84
*    satrec      - structure of satellite parameters for TLE
*    obsrecfile    - array of records containing:
*                  senum, jd, rsvec, obstype,
*                  rng, az, el, drng, daz, del,
*                  trtasc, tdecl data
*    statetype   - type of elements (equinoctial, etc)  'e', 't'
*    proptype    - type of propagation            's', 'n', 'k'
*    xnom        - state vector                   varied
*
*  outputs       :
*    atwa        - atwa matrix
*    atwb        - atwb matrix
*    atw         - atw matrix
*    b           - b matrix, the residuals
*    drng2       - range residual squared
*    daz2        - azimuth residual squared
*    del2        - elevation residual squared
*    ddrng2      - range rate residual squared
*    ddaz2       - azimuth rate residual squared
*    ddel2       - elevation rate residual squared
*    dtrtasc2    - topocentric right ascension residual squared
*    dtdecl2     - topocentric declination residual squared
*    dx2         - x position residual squared
*    dy2         - y position residual squared
*    dz2         - z position residual squared
*    dxdot2      - xdot position residual squared
*    dydot2      - ydot position residual squared
*    dzdot2      - zdot position residual squared
*
*  locals        :
*    rnom        - nom position vector at epoch   km
*    vnom        - nom velocity vector at epoch   km/s
*    a           - a matrix
*    indobs      -
*    at          -
*    w1          -
*    w2          -
*    w3          -
*    lst         -
*    gst         -
*    dtsec        -
*    deltaamt    -
*    rngpert     -
*    azpert      - modified azimuth               -2pi to 2pi
*    elpert      - modified azimuth               -pi/2 to pi/2
*    drng        -
*    daz         -
*    del         -
*    error       -
*    i, j, k     -
*
*  coupling      :
*    findsenptr  - find sensor data
*    rv_razel    - find r and v given range, az, el, and rates
*    rv_tradec   - find r and v given topocentric rtasc and decl
*
*  references    :
*    vallado       2007, 753-765
* --------------------------------------------------------------------------- */

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
		)
	{
		xnom.resize(7);  // rows
		char fullsun;

		obsrec currobsrec;
		senrec currsenrec;

		int rowc, colc, r, c, i, j, ii, eqeterms; //k, sennum;
		double rs[3], r3[3], v3[3], rteme[3], vteme[3], ritrf[3], vitrf[3], aitrf[3], ateme[3], rnom[3], vnom[3], 
			recinom[3], vecinom[3], aeci[3], recipert[3], vecipert[3], rpert[3], vpert[3], rmean1[3], vmean1[3];
		double dut1, tut1, jdut1, ttt, tsince, mfme, lod, xp, yp, ddpsi, ddeps, oldtn,
			iaudx, dy, icrsx, y, s, deltapsi, deltaeps;  // jde

		int timezone, dat;
		int indobs = 7;  // size larger than most number in the state
		std::vector< std::vector<double> > a = std::vector< std::vector<double> >(indobs, std::vector<double>(statesize));
		std::vector< std::vector<double> > at = std::vector< std::vector<double> >(statesize, std::vector<double>(indobs));
		std::vector< std::vector<double> > atwaacc = std::vector< std::vector<double> >(statesize, std::vector<double>(statesize));
		std::vector< std::vector<double> > atwbacc = std::vector< std::vector<double> >(statesize, std::vector<double>(1));
		std::vector< std::vector<double> > X = std::vector< std::vector<double> >(7, std::vector<double>(1, 0.0));
		std::vector< std::vector<double> > XCurrn = std::vector< std::vector<double> >(7, std::vector<double>(1, 0.0));
		std::vector< std::vector<double> > XCurrp = std::vector< std::vector<double> >(7, std::vector<double>(1, 0.0));
		std::vector< std::vector<double> > Xf = std::vector< std::vector<double> >(7, std::vector<double>(1, 0.0));
		std::vector< std::vector<double> > xpert = std::vector< std::vector<double> >(7, std::vector<double>(1, 0.0));
		std::vector< std::vector<double> > trans = std::vector< std::vector<double> >(3, std::vector<double>(3));

		// --------------------- initialize parameters ------------------
		double jdyconv = 1.0 / 36525.0;

		timezone = 0;
		eqeterms = 2;
		for (ii = 0; ii < 6; ii++)
		{
			X[ii][0] = xnom[ii];
		}

		currobsrec = obsrecarr[0];
		switch (currobsrec.obstype)
		{
		// range
		case 0: indobs = 1;
			break;
		// az-el
		case 1: indobs = 2;
			break;
		// rng t rtasc decl
		case 3: indobs = 2;
			break;
		// rng az el
		case 2: indobs = 3;
			break;
        // pos/vel vectors
		case 4: indobs = statesize;  // add one for bstar possibility
			break;
		}  // case

		// ------------- reset these since they will accumulate ---------
		double deltaamt;
		double w1, w2, w3, w4, w5, w6, w7,
			rngnom, aznom, elnom, drngnom, daznom, delnom, rngpert, azpert, elpert, drngpert, dazpert, delpert,
			trtascnom, tdeclnom, dtrtascnom, dtdeclnom, trtascpert, tdeclpert, dtrtascpert, dtdeclpert;  // lst, gst, dtsec,
		double weight, bstarnom, bstarpert,	rng2, dbstar2; 
		
		elsetrec satrecp;

		dbstar2 = 0.0;
		rng2 = 0.0;

		// zero out matrices
		for (r = 0; r < statesize; r++)
		{
			for (c = 0; c < statesize; c++)
				atwa[r][c] = 0.0;
			atwb[r][0] = 0.0;
		}

		// ------------------- loop through all the observations ------------------
		oldtn = 0.0;  // for nominal vectors
		for (i = firstob; i <= lastob; i++)
		{
			currobsrec = obsrecarr[i];
			//printf( "ob %2i rsecef0 %11.5f jd %11.5f dtmin %8.3f hr %3i rng %11.7f \n",
			//         i, currobsrec.rsecef[0], currobsrec.jd, currobsrec.dtmin, currobsrec.hr, currobsrec.rng );  // ritrf
			//printf( "ob %2i jd %11.5f dtmin %9.5f hr %3i currobs %11.7f  %11.7f  %11.7f \n",
			//	i, currobsrec.jd, currobsrec.dtmin, currobsrec.hr, currobsrec.x, currobsrec.y, currobsrec.z);  // ritrf

			// --------- propagate the nominal vector to the epoch time -----------
			// -------------- and find the nominal observations -------------------
			switch (proptype) 
			{
			// dtmin is the minutes from epoch (1st obs in .e file) to currobs time
			case 's':  // sgp4
				SGP4Funcs::sgp4(satrec, currobsrec.dtmin, rteme, vteme);  
				coordFK5::teme_eci(rteme, vteme, ateme, eTo, recinom, vecinom, aeci, iau80rec, ttt, 0.0, 0.0);
				break;
			case 'a': // semianalytical
				for (ii = 0; ii < 3; ii++)
				{
					rnom[ii] = X[ii][0];
					vnom[ii] = X[ii + 3][0];
				}
				tsince = currobsrec.dtmin*60.0;  // should be 0.0 if using first point in .e file
				fullsun = 'y';
				astPert::srpperts(currobsrec.jd, currobsrec.jdf, rnom, vnom, interp, opt, fullsun, tsince, jpldearr, jdjpldestart, tut1, cram, recinom, vecinom, rmean1, vmean1);
				for (ii = 0; ii < 3; ii++)
				{
					Xf[ii][0] = recinom[ii];
					Xf[ii + 3][0] = vecinom[ii];
				}
				break;
			case 'n':  // numerical 
				// save first time through to save prop time for others
				if (i == firstob)
				{
					// reset to be sure it's the nominal value
					for (ii = 0; ii < 6; ii++)
						XCurrn[ii][0] = X[ii][0];
					tsince = currobsrec.dtmin*60.0;
				}
				tsince = (currobsrec.dtmin - oldtn) * 60.0;

				astPert::rk4(currobsrec.jd, currobsrec.jdf, tsince, XCurrn, derivType, cdam, cram, interp, opt, jpldearr, jdjpldestart, iau80rec, jdeopstart, eoparr, Xf);
			    // update for next time step for speed 
				oldtn = currobsrec.dtmin; // min
				for (ii = 0; ii < 3; ii++)
				{
					recinom[ii] = Xf[ii][0];
					vecinom[ii] = Xf[ii + 3][0];
				}
				break;
			default:  // Kepler 2-body
				for (ii = 0; ii < 3; ii++)
				{
					rnom[ii] = X[ii][0];
					vnom[ii] = X[ii + 3][0];
				}
				ast2Body::kepler(rnom, vnom, currobsrec.dtmin*60.0, recinom, vecinom);
				for (ii = 0; ii < 3; ii++)
				{
					Xf[ii][0] = recinom[ii];
					Xf[ii + 3][0] = vecinom[ii];
				}
			}

			// eci to itrf if observation type
			// change obs vector state to ecef so obs are created in the right coord system
			if (currobsrec.obstype != 4)
			{
				mfme = currobsrec.hr * 60 + currobsrec.min + currobsrec.sec / 60.0;
				EopSpw::findeopparam(currobsrec.jd, mfme, interp, 'f', eoparr, jdeopstart,
					dut1, dat, lod, xp, yp, ddpsi, ddeps,
					iaudx, dy, icrsx, y, s, deltapsi, deltaeps);
				double jdut1f = currobsrec.jdf + dut1;
				jdut1 = currobsrec.jdf + dut1;
				tut1 = (jdut1 + jdut1f - 2451545.0) * jdyconv;
				double jdttf = currobsrec.jdf + dat + 32.184;   // sec
				ttt = (currobsrec.jdf + dat + 32.184 + jdttf - 2451545.0) * jdyconv;
				coordFK5::itrf_gcrf(ritrf, vitrf, aitrf, eFrom, recinom, vecinom, aeci, iau80rec, ttt, jdut1, lod, xp, yp, 2, ddpsi, ddeps, deltapsi, deltaeps, trans);
			} // if obstype

			// ----------------- determine sensor characteristics -----------------
			rs[0] = currobsrec.rsecef[0];
			rs[1] = currobsrec.rsecef[1];
			rs[2] = currobsrec.rsecef[2];
			// temporary sensor for now
			getsensorparams(currobsrec.sennum, currsenrec);

			// ------------------------- find b matrix ----------------------------
			if (currobsrec.obstype == 3)
				astIOD::rv_tradec(ritrf, vitrf, rs, eTo, rngnom, trtascnom, tdeclnom, drngnom, dtrtascnom, dtdeclnom);
			else
			if (currobsrec.obstype == 4)
				bstarnom = satrec.bstar;
			else
				astIOD::rv_razel(ritrf, vitrf, rs, currsenrec.senlat, currsenrec.senlon, eTo, rngnom, aznom, elnom, drngnom, daznom, delnom);

			switch (currobsrec.obstype)
			{
			case 0:
				b[0][0] = currobsrec.rng - rngnom;
				break;
			case 1: {
						b[0][0] = currobsrec.az - aznom;
						//fix for 0-360...
						if (fabs(b[0][0]) > pi)
							b[0][0] = b[0][0] - astMath::sgn(b[0][0]) * 2.0 * pi;
						b[1][0] = currobsrec.el - elnom; }
				break;
			case 2: {
						b[0][0] = currobsrec.rng - rngnom;
						b[1][0] = currobsrec.az - aznom;
						// fix for 0-360...
						if (fabs(b[1][0]) > pi)
							b[1][0] = b[1][0] - astMath::sgn(b[1][0]) * 2.0 * pi;
						b[2][0] = currobsrec.el - elnom;  }
				break;
			case 3: {
						b[0][0] = currobsrec.trtasc - trtascnom;
						// fix for 0-360...
						if (fabs(b[0][0]) > pi)
							b[0][0] = b[0][0] - astMath::sgn(b[0][0]) * 2.0 * pi;
						b[1][0] = currobsrec.tdecl - tdeclnom;  }
				break;
			case 4: {
						b[0][0] = currobsrec.x - recinom[0];
						b[1][0] = currobsrec.y - recinom[1];
						b[2][0] = currobsrec.z - recinom[2];
						b[3][0] = currobsrec.xdot - vecinom[0];
						b[4][0] = currobsrec.ydot - vecinom[1];
						b[5][0] = currobsrec.zdot - vecinom[2];
						b[6][0] = currobsrec.bstar - bstarnom; }
				break;
			}  // case

//printf( "rnom %11.5f %11.5f %11.5f %8.3f %8.3f %8.3f %8.3f \n",
//              rteme[0], rteme[1], rteme[2], rngnom, aznom*rad, elnom*rad, currobsrec.rng );  // ritrf
//printf("recinom %11.5f %11.5f %11.5f curr %8.3f %8.3f %8.3f \n",
//	recinom[0], recinom[1], recinom[2], currobsrec.x, currobsrec.y, currobsrec.z);  // ritrf


			// ------------------------ find a matrix -----------------------------
			// ------------- reset the perturbed vector to the nominal ------------
			if (proptype == 's')
			    satrecp = satrec;

			// ----- perturb each element in the state individually (elements or vectors) ------
			for (j = 0; j < statesize; j++)
			{
				//// use the current nominal propagated results from previous section for numerical
				//if (!proptype == 's')
				//{
				//	for (ii = 0; ii < 6; ii++)
				//		xnom[ii] = XCurrn[ii][0];
				//}

				finitediff(whichconst, j, percentchg, deltaamtchg, statetype, statesize, proptype, satrecp, xnom, xpert, deltaamt);

				// --------- propagate the perturbed vector to the epoch time -----------
				// ---------------- and find the perturbed observations -----------------
				switch (proptype)
				{
				case 's':  // sgp4
					SGP4Funcs::sgp4(satrecp, currobsrec.dtmin, r3, v3);
					coordFK5::teme_eci(r3, v3, ateme, eTo, recipert, vecipert, aeci, iau80rec, ttt, 0.0, 0.0);
					break;
				case 'a': // semianalytical
					for (ii = 0; ii < 3; ii++)
					{
						rpert[ii] = xpert[ii][0];
						vpert[ii] = xpert[ii + 3][0];
					}
					tsince = currobsrec.dtmin*60.0;
					fullsun = 'y';
					astPert::srpperts(currobsrec.jd, currobsrec.jdf, rpert, vpert, interp, opt, fullsun, tsince, jpldearr, jdjpldestart, tut1, cram, recipert, vecipert, rmean1, vmean1);
					for (ii = 0; ii < 3; ii++)
					{
						Xf[ii][0] = recipert[ii];
						Xf[ii + 3][0] = vecipert[ii];
					}
					break;
				case 'n':  // numerical use previous states to avoid long runtimes
					// the tsince is the same as the previous step
					astPert::rk4(currobsrec.jd, currobsrec.jdf, tsince, xpert, derivType, cdam, cram, interp, opt, jpldearr, jdjpldestart, iau80rec, jdeopstart, eoparr, Xf);
					// update for next time step for speed 
					for (ii = 0; ii < 3; ii++)
					{
						recipert[ii] = Xf[ii][0];
						vecipert[ii] = Xf[ii + 3][0];
					}
					break;
				default:  // Kepler 2-body
					for (ii = 0; ii < 3; ii++)
					{
						rpert[ii] = xpert[ii][0];
						vpert[ii] = xpert[ii + 3][0];
					}
					ast2Body::kepler(rpert, vpert, currobsrec.dtmin*60.0, recipert, vecipert);
					for (ii = 0; ii < 3; ii++)
					{
						Xf[ii][0] = recipert[ii];
						Xf[ii + 3][0] = vecipert[ii];
					}
				}

				// now get vectors to ITRF for obs processing
				// eci to itrf if observation type
				if (currobsrec.obstype != 4)
				{
					mfme = currobsrec.hr * 60 + currobsrec.min + currobsrec.sec / 60.0;
					EopSpw::findeopparam(currobsrec.jd, mfme, interp, 'f', eoparr, jdeopstart,
						dut1, dat, lod, xp, yp, ddpsi, ddeps,
						iaudx, dy, icrsx, y, s, deltapsi, deltaeps);
					double jdut1f = currobsrec.jdf + dut1;
					jdut1 = currobsrec.jdf + dut1;
					tut1 = (jdut1 + jdut1f - 2451545.0) * jdyconv;
					double jdttf = currobsrec.jdf + dat + 32.184;   // sec
					ttt = (currobsrec.jdf + dat + 32.184 + jdttf - 2451545.0) * jdyconv;
					coordFK5::itrf_gcrf(ritrf, vitrf, aitrf, eFrom, r3, v3, aeci, iau80rec, ttt, jdut1, lod, xp, yp, 2, ddpsi, ddeps, deltapsi, deltaeps, trans);
				} // if obstype

				if (currobsrec.obstype == 3)
					astIOD::rv_tradec(ritrf, vitrf, rs, eTo, rngpert, trtascpert, tdeclpert, drngpert, dtrtascpert, dtdeclpert);
				else
				if (currobsrec.obstype == 4)
					bstarpert = satrec.bstar*(1.0 + percentchg);
				else  // currobsrec.obstype = 0 or 1 or 2
					astIOD::rv_razel(ritrf, vitrf, rs, currsenrec.senlat, currsenrec.senlon, eTo,
					    rngpert, azpert, elpert, drngpert, dazpert, delpert);
				switch (currobsrec.obstype)
				{
				case 0: {
							a[0][j] = (rngpert - rngnom) / deltaamt;
				}
					break;
				case 1: {
							a[0][j] = (azpert - aznom) / deltaamt;
							a[1][j] = (elpert - elnom) / deltaamt;
				}
					break;
				case 2: {
							a[0][j] = (rngpert - rngnom) / deltaamt;
							a[1][j] = (azpert - aznom) / deltaamt;
							a[2][j] = (elpert - elnom) / deltaamt;
				}
					break;
				case 3: {
							a[0][j] = (trtascpert - trtascnom) / deltaamt;
							a[1][j] = (tdeclpert - tdeclnom) / deltaamt;
				}
					break;
				case 4: { 
							a[0][j] = (recipert[0] - recinom[0]) / deltaamt;
							a[1][j] = (recipert[1] - recinom[1]) / deltaamt;
							a[2][j] = (recipert[2] - recinom[2]) / deltaamt;
							a[3][j] = (vecipert[0] - vecinom[0]) / deltaamt;
							a[4][j] = (vecipert[1] - vecinom[1]) / deltaamt;
							a[5][j] = (vecipert[2] - vecinom[2]) / deltaamt;
							a[6][j] = (bstarpert - bstarnom) / deltaamt;
				}
					break;
				} // case

//printf( "rpert %11.5f %11.5f %11.5f %8.3f %8.3f %8.3f \n",
//              r3[0], r3[1], r3[2], rngpert, azpert*rad, elpert*rad );  // ritrf
//printf("recipert %11.5f %11.5f %11.5f \n", recipert[0], recipert[1], recipert[2]);  // ritrf
//printf("xpert %11.5f %11.5f %11.5f \n", xpert[0][0], xpert[1][0], xpert[2][0]);  // ritrf

				// ----------------- reset the modified vector --------------------
				satrecp = satrec;
			}  // for j = 0 to statesize

			// reset state for numerical cases to avoid starting from scratch each time
			if (proptype == 'n')
			{
				for (ii = 0; ii < 3; ii++)
				{
					XCurrn[ii][0] = recinom[ii];
					XCurrn[ii+3][0] = vecinom[ii];
				}
			}

			// ----------------- now form the matrix combinations -----------------
			astMath::mattrans(a, at, indobs, statesize);

			// ------------------------- assign weights ---------------------------
			switch (currobsrec.obstype)
			{
			case 0: {
						w1 = 1.0 / (currsenrec.noiserng * currsenrec.noiserng);
						rng2 = rng2 + b[0][0] * b[0][0] * w1;
			}
				break;
			case 1: {
						w1 = 1.0 / (currsenrec.noiseaz * currsenrec.noiseaz);
						w2 = 1.0 / (currsenrec.noiseel * currsenrec.noiseel);
						daz2 = daz2 + b[0][0] * b[0][0] * w1;
						del2 = del2 + b[1][0] * b[1][0] * w2;
			}
				break;
			case 2: {
						w1 = 1.0 / (currsenrec.noiserng * currsenrec.noiserng);
						w2 = 1.0 / (currsenrec.noiseaz * currsenrec.noiseaz);
						w3 = 1.0 / (currsenrec.noiseel * currsenrec.noiseel);
						drng2 = drng2 + b[0][0] * b[0][0] * w1;
						daz2 = daz2 + b[1][0] * b[1][0] * w2;
						del2 = del2 + b[2][0] * b[2][0] * w3;
			}
				break;
			case 3: {
						w1 = 1.0 / (currsenrec.noisetrtasc * currsenrec.noisetrtasc);
						w2 = 1.0 / (currsenrec.noisetdecl * currsenrec.noisetdecl);
						dtrtasc2 = dtrtasc2 + b[0][0] * b[0][0] * w1;
						dtdecl2 = dtdecl2 + b[1][0] * b[1][0] * w2;
			}
				break;
			case 4: {
						w1 = 1.0 / (currsenrec.noisex * currsenrec.noisex);
						w2 = 1.0 / (currsenrec.noisey * currsenrec.noisey);
						w3 = 1.0 / (currsenrec.noisez * currsenrec.noisez);
						w4 = 1.0 / (currsenrec.noisexdot * currsenrec.noisexdot);
						w5 = 1.0 / (currsenrec.noiseydot * currsenrec.noiseydot);
						w6 = 1.0 / (currsenrec.noisezdot * currsenrec.noisezdot);
						w7 = 1.0 / (currsenrec.noisebstar * currsenrec.noisebstar);
						dx2 = dx2 + b[0][0] * b[0][0] * w1;
						dy2 = dy2 + b[1][0] * b[1][0] * w2;
						dz2 = dz2 + b[2][0] * b[2][0] * w3;
						dxdot2 = dxdot2 + b[3][0] * b[3][0] * w4;
						dydot2 = dydot2 + b[4][0] * b[4][0] * w5;
						dzdot2 = dzdot2 + b[5][0] * b[5][0] * w6;
						dbstar2 = dbstar2 + b[6][0] * b[6][0] * w7;
			}
				break;
			} // case

			for (rowc = 0; rowc < statesize; rowc++)
			{
				for (colc = 0; colc < indobs; colc++)
				{
					switch (colc)
					{
					case 0: weight = w1;
						break;
					case 1: weight = w2;
						break;
					case 2: weight = w3;
						break;
					case 3: weight = w4;
						break;
					case 4: weight = w5;
						break;
					case 5: weight = w6;
						break;
					case 6: weight = w7;
						break;
					default: weight = 1.0;
					}  // case
					atw[rowc][colc] = at[rowc][colc] * weight;
				}  // for colc
			} // for rowc

			// ----------------- find the atwa / atwb matrices --------------------
			astMath::matmult(atw, a, atwaacc, statesize, indobs, statesize);
			astMath::matmult(atw, b, atwbacc, statesize, indobs, 1);

			// ------------------- accumulate the matricies -----------------------
			for (r = 0; r < statesize; r++)
			for (c = 0; c < statesize; c++)
				atwa[r][c] = atwaacc[r][c] + atwa[r][c];

			c = 0;
			for (r = 0; r < statesize; r++)
				atwb[r][c] = atwbacc[r][c] + atwb[r][c];

		} // for i through the observations

		astMath::writeexpmat("atwa ", atwa, statesize, statesize);
		astMath::writeexpmat("atwb ", atwb, statesize, 1);
		astMath::writeexpmat("a ",a,indobs,statesize);
		astMath::writemat("b ", b, indobs, 1);
	}  // findatwaatwb


/* -----------------------------------------------------------------------------
*
*                           procedure finitediff
*
* this procedure perturbs the components of the state vector for processing
* with the finite differencing for the a matrix.
*
*  author        : david vallado                  719-573-2600   15 jan 2008
*
*  inputs          description                    range / units
*    whichconst  - parameter for sgp4 constants   wgs72, wgs721, wgs84
*    pertelem    - which element to perturb
*    percentchg  - amount to modify the vectors   0.001
*                  by in finite differencing
*    deltaamtchg - tolerance for small value in
*                  finite differencing            0.0000001
*    statetype   - type of elements (equinoctial, etc)  'e', 't'
*    xnom        - state vector                   varied
*
*  outputs       :
*    deltaamt    - amount each elemnt is perturbed
*    satrec      - satellite record
*
*  locals        :
*    jj          - index
*
*  coupling      :
*    state2satrec- conversion between state and satellite structure
*    sgp4init    - intiialize sgp4 constants
*
*  references    :
*    vallado       2007, 753-765
* --------------------------------------------------------------------------- */

	void finitediff
		(
		gravconsttype whichconst,
		int pertelem, double percentchg, double deltaamtchg, char statetype, int statesize, char proptype,
		elsetrec& satrec, 
		std::vector<double> xnom,
		std::vector< std::vector<double> > &xpert,
		double &deltaamt
		)
	{
		const double deg2rad = pi / 180.0;         //   0.0174532925199433
		const double xpdotp = 1440.0 / (2.0 * pi);  // 229.1831180523293
		xnom.resize(7);
		xpert.resize(7);
		for (std::vector< std::vector<double> >::iterator it = xpert.begin(); it != xpert.end(); ++it)
			it->resize(1);
		//		double tumin, mu, radiusearthkm, xke, j2, j3, j4, j3oj2;
		int jj;

		// chk if perturbing amt is too small. if so, up the percentchg and try again
		// this will execute 5 times, but leaves percentchg the same after each run
		jj = 1;
		do
		{
			deltaamt = xnom[pertelem] * percentchg;
			xnom[pertelem] = xnom[pertelem] + deltaamt;

			if (proptype == 's')
    			state2satrec(xnom, statetype, statesize, eTo, satrec);

			if (fabs(deltaamt) < deltaamtchg)       // 0.00001
			{
				percentchg = percentchg * 1.4;  // increase by 40% and try again
				// printf(" %2i percentchg chgd %11.8f ",jj,percentchg);
			}
			jj = jj + 1;
		} while ((fabs(deltaamt) < deltaamtchg) && (jj < 5));

		// printf(" \n");
		// ---- obtain various parameters ----
		//      satrec.a    = pow( satrec.no * tumin , (-2.0/3.0) );
		//      satrec.alta = satrec.a * (1.0 + satrec.ecco) - 1.0;
		//      satrec.altp = satrec.a * (1.0 - satrec.ecco) - 1.0;

		// ---------------- initialize the orbit at sgp4epoch --------------------
		if (proptype == 's')
			SGP4Funcs::sgp4init(whichconst, satrec.operationmode, satrec.satnum, satrec.jdsatepoch - 2433281.5, satrec.bstar,
			    satrec.ndot, satrec.nddot, satrec.ecco, satrec.argpo, satrec.inclo, satrec.mo, satrec.no_kozai,
			    satrec.nodeo, satrec);
		
		// assign perturbed vector
		for (jj = 0; jj < 6; jj++)
		{
			xpert[jj][0] = xnom[jj];
		}

	}  // finitediff


/* -----------------------------------------------------------------------------
*
*                           procedure leastsquares
*
* this procedure performs orbit determination using least squares differential
*   correction method. a variety of observation types are possible.
*
*  algorithm     : find the atwa and atwb matrices by accumulation
*                  calculate the matrix combinations
*
*  author        : david vallado                  719-573-2600    6 aug 2008
*
*  inputs          description                    range / units
*    percentchg  - amount to modify the vectors   0.001
*                  by in finite differencing
*    deltaamtchg - tolerance for small value in
*                  finite differencing            0.0000001
*    epsilon     - tolerance for calculations     0.0002
*    whichconst  - parameter for sgp4 for constants wgs72, wgs721, wgs84
*    typeans     - type of dc (bksub or svd)      'b','s'
*    firstob     - first obs record to use        0
*    lastob      - last obs record to use         100
*    statesize   - size of state                  6 , 7
*    obsrecarr   - 10000 array structure containing:
*                  senum, jd, rsvec, obstype,
*                  rng, az, el, drng, daz, del,
*                  trtasc, tdecl data
*    loops       - number of iterations
*    satrec      - record of satellite parameters for TLE
*    satrecx     - record of satellite parameters for TLE answer
*                  this exists for test purposes only
*    statetype   - type of elements (equinoctial, etc)  'e', 't'
*    outfile, outfile1 - output files
*
*  outputs       :
*    xnom        - nominal state vector
*    x           - state vector
*    dx          - state correction
*    atwai       - covariance matrix
*    atwa        - atwa matrix - needed for svd processing
*    atwb        - atwab matrix
*    satrec      - record of satellite parameters for TLE
*
*  locals        :
*    numwork     - number of observations working with
*    sigmaold    -
*    sigmanew    -
*    drng2       -
*    daz2        -
*    del2        -
*    dtrtasc2    -
*    dtdecl2     -
*
*  coupling      :
*    findatwaatwb- find combination matrices for use in final equations
*    matinverse  - matrix inverse routine
*    matmult     - matrix multiply routine
*
*  references    :
*    vallado       2007, 753-765
* --------------------------------------------------------------------------- */

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
		)
	{
		int    i, numwork, ok;  // j,
		obsrec currobsrec;
		double sigmanew, sigmaold, sigmaold2, drng2, daz2, del2, ddrng2, ddaz2, ddel2, dtrtasc2,
			dtdecl2, dx2, dy2, dz2, dxdot2, dydot2, dzdot2, dbstar2, wtest, wmax;
		elsetrec satrecorig;  // for restarting cases
		char limited, restart;
		xnom.resize(7);  // rows
		x.resize(7);  // rows
		for (std::vector< std::vector<double> >::iterator it = x.begin(); it != x.end(); ++it)
			it->resize(1);
		dx.resize(7);  // rows
		for (std::vector< std::vector<double> >::iterator it = dx.begin(); it != dx.end(); ++it)
			it->resize(1);
		atwai.resize(7);  // rows
		for (std::vector< std::vector<double> >::iterator it = atwai.begin(); it != atwai.end(); ++it)
			it->resize(7);
		atwa.resize(7);  // rows
		for (std::vector< std::vector<double> >::iterator it = atwa.begin(); it != atwa.end(); ++it)
			it->resize(7);
		atwb.resize(7);  // rows
		for (std::vector< std::vector<double> >::iterator it = atwb.begin(); it != atwb.end(); ++it)
			it->resize(7);


		double rad;
		int indobs = 7;
		std::vector< std::vector<double> > tmmp = std::vector< std::vector<double> >(statesize, std::vector<double>(statesize));
		std::vector< std::vector<double> > b = std::vector< std::vector<double> >(indobs, std::vector<double>(1));
		std::vector< std::vector<double> > atw = std::vector< std::vector<double> >(statesize, std::vector<double>(indobs));
		std::vector< std::vector<double> > u = std::vector< std::vector<double> >(statesize, std::vector<double>(indobs));
		std::vector< std::vector<double> > w = std::vector< std::vector<double> >(indobs, std::vector<double>(1));
		std::vector< std::vector<double> > wutatwb = std::vector< std::vector<double> >(indobs, std::vector<double>(1));
		std::vector< std::vector<double> > ww = std::vector< std::vector<double> >(indobs, std::vector<double>(indobs));
		std::vector< std::vector<double> > v = std::vector< std::vector<double> >(indobs, std::vector<double>(indobs));
		std::vector< std::vector<double> > ut = std::vector< std::vector<double> >(indobs, std::vector<double>(statesize));
		std::vector< std::vector<double> > vw = std::vector< std::vector<double> >(indobs, std::vector<double>(indobs));
		std::vector< std::vector<double> > vwut = std::vector< std::vector<double> >(indobs, std::vector<double>(1));
		std::vector< std::vector<double> > atwao = std::vector< std::vector<double> >(statesize, std::vector<double>(statesize));

		std::vector< double> limitdx(indobs);

		// ---------------------- initialize parameters ---------------------------
		restart = 'n';
		satrecorig = satrec;
		rad = 180.0 / pi;

		sigmaold = 20000.0;
		sigmanew = 10000.0;

		numwork = lastob - firstob;   // old +2

		limitdx[0] = 0.80 * xnom[0];
		limitdx[1] = 0.80 * xnom[1];
		limitdx[2] = 0.30 * xnom[2];
		limitdx[3] = 0.40 * xnom[3];
		limitdx[4] = 0.40 * xnom[4];
		limitdx[5] = 0.40 * xnom[5];
		if (statesize == 7)
			limitdx[6] = 0.10 * xnom[6];

		fprintf(outfile, "nom coe %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f \n",
			xnom[0], xnom[1], xnom[2], xnom[3], xnom[4], xnom[5]);

		// -------------------------- loop through the iterations --------------------------- 
		loops = 0;
		do
		{
			sigmaold2 = sigmaold;
			sigmaold = sigmanew;
			drng2 = 0.0;
			daz2 = 0.0;
			del2 = 0.0;
			ddrng2 = 0.0;
			ddaz2 = 0.0;
			ddel2 = 0.0;
			dtrtasc2 = 0.0;
			dtdecl2 = 0.0;
			dx2 = 0.0;
			dy2 = 0.0;
			dz2 = 0.0;
			dxdot2 = 0.0;
			dydot2 = 0.0;
			dzdot2 = 0.0;
			dbstar2 = 0.0;

			// ---- place current nominal value into the state vector ----
			findatwaatwb(firstob, lastob, statesize, percentchg, deltaamtchg,
				whichconst, interp, jdeopstart, satrec, obsrecarr, statetype, proptype, xnom,
				derivType, cdam, cram, opt, jpldearr, jdjpldestart, iau80rec, eoparr,
				drng2, daz2, del2, ddrng2, ddaz2, ddel2, dtrtasc2, dtdecl2,
				dx2, dy2, dz2, dxdot2, dydot2, dzdot2, atwa, atwb, atw, b);

			// matrix inversion approach
			if (typeans == 'b')
			{
				astMath::matinverse(atwa, statesize, atwai);
				astMath::matmult(atwai, atwb, dx, statesize, statesize, 1);
				// writeexpmat("atwai1",atwai,statesize,statesize);
				// matmult   ( atwai,atwa,tmmp, statesize,statesize,statesize );
				// writeexpmat("tmmp ",tmmp,statesize,statesize);
			}

			// svd approach for matrix inversion
			if (typeans == 's')
			{
				atwao = atwa; // save for later
				ok = dsvdcmp(atwa, statesize, statesize, w, v);
				wmax = 0.0;
				for (i = 0; i < statesize; i++)
				{
					if (w[i][0] > wmax)
						wmax = w[i][0];
				}
				wtest = 1.0e-14 * wmax;  // -10 seems to be a limit
				for (i = 0; i < statesize; i++)
				{
					if (w[i][0] <= wtest)
						w[i][0] = 1.0e-10; // set to 0.0 with dsvbksb 1e-10 else, delete bad term
					ww[i][i] = 1.0 / w[i][0];
				}

				// dsvbksb(atwa, w, v, statesize, indobs, b, dx);
				astMath::matmult(v, ww, vw, statesize, statesize, statesize);
				astMath::mattrans(atwa, ut, statesize, statesize);
				astMath::matmult(vw, ut, atwai, statesize, statesize, statesize);
				astMath::matmult(atwai, atwb, dx, statesize, statesize, 1);
				// writeexpmat("atwai2",atwai,statesize,statesize);
				// matmult   ( atwai,atwao,tmmp, statesize,statesize,statesize );
				// writeexpmat("tmmp ",tmmp,statesize,statesize);
			}

			// ---- update state vector ----
			limited = 'n';
			for (i = 0; i < statesize; i++)
			{
				/*
				if ( fabs(dx[i][0]) > fabs(0.4 * xnom[i]) )  // 0.40   .8xx seem to be the same
				{
				if (fabs(dx[i][0]/xnom[i]) > 1.0)
				{
				//                       printf("z %1i delta %11.7lf %11.7lf  %11.7lf ",i, xnom[i],dx[i][0]/xnom[i], dx[i][0]);
				dx[i][0] = dx[i][0] * 0.4;   // 0.4 usually needed for bstar solutions, 0.01 for observations
				// i think I like the following corrections better, but the previous ones seem to work better...
				//                     dx[i][0] = 0.10 * xnom[i] * sgn(dx[i][0]);   // 0.90 0.1xx try leaving the same
				limited = 'y';
				//  printf(" new %11.7lf \n",dx[i][0]);
				}
				else
				{
				//                       printf("y %1i delta %11.7lf %11.7lf  %11.7lf ",i, xnom[i],dx[i][0]/xnom[i], dx[i][0]);
				dx[i][0] = dx[i][0] * 0.9;   // 0.90 0.1xx try leaving the same
				//                     dx[i][0] = 0.9 * xnom[i] * sgn(dx[i][0]);   // 0.90 0.1xx try leaving the same
				limited = 'z';
				// printf(" %i limit correction", i );
				}
				//printf("input statesize (6 or 7) \n");
				//scanf( "%i",&statesize );
				//printf("a %1i delta %11.7lf %11.7lf  %11.7lf \n",i, xnom[i],dx[i][0]/xnom[i], dx[i][0]);
				}
				*/
				if (statetype != 'v')
				{
					if ((loops > -1) && (fabs(dx[i][0] / xnom[i]) > 1000.0))   // 100
					{
						dx[i][0] = 0.10 * xnom[i] * astMath::sgn(dx[i][0]);   // 0.30 try leaving the same
						//printf(" 0.00 \n");
					}
					else
					if ((loops > 0) && (fabs(dx[i][0] / xnom[i]) > 200.0))   // 100
					{
						dx[i][0] = 0.30 * xnom[i] * astMath::sgn(dx[i][0]);   // 0.30 try leaving the same
						//printf(" 0.10 \n");
					}
					else
					{
						if ((loops > 0) && (fabs(dx[i][0] / xnom[i]) > 100.0))  // 20
						{
							dx[i][0] = 0.70 * xnom[i] * astMath::sgn(dx[i][0]);   // 0.70 - 0.80 about same
							//printf(" 0.20 \n");
						}
						else
						if ((loops > 0) && (fabs(dx[i][0] / xnom[i]) > 10.0))      // 5
						{
							dx[i][0] = 0.90 * xnom[i] * astMath::sgn(dx[i][0]);   // 0.90 try leaving the same
							//printf(" 0.30 \n");
						}
					}
				}  // limit corrections if not vectors
				else
				{
					if (fabs(dx[i][0] / xnom[i]) > 0.01)
						dx[i][0] = 0.01 * xnom[i] * astMath::sgn(dx[i][0]);
				}  // limit corrections if vectors

				xnom[i] = xnom[i] + dx[i][0];
			}  // for i

			// -------------- re-initialize state with new values -----------------
			if (proptype == 's')
			{
				state2satrec(xnom, statetype, statesize, eTo, satrec);
				SGP4Funcs::sgp4init(whichconst, satrec.operationmode, satrec.satnum, satrec.jdsatepoch - 2433281.5, satrec.bstar,
					satrec.ndot, satrec.nddot, satrec.ecco, satrec.argpo, satrec.inclo, satrec.mo, satrec.no_kozai,
					satrec.nodeo, satrec);
			}

			currobsrec = obsrecarr[0];
			switch (currobsrec.obstype)
			{
			case 0: sigmanew = sqrt(drng2 / numwork);
				break;
			case 1: sigmanew = sqrt((daz2 + del2) / numwork);
				break;
			case 2: sigmanew = sqrt((drng2 + daz2 + del2) / numwork);
				break;
			case 3: sigmanew = sqrt((dtrtasc2 + dtdecl2) / numwork);
				break;
			case 4: sigmanew = sqrt((dx2 + dy2 + dz2 + dxdot2 + dydot2 + dzdot2) / numwork);
				break;
			} // case

			// ----------------------- write out data -----------------------------
			fprintf(outfile, "corrections  %12.7f %12.7f %12.7f %12.7f %12.7f %12.7f ",
				dx[0][0], dx[1][0], dx[2][0], dx[3][0], dx[4][0], dx[5][0]);
			fprintf(outfile, "loop %2i  nominal %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f ",
				loops, xnom[0], xnom[1], xnom[2], xnom[3], xnom[4], xnom[5]);
			fprintf(outfile, "sigold %12.6f %1c  %12.6f %12.6f %12.6f\n", sigmanew,
				limited, sigmaold2, sigmaold, sigmanew);
			// printf( "sigmaold %12.6f  sigmanew %12.6f \n",sigmaold,sigmanew );
			// fprintf( outfile," ---------------------------------------------------- \n" );
			loops = loops + 1;

			// try not solving for bstar if the iterations are diverging rapidly
			if ((sigmanew > sigmaold) && (sigmaold > sigmaold2) && (sigmanew > 500000.0) && (restart == 'n'))   // 1000
			{
				restart = 'y';
				sigmaold = 20000.0;
				sigmanew = 10000.0;
				loops = 0;
				printf("restarted\n");
				fprintf(outfile, "restarted\n");
				fprintf(outfile1, "restarted\n");
				satrec = satrecorig;
				state2satrec(xnom, statetype, statesize, eFrom, satrec);
			}

		}  
		while (((fabs((sigmanew - sigmaold) / sigmaold) >= epsilon) && (loops < 25) && (sigmanew >= epsilon)) &&
		!((sigmanew > sigmaold) && (sigmaold > sigmaold2) && (sigmanew > 500000.0))); // 1000
		// several conditions apply for a stopping condition:
		// changes become small,
		// max iterations met,
		// overall change is negligible,
		// iterations are diverging
		// while through all the iterations

		if (proptype == 's')
		{
			fprintf(outfile, "loop %2i  nom coe %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f \n",
				loops, satrec.no_kozai*1440.0 / (2.0 * pi), satrec.ecco, satrec.inclo*rad, satrec.nodeo*rad,
				satrec.argpo*rad, satrec.mo*rad);
			fprintf(outfile, "loop %2i  ans coe %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f \n",
				loops, satrecx.no_kozai*1440.0 / (2.0 * pi), satrecx.ecco, satrecx.inclo*rad, satrecx.nodeo*rad,
				satrecx.argpo*rad, satrecx.mo*rad);
			fprintf(outfile1, "loop %2i  nom coe %12.3f %12.8f %12.8f %12.8f %13.8f %13.8f %12.8f %12.8f\n",
				loops, satrec.a*6378.135, satrec.no_kozai*1440.0 / (2.0 * pi), satrec.ecco, satrec.inclo*rad, satrec.nodeo*rad,
				satrec.argpo*rad, satrec.mo*rad, satrec.bstar);
			fprintf(outfile1, "loop %2i  ans coe  %12.3f %12.8f %12.8f %12.8f %13.8f %13.8f %12.8f %12.8f",
				loops, satrecx.a*6378.135, satrecx.no_kozai*1440.0 / (2.0 * pi), satrecx.ecco, satrecx.inclo*rad, satrecx.nodeo*rad,
				satrecx.argpo*rad, satrecx.mo*rad, satrecx.bstar);
			fprintf(outfile, "3 pos component uncertainty\n");
		}
		else
		{
			fprintf(outfile, "loop %2i  nom coe %12.8f %12.8f %12.8f %12.8f %12.8f %12.8f \n",
				loops, xnom[0], xnom[1], xnom[2], xnom[3], xnom[4], xnom[5]);
		}

		// printtle(satrec);

	}  // leastsquares

}  // namespace

