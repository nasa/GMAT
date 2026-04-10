KPL/FK

   Fixed grid definition for GOES-R.  The spacecraft has has SPICE ID -143226 
   by convention.

   We are setting up a text kernel (TK) frame, so we use class 4.  The frame is
   set relative to the fixed Earth (399) coordinate system.  We are aligning axes
   so that the Z-axis points to zenith, and the X-axis points north.

   The topocentric frame GOESR_FIXEDGRID is centered at the equator, at the 
   desired longitude (137.2 deg west), and with distance 42164.8 km, which at the 
   J2000 epoch:

      2000 JAN 01 12:00:00.000 TDB

      X = -30937.57299079088;
      Y = -28648.50670592601;
      Z =      0.0;

   This location is set as the GOESR_GRID_ORIGIN, object 399777, below.

   Ref:  Modeled on https://naif.jpl.nasa.gov/pub/naif/generic_kernels/fk/stations/earth_topo_201023.tf


\begindata

   SITES                 +=    'GRID_ORIGIN'
   GRID_ORIGIN_FRAME      =    'EARTH_FIXED'
   GRID_ORIGIN_CENTER     =    399
   GRID_ORIGIN_IDCODE     =    399777
   GRID_ORIGIN_EPOCH      =    @2000-JAN-01/00:00
   GRID_ORIGIN_BOUNDS     = (  @1950-JAN-01/00:00,  @2150-JAN-01/00:00  )
   GRID_ORIGIN_XYZ        = ( -30937.57299079088   -28648.50670592601   0.0 )
   GRID_ORIGIN_DXYZ       = (  0.0                  0.0                 0.0 )
   GRID_ORIGIN_TOPO_EPOCH =    @2020-OCT-23/00:00
   GRID_ORIGIN_UP         =    'Z'
   GRID_ORIGIN_NORTH      =    'X'


   FRAME_GOESR_FIXED_GRID = 1432261 
   FRAME_1432261_NAME     = 'GOESR_FIXED_GRID'
   FRAME_1432261_CLASS    =  4
   FRAME_1432261_CENTER   = 399777
   FRAME_1432261_CLASS_ID = 1432261

   TKFRAME_1432261_SPEC     = 'ANGLES'
   TKFRAME_1432261_RELATIVE = 'EARTH_FIXED'
   TKFRAME_1432261_ANGLES   = ( 137.2, 0.0, 180 )
   TKFRAME_1432261_AXES     = (     3,   2,   3 )
   TKFRAME_1432261_UNITS    = 'DEGREES'
