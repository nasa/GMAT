/* nrlmsise00_sub.f -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "f2c.h"

/* Common Block Declarations */

struct {
    real tlb, s, db04, db16, db28, db32, db40, db48, db01, za, t0, z0, g0, rl,
	     dd, db14, tr12;
} gts3c_;

#define gts3c_1 gts3c_

struct {
    real tn1[5], tn2[4], tn3[5], tgn1[2], tgn2[2], tgn3[2];
} meso7_;

#define meso7_1 meso7_

struct lower7_1_ {
    real ptm[10], pdm[80]	/* was [10][8] */;
};

#define lower7_1 (*(struct lower7_1_ *) &lower7_)

struct parm7_1_ {
    real pt[150], pd[1350]	/* was [150][9] */, ps[150], pdl[50]	/* 
	    was [25][2] */, ptl[400]	/* was [100][4] */, pma[1000]	/* 
	    was [100][10] */, sam[100];
};
struct parm7_2_ {
    real pt1[50], pt2[50], pt3[50], pa1[50], pa2[50], pa3[50], pb1[50], pb2[
	    50], pb3[50], pc1[50], pc2[50], pc3[50], pd1[50], pd2[50], pd3[50]
	    , pe1[50], pe2[50], pe3[50], pf1[50], pf2[50], pf3[50], pg1[50], 
	    pg2[50], pg3[50], ph1[50], ph2[50], ph3[50], pi1[50], pi2[50], 
	    pi3[50], pj1[50], pj2[50], pj3[50], pk1[50], pl1[50], pl2[50], 
	    pm1[50], pm2[50], pn1[50], pn2[50], po1[50], po2[50], pp1[50], 
	    pp2[50], pq1[50], pq2[50], pr1[50], pr2[50], ps1[50], ps2[50], 
	    pu1[50], pu2[50], pv1[50], pv2[50], pw1[50], pw2[50], px1[50], 
	    px2[50], py1[50], py2[50], pz1[50], pz2[50], paa1[50], paa2[50];
};

#define parm7_1 (*(struct parm7_1_ *) &parm7_)
#define parm7_2 (*(struct parm7_2_ *) &parm7_)

struct datim7_1_ {
    integer isd[3], ist[2], nam[2];
};
struct datim7_2_ {
    integer isdate[3], istime[2], name__[2];
};

#define datim7_1 (*(struct datim7_1_ *) &datim7_)
#define datim7_2 (*(struct datim7_2_ *) &datim7_)

struct {
    integer isdate[3], istime[2], name__[2];
} datime_;

#define datime_1 datime_

struct {
    real sw[25];
    integer isw;
    real swc[25];
} csw_;

#define csw_1 csw_

struct mavg7_1_ {
    real pavgm[10];
};

#define mavg7_1 (*(struct mavg7_1_ *) &mavg7_)

struct {
    real dm04, dm16, dm28, dm32, dm40, dm01, dm14;
} dmix_;

#define dmix_1 dmix_

struct {
    real gsurf, re;
} parmb_;

#define parmb_1 parmb_

struct metsel_1_ {
    integer imr;
};

#define metsel_1 (*(struct metsel_1_ *) &metsel_)

union {
    struct {
	real tinfg, gb, rout, tt[15];
    } _1;
    struct {
	real tinf, gb, rout, t[15];
    } _2;
} ttest_;

#define ttest_1 (ttest_._1)
#define ttest_2 (ttest_._2)

union {
    struct {
	real plg[36]	/* was [9][4] */, ctloc, stloc, c2tloc, s2tloc, 
		c3tloc, s3tloc;
	integer iyr;
	real day, df, dfa, apd, apdf, apt[4], xlong;
    } _1;
    struct {
	real plg[36]	/* was [9][4] */, ctloc, stloc, c2tloc, s2tloc, 
		c3tloc, s3tloc;
	integer iyr;
	real day, df, dfa, apd, apdf, apt[4], long__;
    } _2;
} lpoly_;

#define lpoly_1 (lpoly_._1)
#define lpoly_2 (lpoly_._2)

struct {
    integer mp, ii, jg, lt;
    real qpb[50];
    integer ierr, ifun, n, j;
    real dv[60];
} lsqv_;

#define lsqv_1 lsqv_

struct {
    real taf;
} fit_;

#define fit_1 fit_

/* Initialized data */

struct {
    integer e_1;
    } metsel_ = { 0 };

struct {
    real e_1[90];
    } lower7_ = { (float)1041.3, (float)386., (float)195., (float)16.6728, (
	    float)213., (float)120., (float)240., (float)187., (float)-2., (
	    float)0., (float)2.456e7, (float)6.71072e-6, (float)100., (float)
	    0., (float)110., (float)10., (float)0., (float)0., (float)0., (
	    float)0., (float)8.594e10, (float)1., (float)105., (float)-8., (
	    float)110., (float)10., (float)90., (float)2., (float)0., (float)
	    0., (float)2.81e11, (float)0., (float)105., (float)28., (float)
	    28.95, (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)3.3e10, (float).26827, (float)105., (float)1., (float)110., 
	    (float)10., (float)110., (float)-10., (float)0., (float)0., (
	    float)1.33e9, (float).0119615, (float)105., (float)0., (float)
	    110., (float)10., (float)0., (float)0., (float)0., (float)0., (
	    float)176100., (float)1., (float)95., (float)-8., (float)110., (
	    float)10., (float)90., (float)2., (float)0., (float)0., (float)
	    1e7, (float)1., (float)105., (float)-8., (float)110., (float)10., 
	    (float)90., (float)2., (float)0., (float)0., (float)1e6, (float)
	    1., (float)105., (float)-8., (float)550., (float)76., (float)90., 
	    (float)2., (float)0., (float)4e3 };

struct {
    real e_1[3200];
    } parm7_ = { (float).986573, (float).0162228, (float).015527, (float)
	    -.104323, (float)-.00375801, (float)-.00118538, (float)-.124043, (
	    float).0045682, (float).00876018, (float)-.136235, (float)
	    -.0352427, (float).00884181, (float)-.00592127, (float)-8.6165, (
	    float)0., (float).0128492, (float)0., (float)130.096, (float)
	    .0104567, (float).00165686, (float)-5.53887e-6, (float).0029781, (
	    float)0., (float).00513122, (float).0866784, (float).158727, (
	    float)0., (float)0., (float)0., (float)-7.27026e-6, (float)0., (
	    float)6.74494, (float).00493933, (float).00221656, (float)
	    .00250802, (float)0., (float)0., (float)-.0208841, (float)
	    -1.79873, (float).00145103, (float)2.81769e-4, (float)-.00144703, 
	    (float)-5.16394e-5, (float).0847001, (float).170147, (float)
	    .00572562, (float)5.07493e-5, (float).00436148, (float)1.17863e-4,
	     (float).00474364, (float).00661278, (float)4.34292e-5, (float)
	    .00144373, (float)2.4147e-5, (float).00284426, (float)8.5656e-4, (
	    float).00204028, (float)0., (float)-3159.94, (float)-.00246423, (
	    float).00113843, (float)4.20512e-4, (float)0., (float)-97.7214, (
	    float).00677794, (float).00527499, (float).00114936, (float)0., (
	    float)-.00661311, (float)-.0184255, (float)-.0196259, (float)
	    29861.8, (float)0., (float)0., (float)0., (float)644.574, (float)
	    8.84668e-4, (float)5.05066e-4, (float)0., (float)4028.81, (float)
	    -.00189503, (float)0., (float)0., (float)8.21407e-4, (float)
	    .0020678, (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)-.012041, (float)-.00363963, (float)9.9207e-5, (float)
	    -1.15284e-4, (float)-6.33059e-5, (float)-.605545, (float)
	    .00834218, (float)-91.3036, (float)3.71042e-4, (float)0., (float)
	    4.19e-4, (float).00270928, (float).00331507, (float)-.00444508, (
	    float)-.00496334, (float)-.00160449, (float).00395119, (float)
	    .00248924, (float)5.09815e-4, (float).00405302, (float).00224076, 
	    (float)0., (float).00684256, (float)4.66354e-4, (float)0., (float)
	    -3.68328e-4, (float)0., (float)0., (float)-146.87, (float)0., (
	    float)0., (float).00109501, (float)4.65156e-4, (float)5.62583e-4, 
	    (float)3.21596, (float)6.43168e-4, (float).0031486, (float)
	    .00340738, (float).00178481, (float)9.62532e-4, (float)5.58171e-4,
	     (float)3.43731, (float)-.233195, (float)5.10289e-4, (float)0., (
	    float)0., (float)-92534.7, (float)0., (float)-.00199639, (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)1.09979, (
	    float)-.048806, (float)-.197501, (float)-.091028, (float)
	    -.00696558, (float).0242136, (float).391333, (float)-.00720068, (
	    float)-.0322718, (float)1.41508, (float).168194, (float).0185282, 
	    (float).109384, (float)-7.24282, (float)0., (float).296377, (
	    float)-.049721, (float)104.114, (float)-.0861108, (float)
	    -7.29177e-4, (float)1.48998e-6, (float).00108629, (float)0., (
	    float)0., (float).083109, (float).112818, (float)-.0575005, (
	    float)-.0129919, (float)-.0178849, (float)-2.86343e-6, (float)0., 
	    (float)-151.187, (float)-.00665902, (float)0., (float)-.00202069, 
	    (float)0., (float)0., (float).0432264, (float)-28.0444, (float)
	    -.00326789, (float).00247461, (float)0., (float)0., (float).09821,
	     (float).122714, (float)-.039645, (float)0., (float)-.00276489, (
	    float)0., (float).00187723, (float)-.00809813, (float)4.34428e-5, 
	    (float)-.00770932, (float)0., (float)-.00228894, (float)-.0056907,
	     (float)-.00522193, (float).00600692, (float)-7804.34, (float)
	    -.00348336, (float)-.00638362, (float)-.0018219, (float)0., (
	    float)-75.8976, (float)-.0217875, (float)-.0172524, (float)
	    -.00906287, (float)0., (float).0244725, (float).086604, (float)
	    .105712, (float)30254.3, (float)0., (float)0., (float)0., (float)
	    -6013.64, (float)-.00564668, (float)-.00254157, (float)0., (float)
	    315.611, (float)-.00569158, (float)0., (float)0., (float)
	    -.00447216, (float)-.00449523, (float).00464428, (float)0., (
	    float)0., (float)0., (float)0., (float).0451236, (float).024652, (
	    float).00617794, (float)0., (float)0., (float)-.362944, (float)
	    -.0480022, (float)-75.723, (float)-.00199656, (float)0., (float)
	    -.0051878, (float)-.017399, (float)-.00903485, (float).00748465, (
	    float).0153267, (float).0106296, (float).0118655, (float)
	    .00255569, (float).0016902, (float).0351936, (float)-.0181242, (
	    float)0., (float)-.100529, (float)-.00510574, (float)0., (float)
	    .00210228, (float)0., (float)0., (float)-173.255, (float).507833, 
	    (float)-.241408, (float).00875414, (float).00277527, (float)
	    -8.90353e-5, (float)-5.25148, (float)-.00583899, (float)-.0209122,
	     (float)-.0096353, (float).00977164, (float).00407051, (float)
	    2.53555e-4, (float)-5.52875, (float)-.355993, (float)-.00249231, (
	    float)0., (float)0., (float)28.6026, (float)0., (float)3.42722e-4,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    1.02315, (float)-.15971, (float)-.10663, (float)-.0177074, (float)
	    -.00442726, (float).0344803, (float).0445613, (float)-.0333751, (
	    float)-.0573598, (float).35036, (float).0633053, (float).0216221, 
	    (float).0542577, (float)-5.74193, (float)0., (float).190891, (
	    float)-.0139194, (float)101.102, (float).0816363, (float)
	    1.33717e-4, (float)6.54403e-6, (float).00310295, (float)0., (
	    float)0., (float).0538205, (float).12391, (float)-.0139831, (
	    float)0., (float)0., (float)-3.95915e-6, (float)0., (float)
	    -.714651, (float)-.00501027, (float)0., (float)-.00324756, (float)
	    0., (float)0., (float).0442173, (float)-13.1598, (float)
	    -.00315626, (float).00124574, (float)-.00147626, (float)
	    -.00155461, (float).0640682, (float).134898, (float)-.0242415, (
	    float)0., (float)0., (float)0., (float)6.13666e-4, (float)
	    -.00540373, (float)2.61635e-5, (float)-.00333012, (float)0., (
	    float)-.00308101, (float)-.00242679, (float)-.00336086, (float)0.,
	     (float)-1189.79, (float)-.0504738, (float)-.00261547, (float)
	    -.00103132, (float)1.91583e-4, (float)-83.8132, (float)-.0140517, 
	    (float)-.0114167, (float)-.00408012, (float)1.73522e-4, (float)
	    -.0139644, (float)-.0664128, (float)-.0685152, (float)-13441.4, (
	    float)0., (float)0., (float)0., (float)607.916, (float)-.0041222, 
	    (float)-.00220996, (float)0., (float)1702.77, (float)-.00463015, (
	    float)0., (float)0., (float)-.0022536, (float)-.00296204, (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float).0392786, (
	    float).0131186, (float)-.00178086, (float)0., (float)0., (float)
	    -.390083, (float)-.0284741, (float)-77.84, (float)-.00102601, (
	    float)0., (float)-7.26485e-4, (float)-.00542181, (float)
	    -.00559305, (float).0122825, (float).0123868, (float).00668835, (
	    float)-.0103303, (float)-.00951903, (float)2.70021e-4, (float)
	    -.0257084, (float)-.013243, (float)0., (float)-.0381, (float)
	    -.0031681, (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)-9.05762e-4, (float)-.0021459, (
	    float)-.00117824, (float)3.66732, (float)-3.79729e-4, (float)
	    -.00613966, (float)-.00509082, (float)-.00196332, (float)
	    -.0030828, (float)-9.75222e-4, (float)4.03315, (float)-.25271, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    1.16112, (float)0., (float)0., (float).0333725, (float)0., (float)
	    .0348637, (float)-.00544368, (float)0., (float)-.067394, (float)
	    .174754, (float)0., (float)0., (float)0., (float)174.712, (float)
	    0., (float).126733, (float)0., (float)103.154, (float).0552075, (
	    float)0., (float)0., (float)8.13525e-4, (float)0., (float)0., (
	    float).0866784, (float).158727, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)-25.0482, (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.00248894, (float)6.16053e-4, (float)-5.79716e-4, (float)
	    .00295482, (float).0847001, (float).170147, (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)2.47425e-5, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float).944846, (float)0., (float)0., (float)
	    -.0308617, (float)0., (float)-.0244019, (float).00648607, (float)
	    0., (float).0308181, (float).0459392, (float)0., (float)0., (
	    float)0., (float)174.712, (float)0., (float).021326, (float)0., (
	    float)-356.958, (float)0., (float)1.82278e-4, (float)0., (float)
	    3.07472e-4, (float)0., (float)0., (float).0866784, (float).158727,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float).00383054, (float)0., (float)0., (float)-.00193065, (
	    float)-.0014509, (float)0., (float)0., (float)0., (float)0., (
	    float)-.00123493, (float).00136736, (float).0847001, (float)
	    .170147, (float).00371469, (float)0., (float)0., (float)0., (
	    float)0., (float).0051025, (float)2.47425e-5, (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float).00368756, (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)1.3558, (float).144816, (float)0., (float)
	    .0607767, (float)0., (float).0294777, (float).07469, (float)0., (
	    float)-.0923822, (float).0857342, (float)0., (float)0., (float)0.,
	     (float)23.8636, (float)0., (float).0771653, (float)0., (float)
	    81.8751, (float).0187736, (float)0., (float)0., (float).0149667, (
	    float)0., (float)0., (float).0866784, (float).158727, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)-367.874, (
	    float).00548158, (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float).0847001, (float).170147, (float).0122631, (float)0., (
	    float)0., (float)0., (float)0., (float).00817187, (float)
	    3.71617e-5, (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)-.00210826, (float)-.0031364, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.0735742, (float)-.0500266, (float)0., (float)0., (float)0., (
	    float)0., (float).0194965, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)1.04761, (float).200165, (float)
	    .237697, (float).0368552, (float)0., (float).0357202, (float)
	    -.214075, (float)0., (float)-.108018, (float)-.373981, (float)0., 
	    (float).0310022, (float)-.00116305, (float)-20.7596, (float)0., (
	    float).0864502, (float)0., (float)97.4908, (float).0516707, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    .0866784, (float).158727, (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)346.193, (float).0134297, (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)-.00348509,
	     (float)-1.54689e-4, (float)0., (float)0., (float).0847001, (
	    float).170147, (float).0147753, (float)0., (float)0., (float)0., (
	    float)0., (float).018932, (float)3.68181e-5, (float).013257, (
	    float)0., (float)0., (float).00359719, (float).00744328, (float)
	    -.00100023, (float)-6505.28, (float)0., (float).0103485, (float)
	    -.00100983, (float)-.00406916, (float)-66.0864, (float)-.0171533, 
	    (float).0110605, (float).01203, (float)-.00520034, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)-2627.69, (float).00713755, (float).00417999, (float)0., (
	    float)12591., (float)0., (float)0., (float)0., (float)-.00223595, 
	    (float).00460217, (float).00571794, (float)0., (float)0., (float)
	    0., (float)0., (float)-.0318353, (float)-.0235526, (float)
	    -.0136189, (float)0., (float)0., (float)0., (float).0203522, (
	    float)-66.7837, (float)-.00109724, (float)0., (float)-.0138821, (
	    float).0160468, (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float).0151574, (float)
	    -5.4447e-4, (float)0., (float).0728224, (float).0659413, (float)
	    0., (float)-.00515692, (float)0., (float)0., (float)-3703.67, (
	    float)0., (float)0., (float).0136131, (float).00538153, (float)0.,
	     (float)4.76285, (float)-.0175677, (float).0226301, (float)0., (
	    float).0176631, (float).00477162, (float)0., (float)5.39354, (
	    float)0., (float)-.0075171, (float)0., (float)0., (float)-88.2736,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)1.26376, (float)-.214304, (float)-.149984, (
	    float).230404, (float).0298237, (float).0268673, (float).296228, (
	    float).02219, (float)-.0207655, (float).452506, (float).120105, (
	    float).032442, (float).0424816, (float)-9.14313, (float)0., (
	    float).0247178, (float)-.0288229, (float)81.2805, (float).051038, 
	    (float)-.00580611, (float)2.51236e-5, (float)-.0124083, (float)0.,
	     (float)0., (float).0866784, (float).158727, (float)-.034819, (
	    float)0., (float)0., (float)2.89885e-5, (float)0., (float)153.595,
	     (float)-.0168604, (float)0., (float).0101015, (float)0., (float)
	    0., (float)0., (float)0., (float)2.84552e-4, (float)-.00122181, (
	    float)0., (float)0., (float).0847001, (float).170147, (float)
	    -.0104927, (float)0., (float)0., (float)0., (float)-.00591313, (
	    float)-.0230501, (float)3.14758e-5, (float)0., (float)0., (float)
	    .0126956, (float).00835489, (float)3.10513e-4, (float)0., (float)
	    3421.19, (float)-.00245017, (float)-4.27154e-4, (float)5.45152e-4,
	     (float).00189896, (float)28.9121, (float)-.00649973, (float)
	    -.0193855, (float)-.0148492, (float)0., (float)-.0510576, (float)
	    .0787306, (float).0951981, (float)-14942.2, (float)0., (float)0., 
	    (float)0., (float)265.503, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float).0063711, (
	    float)3.24789e-4, (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float).0614274, (float).0100376, (float)-8.41083e-4, (
	    float)0., (float)0., (float)0., (float)-.0127099, (float)0., (
	    float)0., (float)0., (float)-.00394077, (float)-.0128601, (float)
	    -.00797616, (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)-.00671465, (float)-.00169799, (float)
	    .00193772, (float)3.8114, (float)-.0077929, (float)-.0182589, (
	    float)-.012586, (float)-.0104311, (float)-.00302465, (float)
	    .00243063, (float)3.63237, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)70.9557, (float)-.32674, (
	    float)0., (float)-.516829, (float)-.00171664, (float).090931, (
	    float)-.6715, (float)-.147771, (float)-.0927471, (float)-.230862, 
	    (float)-.15641, (float).0134455, (float)-.119717, (float)2.52151, 
	    (float)0., (float)-.241582, (float).0592939, (float)4.39756, (
	    float).091528, (float).00441292, (float)0., (float).00866807, (
	    float)0., (float)0., (float).0866784, (float).158727, (float)
	    .0974701, (float)0., (float)0., (float)0., (float)0., (float)
	    67.0217, (float)-.0013166, (float)0., (float)-.0165317, (float)0.,
	     (float)0., (float).0850247, (float)27.7428, (float).00498658, (
	    float).00615115, (float).00950156, (float)-.0212723, (float)
	    .0847001, (float).170147, (float)-.0238645, (float)0., (float)0., 
	    (float)0., (float).0013738, (float)-.00841918, (float)2.80145e-5, 
	    (float).00712383, (float)0., (float)-.0166209, (float)1.03533e-4, 
	    (float)-.0168898, (float)0., (float)3645.26, (float)0., (float)
	    .00654077, (float)3.6913e-4, (float)9.94419e-4, (float)84.2803, (
	    float)-.0116124, (float)-.00774414, (float)-.00168844, (float)
	    .00142809, (float)-.00192955, (float).117225, (float)-.0241512, (
	    float)15052.1, (float)0., (float)0., (float)0., (float)1602.61, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)-3.54403e-4, (float)-.018727, (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float).0276439, (float)
	    .00643207, (float)-.03543, (float)0., (float)0., (float)0., (
	    float)-.0280221, (float)81.1228, (float)-6.75255e-4, (float)0., (
	    float)-.0105162, (float)-.00348292, (float)-.00697321, (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.00145546, (float)-.013197, (float)-.00357751, (float)-1.09021, (
	    float)-.0150181, (float)-.00712841, (float)-.0066459, (float)
	    -.0035261, (float)-.0187773, (float)-.00222432, (float)-.393895, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float).060405, (float)1.57034, (float).0299387, (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)-1.51018, (
	    float)0., (float)0., (float)0., (float)-8.6165, (float).0126454, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float).00550878, (float)0., (float)0., (float).0866784, (float)
	    .158727, (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float).0623881, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float).0847001, (float).170147, (float)-.0945934, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float).956827, (float).0620637, (float)
	    .0318433, (float)0., (float)0., (float).03949, (float)0., (float)
	    0., (float)-.00924882, (float)-.00794023, (float)0., (float)0., (
	    float)0., (float)174.712, (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float).00274677, (float)0., 
	    (float).0154951, (float).0866784, (float).158727, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)-6.99007e-4, (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float).0124362, (float)
	    -.00528756, (float).0847001, (float).170147, (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)2.47425e-5, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)1.0993, (float)3.90631, (float)
	    3.07165, (float).986161, (float)16.3536, (float)4.6383, (float)1.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)1.2884, (float).0310302, 
	    (float).118339, (float)1., (float).7, (float)1.1502, (float)
	    3.44689, (float)1.2884, (float)1., (float)1.08738, (float)1.22947,
	     (float)1.10016, (float).734129, (float)1.15241, (float)2.22784, (
	    float).795046, (float)4.01612, (float)4.47749, (float)123.435, (
	    float)-.0760535, (float)1.68986e-6, (float).744294, (float)
	    1.03604, (float)172.783, (float)1.1502, (float)3.44689, (float)
	    -.74623, (float).949154, (float)1.00858, (float).0456011, (float)
	    -.0222972, (float)-.0544388, (float)5.23136e-4, (float)-.0188849, 
	    (float).0523707, (float)-.00943646, (float).00631707, (float)
	    -.078046, (float)-.048843, (float)0., (float)0., (float)-7.6025, (
	    float)0., (float)-.0144635, (float)-.0176843, (float)-121.517, (
	    float).0285647, (float)0., (float)0., (float)6.31792e-4, (float)
	    0., (float).00577197, (float).0866784, (float).158727, (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)-8902.72, (
	    float).00330611, (float).00302172, (float)0., (float)-.00213673, (
	    float)-3.2091e-4, (float)0., (float)0., (float).00276034, (float)
	    .00282487, (float)-2.97592e-4, (float)-.00421534, (float).0847001,
	     (float).170147, (float).00896456, (float)0., (float)-.0108596, (
	    float)0., (float)0., (float).00557917, (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float).00965405, (float)0., (float)0., (float)2., (
	    float).939664, (float).0856514, (float)-.00679989, (float)
	    .0265929, (float)-.00474283, (float).0121855, (float)-.0214905, (
	    float).00649651, (float)-.0205477, (float)-.0424952, (float)0., (
	    float)0., (float)0., (float)11.9148, (float)0., (float).0118777, (
	    float)-.072823, (float)-81.5965, (float).0173887, (float)0., (
	    float)0., (float)0., (float)-.0144691, (float)2.80259e-4, (float)
	    .0866784, (float).158727, (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)216.584, (float).00318713, (float).00737479,
	     (float)0., (float)-.00255018, (float)-.00392806, (float)0., (
	    float)0., (float)-.00289757, (float)-.00133549, (float).00102661, 
	    (float)3.53775e-4, (float).0847001, (float).170147, (float)
	    -.00917497, (float)0., (float)0., (float)0., (float)0., (float)
	    .00356082, (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)-.0100902, (
	    float)0., (float)0., (float)2., (float).985982, (float)-.0455435, 
	    (float).0121106, (float).0204127, (float)-.00240836, (float)
	    .0111383, (float)-.0451926, (float).0135074, (float)-.00654139, (
	    float).115275, (float).128247, (float)0., (float)0., (float)
	    -5.30705, (float)0., (float)-.0379332, (float)-.0624741, (float)
	    .771062, (float).0296315, (float)0., (float)0., (float)0., (float)
	    .00681051, (float)-.00434767, (float).0866784, (float).158727, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    10.7003, (float)-.00276907, (float)4.32474e-4, (float)0., (float)
	    .00131497, (float)-6.47517e-4, (float)0., (float)-22.0621, (float)
	    -.00110804, (float)-8.09338e-4, (float)4.18184e-4, (float)
	    .0042965, (float).0847001, (float).170147, (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)-.00404337, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)-9.5255e-4, (float)8.56253e-4, (float)4.33114e-4, (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float).00121223, (float)2.38694e-4, (float)9.15245e-4, (float)
	    .00128385, (float)8.67668e-4, (float)-5.61425e-6, (float)1.04445, 
	    (float)34.1112, (float)0., (float)-.840704, (float)-239.639, (
	    float).706668, (float)-20.5873, (float)-.363696, (float)23.9245, (
	    float)0., (float)-.00106657, (float)-7.67292e-4, (float)
	    1.54534e-4, (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)2., (float)1.0032, (float).0383501, (float)
	    -.00238983, (float).0028395, (float).00420956, (float)5.86619e-4, 
	    (float).0219054, (float)-.0100946, (float)-.00350259, (float)
	    .0417392, (float)-.00844404, (float)0., (float)0., (float)4.96949,
	     (float)0., (float)-.00706478, (float)-.0146494, (float)31.3258, (
	    float)-.00186493, (float)0., (float)-.0167499, (float)0., (float)
	    0., (float)5.12686e-4, (float).0866784, (float).158727, (float)
	    -.00464167, (float)0., (float)0., (float)0., (float).00437353, (
	    float)-199.069, (float)0., (float)-.00534884, (float)0., (float)
	    .00162458, (float).00293016, (float).00267926, (float)590.449, (
	    float)0., (float)0., (float)-.00117266, (float)-3.5889e-4, (float)
	    .0847001, (float).170147, (float)0., (float)0., (float).0138673, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float).00160571, (
	    float)6.28078e-4, (float)5.05469e-5, (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)-.00157829,
	     (float)-4.00855e-4, (float)5.04077e-5, (float)-.00139001, (float)
	    -.00233406, (float)-4.81197e-4, (float)1.46758, (float)6.20332, (
	    float)0., (float).366476, (float)-61.976, (float).309198, (float)
	    -19.8999, (float)0., (float)-329.933, (float)0., (float)-.0011008,
	     (float)-9.3931e-5, (float)1.39638e-4, (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)2., (float)
	    .981637, (float)-.00141317, (float).0387323, (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)-.0358707, (
	    float)-.00863658, (float)0., (float)0., (float)-2.02226, (float)
	    0., (float)-.00869424, (float)-.0191397, (float)87.6779, (float)
	    .00452188, (float)0., (float).022376, (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)-.00707572, (float)0., (
	    float)0., (float)0., (float)-.0041121, (float)35.006, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)-.00836657, (
	    float)16.1347, (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)-.014513, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float).00124152, (float)
	    6.43365e-4, (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float).00133255, (float)
	    .00242657, (float).00160666, (float)-.00185728, (float)-.00146874,
	     (float)-4.79163e-6, (float)1.22464, (float)35.351, (float)0., (
	    float).449223, (float)-47.7466, (float).470681, (float)8.41861, (
	    float)-.288198, (float)167.854, (float)0., (float)7.11493e-4, (
	    float)6.05601e-4, (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)2., (float)1.00422, (float)
	    -.00711212, (float).0052448, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)-.0528914, (float)
	    -.0241301, (float)0., (float)0., (float)-21.2219, (float)-.010383,
	     (float)-.00328077, (float).0165727, (float)1.68564, (float)
	    -.00668154, (float)0., (float).0145155, (float)0., (float)
	    .00842365, (float)0., (float)0., (float)0., (float)-.00434645, (
	    float)0., (float)0., (float).021678, (float)0., (float)-138.459, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    .00704573, (float)-47.3204, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    .0108767, (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)-.00808279, (float)0., (float)0., (float)0., (float)0., (
	    float)5.21769e-4, (float)-2.27387e-4, (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float).00326769, (float).00316901, (float)4.60316e-4, (float)
	    -1.01431e-4, (float).00102131, (float)9.96601e-4, (float)1.25707, 
	    (float)25.0114, (float)0., (float).424472, (float)-27.7655, (
	    float).344625, (float)27.5412, (float)0., (float)794.251, (float)
	    0., (float).00245835, (float).00138871, (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)2., (
	    float)1.0189, (float)-.0246603, (float).0100078, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.0670977, (float)-.0402286, (float)0., (float)0., (float)
	    -22.9466, (float)-.00747019, (float).0022658, (float).0263931, (
	    float)37.2625, (float)-.00639041, (float)0., (float).00958383, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.00185291, (float)0., (float)0., (float)0., (float)0., (float)
	    139.717, (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float).00919771, (float)-369.121, (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.0157067, (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)-.00707265, (float)0., (float)0., (float)0., (float)0., (
	    float)-.00292953, (float)-.00277739, (float)-4.40092e-4, (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float).0024728, (float)2.95035e-4, (float)-.00181246, (float)
	    .00281945, (float).00427296, (float)9.78863e-4, (float)1.40545, (
	    float)-6.19173, (float)0., (float)0., (float)-79.3632, (float)
	    .444643, (float)-403.085, (float)0., (float)11.5603, (float)0., (
	    float).00225068, (float)8.48557e-4, (float)-2.98493e-4, (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)2.,
	     (float).975801, (float).038068, (float)-.0305198, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    .0385575, (float).0504057, (float)0., (float)0., (float)-176.046, 
	    (float).0144594, (float)-.00148297, (float)-.0036856, (float)
	    30.2185, (float)-.00323338, (float)0., (float).0153569, (float)0.,
	     (float)-.0115558, (float)0., (float)0., (float)0., (float)
	    .0048962, (float)0., (float)0., (float)-.0100616, (float)
	    -.00821324, (float)-157.757, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float).00663564, (float)45.841, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)-.025128, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float).00991215, (float)0., (float)0., (
	    float)0., (float)0., (float)-8.73148e-4, (float)-.00129648, (
	    float)-7.32026e-5, (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)-.0046811, (float)
	    -.00466003, (float)-.00131567, (float)-7.3939e-4, (float)
	    6.32499e-4, (float)-4.65588e-4, (float)-1.29785, (float)-157.139, 
	    (float)0., (float).25835, (float)-36.9453, (float).410672, (float)
	    9.78196, (float)-.152064, (float)-3850.84, (float)0., (float)
	    -8.52706e-4, (float)-.00140945, (float)-7.26786e-4, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)2., (
	    float).960722, (float).0703757, (float)-.0300266, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    .0222671, (float).0410423, (float)0., (float)0., (float)-163.07, (
	    float).0106073, (float)5.40747e-4, (float).00779481, (float)
	    144.908, (float)1.51484e-4, (float)0., (float).0197547, (float)0.,
	     (float)-.0141844, (float)0., (float)0., (float)0., (float)
	    .00577884, (float)0., (float)0., (float).00974319, (float)0., (
	    float)-2880.15, (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)-.00444902, (float)-29.276, (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float).0234419, (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float).00536685, (float)0., (float)0., (float)0., (
	    float)0., (float)-4.65325e-4, (float)-5.50628e-4, (float)
	    3.31465e-4, (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)-.00206179, (float)-.00308575, (
	    float)-7.93589e-4, (float)-1.08629e-4, (float)5.95511e-4, (float)
	    -9.0505e-4, (float)1.18997, (float)41.5924, (float)0., (float)
	    -.472064, (float)-947.15, (float).398723, (float)19.8304, (float)
	    0., (float)3732.19, (float)0., (float)-.0015004, (float)
	    -.00114933, (float)-1.56769e-4, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)2., (float)1.03123, (float)
	    -.0705124, (float).00871615, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)-.0382621, (float)
	    -.00980975, (float)0., (float)0., (float)28.9286, (float)
	    .00957341, (float)0., (float)0., (float)86.6153, (float)
	    7.91938e-4, (float)0., (float)0., (float)0., (float).00468917, (
	    float)0., (float)0., (float)0., (float).00786638, (float)0., (
	    float)0., (float).00990827, (float)0., (float)65.5573, (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -40.02, (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float).00707457, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float).00572268, (
	    float)0., (float)0., (float)0., (float)0., (float)-2.0497e-4, (
	    float).0012156, (float)-8.05579e-6, (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)-.00249941,
	     (float)-4.57256e-4, (float)-1.59311e-4, (float)2.96481e-4, (
	    float)-.00177318, (float)-6.37918e-4, (float)1.02395, (float)
	    12.8172, (float)0., (float).149903, (float)-26.3818, (float)0., (
	    float)47.0628, (float)-.222139, (float).0482292, (float)0., (
	    float)-8.67075e-4, (float)-5.86479e-4, (float)5.32462e-4, (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    2., (float)1.00828, (float)-.0910404, (float)-.0226549, (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.023242, (float)-.00908925, (float)0., (float)0., (float)33.6105,
	     (float)0., (float)0., (float)0., (float)-12.4957, (float)
	    -.00587939, (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)0., (float)0., (float)0., (float)0., (float)0.,
	     (float)0., (float)27.9765, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)2012.37, (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)-.0175553, (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float).00329699, (float).00126659, (float)2.68402e-4, (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float).00117894, (float).00148746, (float)1.06478e-4, (float)
	    1.34743e-4, (float)-.00220939, (float)-6.23523e-4, (float).636539,
	     (float)11.3621, (float)0., (float)-.393777, (float)2386.87, (
	    float)0., (float)661.865, (float)-.121434, (float)9.27608, (float)
	    0., (float)1.68478e-4, (float).00124892, (float).00171345, (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    2., (float)1.57293, (float)-.6784, (float).6475, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.0762974, (float)-.360423, (float)0., (float)0., (float)128.358, 
	    (float)0., (float)0., (float)0., (float)46.8038, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)-.167898, (float)0., (float)0., (float)0., (
	    float)0., (float)29099.4, (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)31.5706, (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)2., (float)
	    .860028, (float).377052, (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)-1.1757, (float)0., (
	    float)0., (float)0., (float).00777757, (float)0., (float)0., (
	    float)0., (float)101.024, (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)654.251, (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.0156959, (float).0191001, (float).0315971, (float).0100982, (
	    float)-.00671565, (float).00257693, (float)1.38692, (float)
	    .282132, (float)0., (float)0., (float)381.511, (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    0., (float)2., (float)1.06029, (float)-.0525231, (float).373034, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float).0331072, (float)-.388409, (float)0., (float)0., (float)
	    -165.295, (float)-.213801, (float)-.0438916, (float)-.322716, (
	    float)-88.2393, (float).118458, (float)0., (float)-.435863, (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.119782, (float)0., (float)0., (float)0., (float)0., (float)
	    26.2229, (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)-53.7443, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    -.455788, (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    .0384009, (float).0396733, (float)0., (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float)0., (float)0., (float)
	    .0505494, (float).0739617, (float).01922, (float)-.00846151, (
	    float)-.0134244, (float).0196338, (float)1.50421, (float)18.8368, 
	    (float)0., (float)0., (float)-51.3114, (float)0., (float)0., (
	    float)0., (float)0., (float)0., (float).0511923, (float).0361225, 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)2., (float)1., (float)1., (float)1., (float)1., 
	    (float)1., (float)1., (float)1., (float)1., (float)1., (float)1., 
	    (float)1., (float)1., (float)1., (float)1., (float)1., (float)1., 
	    (float)1., (float)1., (float)1., (float)1., (float)1., (float)1., 
	    (float)1., (float)1., (float)1., (float)1., (float)1., (float)1., 
	    (float)1., (float)1., (float)1., (float)1., (float)1., (float)1., 
	    (float)1., (float)1., (float)1., (float)1., (float)1., (float)1., 
	    (float)1., (float)1., (float)1., (float)1., (float)1., (float)1., 
	    (float)1., (float)1., (float)1., (float)1., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0., 
	    (float)0., (float)0., (float)0., (float)0., (float)0., (float)0. }
	    ;

struct {
    char e_1[28];
    integer e_2;
    } datim7_ = { {'0', '1', '-', 'F', 'E', 'B', '-', '0', '2', ' ', ' ', ' ',
	     '1', '5', ':', '4', '9', ':', '2', '7', 'M', 'S', 'I', 'S', 'E', 
	    '-', '0', '0'}, 0 };

struct {
    real e_1[10];
    } mavg7_ = { (float)261., (float)264., (float)229., (float)217., (float)
	    217., (float)223., (float)286.76, (float)-2.9394, (float)2.5, (
	    float)0. };


/* Table of constant values */

static integer c__1 = 1;
static real c_b14 = (float)1.;
static real c_b15 = (float)0.;
static integer c__48 = 48;
static integer c__2 = 2;
static real c_b37 = (float)1e3;
static real c_b45 = (float)28.;
static real c_b54 = (float)4.;
static real c_b58 = (float)16.;
static real c_b62 = (float)32.;
static real c_b67 = (float)40.;
static real c_b75 = (float)14.;
static doublereal c_b96 = .5;
static real c_b105 = (float)2.;
static integer c__9 = 9;
static integer c__4 = 4;

/* ----------------------------------------------------------------------- */
/* Subroutine */ int gtd7_(integer *iyd, real *sec, real *alt, real *glat, 
	real *glong, real *stl, real *f107a, real *f107, real *ap, integer *
	mass, real *d__, real *t)
{
    /* Initialized data */

    static integer mn3 = 5;
    static real zn3[5] = { (float)32.5,(float)20.,(float)15.,(float)10.,(
	    float)0. };
    static integer mn2 = 4;
    static real zn2[4] = { (float)72.5,(float)55.,(float)45.,(float)32.5 };
    static real zmix = (float)62.5;
    static real alast = (float)99999.;
    static integer mssl = -999;
    /* 25 ELEMENT ARRAY CONTAINING 0. FOR OFF, 1. FOR ON, OR 2. FOR MAIN EFFECTS OFF BUT CROSS TERMS ON */
    static real sv[25] = {
       (float)1.,          // F10.7 EFFECT ON MEAN
       (float)1.,          // TIME INDEPENDENT
       (float)1.,          // SYMMETRICAL ANNUAL
       (float)1.,          // SYMMETRICAL SEMIANNUAL
       (float)1.,          // ASYMMETRICAL ANNUAL
       (float)1.,          // ASYMMETRICAL SEMIANNUAL
       (float)1.,          // DIURNAL
       (float)1.,          // SEMIDIURNAL
       (float)1.,          // DAILY AP
       (float)1.,          // ALL UT/LONG EFFECTS
       (float)1.,          // LONGITUDINAL
       (float)1.,          // UT AND MIXED UT/LONG
       (float)1.,          // MIXED AP/UT/LONG
       (float)1.,          // TERDIURNAL
       (float)1.,          // DEPARTURES FROM DIFFUSIVE EQUILIBRIUM
       (float)1.,          // ALL TINF VAR
       (float)1.,          // ALL TLB VAR
       (float)1.,          // ALL TN1 VAR
       (float)1.,          // ALL S VAR
       (float)1.,          // ALL TN2 VAR
       (float)1.,          // ALL NLB VAR
       (float)1.,          // ALL TN3 VAR
       (float)1.,          // TURBO SCALE HEIGHT VAR
       (float)1.,          //
       (float)1.           //
    };

    /* System generated locals */
    real r__1;

    /* Local variables */
    static integer i__, j;
    static real v1, ds[9], ts[2], tz, dmc, dmr, dz28, xmm;
    static integer mss;
    extern /* Subroutine */ int gts7_(integer *, real *, real *, real *, real 
	    *, real *, real *, real *, real *, integer *, real *, real *);
    static real dm28m, altt, xlat;
    extern doublereal vtst7_(integer *, real *, real *, real *, real *, real *
	    , real *, real *, integer *);
    extern /* Subroutine */ int glatf_(real *, real *, real *);
    extern doublereal densm_(real *, real *, real *, real *, integer *, real *
	    , real *, real *, integer *, real *, real *, real *), glob7s_(
	    real *);
    extern /* Subroutine */ int tselec_(real *);


/*     NRLMSISE-00 */
/*     ----------- */
/*        Neutral Atmosphere Empirical Model from the surface to lower */
/*        exosphere */

/*        NEW FEATURES: */
/*          *Extensive satellite drag database used in model generation */
/*          *Revised O2 (and O) in lower thermosphere */
/*          *Additional nonlinear solar activity term */
/*          *"ANOMALOUS OXYGEN" NUMBER DENSITY, OUTPUT D(9) */
/*           At high altitudes (> 500 km), hot atomic oxygen or ionized */
/*           oxygen can become appreciable for some ranges of subroutine */
/*           inputs, thereby affecting drag on satellites and debris. We */
/*           group these species under the term "anomalous oxygen," since */
/*           their individual variations are not presently separable with */
/*           the drag data used to define this model component. */

/*        SUBROUTINES FOR SPECIAL OUTPUTS: */

/*        HIGH ALTITUDE DRAG: EFFECTIVE TOTAL MASS DENSITY */
/*        (SUBROUTINE GTD7D, OUTPUT D(6)) */
/*           For atmospheric drag calculations at altitudes above 500 km, */
/*           call SUBROUTINE GTD7D to compute the "effective total mass */
/*           density" by including contributions from "anomalous oxygen." */
/*           See "NOTES ON OUTPUT VARIABLES" below on D(6). */

/*        PRESSURE GRID (SUBROUTINE GHP7) */
/*          See subroutine GHP7 to specify outputs at a pressure level */
/*          rather than at an altitude. */

/*        OUTPUT IN M-3 and KG/M3:   CALL METERS(.TRUE.) */

/*     INPUT VARIABLES: */
/*        IYD - YEAR AND DAY AS YYDDD (day of year from 1 to 365 (or 366)) */
/*              (Year ignored in current model) */
/*        SEC - UT(SEC) */
/*        ALT - ALTITUDE(KM) */
/*        GLAT - GEODETIC LATITUDE(DEG) */
/*        GLONG - GEODETIC LONGITUDE(DEG) */
/*        STL - LOCAL APPARENT SOLAR TIME(HRS; see Note below) */
/*        F107A - 81 day AVERAGE OF F10.7 FLUX (centered on day DDD) */
/*        F107 - DAILY F10.7 FLUX FOR PREVIOUS DAY */
/*        AP - MAGNETIC INDEX(DAILY) OR WHEN SW(9)=-1. : */
/*           - ARRAY CONTAINING: */
/*             (1) DAILY AP */
/*             (2) 3 HR AP INDEX FOR CURRENT TIME */
/*             (3) 3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME */
/*             (4) 3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME */
/*             (5) 3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME */
/*             (6) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR */
/*                    TO CURRENT TIME */
/*             (7) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 57 HRS PRIOR */
/*                    TO CURRENT TIME */
/*        MASS - MASS NUMBER (ONLY DENSITY FOR SELECTED GAS IS */
/*                 CALCULATED.  MASS 0 IS TEMPERATURE.  MASS 48 FOR ALL. */
/*                 MASS 17 IS Anomalous O ONLY.) */

/*     NOTES ON INPUT VARIABLES: */
/*        UT, Local Time, and Longitude are used independently in the */
/*        model and are not of equal importance for every situation. */
/*        For the most physically realistic calculation these three */
/*        variables should be consistent (STL=SEC/3600+GLONG/15). */
/*        The Equation of Time departures from the above formula */
/*        for apparent local time can be included if available but */
/*        are of minor importance. */

/*        F107 and F107A values used to generate the model correspond */
/*        to the 10.7 cm radio flux at the actual distance of the Earth */
/*        from the Sun rather than the radio flux at 1 AU. The following */
/*        site provides both classes of values: */
/*        ftp://ftp.ngdc.noaa.gov/STP/SOLAR_DATA/SOLAR_RADIO/FLUX/ */

/*        F107, F107A, and AP effects are neither large nor well */
/*        established below 80 km and these parameters should be set to */
/*        150., 150., and 4. respectively. */

/*     OUTPUT VARIABLES: */
/*        D(1) - HE NUMBER DENSITY(CM-3) */
/*        D(2) - O NUMBER DENSITY(CM-3) */
/*        D(3) - N2 NUMBER DENSITY(CM-3) */
/*        D(4) - O2 NUMBER DENSITY(CM-3) */
/*        D(5) - AR NUMBER DENSITY(CM-3) */
/*        D(6) - TOTAL MASS DENSITY(GM/CM3) */
/*        D(7) - H NUMBER DENSITY(CM-3) */
/*        D(8) - N NUMBER DENSITY(CM-3) */
/*        D(9) - Anomalous oxygen NUMBER DENSITY(CM-3) */
/*        T(1) - EXOSPHERIC TEMPERATURE */
/*        T(2) - TEMPERATURE AT ALT */

/*     NOTES ON OUTPUT VARIABLES: */
/*        TO GET OUTPUT IN M-3 and KG/M3:   CALL METERS(.TRUE.) */

/*        O, H, and N are set to zero below 72.5 km */

/*        T(1), Exospheric temperature, is set to global average for */
/*        altitudes below 120 km. The 120 km gradient is left at global */
/*        average value for altitudes below 72 km. */

/*        D(6), TOTAL MASS DENSITY, is NOT the same for subroutines GTD7 */
/*        and GTD7D */

/*          SUBROUTINE GTD7 -- D(6) is the sum of the mass densities of the */
/*          species labeled by indices 1-5 and 7-8 in output variable D. */
/*          This includes He, O, N2, O2, Ar, H, and N but does NOT include */
/*          anomalous oxygen (species index 9). */

/*          SUBROUTINE GTD7D -- D(6) is the "effective total mass density */
/*          for drag" and is the sum of the mass densities of all species */
/*          in this model, INCLUDING anomalous oxygen. */

/*     SWITCHES: The following is for test and special purposes: */

/*        TO TURN ON AND OFF PARTICULAR VARIATIONS CALL TSELEC(SW), */
/*        WHERE SW IS A 25 ELEMENT ARRAY CONTAINING 0. FOR OFF, 1. */
/*        FOR ON, OR 2. FOR MAIN EFFECTS OFF BUT CROSS TERMS ON */
/*        FOR THE FOLLOWING VARIATIONS */
/*               1 - F10.7 EFFECT ON MEAN  2 - TIME INDEPENDENT */
/*               3 - SYMMETRICAL ANNUAL    4 - SYMMETRICAL SEMIANNUAL */
/*               5 - ASYMMETRICAL ANNUAL   6 - ASYMMETRICAL SEMIANNUAL */
/*               7 - DIURNAL               8 - SEMIDIURNAL */
/*               9 - DAILY AP             10 - ALL UT/LONG EFFECTS */
/*              11 - LONGITUDINAL         12 - UT AND MIXED UT/LONG */
/*              13 - MIXED AP/UT/LONG     14 - TERDIURNAL */
/*              15 - DEPARTURES FROM DIFFUSIVE EQUILIBRIUM */
/*              16 - ALL TINF VAR         17 - ALL TLB VAR */
/*              18 - ALL TN1 VAR           19 - ALL S VAR */
/*              20 - ALL TN2 VAR           21 - ALL NLB VAR */
/*              22 - ALL TN3 VAR           23 - TURBO SCALE HEIGHT VAR */

/*        To get current values of SW: CALL TRETRV(SW) */

    /* Parameter adjustments */
    // Adjust the sv flag for hourly Ap is variation is seen
    integer i;
    for (i = 1; i < 7; ++i)
    {
       if (ap[i] != ap[0])
       {
          // If we are using daily/3hr observations, flag 9 should be -1.0
          sv[8] = -1.0;
          break;
       }
    }

    --t;
    --d__;
    --ap;

    /* Function Body */
    if (csw_1.isw != 64999) {
	tselec_(sv);
    }
/*      Put identification data into common/datime/ */
    for (i__ = 1; i__ <= 3; ++i__) {
	datime_1.isdate[i__ - 1] = datim7_1.isd[i__ - 1];
/* L1: */
    }
    for (i__ = 1; i__ <= 2; ++i__) {
	datime_1.istime[i__ - 1] = datim7_1.ist[i__ - 1];
	datime_1.name__[i__ - 1] = datim7_1.nam[i__ - 1];
/* L2: */
    }

/*        Test for changed input */
    v1 = vtst7_(iyd, sec, glat, glong, stl, f107a, f107, &ap[1], &c__1);
/*       Latitude variation of gravity (none for SW(2)=0) */
    xlat = *glat;
    if (csw_1.sw[1] == (float)0.) {
	xlat = (float)45.;
    }
    glatf_(&xlat, &parmb_1.gsurf, &parmb_1.re);

    xmm = lower7_1.pdm[24];

/*       THERMOSPHERE/MESOSPHERE (above ZN2(1)) */
    altt = dmax(*alt,zn2[0]);
    mss = *mass;
/*       Only calculate N2 in thermosphere if alt in mixed region */
    if (*alt < zmix && *mass > 0) {
	mss = 28;
    }
/*       Only calculate thermosphere if input parameters changed */
/*         or altitude above ZN2(1) in mesosphere */
    if (v1 == (float)1. || *alt > zn2[0] || alast > zn2[0] || mss != mssl) {
	gts7_(iyd, sec, &altt, glat, glong, stl, f107a, f107, &ap[1], &mss, 
		ds, ts);
	dm28m = dmix_1.dm28;
/*         metric adjustment */
	if (metsel_1.imr == 1) {
	    dm28m = dmix_1.dm28 * (float)1e6;
	}
	mssl = mss;
    }
    t[1] = ts[0];
    t[2] = ts[1];
    if (*alt >= zn2[0]) {
	for (j = 1; j <= 9; ++j) {
	    d__[j] = ds[j - 1];
/* L5: */
	}
	goto L10;
    }

/*       LOWER MESOSPHERE/UPPER STRATOSPHERE [between ZN3(1) and ZN2(1)] */
/*         Temperature at nodes and gradients at end nodes */
/*         Inverse temperature a linear function of spherical harmonics */
/*         Only calculate nodes if input changed */
    if (v1 == (float)1. || alast >= zn2[0]) {
	meso7_1.tgn2[0] = meso7_1.tgn1[1];
	meso7_1.tn2[0] = meso7_1.tn1[4];
	meso7_1.tn2[1] = parm7_1.pma[0] * mavg7_1.pavgm[0] / ((float)1. - 
		csw_1.sw[19] * glob7s_(parm7_1.pma));
	meso7_1.tn2[2] = parm7_1.pma[100] * mavg7_1.pavgm[1] / ((float)1. - 
		csw_1.sw[19] * glob7s_(&parm7_1.pma[100]));
	meso7_1.tn2[3] = parm7_1.pma[200] * mavg7_1.pavgm[2] / ((float)1. - 
		csw_1.sw[19] * csw_1.sw[21] * glob7s_(&parm7_1.pma[200]));
/* Computing 2nd power */
	r__1 = parm7_1.pma[200] * mavg7_1.pavgm[2];
	meso7_1.tgn2[1] = mavg7_1.pavgm[8] * parm7_1.pma[900] * (csw_1.sw[19] 
		* csw_1.sw[21] * glob7s_(&parm7_1.pma[900]) + (float)1.) * 
		meso7_1.tn2[3] * meso7_1.tn2[3] / (r__1 * r__1);
	meso7_1.tn3[0] = meso7_1.tn2[3];
    }
    if (*alt >= zn3[0]) {
	goto L6;
    }

/*       LOWER STRATOSPHERE AND TROPOSPHERE [below ZN3(1)] */
/*         Temperature at nodes and gradients at end nodes */
/*         Inverse temperature a linear function of spherical harmonics */
/*         Only calculate nodes if input changed */
    if (v1 == (float)1. || alast >= zn3[0]) {
	meso7_1.tgn3[0] = meso7_1.tgn2[1];
	meso7_1.tn3[1] = parm7_1.pma[300] * mavg7_1.pavgm[3] / ((float)1. - 
		csw_1.sw[21] * glob7s_(&parm7_1.pma[300]));
	meso7_1.tn3[2] = parm7_1.pma[400] * mavg7_1.pavgm[4] / ((float)1. - 
		csw_1.sw[21] * glob7s_(&parm7_1.pma[400]));
	meso7_1.tn3[3] = parm7_1.pma[500] * mavg7_1.pavgm[5] / ((float)1. - 
		csw_1.sw[21] * glob7s_(&parm7_1.pma[500]));
	meso7_1.tn3[4] = parm7_1.pma[600] * mavg7_1.pavgm[6] / ((float)1. - 
		csw_1.sw[21] * glob7s_(&parm7_1.pma[600]));
/* Computing 2nd power */
	r__1 = parm7_1.pma[600] * mavg7_1.pavgm[6];
	meso7_1.tgn3[1] = parm7_1.pma[700] * mavg7_1.pavgm[7] * (csw_1.sw[21] 
		* glob7s_(&parm7_1.pma[700]) + (float)1.) * meso7_1.tn3[4] * 
		meso7_1.tn3[4] / (r__1 * r__1);
    }
L6:
    if (*mass == 0) {
	goto L50;
    }
/*          LINEAR TRANSITION TO FULL MIXING BELOW ZN2(1) */
    dmc = (float)0.;
    if (*alt > zmix) {
	dmc = (float)1. - (zn2[0] - *alt) / (zn2[0] - zmix);
    }
    dz28 = ds[2];
/*      ***** N2 DENSITY **** */
    dmr = ds[2] / dm28m - (float)1.;
    d__[3] = densm_(alt, &dm28m, &xmm, &tz, &mn3, zn3, meso7_1.tn3, 
	    meso7_1.tgn3, &mn2, zn2, meso7_1.tn2, meso7_1.tgn2);
    d__[3] *= dmr * dmc + (float)1.;
/*      ***** HE DENSITY **** */
    d__[1] = (float)0.;
    if (*mass != 4 && *mass != 48) {
	goto L204;
    }
    dmr = ds[0] / (dz28 * lower7_1.pdm[1]) - (float)1.;
    d__[1] = d__[3] * lower7_1.pdm[1] * (dmr * dmc + (float)1.);
L204:
/*      **** O DENSITY **** */
    d__[2] = (float)0.;
    d__[9] = (float)0.;
/* L216: */
/*      ***** O2 DENSITY **** */
    d__[4] = (float)0.;
    if (*mass != 32 && *mass != 48) {
	goto L232;
    }
    dmr = ds[3] / (dz28 * lower7_1.pdm[31]) - (float)1.;
    d__[4] = d__[3] * lower7_1.pdm[31] * (dmr * dmc + (float)1.);
L232:
/*      ***** AR DENSITY **** */
    d__[5] = (float)0.;
    if (*mass != 40 && *mass != 48) {
	goto L240;
    }
    dmr = ds[4] / (dz28 * lower7_1.pdm[41]) - (float)1.;
    d__[5] = d__[3] * lower7_1.pdm[41] * (dmr * dmc + (float)1.);
L240:
/*      ***** HYDROGEN DENSITY **** */
    d__[7] = (float)0.;
/*      ***** ATOMIC NITROGEN DENSITY **** */
    d__[8] = (float)0.;

/*       TOTAL MASS DENSITY */

    if (*mass == 48) {
	d__[6] = (d__[1] * (float)4. + d__[2] * (float)16. + d__[3] * (float)
		28. + d__[4] * (float)32. + d__[5] * (float)40. + d__[7] + 
		d__[8] * (float)14.) * (float)1.66e-24;
	if (metsel_1.imr == 1) {
	    d__[6] /= (float)1e3;
	}
    }
    t[2] = tz;
L10:
    goto L90;
L50:
    gts3c_1.dd = densm_(alt, &c_b14, &c_b15, &tz, &mn3, zn3, meso7_1.tn3, 
	    meso7_1.tgn3, &mn2, zn2, meso7_1.tn2, meso7_1.tgn2);
    t[2] = tz;
L90:
    alast = *alt;
    return 0;
} /* gtd7_ */

/* ----------------------------------------------------------------------- */
/* Subroutine */ int gtd7d_(integer *iyd, real *sec, real *alt, real *glat, 
	real *glong, real *stl, real *f107a, real *f107, real *ap, integer *
	mass, real *d__, real *t)
{
    extern /* Subroutine */ int gtd7_(integer *, real *, real *, real *, real 
	    *, real *, real *, real *, real *, integer *, real *, real *);


/*     NRLMSISE-00 */
/*     ----------- */
/*        This subroutine provides Effective Total Mass Density for */
/*        output D(6) which includes contributions from "anomalous */
/*        oxygen" which can affect satellite drag above 500 km.  This */
/*        subroutine is part of the distribution package for the */
/*        Neutral Atmosphere Empirical Model from the surface to lower */
/*        exosphere.  See subroutine GTD7 for more extensive comments. */

/*     INPUT VARIABLES: */
/*        IYD - YEAR AND DAY AS YYDDD (day of year from 1 to 365 (or 366)) */
/*              (Year ignored in current model) */
/*        SEC - UT(SEC) */
/*        ALT - ALTITUDE(KM) */
/*        GLAT - GEODETIC LATITUDE(DEG) */
/*        GLONG - GEODETIC LONGITUDE(DEG) */
/*        STL - LOCAL APPARENT SOLAR TIME(HRS; see Note below) */
/*        F107A - 81 day AVERAGE OF F10.7 FLUX (centered on day DDD) */
/*        F107 - DAILY F10.7 FLUX FOR PREVIOUS DAY */
/*        AP - MAGNETIC INDEX(DAILY) OR WHEN SW(9)=-1. : */
/*           - ARRAY CONTAINING: */
/*             (1) DAILY AP */
/*             (2) 3 HR AP INDEX FOR CURRENT TIME */
/*             (3) 3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME */
/*             (4) 3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME */
/*             (5) 3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME */
/*             (6) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR */
/*                    TO CURRENT TIME */
/*             (7) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 57 HRS PRIOR */
/*                    TO CURRENT TIME */
/*        MASS - MASS NUMBER (ONLY DENSITY FOR SELECTED GAS IS */
/*                 CALCULATED.  MASS 0 IS TEMPERATURE.  MASS 48 FOR ALL. */
/*                 MASS 17 IS Anomalous O ONLY.) */

/*     NOTES ON INPUT VARIABLES: */
/*        UT, Local Time, and Longitude are used independently in the */
/*        model and are not of equal importance for every situation. */
/*        For the most physically realistic calculation these three */
/*        variables should be consistent (STL=SEC/3600+GLONG/15). */
/*        The Equation of Time departures from the above formula */
/*        for apparent local time can be included if available but */
/*        are of minor importance. */

/*        F107 and F107A values used to generate the model correspond */
/*        to the 10.7 cm radio flux at the actual distance of the Earth */
/*        from the Sun rather than the radio flux at 1 AU. */

/*     OUTPUT VARIABLES: */
/*        D(1) - HE NUMBER DENSITY(CM-3) */
/*        D(2) - O NUMBER DENSITY(CM-3) */
/*        D(3) - N2 NUMBER DENSITY(CM-3) */
/*        D(4) - O2 NUMBER DENSITY(CM-3) */
/*        D(5) - AR NUMBER DENSITY(CM-3) */
/*        D(6) - TOTAL MASS DENSITY(GM/CM3) [includes anomalous oxygen] */
/*        D(7) - H NUMBER DENSITY(CM-3) */
/*        D(8) - N NUMBER DENSITY(CM-3) */
/*        D(9) - Anomalous oxygen NUMBER DENSITY(CM-3) */
/*        T(1) - EXOSPHERIC TEMPERATURE */
/*        T(2) - TEMPERATURE AT ALT */

    /* Parameter adjustments */
    --t;
    --d__;
    --ap;

    /* Function Body */
    gtd7_(iyd, sec, alt, glat, glong, stl, f107a, f107, &ap[1], mass, &d__[1],
	     &t[1]);
/*       TOTAL MASS DENSITY */

    if (*mass == 48) {
	d__[6] = (d__[1] * (float)4. + d__[2] * (float)16. + d__[3] * (float)
		28. + d__[4] * (float)32. + d__[5] * (float)40. + d__[7] + 
		d__[8] * (float)14. + d__[9] * (float)16.) * (float)1.66e-24;
	if (metsel_1.imr == 1) {
	    d__[6] /= (float)1e3;
	}
    }
    return 0;
} /* gtd7d_ */

/* ----------------------------------------------------------------------- */
/* Subroutine */ int ghp7_(integer *iyd, real *sec, real *alt, real *glat, 
	real *glong, real *stl, real *f107a, real *f107, real *ap, real *d__, 
	real *t, real *press)
{
    /* Initialized data */

    static real bm = (float)1.3806e-19;
    static real rgas = (float)831.4;
    static real test = (float)4.3e-4;
    static integer ltest = 12;

    /* Format strings */
    static char fmt_100[] = "(1x,\002GHP7 NOT CONVERGING FOR PRESS\002,1pe12\
.2,e12.2)";

    /* System generated locals */
    real r__1;

    /* Builtin functions */
    double r_lg10(real *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();

    /* Local variables */
    static real g;
    static integer l;
    static real p, z__, ca, cd, cl, sh, pl, zi, xm, xn, cl2;
    extern /* Subroutine */ int gtd7_(integer *, real *, real *, real *, real 
	    *, real *, real *, real *, real *, integer *, real *, real *);
    static real diff;
    static integer iday;

    /* Fortran I/O blocks */
    static cilist io___42 = { 0, 6, 0, fmt_100, 0 };


/*       FIND ALTITUDE OF PRESSURE SURFACE (PRESS) FROM GTD7 */
/*     INPUT: */
/*        IYD - YEAR AND DAY AS YYDDD */
/*        SEC - UT(SEC) */
/*        GLAT - GEODETIC LATITUDE(DEG) */
/*        GLONG - GEODETIC LONGITUDE(DEG) */
/*        STL - LOCAL APPARENT SOLAR TIME(HRS) */
/*        F107A - 3 MONTH AVERAGE OF F10.7 FLUX */
/*        F107 - DAILY F10.7 FLUX FOR PREVIOUS DAY */
/*        AP - MAGNETIC INDEX(DAILY) OR WHEN SW(9)=-1. : */
/*           - ARRAY CONTAINING: */
/*             (1) DAILY AP */
/*             (2) 3 HR AP INDEX FOR CURRENT TIME */
/*             (3) 3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME */
/*             (4) 3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME */
/*             (5) 3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME */
/*             (6) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR */
/*                    TO CURRENT TIME */
/*             (7) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 59 HRS PRIOR */
/*                    TO CURRENT TIME */
/*        PRESS - PRESSURE LEVEL(MB) */
/*     OUTPUT: */
/*        ALT - ALTITUDE(KM) */
/*        D(1) - HE NUMBER DENSITY(CM-3) */
/*        D(2) - O NUMBER DENSITY(CM-3) */
/*        D(3) - N2 NUMBER DENSITY(CM-3) */
/*        D(4) - O2 NUMBER DENSITY(CM-3) */
/*        D(5) - AR NUMBER DENSITY(CM-3) */
/*        D(6) - TOTAL MASS DENSITY(GM/CM3) */
/*        D(7) - H NUMBER DENSITY(CM-3) */
/*        D(8) - N NUMBER DENSITY(CM-3) */
/*        D(9) - HOT O NUMBER DENSITY(CM-3) */
/*        T(1) - EXOSPHERIC TEMPERATURE */
/*        T(2) - TEMPERATURE AT ALT */

    /* Parameter adjustments */
    --t;
    --d__;
    --ap;

    /* Function Body */
    pl = r_lg10(press);
/*      Initial altitude estimate */
    if (pl >= (float)-5.) {
	if (pl > (float)2.5) {
	    zi = ((float)3. - pl) * (float)18.06;
	}
	if (pl > (float).75 && pl <= (float)2.5) {
	    zi = ((float)3.08 - pl) * (float)14.98;
	}
	if (pl > (float)-1. && pl <= (float).75) {
	    zi = ((float)2.72 - pl) * (float)17.8;
	}
	if (pl > (float)-2. && pl <= (float)-1.) {
	    zi = ((float)3.64 - pl) * (float)14.28;
	}
	if (pl > (float)-4. && pl <= (float)-2.) {
	    zi = ((float)4.32 - pl) * (float)12.72;
	}
	if (pl <= (float)-4.) {
	    zi = ((float).11 - pl) * (float)25.3;
	}
	iday = *iyd % 1000;
	cl = *glat / (float)90.;
	cl2 = cl * cl;
	if (iday < 182) {
	    cd = (float)1. - iday / (float)91.25;
	}
	if (iday >= 182) {
	    cd = iday / (float)91.25 - (float)3.;
	}
	ca = (float)0.;
	if (pl > (float)-1.11 && pl <= (float)-.23) {
	    ca = (float)1.;
	}
	if (pl > (float)-.23) {
	    ca = ((float)2.79 - pl) / (float)3.02;
	}
	if (pl <= (float)-1.11 && pl > (float)-3.) {
	    ca = ((float)-2.93 - pl) / (float)-1.8200000000000001;
	}
	z__ = zi - cl * (float)4.87 * cd * ca - cl2 * (float)1.64 * ca + ca * 
		(float).31 * cl;
    }
    if (pl < (float)-5.) {
/* Computing 2nd power */
	r__1 = pl + (float)4.;
	z__ = r__1 * r__1 * (float)22. + 110;
    }
/*      ITERATION LOOP */
    l = 0;
L10:
    ++l;
    gtd7_(iyd, sec, &z__, glat, glong, stl, f107a, f107, &ap[1], &c__48, &d__[
	    1], &t[1]);
    xn = d__[1] + d__[2] + d__[3] + d__[4] + d__[5] + d__[7] + d__[8];
    p = bm * xn * t[2];
    if (metsel_1.imr == 1) {
	p *= (float)1e-6;
    }
    diff = pl - r_lg10(&p);
    if (dabs(diff) < test || l == ltest) {
	goto L20;
    }
    xm = d__[6] / xn / (float)1.66e-24;
    if (metsel_1.imr == 1) {
	xm *= (float)1e3;
    }
/* Computing 2nd power */
    r__1 = z__ / parmb_1.re + (float)1.;
    g = parmb_1.gsurf / (r__1 * r__1);
    sh = rgas * t[2] / (xm * g);
/*         New altitude estimate using scale height */
    if (l < 6) {
	z__ -= sh * diff * (float)2.302;
    } else {
	z__ -= sh * diff;
    }
    goto L10;
L20:
    if (l == ltest) {
	s_wsfe(&io___42);
	do_fio(&c__1, (char *)&(*press), (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&diff, (ftnlen)sizeof(real));
	e_wsfe();
    }
    *alt = z__;
    return 0;
} /* ghp7_ */

/* ----------------------------------------------------------------------- */
/* Subroutine */ int glatf_(real *lat, real *gv, real *reff)
{
    /* Initialized data */

    static real dgtr = (float).0174533;

    /* Builtin functions */
    double cos(doublereal);

    /* Local variables */
    static real c2;

/*      CALCULATE LATITUDE VARIABLE GRAVITY (GV) AND EFFECTIVE */
/*      RADIUS (REFF) */
    c2 = cos((float)2. * dgtr * *lat);
    *gv = ((float)1. - c2 * (float).0026373) * (float)980.616;
    *reff = *gv * (float)2. / (c2 * (float)2.27e-9 + (float)3.085462e-6) * (
	    float)1e-5;
    return 0;
} /* glatf_ */

/* ----------------------------------------------------------------------- */
doublereal vtst7_(integer *iyd, real *sec, real *glat, real *glong, real *stl,
	 real *f107a, real *f107, real *ap, integer *ic)
{
    /* Initialized data */

    static integer iydl[2] = { -999,-999 };
    static real secl[2] = { (float)-999.,(float)-999. };
    static real glatl[2] = { (float)-999.,(float)-999. };
    static real gll[2] = { (float)-999.,(float)-999. };
    static real stll[2] = { (float)-999.,(float)-999. };
    static real fal[2] = { (float)-999.,(float)-999. };
    static real fl[2] = { (float)-999.,(float)-999. };
    static real apl[14]	/* was [7][2] */ = { (float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999. };
    static real swl[50]	/* was [25][2] */ = { (float)-999.,(float)-999.,(
	    float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(
	    float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(
	    float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(
	    float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(
	    float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(
	    float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(
	    float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(
	    float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(
	    float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(
	    float)-999.,(float)-999.,(float)-999. };
    static real swcl[50]	/* was [25][2] */ = { (float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999.,(float)-999.,(float)
	    -999.,(float)-999.,(float)-999.,(float)-999. };

    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer i__;

/*       Test if geophysical variables or switches changed and save */
/*       Return 0 if unchanged and 1 if changed */
    /* Parameter adjustments */
    --ap;

    /* Function Body */
    ret_val = (float)0.;
    if (*iyd != iydl[*ic - 1]) {
	goto L10;
    }
    if (*sec != secl[*ic - 1]) {
	goto L10;
    }
    if (*glat != glatl[*ic - 1]) {
	goto L10;
    }
    if (*glong != gll[*ic - 1]) {
	goto L10;
    }
    if (*stl != stll[*ic - 1]) {
	goto L10;
    }
    if (*f107a != fal[*ic - 1]) {
	goto L10;
    }
    if (*f107 != fl[*ic - 1]) {
	goto L10;
    }
    for (i__ = 1; i__ <= 7; ++i__) {
	if (ap[i__] != apl[i__ + *ic * 7 - 8]) {
	    goto L10;
	}
/* L5: */
    }
    for (i__ = 1; i__ <= 25; ++i__) {
	if (csw_1.sw[i__ - 1] != swl[i__ + *ic * 25 - 26]) {
	    goto L10;
	}
	if (csw_1.swc[i__ - 1] != swcl[i__ + *ic * 25 - 26]) {
	    goto L10;
	}
/* L7: */
    }
    goto L20;
L10:
    ret_val = (float)1.;
    iydl[*ic - 1] = *iyd;
    secl[*ic - 1] = *sec;
    glatl[*ic - 1] = *glat;
    gll[*ic - 1] = *glong;
    stll[*ic - 1] = *stl;
    fal[*ic - 1] = *f107a;
    fl[*ic - 1] = *f107;
    for (i__ = 1; i__ <= 7; ++i__) {
	apl[i__ + *ic * 7 - 8] = ap[i__];
/* L15: */
    }
    for (i__ = 1; i__ <= 25; ++i__) {
	swl[i__ + *ic * 25 - 26] = csw_1.sw[i__ - 1];
	swcl[i__ + *ic * 25 - 26] = csw_1.swc[i__ - 1];
/* L16: */
    }
L20:
    return ret_val;
} /* vtst7_ */

/* ----------------------------------------------------------------------- */
/* Subroutine */ int gts7_(integer *iyd, real *sec, real *alt, real *glat, 
	real *glong, real *stl, real *f107a, real *f107, real *ap, integer *
	mass, real *d__, real *t)
{
    /* Initialized data */

    static integer mt[11] = { 48,0,4,16,28,32,40,1,49,14,17 };
    static real altl[8] = { (float)200.,(float)300.,(float)160.,(float)250.,(
	    float)240.,(float)450.,(float)320.,(float)450. };
    static integer mn1 = 5;
    static real zn1[5] = { (float)120.,(float)110.,(float)100.,(float)90.,(
	    float)72.5 };
    static real dgtr = (float).0174533;
    static real dr = (float).0172142;
    static real alast = (float)-999.;
    static real alpha[9] = { (float)-.38,(float)0.,(float)0.,(float)0.,(float)
	    .17,(float)0.,(float)-.38,(float)0.,(float)0. };

    /* Format strings */
    static char fmt_100[] = "(1x,\002MASS\002,i5,\002  NOT VALID\002)";

    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double r_mod(real *, real *), sin(doublereal), cos(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    double exp(doublereal), log(doublereal);

    /* Local variables */
    static integer i__, j;
    static real z__, g1, g4, t2, v2, b01, b04, b32, b16, g40, b28, g32, g16, 
	    b40, g14, g28, b14, tz, hc04, hc32, hc16, rc32, rc16, day, zh04, 
	    zhf, zh28, yrd, xmm, xmd, zc04, zh16, zc16, zh32, zc32, zh40, 
	    hc40, zc40, zh01, hc01, zc01, rc01, hc216, zh14, hc14, zc14, rc14,
	     g16h, tho, hcc01, hcc32, hcc14, hcc16, db16h, zcc01, zcc32, 
	    zcc14;
    extern doublereal ccor_(real *, real *, real *, real *);
    static real zcc16, ddum;
    extern doublereal dnet_(real *, real *, real *, real *, real *);
    static real zhm01, tinf, zhm04, zhm32, zhm16, zhm40, zhm14, zhm28, zmho, 
	    zsho, hcc232, zsht;
    extern doublereal ccor2_(real *, real *, real *, real *, real *), vtst7_(
	    integer *, real *, real *, real *, real *, real *, real *, real *,
	     integer *), scalh_(real *, real *, real *), densu_(real *, real *
	    , real *, real *, real *, real *, real *, real *, real *, integer 
	    *, real *, real *, real *), globe7_(real *, real *, real *, real *
	    , real *, real *, real *, real *, real *), glob7s_(real *);

    /* Fortran I/O blocks */
    static cilist io___73 = { 0, 6, 0, fmt_100, 0 };



/*     Thermospheric portion of NRLMSISE-00 */
/*     See GTD7 for more extensive comments */

/*        OUTPUT IN M-3 and KG/M3:   CALL METERS(.TRUE.) */

/*     INPUT VARIABLES: */
/*        IYD - YEAR AND DAY AS YYDDD (day of year from 1 to 365 (or 366)) */
/*              (Year ignored in current model) */
/*        SEC - UT(SEC) */
/*        ALT - ALTITUDE(KM) (>72.5 km) */
/*        GLAT - GEODETIC LATITUDE(DEG) */
/*        GLONG - GEODETIC LONGITUDE(DEG) */
/*        STL - LOCAL APPARENT SOLAR TIME(HRS; see Note below) */
/*        F107A - 81 day AVERAGE OF F10.7 FLUX (centered on day DDD) */
/*        F107 - DAILY F10.7 FLUX FOR PREVIOUS DAY */
/*        AP - MAGNETIC INDEX(DAILY) OR WHEN SW(9)=-1. : */
/*           - ARRAY CONTAINING: */
/*             (1) DAILY AP */
/*             (2) 3 HR AP INDEX FOR CURRENT TIME */
/*             (3) 3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME */
/*             (4) 3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME */
/*             (5) 3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME */
/*             (6) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR */
/*                    TO CURRENT TIME */
/*             (7) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 57 HRS PRIOR */
/*                    TO CURRENT TIME */
/*        MASS - MASS NUMBER (ONLY DENSITY FOR SELECTED GAS IS */
/*                 CALCULATED.  MASS 0 IS TEMPERATURE.  MASS 48 FOR ALL. */
/*                 MASS 17 IS Anomalous O ONLY.) */

/*     NOTES ON INPUT VARIABLES: */
/*        UT, Local Time, and Longitude are used independently in the */
/*        model and are not of equal importance for every situation. */
/*        For the most physically realistic calculation these three */
/*        variables should be consistent (STL=SEC/3600+GLONG/15). */
/*        The Equation of Time departures from the above formula */
/*        for apparent local time can be included if available but */
/*        are of minor importance. */

/*        F107 and F107A values used to generate the model correspond */
/*        to the 10.7 cm radio flux at the actual distance of the Earth */
/*        from the Sun rather than the radio flux at 1 AU. The following */
/*        site provides both classes of values: */
/*        ftp://ftp.ngdc.noaa.gov/STP/SOLAR_DATA/SOLAR_RADIO/FLUX/ */

/*        F107, F107A, and AP effects are neither large nor well */
/*        established below 80 km and these parameters should be set to */
/*        150., 150., and 4. respectively. */

/*     OUTPUT VARIABLES: */
/*        D(1) - HE NUMBER DENSITY(CM-3) */
/*        D(2) - O NUMBER DENSITY(CM-3) */
/*        D(3) - N2 NUMBER DENSITY(CM-3) */
/*        D(4) - O2 NUMBER DENSITY(CM-3) */
/*        D(5) - AR NUMBER DENSITY(CM-3) */
/*        D(6) - TOTAL MASS DENSITY(GM/CM3) [Anomalous O NOT included] */
/*        D(7) - H NUMBER DENSITY(CM-3) */
/*        D(8) - N NUMBER DENSITY(CM-3) */
/*        D(9) - Anomalous oxygen NUMBER DENSITY(CM-3) */
/*        T(1) - EXOSPHERIC TEMPERATURE */
/*        T(2) - TEMPERATURE AT ALT */

    /* Parameter adjustments */
    --t;
    --d__;
    --ap;

    /* Function Body */
/*        Test for changed input */
    v2 = vtst7_(iyd, sec, glat, glong, stl, f107a, f107, &ap[1], &c__2);

    yrd = (real) (*iyd);
    gts3c_1.za = parm7_1.pdl[40];
    zn1[0] = gts3c_1.za;
    for (j = 1; j <= 9; ++j) {
	d__[j] = (float)0.;
/* L2: */
    }
/*        TINF VARIATIONS NOT IMPORTANT BELOW ZA OR ZN1(1) */
    if (*alt > zn1[0]) {
	if (v2 == (float)1. || alast <= zn1[0]) {
	    tinf = lower7_1.ptm[0] * parm7_1.pt[0] * (csw_1.sw[15] * globe7_(&
		    yrd, sec, glat, glong, stl, f107a, f107, &ap[1], 
		    parm7_1.pt) + (float)1.);
	}
    } else {
	tinf = lower7_1.ptm[0] * parm7_1.pt[0];
    }
    t[1] = tinf;
/*          GRADIENT VARIATIONS NOT IMPORTANT BELOW ZN1(5) */
    if (*alt > zn1[4]) {
	if (v2 == (float)1. || alast <= zn1[4]) {
	    gts3c_1.g0 = lower7_1.ptm[3] * parm7_1.ps[0] * (csw_1.sw[18] * 
		    globe7_(&yrd, sec, glat, glong, stl, f107a, f107, &ap[1], 
		    parm7_1.ps) + (float)1.);
	}
    } else {
	gts3c_1.g0 = lower7_1.ptm[3] * parm7_1.ps[0];
    }
/*      Calculate these temperatures only if input changed */
    if (v2 == (float)1. || *alt < (float)300.) {
	gts3c_1.tlb = lower7_1.ptm[1] * (csw_1.sw[16] * globe7_(&yrd, sec, 
		glat, glong, stl, f107a, f107, &ap[1], &parm7_1.pd[450]) + (
		float)1.) * parm7_1.pd[450];
    }
    gts3c_1.s = gts3c_1.g0 / (tinf - gts3c_1.tlb);
/*       Lower thermosphere temp variations not significant for */
/*        density above 300 km */
    if (*alt < (float)300.) {
	if (v2 == (float)1. || alast >= (float)300.) {
	    meso7_1.tn1[1] = lower7_1.ptm[6] * parm7_1.ptl[0] / ((float)1. - 
		    csw_1.sw[17] * glob7s_(parm7_1.ptl));
	    meso7_1.tn1[2] = lower7_1.ptm[2] * parm7_1.ptl[100] / ((float)1. 
		    - csw_1.sw[17] * glob7s_(&parm7_1.ptl[100]));
	    meso7_1.tn1[3] = lower7_1.ptm[7] * parm7_1.ptl[200] / ((float)1. 
		    - csw_1.sw[17] * glob7s_(&parm7_1.ptl[200]));
	    meso7_1.tn1[4] = lower7_1.ptm[4] * parm7_1.ptl[300] / ((float)1. 
		    - csw_1.sw[17] * csw_1.sw[19] * glob7s_(&parm7_1.ptl[300])
		    );
/* Computing 2nd power */
	    r__1 = lower7_1.ptm[4] * parm7_1.ptl[300];
	    meso7_1.tgn1[1] = lower7_1.ptm[8] * parm7_1.pma[800] * (csw_1.sw[
		    17] * csw_1.sw[19] * glob7s_(&parm7_1.pma[800]) + (float)
		    1.) * meso7_1.tn1[4] * meso7_1.tn1[4] / (r__1 * r__1);
	}
    } else {
	meso7_1.tn1[1] = lower7_1.ptm[6] * parm7_1.ptl[0];
	meso7_1.tn1[2] = lower7_1.ptm[2] * parm7_1.ptl[100];
	meso7_1.tn1[3] = lower7_1.ptm[7] * parm7_1.ptl[200];
	meso7_1.tn1[4] = lower7_1.ptm[4] * parm7_1.ptl[300];
/* Computing 2nd power */
	r__1 = lower7_1.ptm[4] * parm7_1.ptl[300];
	meso7_1.tgn1[1] = lower7_1.ptm[8] * parm7_1.pma[800] * meso7_1.tn1[4] 
		* meso7_1.tn1[4] / (r__1 * r__1);
    }

    gts3c_1.z0 = zn1[3];
    gts3c_1.t0 = meso7_1.tn1[3];
    gts3c_1.tr12 = (float)1.;

    if (*mass == 0) {
	goto L50;
    }
/*       N2 variation factor at Zlb */
    g28 = csw_1.sw[20] * globe7_(&yrd, sec, glat, glong, stl, f107a, f107, &
	    ap[1], &parm7_1.pd[300]);
    day = r_mod(&yrd, &c_b37);
/*        VARIATION OF TURBOPAUSE HEIGHT */
    zhf = parm7_1.pdl[49] * (csw_1.sw[4] * parm7_1.pdl[24] * sin(dgtr * *glat)
	     * cos(dr * (day - parm7_1.pt[13])) + (float)1.);
    yrd = (real) (*iyd);
    t[1] = tinf;
    xmm = lower7_1.pdm[24];
    z__ = *alt;

    for (j = 1; j <= 11; ++j) {
	if (*mass == mt[j - 1]) {
	    goto L15;
	}
/* L10: */
    }
    s_wsfe(&io___73);
    do_fio(&c__1, (char *)&(*mass), (ftnlen)sizeof(integer));
    e_wsfe();
    goto L90;
L15:
    if (z__ > altl[5] && *mass != 28 && *mass != 48) {
	goto L17;
    }

/*       **** N2 DENSITY **** */

/*      Diffusive density at Zlb */
    gts3c_1.db28 = lower7_1.pdm[20] * exp(g28) * parm7_1.pd[300];
/*      Diffusive density at Alt */
    d__[3] = densu_(&z__, &gts3c_1.db28, &tinf, &gts3c_1.tlb, &c_b45, &alpha[
	    2], &t[2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    gts3c_1.dd = d__[3];
/*      Turbopause */
    zh28 = lower7_1.pdm[22] * zhf;
    zhm28 = lower7_1.pdm[23] * parm7_1.pdl[30];
    xmd = (float)28. - xmm;
/*      Mixed density at Zlb */
    r__1 = alpha[2] - (float)1.;
    b28 = densu_(&zh28, &gts3c_1.db28, &tinf, &gts3c_1.tlb, &xmd, &r__1, &tz, 
	    &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    if (z__ > altl[2] || csw_1.sw[14] == (float)0.) {
	goto L17;
    }
/*      Mixed density at Alt */
    dmix_1.dm28 = densu_(&z__, &b28, &tinf, &gts3c_1.tlb, &xmm, &alpha[2], &
	    tz, &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
/*      Net density at Alt */
    d__[3] = dnet_(&d__[3], &dmix_1.dm28, &zhm28, &xmm, &c_b45);
L17:
    switch (j) {
	case 1:  goto L20;
	case 2:  goto L50;
	case 3:  goto L20;
	case 4:  goto L25;
	case 5:  goto L90;
	case 6:  goto L35;
	case 7:  goto L40;
	case 8:  goto L45;
	case 9:  goto L25;
	case 10:  goto L48;
	case 11:  goto L46;
    }
L20:

/*       **** HE DENSITY **** */

/*       Density variation factor at Zlb */
    g4 = csw_1.sw[20] * globe7_(&yrd, sec, glat, glong, stl, f107a, f107, &ap[
	    1], parm7_1.pd);
/*      Diffusive density at Zlb */
    gts3c_1.db04 = lower7_1.pdm[0] * exp(g4) * parm7_1.pd[0];
/*      Diffusive density at Alt */
    d__[1] = densu_(&z__, &gts3c_1.db04, &tinf, &gts3c_1.tlb, &c_b54, alpha, &
	    t[2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    gts3c_1.dd = d__[1];
    if (z__ > altl[0] || csw_1.sw[14] == (float)0.) {
	goto L24;
    }
/*      Turbopause */
    zh04 = lower7_1.pdm[2];
/*      Mixed density at Zlb */
    r__1 = (float)4. - xmm;
    r__2 = alpha[0] - (float)1.;
    b04 = densu_(&zh04, &gts3c_1.db04, &tinf, &gts3c_1.tlb, &r__1, &r__2, &t[
	    2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
/*      Mixed density at Alt */
    dmix_1.dm04 = densu_(&z__, &b04, &tinf, &gts3c_1.tlb, &xmm, &c_b15, &t[2],
	     &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    zhm04 = zhm28;
/*      Net density at Alt */
    d__[1] = dnet_(&d__[1], &dmix_1.dm04, &zhm04, &xmm, &c_b54);
/*      Correction to specified mixing ratio at ground */
    gts3c_1.rl = log(b28 * lower7_1.pdm[1] / b04);
    zc04 = lower7_1.pdm[4] * parm7_1.pdl[25];
    hc04 = lower7_1.pdm[5] * parm7_1.pdl[26];
/*      Net density corrected at Alt */
    d__[1] *= ccor_(&z__, &gts3c_1.rl, &hc04, &zc04);
L24:
    if (*mass != 48) {
	goto L90;
    }
L25:

/*      **** O DENSITY **** */

/*       Density variation factor at Zlb */
    g16 = csw_1.sw[20] * globe7_(&yrd, sec, glat, glong, stl, f107a, f107, &
	    ap[1], &parm7_1.pd[150]);
/*      Diffusive density at Zlb */
    gts3c_1.db16 = lower7_1.pdm[10] * exp(g16) * parm7_1.pd[150];
/*       Diffusive density at Alt */
    d__[2] = densu_(&z__, &gts3c_1.db16, &tinf, &gts3c_1.tlb, &c_b58, &alpha[
	    1], &t[2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    gts3c_1.dd = d__[2];
    if (z__ > altl[1] || csw_1.sw[14] == (float)0.) {
	goto L34;
    }
/*  Corrected from PDM(3,1) to PDM(3,2)  12/2/85 */
/*       Turbopause */
    zh16 = lower7_1.pdm[12];
/*      Mixed density at Zlb */
    r__1 = 16 - xmm;
    r__2 = alpha[1] - (float)1.;
    b16 = densu_(&zh16, &gts3c_1.db16, &tinf, &gts3c_1.tlb, &r__1, &r__2, &t[
	    2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
/*      Mixed density at Alt */
    dmix_1.dm16 = densu_(&z__, &b16, &tinf, &gts3c_1.tlb, &xmm, &c_b15, &t[2],
	     &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    zhm16 = zhm28;
/*      Net density at Alt */
    d__[2] = dnet_(&d__[2], &dmix_1.dm16, &zhm16, &xmm, &c_b58);
/*   3/16/99 Change form to match O2 departure from diff equil near 150 */
/*   km and add dependence on F10.7 */
/*      RL=ALOG(B28*PDM(2,2)*ABS(PDL(17,2))/B16) */
    gts3c_1.rl = lower7_1.pdm[11] * parm7_1.pdl[41] * (csw_1.sw[0] * 
	    parm7_1.pdl[23] * (*f107a - (float)150.) + (float)1.);
    hc16 = lower7_1.pdm[15] * parm7_1.pdl[28];
    zc16 = lower7_1.pdm[14] * parm7_1.pdl[27];
    hc216 = lower7_1.pdm[15] * parm7_1.pdl[29];
    d__[2] *= ccor2_(&z__, &gts3c_1.rl, &hc16, &zc16, &hc216);
/*       Chemistry correction */
    hcc16 = lower7_1.pdm[17] * parm7_1.pdl[38];
    zcc16 = lower7_1.pdm[16] * parm7_1.pdl[37];
    rc16 = lower7_1.pdm[13] * parm7_1.pdl[39];
/*      Net density corrected at Alt */
    d__[2] *= ccor_(&z__, &rc16, &hcc16, &zcc16);
L34:
    if (*mass != 48 && *mass != 49) {
	goto L90;
    }
L35:

/*       **** O2 DENSITY **** */

/*       Density variation factor at Zlb */
    g32 = csw_1.sw[20] * globe7_(&yrd, sec, glat, glong, stl, f107a, f107, &
	    ap[1], &parm7_1.pd[600]);
/*      Diffusive density at Zlb */
    gts3c_1.db32 = lower7_1.pdm[30] * exp(g32) * parm7_1.pd[600];
/*       Diffusive density at Alt */
    d__[4] = densu_(&z__, &gts3c_1.db32, &tinf, &gts3c_1.tlb, &c_b62, &alpha[
	    3], &t[2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    if (*mass == 49) {
	gts3c_1.dd += d__[4] * (float)2.;
    } else {
	gts3c_1.dd = d__[4];
    }
    if (csw_1.sw[14] == (float)0.) {
	goto L39;
    }
    if (z__ > altl[3]) {
	goto L38;
    }
/*       Turbopause */
    zh32 = lower7_1.pdm[32];
/*      Mixed density at Zlb */
    r__1 = (float)32. - xmm;
    r__2 = alpha[3] - (float)1.;
    b32 = densu_(&zh32, &gts3c_1.db32, &tinf, &gts3c_1.tlb, &r__1, &r__2, &t[
	    2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
/*      Mixed density at Alt */
    dmix_1.dm32 = densu_(&z__, &b32, &tinf, &gts3c_1.tlb, &xmm, &c_b15, &t[2],
	     &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    zhm32 = zhm28;
/*      Net density at Alt */
    d__[4] = dnet_(&d__[4], &dmix_1.dm32, &zhm32, &xmm, &c_b62);
/*       Correction to specified mixing ratio at ground */
    gts3c_1.rl = log(b28 * lower7_1.pdm[31] / b32);
    hc32 = lower7_1.pdm[35] * parm7_1.pdl[32];
    zc32 = lower7_1.pdm[34] * parm7_1.pdl[31];
    d__[4] *= ccor_(&z__, &gts3c_1.rl, &hc32, &zc32);
L38:
/*      Correction for general departure from diffusive equilibrium above Zlb */
    hcc32 = lower7_1.pdm[37] * parm7_1.pdl[47];
    hcc232 = lower7_1.pdm[37] * parm7_1.pdl[22];
    zcc32 = lower7_1.pdm[36] * parm7_1.pdl[46];
    rc32 = lower7_1.pdm[33] * parm7_1.pdl[48] * (csw_1.sw[0] * parm7_1.pdl[23]
	     * (*f107a - (float)150.) + (float)1.);
/*      Net density corrected at Alt */
    d__[4] *= ccor2_(&z__, &rc32, &hcc32, &zcc32, &hcc232);
L39:
    if (*mass != 48) {
	goto L90;
    }
L40:

/*       **** AR DENSITY **** */

/*       Density variation factor at Zlb */
    g40 = csw_1.sw[20] * globe7_(&yrd, sec, glat, glong, stl, f107a, f107, &
	    ap[1], &parm7_1.pd[750]);
/*      Diffusive density at Zlb */
    gts3c_1.db40 = lower7_1.pdm[40] * exp(g40) * parm7_1.pd[750];
/*       Diffusive density at Alt */
    d__[5] = densu_(&z__, &gts3c_1.db40, &tinf, &gts3c_1.tlb, &c_b67, &alpha[
	    4], &t[2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    gts3c_1.dd = d__[5];
    if (z__ > altl[4] || csw_1.sw[14] == (float)0.) {
	goto L44;
    }
/*       Turbopause */
    zh40 = lower7_1.pdm[42];
/*      Mixed density at Zlb */
    r__1 = (float)40. - xmm;
    r__2 = alpha[4] - (float)1.;
    b40 = densu_(&zh40, &gts3c_1.db40, &tinf, &gts3c_1.tlb, &r__1, &r__2, &t[
	    2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
/*      Mixed density at Alt */
    dmix_1.dm40 = densu_(&z__, &b40, &tinf, &gts3c_1.tlb, &xmm, &c_b15, &t[2],
	     &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    zhm40 = zhm28;
/*      Net density at Alt */
    d__[5] = dnet_(&d__[5], &dmix_1.dm40, &zhm40, &xmm, &c_b67);
/*       Correction to specified mixing ratio at ground */
    gts3c_1.rl = log(b28 * lower7_1.pdm[41] / b40);
    hc40 = lower7_1.pdm[45] * parm7_1.pdl[34];
    zc40 = lower7_1.pdm[44] * parm7_1.pdl[33];
/*      Net density corrected at Alt */
    d__[5] *= ccor_(&z__, &gts3c_1.rl, &hc40, &zc40);
L44:
    if (*mass != 48) {
	goto L90;
    }
L45:

/*        **** HYDROGEN DENSITY **** */

/*       Density variation factor at Zlb */
    g1 = csw_1.sw[20] * globe7_(&yrd, sec, glat, glong, stl, f107a, f107, &ap[
	    1], &parm7_1.pd[900]);
/*      Diffusive density at Zlb */
    gts3c_1.db01 = lower7_1.pdm[50] * exp(g1) * parm7_1.pd[900];
/*       Diffusive density at Alt */
    d__[7] = densu_(&z__, &gts3c_1.db01, &tinf, &gts3c_1.tlb, &c_b14, &alpha[
	    6], &t[2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    gts3c_1.dd = d__[7];
    if (z__ > altl[6] || csw_1.sw[14] == (float)0.) {
	goto L47;
    }
/*       Turbopause */
    zh01 = lower7_1.pdm[52];
/*      Mixed density at Zlb */
    r__1 = (float)1. - xmm;
    r__2 = alpha[6] - (float)1.;
    b01 = densu_(&zh01, &gts3c_1.db01, &tinf, &gts3c_1.tlb, &r__1, &r__2, &t[
	    2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
/*      Mixed density at Alt */
    dmix_1.dm01 = densu_(&z__, &b01, &tinf, &gts3c_1.tlb, &xmm, &c_b15, &t[2],
	     &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    zhm01 = zhm28;
/*      Net density at Alt */
    d__[7] = dnet_(&d__[7], &dmix_1.dm01, &zhm01, &xmm, &c_b14);
/*       Correction to specified mixing ratio at ground */
    gts3c_1.rl = log(b28 * lower7_1.pdm[51] * dabs(parm7_1.pdl[42]) / b01);
    hc01 = lower7_1.pdm[55] * parm7_1.pdl[36];
    zc01 = lower7_1.pdm[54] * parm7_1.pdl[35];
    d__[7] *= ccor_(&z__, &gts3c_1.rl, &hc01, &zc01);
/*       Chemistry correction */
    hcc01 = lower7_1.pdm[57] * parm7_1.pdl[44];
    zcc01 = lower7_1.pdm[56] * parm7_1.pdl[43];
    rc01 = lower7_1.pdm[53] * parm7_1.pdl[45];
/*      Net density corrected at Alt */
    d__[7] *= ccor_(&z__, &rc01, &hcc01, &zcc01);
L47:
    if (*mass != 48) {
	goto L90;
    }
L48:

/*        **** ATOMIC NITROGEN DENSITY **** */

/*       Density variation factor at Zlb */
    g14 = csw_1.sw[20] * globe7_(&yrd, sec, glat, glong, stl, f107a, f107, &
	    ap[1], &parm7_1.pd[1050]);
/*      Diffusive density at Zlb */
    gts3c_1.db14 = lower7_1.pdm[60] * exp(g14) * parm7_1.pd[1050];
/*       Diffusive density at Alt */
    d__[8] = densu_(&z__, &gts3c_1.db14, &tinf, &gts3c_1.tlb, &c_b75, &alpha[
	    7], &t[2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    gts3c_1.dd = d__[8];
    if (z__ > altl[7] || csw_1.sw[14] == (float)0.) {
	goto L49;
    }
/*       Turbopause */
    zh14 = lower7_1.pdm[62];
/*      Mixed density at Zlb */
    r__1 = (float)14. - xmm;
    r__2 = alpha[7] - (float)1.;
    b14 = densu_(&zh14, &gts3c_1.db14, &tinf, &gts3c_1.tlb, &r__1, &r__2, &t[
	    2], &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
/*      Mixed density at Alt */
    dmix_1.dm14 = densu_(&z__, &b14, &tinf, &gts3c_1.tlb, &xmm, &c_b15, &t[2],
	     &lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, 
	    meso7_1.tgn1);
    zhm14 = zhm28;
/*      Net density at Alt */
    d__[8] = dnet_(&d__[8], &dmix_1.dm14, &zhm14, &xmm, &c_b75);
/*       Correction to specified mixing ratio at ground */
    gts3c_1.rl = log(b28 * lower7_1.pdm[61] * dabs(parm7_1.pdl[2]) / b14);
    hc14 = lower7_1.pdm[65] * parm7_1.pdl[1];
    zc14 = lower7_1.pdm[64] * parm7_1.pdl[0];
    d__[8] *= ccor_(&z__, &gts3c_1.rl, &hc14, &zc14);
/*       Chemistry correction */
    hcc14 = lower7_1.pdm[67] * parm7_1.pdl[4];
    zcc14 = lower7_1.pdm[66] * parm7_1.pdl[3];
    rc14 = lower7_1.pdm[63] * parm7_1.pdl[5];
/*      Net density corrected at Alt */
    d__[8] *= ccor_(&z__, &rc14, &hcc14, &zcc14);
L49:
    if (*mass != 48) {
	goto L90;
    }
L46:

/*        **** Anomalous OXYGEN DENSITY **** */

    g16h = csw_1.sw[20] * globe7_(&yrd, sec, glat, glong, stl, f107a, f107, &
	    ap[1], &parm7_1.pd[1200]);
    db16h = lower7_1.pdm[70] * exp(g16h) * parm7_1.pd[1200];
    tho = lower7_1.pdm[79] * parm7_1.pdl[6];
    gts3c_1.dd = densu_(&z__, &db16h, &tho, &tho, &c_b58, &alpha[8], &t2, &
	    lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, meso7_1.tgn1)
	    ;
    zsht = lower7_1.pdm[75];
    zmho = lower7_1.pdm[74];
    zsho = scalh_(&zmho, &c_b58, &tho);
    d__[9] = gts3c_1.dd * exp(-zsht / zsho * (exp(-(z__ - zmho) / zsht) - (
	    float)1.));
    if (*mass != 48) {
	goto L90;
    }

/*       TOTAL MASS DENSITY */

    d__[6] = (d__[1] * (float)4. + d__[2] * (float)16. + d__[3] * (float)28. 
	    + d__[4] * (float)32. + d__[5] * (float)40. + d__[7] + d__[8] * (
	    float)14.) * (float)1.66e-24;
    gts3c_1.db48 = (gts3c_1.db04 * (float)4. + gts3c_1.db16 * (float)16. + 
	    gts3c_1.db28 * (float)28. + gts3c_1.db32 * (float)32. + 
	    gts3c_1.db40 * (float)40. + gts3c_1.db01 + gts3c_1.db14 * (float)
	    14.) * (float)1.66e-24;
    goto L90;
/*       TEMPERATURE AT ALTITUDE */
L50:
    z__ = dabs(*alt);
    ddum = densu_(&z__, &c_b14, &tinf, &gts3c_1.tlb, &c_b15, &c_b15, &t[2], &
	    lower7_1.ptm[5], &gts3c_1.s, &mn1, zn1, meso7_1.tn1, meso7_1.tgn1)
	    ;
L90:
/*       ADJUST DENSITIES FROM CGS TO KGM */
    if (metsel_1.imr == 1) {
	for (i__ = 1; i__ <= 9; ++i__) {
	    d__[i__] *= (float)1e6;
/* L95: */
	}
	d__[6] /= (float)1e3;
    }
    alast = *alt;
    return 0;
} /* gts7_ */

/* ----------------------------------------------------------------------- */
/* Subroutine */ int meters_(logical *meter)
{
/*      Convert outputs to Kg & Meters if METER true */
    metsel_1.imr = 0;
    if (*meter) {
	metsel_1.imr = 1;
    }
    return 0;
} /* meters_ */

/* ----------------------------------------------------------------------- */
doublereal scalh_(real *alt, real *xm, real *temp)
{
    /* Initialized data */

    static real rgas = (float)831.4;

    /* System generated locals */
    real ret_val, r__1;

    /* Local variables */
    static real g;

/*      Calculate scale height (km) */
/* Computing 2nd power */
    r__1 = (float)1. + *alt / parmb_1.re;
    g = parmb_1.gsurf / (r__1 * r__1);
    ret_val = rgas * *temp / (g * *xm);
    return ret_val;
} /* scalh_ */

/* ----------------------------------------------------------------------- */
doublereal globe7_(real *yrd, real *sec, real *lat, real *long__, real *tloc, 
	real *f107a, real *f107, real *ap, real *p)
{
    /* Initialized data */

    static real dgtr = (float).0174533;
    static real dr = (float).0172142;
    static real xl = (float)1e3;
    static real tll = (float)1e3;
    static real sw9 = (float)1.;
    static real dayl = (float)-1.;
    static real p14 = (float)-1e3;
    static real p18 = (float)-1e3;
    static real p32 = (float)-1e3;
    static real hr = (float).2618;
    static real sr = (float)7.2722e-5;
    static real sv[25] = { (float)1.,(float)1.,(float)1.,(float)1.,(float)1.,(
	    float)1.,(float)1.,(float)1.,(float)1.,(float)1.,(float)1.,(float)
	    1.,(float)1.,(float)1.,(float)1.,(float)1.,(float)1.,(float)1.,(
	    float)1.,(float)1.,(float)1.,(float)1.,(float)1.,(float)1.,(float)
	    1. };
    static integer nsw = 14;
    static real p39 = (float)-1e3;

    /* System generated locals */
    integer i__1;
    real ret_val, r__1, r__2, r__3, r__4, r__5, r__6;
    doublereal d__1;

    /* Builtin functions */
    double sin(doublereal), cos(doublereal), exp(doublereal), pow_dd(
	    doublereal *, doublereal *);

    /* Local variables */
    static real c__;
    static integer i__, j;
    static real s, c2, c4, f1, f2, s2, p44, p45, t71, t72, t81, t82, cd14, 
	    cd32, cd18, cd39, exp1;
    extern /* Subroutine */ int tselec_(real *);

/*       CALCULATE G(L) FUNCTION */
/*       Upper Thermosphere Parameters */
    /* Parameter adjustments */
    // Adjust the sv flag for hourly Ap is variation is seen
    integer i;
    for (i = 1; i < 7; ++i)
    {
       if (ap[i] != ap[0])
       {
          // If we are using daily/3hr observations, flag 9 should be -1.0
          sv[8] = -1.0;
          break;
       }
    }
    --p;
    --ap;


    /* Function Body */
/*       3hr Magnetic activity functions */
/*      Eq. A24d */
/*       Eq. A24c */
/*       Eq. A24a */
    if (csw_1.isw != 64999) {
	tselec_(sv);
    }
    for (j = 1; j <= 14; ++j) {
	ttest_2.t[j - 1] = (float)0.;
/* L10: */
    }
    if (csw_1.sw[8] > (float)0.) {
	sw9 = (float)1.;
    }
    if (csw_1.sw[8] < (float)0.) {
	sw9 = (float)-1.;
    }
    lpoly_1.iyr = *yrd / (float)1e3;
    lpoly_1.day = *yrd - lpoly_1.iyr * (float)1e3;
    lpoly_1.xlong = *long__;
/*      Eq. A22 (remainder of code) */
    if (xl == *lat) {
	goto L15;
    }
/*          CALCULATE LEGENDRE POLYNOMIALS */
    c__ = sin(*lat * dgtr);
    s = cos(*lat * dgtr);
    c2 = c__ * c__;
    c4 = c2 * c2;
    s2 = s * s;
    lpoly_1.plg[1] = c__;
    lpoly_1.plg[2] = (c2 * (float)3. - (float)1.) * (float).5;
    lpoly_1.plg[3] = (c__ * (float)5. * c2 - c__ * (float)3.) * (float).5;
    lpoly_1.plg[4] = (c4 * (float)35. - c2 * (float)30. + (float)3.) / (float)
	    8.;
    lpoly_1.plg[5] = (c2 * (float)63. * c2 * c__ - c2 * (float)70. * c__ + 
	    c__ * (float)15.) / (float)8.;
    lpoly_1.plg[6] = (c__ * (float)11. * lpoly_1.plg[5] - lpoly_1.plg[4] * (
	    float)5.) / (float)6.;
/*     PLG(8,1) = (13.*C*PLG(7,1) - 6.*PLG(6,1))/7. */
    lpoly_1.plg[10] = s;
    lpoly_1.plg[11] = c__ * (float)3. * s;
    lpoly_1.plg[12] = (c2 * (float)5. - (float)1.) * (float)1.5 * s;
    lpoly_1.plg[13] = (c2 * (float)7. * c__ - c__ * (float)3.) * (float)2.5 * 
	    s;
    lpoly_1.plg[14] = (c4 * (float)21. - c2 * (float)14. + (float)1.) * (
	    float)1.875 * s;
    lpoly_1.plg[15] = (c__ * (float)11. * lpoly_1.plg[14] - lpoly_1.plg[13] * 
	    (float)6.) / (float)5.;
/*     PLG(8,2) = (13.*C*PLG(7,2)-7.*PLG(6,2))/6. */
/*     PLG(9,2) = (15.*C*PLG(8,2)-8.*PLG(7,2))/7. */
    lpoly_1.plg[20] = s2 * (float)3.;
    lpoly_1.plg[21] = s2 * (float)15. * c__;
    lpoly_1.plg[22] = (c2 * (float)7. - (float)1.) * (float)7.5 * s2;
    lpoly_1.plg[23] = c__ * (float)3. * lpoly_1.plg[22] - lpoly_1.plg[21] * (
	    float)2.;
    lpoly_1.plg[24] = (c__ * (float)11. * lpoly_1.plg[23] - lpoly_1.plg[22] * 
	    (float)7.) / (float)4.;
    lpoly_1.plg[25] = (c__ * (float)13. * lpoly_1.plg[24] - lpoly_1.plg[23] * 
	    (float)8.) / (float)5.;
    lpoly_1.plg[30] = s2 * (float)15. * s;
    lpoly_1.plg[31] = s2 * (float)105. * s * c__;
    lpoly_1.plg[32] = (c__ * (float)9. * lpoly_1.plg[31] - lpoly_1.plg[30] * (
	    float)7.) / (float)2.;
    lpoly_1.plg[33] = (c__ * (float)11. * lpoly_1.plg[32] - lpoly_1.plg[31] * 
	    (float)8.) / (float)3.;
    xl = *lat;
L15:
    if (tll == *tloc) {
	goto L16;
    }
    if (csw_1.sw[6] == (float)0. && csw_1.sw[7] == (float)0. && csw_1.sw[13] 
	    == (float)0.) {
	goto L16;
    }
    lpoly_1.stloc = sin(hr * *tloc);
    lpoly_1.ctloc = cos(hr * *tloc);
    lpoly_1.s2tloc = sin(hr * (float)2. * *tloc);
    lpoly_1.c2tloc = cos(hr * (float)2. * *tloc);
    lpoly_1.s3tloc = sin(hr * (float)3. * *tloc);
    lpoly_1.c3tloc = cos(hr * (float)3. * *tloc);
    tll = *tloc;
L16:
    if (lpoly_1.day != dayl || p[14] != p14) {
	cd14 = cos(dr * (lpoly_1.day - p[14]));
    }
    if (lpoly_1.day != dayl || p[18] != p18) {
	cd18 = cos(dr * (float)2. * (lpoly_1.day - p[18]));
    }
    if (lpoly_1.day != dayl || p[32] != p32) {
	cd32 = cos(dr * (lpoly_1.day - p[32]));
    }
    if (lpoly_1.day != dayl || p[39] != p39) {
	cd39 = cos(dr * (float)2. * (lpoly_1.day - p[39]));
    }
    dayl = lpoly_1.day;
    p14 = p[14];
    p18 = p[18];
    p32 = p[32];
    p39 = p[39];
/*         F10.7 EFFECT */
    lpoly_1.df = *f107 - *f107a;
    lpoly_1.dfa = *f107a - (float)150.;
/* Computing 2nd power */
    r__1 = lpoly_1.dfa;
    ttest_2.t[0] = p[20] * lpoly_1.df * (p[60] * lpoly_1.dfa + (float)1.) + p[
	    21] * lpoly_1.df * lpoly_1.df + p[22] * lpoly_1.dfa + p[30] * (
	    r__1 * r__1);
    f1 = (p[48] * lpoly_1.dfa + p[20] * lpoly_1.df + p[21] * lpoly_1.df * 
	    lpoly_1.df) * csw_1.swc[0] + (float)1.;
    f2 = (p[50] * lpoly_1.dfa + p[20] * lpoly_1.df + p[21] * lpoly_1.df * 
	    lpoly_1.df) * csw_1.swc[0] + (float)1.;
/*        TIME INDEPENDENT */
    ttest_2.t[1] = p[2] * lpoly_1.plg[2] + p[3] * lpoly_1.plg[4] + p[23] * 
	    lpoly_1.plg[6] + p[15] * lpoly_1.plg[2] * lpoly_1.dfa * csw_1.swc[
	    0] + p[27] * lpoly_1.plg[1];
/*        SYMMETRICAL ANNUAL */
    ttest_2.t[2] = p[19] * cd32;
/*        SYMMETRICAL SEMIANNUAL */
    ttest_2.t[3] = (p[16] + p[17] * lpoly_1.plg[2]) * cd18;
/*        ASYMMETRICAL ANNUAL */
    ttest_2.t[4] = f1 * (p[10] * lpoly_1.plg[1] + p[11] * lpoly_1.plg[3]) * 
	    cd14;
/*         ASYMMETRICAL SEMIANNUAL */
    ttest_2.t[5] = p[38] * lpoly_1.plg[1] * cd39;
/*        DIURNAL */
    if (csw_1.sw[6] == (float)0.) {
	goto L200;
    }
    t71 = p[12] * lpoly_1.plg[11] * cd14 * csw_1.swc[4];
    t72 = p[13] * lpoly_1.plg[11] * cd14 * csw_1.swc[4];
    ttest_2.t[6] = f2 * ((p[4] * lpoly_1.plg[10] + p[5] * lpoly_1.plg[12] + p[
	    28] * lpoly_1.plg[14] + t71) * lpoly_1.ctloc + (p[7] * 
	    lpoly_1.plg[10] + p[8] * lpoly_1.plg[12] + p[29] * lpoly_1.plg[14]
	     + t72) * lpoly_1.stloc);
L200:
/*        SEMIDIURNAL */
    if (csw_1.sw[7] == (float)0.) {
	goto L210;
    }
    t81 = (p[24] * lpoly_1.plg[21] + p[36] * lpoly_1.plg[23]) * cd14 * 
	    csw_1.swc[4];
    t82 = (p[34] * lpoly_1.plg[21] + p[37] * lpoly_1.plg[23]) * cd14 * 
	    csw_1.swc[4];
    ttest_2.t[7] = f2 * ((p[6] * lpoly_1.plg[20] + p[42] * lpoly_1.plg[22] + 
	    t81) * lpoly_1.c2tloc + (p[9] * lpoly_1.plg[20] + p[43] * 
	    lpoly_1.plg[22] + t82) * lpoly_1.s2tloc);
L210:
/*        TERDIURNAL */
    if (csw_1.sw[13] == (float)0.) {
	goto L220;
    }
    ttest_2.t[13] = f2 * ((p[40] * lpoly_1.plg[30] + (p[94] * lpoly_1.plg[31] 
	    + p[47] * lpoly_1.plg[33]) * cd14 * csw_1.swc[4]) * 
	    lpoly_1.s3tloc + (p[41] * lpoly_1.plg[30] + (p[95] * lpoly_1.plg[
	    31] + p[49] * lpoly_1.plg[33]) * cd14 * csw_1.swc[4]) * 
	    lpoly_1.c3tloc);
L220:
/*          MAGNETIC ACTIVITY BASED ON DAILY AP */
    if (sw9 == (float)-1.) {
	goto L30;
    }
    lpoly_1.apd = ap[1] - (float)4.;
    p44 = p[44];
    p45 = p[45];
    if (p44 < (float)0.) {
	p44 = (float)1e-5;
    }
    lpoly_1.apdf = lpoly_1.apd + (p45 - (float)1.) * (lpoly_1.apd + (exp(-p44 
	    * lpoly_1.apd) - (float)1.) / p44);
    if (csw_1.sw[8] == (float)0.) {
	goto L40;
    }
    ttest_2.t[8] = lpoly_1.apdf * (p[33] + p[46] * lpoly_1.plg[2] + p[35] * 
	    lpoly_1.plg[4] + (p[101] * lpoly_1.plg[1] + p[102] * lpoly_1.plg[
	    3] + p[103] * lpoly_1.plg[5]) * cd14 * csw_1.swc[4] + (p[122] * 
	    lpoly_1.plg[10] + p[123] * lpoly_1.plg[12] + p[124] * lpoly_1.plg[
	    14]) * csw_1.swc[6] * cos(hr * (*tloc - p[125])));
    goto L40;
L30:
    if (p[52] == (float)0.) {
	goto L40;
    }
    exp1 = exp(dabs(p[52]) * (float)-10800. / (p[139] * ((float)45. - dabs(*
	    lat)) + (float)1.));
    if (exp1 > (float).99999) {
	exp1 = (float).99999;
    }
    if (p[25] < (float)1e-4) {
	p[25] = (float)1e-4;
    }
/* Computing 3rd power */
    r__1 = exp1;
/* Computing 4th power */
    r__2 = exp1, r__2 *= r__2;
/* Computing 12th power */
    r__3 = exp1, r__3 *= r__3, r__3 *= r__3;
/* Computing 8th power */
    r__4 = exp1, r__4 *= r__4, r__4 *= r__4;
/* Computing 19th power */
    r__5 = exp1, r__6 = r__5, r__5 *= r__5, r__6 *= r__5, r__5 *= r__5, r__5 
	    *= r__5;
    d__1 = (doublereal) exp1;
    lpoly_1.apt[0] = (ap[2] - (float)4. + (p[26] - (float)1.) * (ap[2] - (
	    float)4. + (exp(-dabs(p[25]) * (ap[2] - (float)4.)) - (float)1.) /
	     dabs(p[25])) + ((ap[3] - (float)4. + (p[26] - (float)1.) * (ap[3]
	     - (float)4. + (exp(-dabs(p[25]) * (ap[3] - (float)4.)) - (float)
	    1.) / dabs(p[25]))) * exp1 + (ap[4] - (float)4. + (p[26] - (float)
	    1.) * (ap[4] - (float)4. + (exp(-dabs(p[25]) * (ap[4] - (float)4.)
	    ) - (float)1.) / dabs(p[25]))) * exp1 * exp1 + (ap[5] - (float)4. 
	    + (p[26] - (float)1.) * (ap[5] - (float)4. + (exp(-dabs(p[25]) * (
	    ap[5] - (float)4.)) - (float)1.) / dabs(p[25]))) * (r__1 * (r__1 *
	     r__1)) + ((ap[6] - (float)4. + (p[26] - (float)1.) * (ap[6] - (
	    float)4. + (exp(-dabs(p[25]) * (ap[6] - (float)4.)) - (float)1.) /
	     dabs(p[25]))) * (r__2 * r__2) + (ap[7] - (float)4. + (p[26] - (
	    float)1.) * (ap[7] - (float)4. + (exp(-dabs(p[25]) * (ap[7] - (
	    float)4.)) - (float)1.) / dabs(p[25]))) * (r__3 * (r__3 * r__3))) 
	    * ((float)1. - r__4 * r__4) / ((float)1. - exp1))) / ((float)1. + 
	    ((float)1. - r__6 * (r__5 * r__5)) / ((float)1. - exp1) * pow_dd(&
	    d__1, &c_b96));
/*      APT(2)=SG2(EXP1) */
/*      APT(3)=SG0(EXP2) */
/*      APT(4)=SG2(EXP2) */
    if (csw_1.sw[8] == (float)0.) {
	goto L40;
    }
    ttest_2.t[8] = lpoly_1.apt[0] * (p[51] + p[97] * lpoly_1.plg[2] + p[55] * 
	    lpoly_1.plg[4] + (p[126] * lpoly_1.plg[1] + p[127] * lpoly_1.plg[
	    3] + p[128] * lpoly_1.plg[5]) * cd14 * csw_1.swc[4] + (p[129] * 
	    lpoly_1.plg[10] + p[130] * lpoly_1.plg[12] + p[131] * lpoly_1.plg[
	    14]) * csw_1.swc[6] * cos(hr * (*tloc - p[132])));
L40:
    if (csw_1.sw[9] == (float)0. || *long__ <= (float)-1e3) {
	goto L49;
    }
/*        LONGITUDINAL */
    if (csw_1.sw[10] == (float)0.) {
	goto L230;
    }
    ttest_2.t[10] = (p[81] * lpoly_1.dfa * csw_1.swc[0] + (float)1.) * ((p[65]
	     * lpoly_1.plg[11] + p[66] * lpoly_1.plg[13] + p[67] * 
	    lpoly_1.plg[15] + p[104] * lpoly_1.plg[10] + p[105] * lpoly_1.plg[
	    12] + p[106] * lpoly_1.plg[14] + csw_1.swc[4] * (p[110] * 
	    lpoly_1.plg[10] + p[111] * lpoly_1.plg[12] + p[112] * lpoly_1.plg[
	    14]) * cd14) * cos(dgtr * *long__) + (p[91] * lpoly_1.plg[11] + p[
	    92] * lpoly_1.plg[13] + p[93] * lpoly_1.plg[15] + p[107] * 
	    lpoly_1.plg[10] + p[108] * lpoly_1.plg[12] + p[109] * lpoly_1.plg[
	    14] + csw_1.swc[4] * (p[113] * lpoly_1.plg[10] + p[114] * 
	    lpoly_1.plg[12] + p[115] * lpoly_1.plg[14]) * cd14) * sin(dgtr * *
	    long__));
L230:
/*        UT AND MIXED UT,LONGITUDE */
    if (csw_1.sw[11] == (float)0.) {
	goto L240;
    }
    ttest_2.t[11] = (p[96] * lpoly_1.plg[1] + (float)1.) * (p[82] * 
	    lpoly_1.dfa * csw_1.swc[0] + (float)1.) * (p[120] * lpoly_1.plg[1]
	     * csw_1.swc[4] * cd14 + (float)1.) * ((p[69] * lpoly_1.plg[1] + 
	    p[70] * lpoly_1.plg[3] + p[71] * lpoly_1.plg[5]) * cos(sr * (*sec 
	    - p[72])));
    ttest_2.t[11] += csw_1.swc[10] * (p[77] * lpoly_1.plg[21] + p[78] * 
	    lpoly_1.plg[23] + p[79] * lpoly_1.plg[25]) * cos(sr * (*sec - p[
	    80]) + dgtr * (float)2. * *long__) * (p[138] * lpoly_1.dfa * 
	    csw_1.swc[0] + (float)1.);
L240:
/*        UT,LONGITUDE MAGNETIC ACTIVITY */
    if (csw_1.sw[12] == (float)0.) {
	goto L48;
    }
    if (sw9 == (float)-1.) {
	goto L45;
    }
    ttest_2.t[12] = lpoly_1.apdf * csw_1.swc[10] * (p[121] * lpoly_1.plg[1] + 
	    (float)1.) * ((p[61] * lpoly_1.plg[11] + p[62] * lpoly_1.plg[13] 
	    + p[63] * lpoly_1.plg[15]) * cos(dgtr * (*long__ - p[64]))) + 
	    lpoly_1.apdf * csw_1.swc[10] * csw_1.swc[4] * (p[116] * 
	    lpoly_1.plg[10] + p[117] * lpoly_1.plg[12] + p[118] * lpoly_1.plg[
	    14]) * cd14 * cos(dgtr * (*long__ - p[119])) + lpoly_1.apdf * 
	    csw_1.swc[11] * (p[84] * lpoly_1.plg[1] + p[85] * lpoly_1.plg[3] 
	    + p[86] * lpoly_1.plg[5]) * cos(sr * (*sec - p[76]));
    goto L48;
L45:
    if (p[52] == (float)0.) {
	goto L48;
    }
    ttest_2.t[12] = lpoly_1.apt[0] * csw_1.swc[10] * (p[133] * lpoly_1.plg[1] 
	    + (float)1.) * ((p[53] * lpoly_1.plg[11] + p[99] * lpoly_1.plg[13]
	     + p[68] * lpoly_1.plg[15]) * cos(dgtr * (*long__ - p[98]))) + 
	    lpoly_1.apt[0] * csw_1.swc[10] * csw_1.swc[4] * (p[134] * 
	    lpoly_1.plg[10] + p[135] * lpoly_1.plg[12] + p[136] * lpoly_1.plg[
	    14]) * cd14 * cos(dgtr * (*long__ - p[137])) + lpoly_1.apt[0] * 
	    csw_1.swc[11] * (p[56] * lpoly_1.plg[1] + p[57] * lpoly_1.plg[3] 
	    + p[58] * lpoly_1.plg[5]) * cos(sr * (*sec - p[59]));
L48:
/*  PARMS NOT USED: 83, 90,100,140-150 */
L49:
    ttest_2.tinf = p[31];
    i__1 = nsw;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L50: */
	ttest_2.tinf += (r__1 = csw_1.sw[i__ - 1], dabs(r__1)) * ttest_2.t[
		i__ - 1];
    }
    ret_val = ttest_2.tinf;
    return ret_val;
} /* globe7_ */

/* ----------------------------------------------------------------------- */
/* Subroutine */ int tselec_0_(int n__, real *sv, real *svv)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double r_mod(real *, real *);

    /* Local variables */
    static integer i__;
    static real sav[25];

/*        SET SWITCHES */
/*        Output in  COMMON/CSW/SW(25),ISW,SWC(25) */
/*        SW FOR MAIN TERMS, SWC FOR CROSS TERMS */

/*        TO TURN ON AND OFF PARTICULAR VARIATIONS CALL TSELEC(SV), */
/*        WHERE SV IS A 25 ELEMENT ARRAY CONTAINING 0. FOR OFF, 1. */
/*        FOR ON, OR 2. FOR MAIN EFFECTS OFF BUT CROSS TERMS ON */

/*        To get current values of SW: CALL TRETRV(SW) */

    /* Parameter adjustments */
    if (sv) {
	--sv;
	}
    if (svv) {
	--svv;
	}

    /* Function Body */
    switch(n__) {
	case 1: goto L_tretrv;
	}

    for (i__ = 1; i__ <= 25; ++i__) {
	sav[i__ - 1] = sv[i__];
	csw_1.sw[i__ - 1] = r_mod(&sv[i__], &c_b105);
	if ((r__1 = sv[i__], dabs(r__1)) == (float)1. || (r__2 = sv[i__], 
		dabs(r__2)) == (float)2.) {
	    csw_1.swc[i__ - 1] = (float)1.;
	} else {
	    csw_1.swc[i__ - 1] = (float)0.;
	}
/* L100: */
    }
    csw_1.isw = 64999;
    return 0;

L_tretrv:
    for (i__ = 1; i__ <= 25; ++i__) {
	svv[i__] = sav[i__ - 1];
/* L200: */
    }
    return 0;
} /* tselec_ */

/* Subroutine */ int tselec_(real *sv)
{
    return tselec_0_(0, sv, (real *)0);
    }

/* Subroutine */ int tretrv_(real *svv)
{
    return tselec_0_(1, (real *)0, svv);
    }

/* ----------------------------------------------------------------------- */
doublereal glob7s_(real *p)
{
    /* Initialized data */

    static real dr = (float).0172142;
    static real dgtr = (float).0174533;
    static real pset = (float)2.;
    static real dayl = (float)-1.;
    static real p32 = (float)-1e3;
    static real p18 = (float)-1e3;
    static real p14 = (float)-1e3;
    static real p39 = (float)-1e3;

    /* Format strings */
    static char fmt_900[] = "(1x,\002WRONG PARAMETER SET FOR GLOB7S\002,3f10\
.1)";

    /* System generated locals */
    real ret_val, r__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe();
    /* Subroutine */ int s_stop(char *, ftnlen);
    double cos(doublereal), sin(doublereal);

    /* Local variables */
    static integer i__, j;
    static real t[14], t71, t72, t81, t82, tt, cd32, cd14, cd18, cd39;

    /* Fortran I/O blocks */
    static cilist io___184 = { 0, 6, 0, fmt_900, 0 };


/*      VERSION OF GLOBE FOR LOWER ATMOSPHERE 10/26/99 */
    /* Parameter adjustments */
    --p;

    /* Function Body */
/*       CONFIRM PARAMETER SET */
    if (p[100] == (float)0.) {
	p[100] = pset;
    }
    if (p[100] != pset) {
	s_wsfe(&io___184);
	do_fio(&c__1, (char *)&pset, (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&p[100], (ftnlen)sizeof(real));
	e_wsfe();
	s_stop("", (ftnlen)0);
    }
    for (j = 1; j <= 14; ++j) {
	t[j - 1] = (float)0.;
/* L10: */
    }
    if (lpoly_2.day != dayl || p32 != p[32]) {
	cd32 = cos(dr * (lpoly_2.day - p[32]));
    }
    if (lpoly_2.day != dayl || p18 != p[18]) {
	cd18 = cos(dr * (float)2. * (lpoly_2.day - p[18]));
    }
    if (lpoly_2.day != dayl || p14 != p[14]) {
	cd14 = cos(dr * (lpoly_2.day - p[14]));
    }
    if (lpoly_2.day != dayl || p39 != p[39]) {
	cd39 = cos(dr * (float)2. * (lpoly_2.day - p[39]));
    }
    dayl = lpoly_2.day;
    p32 = p[32];
    p18 = p[18];
    p14 = p[14];
    p39 = p[39];

/*       F10.7 */
    t[0] = p[22] * lpoly_2.dfa;
/*       TIME INDEPENDENT */
    t[1] = p[2] * lpoly_2.plg[2] + p[3] * lpoly_2.plg[4] + p[23] * 
	    lpoly_2.plg[6] + p[27] * lpoly_2.plg[1] + p[15] * lpoly_2.plg[3] 
	    + p[60] * lpoly_2.plg[5];
/*       SYMMETRICAL ANNUAL */
    t[2] = (p[19] + p[48] * lpoly_2.plg[2] + p[30] * lpoly_2.plg[4]) * cd32;
/*       SYMMETRICAL SEMIANNUAL */
    t[3] = (p[16] + p[17] * lpoly_2.plg[2] + p[31] * lpoly_2.plg[4]) * cd18;
/*       ASYMMETRICAL ANNUAL */
    t[4] = (p[10] * lpoly_2.plg[1] + p[11] * lpoly_2.plg[3] + p[21] * 
	    lpoly_2.plg[5]) * cd14;
/*       ASYMMETRICAL SEMIANNUAL */
    t[5] = p[38] * lpoly_2.plg[1] * cd39;
/*        DIURNAL */
    if (csw_1.sw[6] == (float)0.) {
	goto L200;
    }
    t71 = p[12] * lpoly_2.plg[11] * cd14 * csw_1.swc[4];
    t72 = p[13] * lpoly_2.plg[11] * cd14 * csw_1.swc[4];
    t[6] = (p[4] * lpoly_2.plg[10] + p[5] * lpoly_2.plg[12] + t71) * 
	    lpoly_2.ctloc + (p[7] * lpoly_2.plg[10] + p[8] * lpoly_2.plg[12] 
	    + t72) * lpoly_2.stloc;
L200:
/*        SEMIDIURNAL */
    if (csw_1.sw[7] == (float)0.) {
	goto L210;
    }
    t81 = (p[24] * lpoly_2.plg[21] + p[36] * lpoly_2.plg[23]) * cd14 * 
	    csw_1.swc[4];
    t82 = (p[34] * lpoly_2.plg[21] + p[37] * lpoly_2.plg[23]) * cd14 * 
	    csw_1.swc[4];
    t[7] = (p[6] * lpoly_2.plg[20] + p[42] * lpoly_2.plg[22] + t81) * 
	    lpoly_2.c2tloc + (p[9] * lpoly_2.plg[20] + p[43] * lpoly_2.plg[22]
	     + t82) * lpoly_2.s2tloc;
L210:
/*        TERDIURNAL */
    if (csw_1.sw[13] == (float)0.) {
	goto L220;
    }
    t[13] = p[40] * lpoly_2.plg[30] * lpoly_2.s3tloc + p[41] * lpoly_2.plg[30]
	     * lpoly_2.c3tloc;
L220:
/*       MAGNETIC ACTIVITY */
    if (csw_1.sw[8] == (float)0.) {
	goto L40;
    }
    if (csw_1.sw[8] == (float)1.) {
	t[8] = lpoly_2.apdf * (p[33] + p[46] * lpoly_2.plg[2] * csw_1.swc[1]);
    }
    if (csw_1.sw[8] == (float)-1.) {
	t[8] = p[51] * lpoly_2.apt[0] + p[97] * lpoly_2.plg[2] * lpoly_2.apt[
		0] * csw_1.swc[1];
    }
L40:
    if (csw_1.sw[9] == (float)0. || csw_1.sw[10] == (float)0. || 
	    lpoly_2.long__ <= (float)-1e3) {
	goto L49;
    }
/*        LONGITUDINAL */
    t[10] = (lpoly_2.plg[1] * (p[81] * csw_1.swc[4] * cos(dr * (lpoly_2.day - 
	    p[82])) + p[86] * csw_1.swc[5] * cos(dr * (float)2. * (
	    lpoly_2.day - p[87]))) + (float)1. + p[84] * csw_1.swc[2] * cos(
	    dr * (lpoly_2.day - p[85])) + p[88] * csw_1.swc[3] * cos(dr * (
	    float)2. * (lpoly_2.day - p[89]))) * ((p[65] * lpoly_2.plg[11] + 
	    p[66] * lpoly_2.plg[13] + p[67] * lpoly_2.plg[15] + p[75] * 
	    lpoly_2.plg[10] + p[76] * lpoly_2.plg[12] + p[77] * lpoly_2.plg[
	    14]) * cos(dgtr * lpoly_2.long__) + (p[91] * lpoly_2.plg[11] + p[
	    92] * lpoly_2.plg[13] + p[93] * lpoly_2.plg[15] + p[78] * 
	    lpoly_2.plg[10] + p[79] * lpoly_2.plg[12] + p[80] * lpoly_2.plg[
	    14]) * sin(dgtr * lpoly_2.long__));
L49:
    tt = (float)0.;
    for (i__ = 1; i__ <= 14; ++i__) {
/* L50: */
	tt += (r__1 = csw_1.sw[i__ - 1], dabs(r__1)) * t[i__ - 1];
    }
    ret_val = tt;
    return ret_val;
} /* glob7s_ */

/* -------------------------------------------------------------------- */
doublereal densu_(real *alt, real *dlb, real *tinf, real *tlb, real *xm, real 
	*alpha, real *tz, real *zlb, real *s2, integer *mn1, real *zn1, real *
	tn1, real *tgn1)
{
    /* Initialized data */

    static real rgas = (float)831.4;

    /* System generated locals */
    integer i__1;
    real ret_val, r__1, r__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    double exp(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static integer k;
    static real x, y, z__, t1, t2, z1, z2, ta, za;
    static integer mn;
    static real zg, yi, tt, xs[5], ys[5], yd1, yd2, zg2, glb, dta, gamm, expl,
	     y2out[5], gamma, densa, zgdif;
    extern /* Subroutine */ int spline_(real *, real *, integer *, real *, 
	    real *, real *), splini_(real *, real *, real *, integer *, real *
	    , real *), splint_(real *, real *, real *, integer *, real *, 
	    real *);

/*       Calculate Temperature and Density Profiles for MSIS models */
/*       New lower thermo polynomial 10/30/89 */
    /* Parameter adjustments */
    --tn1;
    --zn1;
    --tgn1;

    /* Function Body */
/* CCCCCWRITE(6,*) 'DB',ALT,DLB,TINF,TLB,XM,ALPHA,ZLB,S2,MN1,ZN1,TN1 */
    ret_val = (float)1.;
/*        Joining altitude of Bates and spline */
    za = zn1[1];
    z__ = dmax(*alt,za);
/*      Geopotential altitude difference from ZLB */
    zg2 = (z__ - *zlb) * (parmb_1.re + *zlb) / (parmb_1.re + z__);
/*      Bates temperature */
    tt = *tinf - (*tinf - *tlb) * exp(-(*s2) * zg2);
    ta = tt;
    *tz = tt;
    ret_val = *tz;
    if (*alt >= za) {
	goto L10;
    }

/*       CALCULATE TEMPERATURE BELOW ZA */
/*      Temperature gradient at ZA from Bates profile */
/* Computing 2nd power */
    r__1 = (parmb_1.re + *zlb) / (parmb_1.re + za);
    dta = (*tinf - ta) * *s2 * (r__1 * r__1);
    tgn1[1] = dta;
    tn1[1] = ta;
/* Computing MAX */
    r__1 = *alt, r__2 = zn1[*mn1];
    z__ = dmax(r__1,r__2);
    mn = *mn1;
    z1 = zn1[1];
    z2 = zn1[mn];
    t1 = tn1[1];
    t2 = tn1[mn];
/*      Geopotental difference from Z1 */
    zg = (z__ - z1) * (parmb_1.re + z1) / (parmb_1.re + z__);
    zgdif = (z2 - z1) * (parmb_1.re + z1) / (parmb_1.re + z2);
/*       Set up spline nodes */
    i__1 = mn;
    for (k = 1; k <= i__1; ++k) {
	xs[k - 1] = (zn1[k] - z1) * (parmb_1.re + z1) / (parmb_1.re + zn1[k]) 
		/ zgdif;
	ys[k - 1] = (float)1. / tn1[k];
/* L20: */
    }
/*        End node derivatives */
    yd1 = -tgn1[1] / (t1 * t1) * zgdif;
/* Computing 2nd power */
    r__1 = (parmb_1.re + z2) / (parmb_1.re + z1);
    yd2 = -tgn1[2] / (t2 * t2) * zgdif * (r__1 * r__1);
/*       Calculate spline coefficients */
    spline_(xs, ys, &mn, &yd1, &yd2, y2out);
    x = zg / zgdif;
    splint_(xs, ys, y2out, &mn, &x, &y);
/*       temperature at altitude */
    *tz = (float)1. / y;
    ret_val = *tz;
L10:
    if (*xm == (float)0.) {
	goto L50;
    }

/*      CALCULATE DENSITY ABOVE ZA */
/* Computing 2nd power */
    r__1 = *zlb / parmb_1.re + (float)1.;
    glb = parmb_1.gsurf / (r__1 * r__1);
    gamma = *xm * glb / (*s2 * rgas * *tinf);
    expl = exp(-(*s2) * gamma * zg2);
    if (expl > (float)50. || tt <= (float)0.) {
	expl = (float)50.;
    }
/*       Density at altitude */
    d__1 = (doublereal) (*tlb / tt);
    d__2 = (doublereal) (*alpha + (float)1. + gamma);
    densa = *dlb * pow_dd(&d__1, &d__2) * expl;
    ret_val = densa;
    if (*alt >= za) {
	goto L50;
    }

/*      CALCULATE DENSITY BELOW ZA */
/* Computing 2nd power */
    r__1 = z1 / parmb_1.re + (float)1.;
    glb = parmb_1.gsurf / (r__1 * r__1);
    gamm = *xm * glb * zgdif / rgas;
/*       integrate spline temperatures */
    splini_(xs, ys, y2out, &mn, &x, &yi);
    expl = gamm * yi;
    if (expl > (float)50. || *tz <= (float)0.) {
	expl = (float)50.;
    }
/*       Density at altitude */
    d__1 = (doublereal) (t1 / *tz);
    d__2 = (doublereal) (*alpha + (float)1.);
    ret_val = ret_val * pow_dd(&d__1, &d__2) * exp(-expl);
L50:
    return ret_val;
} /* densu_ */

/* -------------------------------------------------------------------- */
doublereal densm_(real *alt, real *d0, real *xm, real *tz, integer *mn3, real 
	*zn3, real *tn3, real *tgn3, integer *mn2, real *zn2, real *tn2, real 
	*tgn2)
{
    /* Initialized data */

    static real rgas = (float)831.4;

    /* System generated locals */
    integer i__1;
    real ret_val, r__1, r__2;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static integer k;
    static real x, y, z__, t1, t2, z1, z2;
    static integer mn;
    static real zg, yi, xs[10], ys[10], yd1, yd2, glb, gamm, expl, y2out[10], 
	    zgdif;
    extern /* Subroutine */ int spline_(real *, real *, integer *, real *, 
	    real *, real *), splini_(real *, real *, real *, integer *, real *
	    , real *), splint_(real *, real *, real *, integer *, real *, 
	    real *);

/*       Calculate Temperature and Density Profiles for lower atmos. */
    /* Parameter adjustments */
    --tn3;
    --zn3;
    --tgn3;
    --tn2;
    --zn2;
    --tgn2;

    /* Function Body */
    ret_val = *d0;
    if (*alt > zn2[1]) {
	goto L50;
    }
/*      STRATOSPHERE/MESOSPHERE TEMPERATURE */
/* Computing MAX */
    r__1 = *alt, r__2 = zn2[*mn2];
    z__ = dmax(r__1,r__2);
    mn = *mn2;
    z1 = zn2[1];
    z2 = zn2[mn];
    t1 = tn2[1];
    t2 = tn2[mn];
    zg = (z__ - z1) * (parmb_1.re + z1) / (parmb_1.re + z__);
    zgdif = (z2 - z1) * (parmb_1.re + z1) / (parmb_1.re + z2);
/*       Set up spline nodes */
    i__1 = mn;
    for (k = 1; k <= i__1; ++k) {
	xs[k - 1] = (zn2[k] - z1) * (parmb_1.re + z1) / (parmb_1.re + zn2[k]) 
		/ zgdif;
	ys[k - 1] = (float)1. / tn2[k];
/* L210: */
    }
    yd1 = -tgn2[1] / (t1 * t1) * zgdif;
/* Computing 2nd power */
    r__1 = (parmb_1.re + z2) / (parmb_1.re + z1);
    yd2 = -tgn2[2] / (t2 * t2) * zgdif * (r__1 * r__1);
/*       Calculate spline coefficients */
    spline_(xs, ys, &mn, &yd1, &yd2, y2out);
    x = zg / zgdif;
    splint_(xs, ys, y2out, &mn, &x, &y);
/*       Temperature at altitude */
    *tz = (float)1. / y;
    if (*xm == (float)0.) {
	goto L20;
    }

/*      CALCULATE STRATOSPHERE/MESOSPHERE DENSITY */
/* Computing 2nd power */
    r__1 = z1 / parmb_1.re + (float)1.;
    glb = parmb_1.gsurf / (r__1 * r__1);
    gamm = *xm * glb * zgdif / rgas;
/*       Integrate temperature profile */
    splini_(xs, ys, y2out, &mn, &x, &yi);
    expl = gamm * yi;
    if (expl > (float)50.) {
	expl = (float)50.;
    }
/*       Density at altitude */
    ret_val = ret_val * (t1 / *tz) * exp(-expl);
L20:
    if (*alt > zn3[1]) {
	goto L50;
    }

/*      TROPOSPHERE/STRATOSPHERE TEMPERATURE */
    z__ = *alt;
    mn = *mn3;
    z1 = zn3[1];
    z2 = zn3[mn];
    t1 = tn3[1];
    t2 = tn3[mn];
    zg = (z__ - z1) * (parmb_1.re + z1) / (parmb_1.re + z__);
    zgdif = (z2 - z1) * (parmb_1.re + z1) / (parmb_1.re + z2);
/*       Set up spline nodes */
    i__1 = mn;
    for (k = 1; k <= i__1; ++k) {
	xs[k - 1] = (zn3[k] - z1) * (parmb_1.re + z1) / (parmb_1.re + zn3[k]) 
		/ zgdif;
	ys[k - 1] = (float)1. / tn3[k];
/* L220: */
    }
    yd1 = -tgn3[1] / (t1 * t1) * zgdif;
/* Computing 2nd power */
    r__1 = (parmb_1.re + z2) / (parmb_1.re + z1);
    yd2 = -tgn3[2] / (t2 * t2) * zgdif * (r__1 * r__1);
/*       Calculate spline coefficients */
    spline_(xs, ys, &mn, &yd1, &yd2, y2out);
    x = zg / zgdif;
    splint_(xs, ys, y2out, &mn, &x, &y);
/*       temperature at altitude */
    *tz = (float)1. / y;
    if (*xm == (float)0.) {
	goto L30;
    }

/*      CALCULATE TROPOSPHERIC/STRATOSPHERE DENSITY */

/* Computing 2nd power */
    r__1 = z1 / parmb_1.re + (float)1.;
    glb = parmb_1.gsurf / (r__1 * r__1);
    gamm = *xm * glb * zgdif / rgas;
/*        Integrate temperature profile */
    splini_(xs, ys, y2out, &mn, &x, &yi);
    expl = gamm * yi;
    if (expl > (float)50.) {
	expl = (float)50.;
    }
/*        Density at altitude */
    ret_val = ret_val * (t1 / *tz) * exp(-expl);
L30:
L50:
    if (*xm == (float)0.) {
	ret_val = *tz;
    }
    return ret_val;
} /* densm_ */

/* ----------------------------------------------------------------------- */
/* Subroutine */ int spline_(real *x, real *y, integer *n, real *yp1, real *
	ypn, real *y2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, k;
    static real p, u[100], qn, un, sig;

/*        CALCULATE 2ND DERIVATIVES OF CUBIC SPLINE INTERP FUNCTION */
/*        ADAPTED FROM NUMERICAL RECIPES BY PRESS ET AL */
/*        X,Y: ARRAYS OF TABULATED FUNCTION IN ASCENDING ORDER BY X */
/*        N: SIZE OF ARRAYS X,Y */
/*        YP1,YPN: SPECIFIED DERIVATIVES AT X(1) AND X(N); VALUES */
/*                 >= 1E30 SIGNAL SIGNAL SECOND DERIVATIVE ZERO */
/*        Y2: OUTPUT ARRAY OF SECOND DERIVATIVES */
    /* Parameter adjustments */
    --y2;
    --y;
    --x;

    /* Function Body */
    if (*yp1 > (float)9.9e29) {
	y2[1] = (float)0.;
	u[0] = (float)0.;
    } else {
	y2[1] = (float)-.5;
	u[0] = (float)3. / (x[2] - x[1]) * ((y[2] - y[1]) / (x[2] - x[1]) - *
		yp1);
    }
    i__1 = *n - 1;
    for (i__ = 2; i__ <= i__1; ++i__) {
	sig = (x[i__] - x[i__ - 1]) / (x[i__ + 1] - x[i__ - 1]);
	p = sig * y2[i__ - 1] + (float)2.;
	y2[i__] = (sig - (float)1.) / p;
	u[i__ - 1] = (((y[i__ + 1] - y[i__]) / (x[i__ + 1] - x[i__]) - (y[i__]
		 - y[i__ - 1]) / (x[i__] - x[i__ - 1])) * (float)6. / (x[i__ 
		+ 1] - x[i__ - 1]) - sig * u[i__ - 2]) / p;
/* L11: */
    }
    if (*ypn > (float)9.9e29) {
	qn = (float)0.;
	un = (float)0.;
    } else {
	qn = (float).5;
	un = (float)3. / (x[*n] - x[*n - 1]) * (*ypn - (y[*n] - y[*n - 1]) / (
		x[*n] - x[*n - 1]));
    }
    y2[*n] = (un - qn * u[*n - 2]) / (qn * y2[*n - 1] + (float)1.);
    for (k = *n - 1; k >= 1; --k) {
	y2[k] = y2[k] * y2[k + 1] + u[k - 1];
/* L12: */
    }
    return 0;
} /* spline_ */

/* ----------------------------------------------------------------------- */
/* Subroutine */ int splint_(real *xa, real *ya, real *y2a, integer *n, real *
	x, real *y)
{
    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle();

    /* Local variables */
    static real a, b, h__;
    static integer k, khi, klo;

    /* Fortran I/O blocks */
    static cilist io___257 = { 0, 6, 0, 0, 0 };


/*        CALCULATE CUBIC SPLINE INTERP VALUE */
/*        ADAPTED FROM NUMERICAL RECIPES BY PRESS ET AL. */
/*        XA,YA: ARRAYS OF TABULATED FUNCTION IN ASCENDING ORDER BY X */
/*        Y2A: ARRAY OF SECOND DERIVATIVES */
/*        N: SIZE OF ARRAYS XA,YA,Y2A */
/*        X: ABSCISSA FOR INTERPOLATION */
/*        Y: OUTPUT VALUE */
    /* Parameter adjustments */
    --y2a;
    --ya;
    --xa;

    /* Function Body */
    klo = 1;
    khi = *n;
L1:
    if (khi - klo > 1) {
	k = (khi + klo) / 2;
	if (xa[k] > *x) {
	    khi = k;
	} else {
	    klo = k;
	}
	goto L1;
    }
    h__ = xa[khi] - xa[klo];
    if (h__ == (float)0.) {
	s_wsle(&io___257);
	do_lio(&c__9, &c__1, "BAD XA INPUT TO SPLINT", (ftnlen)22);
	e_wsle();
    }
    a = (xa[khi] - *x) / h__;
    b = (*x - xa[klo]) / h__;
    *y = a * ya[klo] + b * ya[khi] + ((a * a * a - a) * y2a[klo] + (b * b * b 
	    - b) * y2a[khi]) * h__ * h__ / (float)6.;
    return 0;
} /* splint_ */

/* ----------------------------------------------------------------------- */
/* Subroutine */ int splini_(real *xa, real *ya, real *y2a, integer *n, real *
	x, real *yi)
{
    /* System generated locals */
    real r__1, r__2;

    /* Local variables */
    static real a, b, h__, a2, b2, xx;
    static integer khi, klo;

/*       INTEGRATE CUBIC SPLINE FUNCTION FROM XA(1) TO X */
/*        XA,YA: ARRAYS OF TABULATED FUNCTION IN ASCENDING ORDER BY X */
/*        Y2A: ARRAY OF SECOND DERIVATIVES */
/*        N: SIZE OF ARRAYS XA,YA,Y2A */
/*        X: ABSCISSA ENDPOINT FOR INTEGRATION */
/*        Y: OUTPUT VALUE */
    /* Parameter adjustments */
    --y2a;
    --ya;
    --xa;

    /* Function Body */
    *yi = (float)0.;
    klo = 1;
    khi = 2;
L1:
    if (*x > xa[klo] && khi <= *n) {
	xx = *x;
	if (khi < *n) {
/* Computing MIN */
	    r__1 = *x, r__2 = xa[khi];
	    xx = dmin(r__1,r__2);
	}
	h__ = xa[khi] - xa[klo];
	a = (xa[khi] - xx) / h__;
	b = (xx - xa[klo]) / h__;
	a2 = a * a;
	b2 = b * b;
	*yi += (((float)1. - a2) * ya[klo] / (float)2. + b2 * ya[khi] / (
		float)2. + ((-(a2 * a2 + (float)1.) / (float)4. + a2 / (float)
		2.) * y2a[klo] + (b2 * b2 / (float)4. - b2 / (float)2.) * y2a[
		khi]) * h__ * h__ / (float)6.) * h__;
	++klo;
	++khi;
	goto L1;
    }
    return 0;
} /* splini_ */

/* ----------------------------------------------------------------------- */
doublereal dnet_(real *dd, real *dm, real *zhm, real *xmm, real *xm)
{
    /* System generated locals */
    real ret_val;
    doublereal d__1, d__2;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle();
    double log(doublereal), exp(doublereal), pow_dd(doublereal *, doublereal *
	    );

    /* Local variables */
    static real a, ylog;

    /* Fortran I/O blocks */
    static cilist io___269 = { 0, 6, 0, 0, 0 };


/*       TURBOPAUSE CORRECTION FOR MSIS MODELS */
/*         Root mean density */
/*       8/20/80 */
/*          DD - diffusive density */
/*          DM - full mixed density */
/*          ZHM - transition scale length */
/*          XMM - full mixed molecular weight */
/*          XM  - species molecular weight */
/*          DNET - combined density */
    a = *zhm / (*xmm - *xm);
    if (*dm > (float)0. && *dd > (float)0.) {
	goto L5;
    }
    s_wsle(&io___269);
    do_lio(&c__9, &c__1, "DNET LOG ERROR", (ftnlen)14);
    do_lio(&c__4, &c__1, (char *)&(*dm), (ftnlen)sizeof(real));
    do_lio(&c__4, &c__1, (char *)&(*dd), (ftnlen)sizeof(real));
    do_lio(&c__4, &c__1, (char *)&(*xm), (ftnlen)sizeof(real));
    e_wsle();
    if (*dd == (float)0. && *dm == (float)0.) {
	*dd = (float)1.;
    }
    if (*dm == (float)0.) {
	goto L10;
    }
    if (*dd == (float)0.) {
	goto L20;
    }
L5:
    ylog = a * log(*dm / *dd);
    if (ylog < (float)-10.) {
	goto L10;
    }
    if (ylog > (float)10.) {
	goto L20;
    }
    d__1 = (doublereal) (exp(ylog) + (float)1.);
    d__2 = (doublereal) (1 / a);
    ret_val = *dd * pow_dd(&d__1, &d__2);
    goto L50;
L10:
    ret_val = *dd;
    goto L50;
L20:
    ret_val = *dm;
    goto L50;
L50:
    return ret_val;
} /* dnet_ */

/* ----------------------------------------------------------------------- */
doublereal ccor_(real *alt, real *r__, real *h1, real *zh)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real e, ex;

/*        CHEMISTRY/DISSOCIATION CORRECTION FOR MSIS MODELS */
/*        ALT - altitude */
/*        R - target ratio */
/*        H1 - transition scale length */
/*        ZH - altitude of 1/2 R */
    e = (*alt - *zh) / *h1;
    if (e > (float)70.) {
	goto L20;
    }
    if (e < (float)-70.) {
	goto L10;
    }
    ex = exp(e);
    ret_val = *r__ / (ex + (float)1.);
    goto L50;
L10:
    ret_val = *r__;
    goto L50;
L20:
    ret_val = (float)0.;
    goto L50;
L50:
    ret_val = exp(ret_val);
    return ret_val;
} /* ccor_ */

/* ----------------------------------------------------------------------- */
doublereal ccor2_(real *alt, real *r__, real *h1, real *zh, real *h2)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real e1, e2, ex1, ex2;

/*       O&O2 CHEMISTRY/DISSOCIATION CORRECTION FOR MSIS MODELS */
    e1 = (*alt - *zh) / *h1;
    e2 = (*alt - *zh) / *h2;
    if (e1 > (float)70. || e2 > (float)70.) {
	goto L20;
    }
    if (e1 < (float)-70. && e2 < (float)-70.) {
	goto L10;
    }
    ex1 = exp(e1);
    ex2 = exp(e2);
    ret_val = *r__ / ((ex1 + ex2) * (float).5 + (float)1.);
    goto L50;
L10:
    ret_val = *r__;
    goto L50;
L20:
    ret_val = (float)0.;
    goto L50;
L50:
    ret_val = exp(ret_val);
    return ret_val;
} /* ccor2_ */

/* ----------------------------------------------------------------------- */
/* Subroutine */ int gtd7bk_()
{
    return 0;
} /* gtd7bk_ */

/*          MSISE-00 01-FEB-02 */
/*         TEMPERATURE */
/*         HE DENSITY */
/*         O DENSITY */
/*         N2 DENSITY */
/*         TLB */
/*         O2 DENSITY */
/*         AR DENSITY */
/*          H DENSITY */
/*          N DENSITY */
/*        HOT O DENSITY */
/*          S PARAM */
/*          TURBO */
/*         LOWER BOUNDARY */







/*         TN1(2) */
/*         TN1(3) */
/*         TN1(4) */
/*         TN1(5) TN2(1) */
/*          TN2(2) */
/*          TN2(3) */
/*          TN2(4) TN3(1) */
/*          TN3(2) */
/*          TN3(3) */
/*          TN3(4) */
/*          TN3(5) SURFACE TEMP TSL */
/*          TGN3(2) SURFACE GRAD TSLG */
/*          TGN2(1) TGN1(2) */
/*          TGN3(1) TGN2(2) */
/*          SEMIANNUAL MULT SAM */
/*         MIDDLE ATMOSPHERE AVERAGES */

#ifdef __cplusplus
	}
#endif
