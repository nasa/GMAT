KPL/FK

Mars Express Spacecraft and Beagle-2 Lander Frames Kernel
========================================================================

   This frame kernel contains complete set of frame definitions for the
   Mars Express Spacecraft (MEX) and Beagle-2 Lander (BEAGLE2) including
   definitions for the MEX fixed and MEX science instrument frames and
   BEAGLE2 fixed, BEAGLE2 instrument, and landing site local frames. This
   kernel also contains NAIF ID/name mapping for the MEX and BEAGLE2
   instruments.


Version and Date
========================================================================


   Version 1.0 -- June 16, 2008 -- Jorge Diaz del Rio, MIG
   
      Added Visial Monitoring Camera (VMC) frame.


   Version 0.9 -- May 21, 2008 -- J. Vazquez
   
      - Added MEX_SA+Y_ZERO and MEX_SA-Y_ZERO, fixed offset frames with
        respect to MEX_SPACECRAFT.
        
      - Modified MEX_SA_+Y and MEX_SA_-Y. The cells of the Solar Arrays
        face the +Z axis of the frames. The frames are type 3 frames that
        rotate with respect to MEX_SA+Y_ZERO and MEX_SA-Y_ZERO.

   Version 0.8 -- September 17, 2004 -- Boris Semenov, NAIF

      Added ASPERA_IMAS frame. This frame was introduced because it
      matches the "natural" IMA spherical coordinate system and the way
      IMA operates better than the ASPERA_IMA frame defined originally
      (see the latest ASPERA IK file for more discussion on this
      issue.)

   Version 0.7 -- July 19, 2004 -- Boris Semenov, NAIF

      Corrected HGA frame to incorporate 5 degree boresight offset
      towards +Z.

   Version 0.6 -- March 15, 2004 -- Boris Semenov, NAIF

      Incorporated revised OMEGA frame layout (no OMEGA_BASE and
      OMEGA_SWIR frames, OMEGA_SWIR_S defined w.r.t. SPACECRAFT,
      OMEGA_SWIR_L and OMEGA_VNIR defined w.r.t OMEGA_SWIR_S) and
      initial in-flight calibrated alignments provided by Nicolas
      Manaud on March 12, 2004. Renamed OMEGA_SWIR_S to OMEGA_SWIR_C
      for consistency with the conventions accepted by OMEGA team.

   Version 0.5 -- December 15, 2003 -- Boris Semenov, NAIF

      Incorporated in-flight calibrated alignments provided by Thomas
      Roatsch on December 15, 2003 into the HRSC_HEAD and HRSC_SRC
      frame definitions.
 
      Renamed and re-defined ASPERA Solar Sensor frames, MEX_ASPERA_SS1
      and MEX_ASPERA_SS2.

   Version 0.4 -- October 5, 2003 -- Boris Semenov, NAIF

      Modified ASPERA frames to match the conventions accepted by the
      ASPERA team, specifically:

         -- changed MEX_ASPERA_BASE frame name to MEX_ASPERA_URF

         -- changed MEX_ASPERA_SCANNER frame name to MEX_ASPERA_SAF

         -- replaced MEX_ASPERA_NPD frame with MEX_ASPERA_NPD1 and 
            MEX_ASPERA_NPD2

         -- added MEX_ASPERA_IMA_URF frame (in the IMA branch between
            MEX_SPACECRAFT and MEX_ASPERA_IMA frames)

         -- added MEX_ASPERA_SS0 and MEX_ASPERA_SS1 frames

      Updated ASPERA frame tree diagrams and descriptions accordingly

      Modified MEX_HRSC_SRC frame to incorporate 90 degree rotation 
      w.r.t to the MEX_HRSC_BASE frame (per HRSC calibration results)

      Updated BEAGLE2_LOCAL_LEVEL frame to correspond to the project
      official landing site coordinates (areocentric LON=90.75 &
      LAT=11.6)
      
   Version 0.3 -- June 2, 2003 -- Boris Semenov, NAIF

      Changed the types of and relationship between the MEX_SPACECRAFT
      (mechanical) and MEX_SC_REF (ACS reference) frames. Now the
      MEX_SC_REF frame is CK-based because telemetry and s/c CKs
      contains orientation for it and the MEX_SPACECRAFT frame is fixed
      offset (rotated by 180 about Z) with respect to MEX_SC_REF.
 
   Version 0.2 -- January 2, 2002 -- Boris Semenov, NAIF

      Added frames and IDs for the short and long wavelength OMEGA SWIR
      channels per review feedback from Yves Langevin, OMEGA Team 
      (December 2001.)
 
   Version 0.1 -- December  5, 2001 -- Boris Semenov, NAIF

      Updated SPICAM frames per review feedback from Emmanuel Dimarellis
      (e-mail from November 11, 2001.)
 
   Version 0.0 -- June 18, 2001 -- Boris Semenov, NAIF

      Preliminary Version. Pending review and approval by MEX and
      BEAGLE2 instrument teams and ESOC Science operations team.
 

References
========================================================================

   1. ``Frames Required Reading'', NAIF Document No.____

   2. ``Kernel Pool Required Reading'', NAIF Document No.____

   3. ``C-Kernel Required Reading'', NAIF Document No.____

   4. ``ASPERA-3'' ME-ASP-DS-0002, Draft Rev. 1, 23 April, 1999

   5. ``HRSC on Mars Express'' Presentation by R.Pischel et al, May 9, 2001

   6. ``Mars Express Hypotheses for AOCS Studies'', Draft, Issue 02, Rev 00,
      9/11/00, by Astrium

   7. ``OMEGA PID'', PID-B, OME-CI-0022-003-IAS, 25/02/00

   8. ``PFS Instrument Description'', PFS-ICDR-02, June 5, 2000

   9. ``SPICAM LIGHT'', PID-B, PS-DES-011, August 19, 1999

  10. ``Beagle-2 Landing Site Selection Press Release'', December 20, 2000
       http://spdext.estec.esa.nl/content/news/index.cfm?aid=9&cid=260
       &oid=25649.

  11. Review comments by Emmanuel Dimarellis, SPICAM Team, e-mail from
      November 11, 2001.

  12. Review comments by Yves Langevin, OMEGA Team, personal communication
      in December 2001.

  13. ASPERA Sensor Numbering, by Stas Barabash, Latest Version, Aug 2003

  14. HRSC Earth-Moon Calibration Results, notes by T. Roatsch, Jul 2003

  15. OMEGA in-flight calibrated alignments, e-mail by N. Manaud, Mar 2004

  16. ``VMC for MEX. Flight User Manual'', MEX-ESA-VMC-MA-0003, Issue 4,
      Revision 2, January 17, 2003


Contact Information
========================================================================

   - Boris V. Semenov, NAIF/JPL, (818)-354-8136, boris.semenov@jpl.nasa.gov
   - Jorge Diaz del Rio, MIG/ESA,(34)-918-131-166, jdiaz@sciops.esa.int
   - Jose Luis Vazquez Garcia, MIG/ESA, (34)-918-131-310, jlvazquez@sciops.esa.int
       and
   - ESA SPICE Support Team, esa_spice@sciops.esa.int



Implementation Notes
========================================================================

   This file is used by the SPICE system as follows: programs that make
   use of this frame kernel must `load' the kernel, normally during program
   initialization. The SPICELIB routine FURNSH and CSPICE function furnsh_c
   load a kernel file into the kernel pool as shown below.

      CALL FURNSH ( 'frame_kernel_name' )
      furnsh_c    ( "frame_kernel_name" );

   This file was created and may be updated with a text editor or word
   processor.


Mars Express Mission NAIF ID Codes
========================================================================

   The following names and NAIF ID codes are assigned to the MEX spacecraft,
   its structures and science instruments (the keywords implementing these
   definitions are located in the section "Mars Express Mission NAIF ID
   Codes -- Definition Section" at the end of this file):

   MEX Spacecraft and Spacecraft Structures names/IDs:

            MEX                    -41      (synonyms: MARS EXPRESS,
                                             MARS-EXPRESS, MARS_EXPRESS )
            MEX_SPACECRAFT         -41000   (synonym: MEX_SC)
            MEX_SA+Y               -41011
            MEX_SA-Y               -41012
            MEX_SA+Y_GIMBAL        -41013
            MEX_SA-Y_GIMBAL        -41014
            MEX_HGA                -41020
            MEX_MELACOM_1          -41031
            MEX_MELACOM_2          -41032
            MEX_LGA                -41040
            MEX_VMC                -41050

   ASPERA names/IDs:

            MEX_ASPERA             -41100
            MEX_ASPERA_URF         -41110
            MEX_ASPERA_SAF         -41111
            MEX_ASPERA_ELS         -41120
            MEX_ASPERA_NPI         -41130
            MEX_ASPERA_NPD1        -41141
            MEX_ASPERA_NPD2        -41142
            MEX_ASPERA_IMA_URF     -41150
            MEX_ASPERA_IMA         -41151
            MEX_ASPERA_IMAS        -41152
            MEX_ASPERA_SS1         -41161
            MEX_ASPERA_SS2         -41162

   HRSC names/IDs:

            MEX_HRSC               -41200
            MEX_HRSC_HEAD          -41210
            MEX_HRSC_S2            -41211
            MEX_HRSC_RED           -41212
            MEX_HRSC_P2            -41213
            MEX_HRSC_BLUE          -41214
            MEX_HRSC_NADIR         -41215
            MEX_HRSC_GREEN         -41216
            MEX_HRSC_P1            -41217
            MEX_HRSC_IR            -41218
            MEX_HRSC_S1            -41219
            MEX_HRSC_SRC           -41220

   MARSIS names/IDs:

            MEX_MARSIS             -41300
            MEX_MARSIS_DIPOLE_1    -41310
            MEX_MARSIS_DIPOLE_2    -41320
            MEX_MARSIS_MONOPOLE    -41330

   OMEGA names/IDs:

            MEX_OMEGA              -41400
            MEX_OMEGA_VNIR         -41410
            MEX_OMEGA_SWIR         -41420
            MEX_OMEGA_SWIR_C       -41421
            MEX_OMEGA_SWIR_L       -41422

   PFS names/IDs:

            MEX_PFS                -41500
            MEX_PFS_SWC            -41510
            MEX_PFS_LWC            -41520
            MEX_PFS_SCANNER        -41530

   SPICAM names/IDs:

            MEX_SPICAM             -41600
            MEX_SPICAM_SIR         -41610
            MEX_SPICAM_SIR_SOLAR   -41611
            MEX_SPICAM_SUV         -41620
            MEX_SPICAM_SUV_SOLAR   -41621


   The following names and NAIF ID codes are assigned to the Beagle-2
   lander, its structures and science instruments:

            BEAGLE2                -44      (synonyms: BEAGLE 2, BEAGLE-2,
                                             BEAGLE_2)
            BEAGLE2_LANDER         -44000
            BEAGLE2_GAP            -44100
            BEAGLE2_PAW            -44200
            BEAGLE2_LANDING_SITE   -44900   (synonyms: BEAGLE2_LS,
                                             BEAGLE2_SITE)


Mars Express Frames
========================================================================

   The following MEX and BEAGLE2 frames are defined in this kernel file:

           Name                  Relative to           Type       NAIF ID
      ======================  ===================  ============   =======

   MEX Spacecraft and Spacecraft Structures frames:
   ------------------------------------------------
      MEX_SPACECRAFT          MEX_SC_REF           FIXED          -41000
      MEX_SC_REF              J2000                CK             -41001
      MEX_SA+Y_ZERO           MEX_SPACECRAFT       FIXED          -41013
      MEX_SA+Y                MEX_SA+Y_ZERO        CK             -41011
      MEX_SA-Y_ZERO           MEX_SPACECRAFT       FIXED          -41014
      MEX_SA-Y                MEX_SA-Y_ZERO        CK             -41012
      MEX_HGA                 MEX_SPACECRAFT       FIXED          -41020
      MEX_MELACOM_1           MEX_SPACECRAFT       FIXED          -41031
      MEX_MELACOM_2           MEX_SPACECRAFT       FIXED          -41032
      MEX_LGA                 MEX_SPACECRAFT       FIXED          -41040
      MEX_VMC                 MEX_SPACECRAFT       FIXED          -41050

   ASPERA frames:
   --------------
      MEX_ASPERA_URF          MEX_SPACECRAFT       FIXED          -41110
      MEX_ASPERA_SAF          MEX_ASPERA_URF       CK             -41111
      MEX_ASPERA_ELS          MEX_ASPERA_SAF       FIXED          -41120
      MEX_ASPERA_NPI          MEX_ASPERA_SAF       FIXED          -41130
      MEX_ASPERA_NPD1         MEX_ASPERA_SAF       FIXED          -41141
      MEX_ASPERA_NPD2         MEX_ASPERA_SAF       FIXED          -41142 
      MEX_ASPERA_IMA_URF      MEX_SPACECRAFT       FIXED          -41150
      MEX_ASPERA_IMA          MEX_ASPERA_IMA_URF   FIXED          -41151
      MEX_ASPERA_IMAS         MEX_ASPERA_IMA       FIXED          -41152
      MEX_ASPERA_SS1          MEX_ASPERA_SAF       FIXED          -41161
      MEX_ASPERA_SS2          MEX_ASPERA_SAF       FIXED          -41162 

   HRSC frames:
   ------------
      MEX_HRSC_BASE           MEX_SPACECRAFT       FIXED          -41200
      MEX_HRSC_HEAD           MEX_HRSC_BASE        FIXED          -41210
      MEX_HRSC_SRC            MEX_HRSC_BASE        FIXED          -41220

   MARSIS frames:
   --------------
      MEX_MARSIS_DIPOLE_1     MEX_SPACECRAFT       FIXED          -41310
      MEX_MARSIS_DIPOLE_2     MEX_SPACECRAFT       FIXED          -41320
      MEX_MARSIS_MONOPOLE     MEX_SPACECRAFT       FIXED          -41330

   OMEGA frames:
   -------------
      MEX_OMEGA_SWIR_C        MEX_SPACECRAFT       FIXED          -41421
      MEX_OMEGA_SWIR_L        MEX_OMEGA_SWIR_C     FIXED          -41422
      MEX_OMEGA_VNIR          MEX_OMEGA_SWIR_C     FIXED          -41410

   PFS frames:
   -------------
      MEX_PFS_BASE            MEX_SPACECRAFT       FIXED          -41500
      MEX_PFS_SCANNER         MEX_PFS_BASE         CK             -41530
      MEX_PFS_SWC             MEX_PFS_SCANNER      FIXED          -41510
      MEX_PFS_LWC             MEX_PFS_SCANNER      FIXED          -41520
      MEX_PFS_25_LEFT         MEX_PFS_BASE         FIXED          -41531
      MEX_PFS_12_LEFT         MEX_PFS_BASE         FIXED          -41532
      MEX_PFS_NADIR           MEX_PFS_BASE         FIXED          -41533
      MEX_PFS_12_RIGHT        MEX_PFS_BASE         FIXED          -41534
      MEX_PFS_25_RIGHT        MEX_PFS_BASE         FIXED          -41535
      MEX_PFS_COLD_SPACE      MEX_PFS_BASE         FIXED          -41536

   SPICAM frames:
   --------------
      MEX_SPICAM_BASE         MEX_SPACECRAFT       FIXED          -41600
      MEX_SPICAM_SIR          MEX_SPICAM_BASE      FIXED          -41610
      MEX_SPICAM_SIR_SOLAR    MEX_SPICAM_BASE      FIXED          -41611
      MEX_SPICAM_SUV          MEX_SPICAM_BASE      FIXED          -41620
      MEX_SPICAM_SUV_SOLAR    MEX_SPICAM_BASE      FIXED          -41621

   Beagle-2 Lander Frame:
   ---------------
      BEAGLE2_LOCAL_LEVEL     IAU_MARS             FIXED          -44900
      BEAGLE2_LANDER          BEAGLE2_LOCAL_LEVEL  FIXED          -44000

   Other Beagle-2 frames are TBD.


Spacecraft and Its Structures Frame Tree
========================================================================

   The diagram below shows the Mars Express spacecraft and its structures
   frame hierarchy (not including science instrument frames.)


                               "J2000" INERTIAL
           +-----------------------------------------------------+
           |                          |                          |
           |<-pck                     |                          |<-pck
           |                          |                          |
           V                          |                          V
       "IAU_MARS"                     |                     "IAU_EARTH"
     MARS BODY-FIXED                  |                   EARTH BODY-FIXED
     ---------------                  |                   ----------------
           |                          |
           |<-fixed                   |
           |                          |
           V                          |
  "BEAGLE2_LOCAL_LEVEL"               |
  --------------------                |
           |                          |
           |<-fixed                   |
           |                          |
           V                          |
    "BEAGLE2_LANDER"                  |
    ----------------                  |
                                      |
                                      |<-ck
                                      |
                                      V
                                 "MEX_SC_REF"
                                 ------------
                                      |
                                      |<-fixed
                                      |
                                      V
                               "MEX_SPACECRAFT"
      +----------------------------------------------------------+
      |                |              .        |     |     |     |
      |<-fixed         |<-fixed       . fixed->|     |     |     |<-fixed
      |                |              .        |     |     |     |
      V                V              .        V     |     |     V
 "MEX_SA+Y_ZERO"   "MEX_SA-Y_ZERO"    .    "MEX_HGA" |     | "MEX_LGA"
 --------------    --------------     .    --------- |     | ---------
      |                |              .              |     |
      |<-ck            |<-ck          .       fixed->|     |<-fixed
      |                |              .              |     |
      V                v              .              V     V
  "MEX_SA+Y"        "MEX_SA-Y"        .  "MEX_MELACOM_1"  "MEX_MELACOM_2"
   --------          --------         .  ---------------  ---------------
                                      .
                                      .
                                      V
                Individual instrument frame trees are provided
                   in the corresponding sections of this file



MEX Spacecraft and Spacecraft Structures Frames
========================================================================

   This section of the file contains the definitions of the spacecraft
   and spacecraft structures frames.


MEX Spacecraft Frames
--------------------------------------

   Two reference frames are defined for the MEX spacecraft (see [6]) --
   "mechanical/structure frame" (Xb,Yb,Zb) and "spacecraft reference
   frame" (Xa,Ya,Za).

   The "mechanical/structure frame" frame (Xb,Yb,Zb), with respect to
   which orientation of all science instruments and spacecraft
   structures is defined, is called MEX_SPACECRAFT frame in the MEX
   SPICE implementation.

   This frame is defined as follows:

      -  the payloads are located on the +Zb axis (the Main Engine being
         on the -Zb axis);

      -  the HGA is located on -Xb axis;

      -  the +Y axis is defined so that the (Xb,Yb,Zb) frame is right-
         handed.

      -  the origin of this frames is the launch vehicle interface point.

   The "spacecraft reference frame" (Xa,Ya,Za) frame, used primarily in
   AOSC studies, is the one for which orientation is determined
   on-board the spacecraft. This frame is called MEX_SC_REF in MEX
   SPICE implementation.

   This frame is related to MEX_SPACECRAFT frame (Xb,Yb,Zb) as a
   follows:

      -  +Xa = -Xb;

      -  +Ya = -Yb;

      -  +Za = Zb ;

      -  the origin of this frame is also located at the launch vehicle
         interface point.

   These diagrams illustrate the MEX_SPACECRAFT and MEX_SC_REF frames:

   +X s/c side view:
   -----------------
                                    ^
                                    | Nadir
                                    |          Direction
                                               of flight
                                                 ---->

                          Beagle-2 .'.
                                 .'   `.
                                 \_____/
                             ._____________.
                             |Science Deck |
      =====================o |             | o=====================
      -Y Solar Array         |    +Zsc     |         +Y Solar Array
                             |  +Zsc_ref   |
                             |      ^      |
                             |      |      |
                             |      |      |
                             .______|______.              +Xsc is out 
                               |    |    |                of the page
                   +Ysc_ref <-------*-------> +Ysc
                                  /   \                +Xsc_ref is into
                                 /_____\ Main Engine       the page

   +Z s/c side view:
   -----------------
                             HGA
                               ____  +Xsc_ref
                               \    ^    /
      .________________.     .__`.__|__.'__.     .________________.
      |                 \    |      |      |    /                 |
      |                  \   |     _|_     |   /                  |
      |                 +Ysc_ref .' | `  +Ysc |                   |
      |                   |o<-------o------->o|                   |
      |                   |  |    `_|+Zsc  |  |                   |
      |                  /   |      |+Zsc_ref  \                  |
      ._________________/    .______|______.    \_________________.
       -Y Solar Array               |               +Y Solar Array
                                    V +Xsc

                                                Both, +Zsc and +Zsc_ref
                                                  are out of the page


   As seen on the diagram, the MEX_SPACECRAFT and MEX_SC_REF frames are
   rotated 180 degrees about +Z with respect to each other.
 
   Since the orientation of the MEX_SC_REF frame is computed on-board,
   sent down in telemetry, and stored in the s/c CK files, it is
   defined as a CK-based frame.
 
   The MEX_SPACECRAFT frame is then defined as a fixed-offset frame --
   rotated by 180 degrees about +Z axis -- with respect to the
   MEX_SC_REF frame.

   These sets of keywords define the MEX_SPACECRAFT and MEX_SC_REF frames:

   \begindata

      FRAME_MEX_SPACECRAFT            =  -41000
      FRAME_-41000_NAME               = 'MEX_SPACECRAFT'
      FRAME_-41000_CLASS              =  4
      FRAME_-41000_CLASS_ID           =  -41000
      FRAME_-41000_CENTER             =  -41
      TKFRAME_-41000_RELATIVE         = 'MEX_SC_REF'
      TKFRAME_-41000_SPEC             = 'ANGLES'
      TKFRAME_-41000_UNITS            = 'DEGREES'
      TKFRAME_-41000_AXES             = ( 1,   2,     3   )
      TKFRAME_-41000_ANGLES           = ( 0.0, 0.0, 180.0 )

      FRAME_MEX_SC_REF                = -41001
      FRAME_-41001_NAME               = 'MEX_SC_REF'
      FRAME_-41001_CLASS              =  3
      FRAME_-41001_CLASS_ID           = -41001
      FRAME_-41001_CENTER             = -41
      CK_-41001_SCLK                  = -41
      CK_-41001_SPK                   = -41

   \begintext


MEX Solar Array Frames
--------------------------------------

   Note that, unlike in most spacecrafts, the +Y panel is situated along the
   -Y axis of the MEX_SPACECRAFT frame, and the -Y panel is situated along
   the +Y axis. The reason is that the solar arrays are named with respect to
   the MEX_SC_REF frame.

   Two auxiliary frames, MEX_SA+Y_ZERO and MEX_SA-Y_ZERO, fixed with respect 
   to the MEX_SPACECRAFT frame, are defined as follows:

      - +Y is parallel to the longest side of the array, positively orientated
        from the yoke to the end of the wing.
       
      - +Z is orientated along the -X axis of MEX_SPACECRAFT. It is equal to
        the -X axis of MEX_SPACECRAFT.
        
      - +X is defined such that (X,Y,Z) is right handed.

   Since the MEX solar arrays can be articulated (having one degree of
   freedom), the solar Array frames, MEX_SA+Y and MEX_SA-Y, are defined as
   CK frames with their orientation given relative to MEX_SA+Y_ZERO and
   MEX_SA-Y_ZERO.

   Both array frames are defined as follows (from [6]):

      -  +Y is parallel to the longest side of the array, positively oriented
         from the yoke to the end of the wing;

      -  +Z is normal to the solar array plane, the solar cells facing +Z;

      -  +X is defined such that (X,Y,Z) is right handed;

      -  the origin of the frame is located at the yoke geometric center.

   The axis of rotation is parallel to the Y axis of the spacecraft and
   solar array frames.

   This diagram illustrates the solar array frames:

   +X s/c side view:
   -----------------


                                    ^ +Zsc
                                    |
                                   .'.       ^ +Xsa-y_zero
                                  / | \      |
                                 /Beagle-2   |
                  +Zsa+y_zero    \_____/     | +Zsa-y_zero is into the page
            is into the page .______|______. |
     +Ysa+y_zero <------x    |Science Deck | x---------> +Ysa-y_zero
  ._____________________|__  |      |      |  ________________________.           
  |                     |  \ |      |      | /                        |
  |                     |   o|      |      |o                         |
  |_____________________|__/ |      |      | \________________________|     
                        |    |      |      o---------> +Ysc                            
                        |    |    +Xsc     |                                                 
                        V    |  (out of the page )                                                 
             +Xsa+y_zero     ._____________.
                               |         |
                               ._________.       
                                  /   \
                                 /_____\ Main Engine
                                 



   These sets of keywords define solar array frames as CK frames:

   \begindata

      FRAME_MEX_SA+Y_ZERO             = -41013
      FRAME_-41013_NAME               = 'MEX_SA+Y_ZERO'
      FRAME_-41013_CLASS              = 4
      FRAME_-41013_CLASS_ID           = -41013
      FRAME_-41013_CENTER             = -41
      TKFRAME_-41013_SPEC             = 'ANGLES'
      TKFRAME_-41013_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41013_ANGLES           = ( 180.0, -90.0, 0.0 )
      TKFRAME_-41013_AXES             = (   3,     2,   3   )
      TKFRAME_-41013_UNITS            = 'DEGREES'
      
      FRAME_MEX_SA+Y                  = -41011
      FRAME_-41011_NAME               = 'MEX_SA+Y'
      FRAME_-41011_CLASS              =  3
      FRAME_-41011_CLASS_ID           = -41011
      FRAME_-41011_CENTER             = -41
      CK_-41011_SCLK                  = -41
      CK_-41011_SPK                   = -41

      FRAME_MEX_SA-Y_ZERO             = -41014
      FRAME_-41014_NAME               = 'MEX_SA-Y_ZERO'
      FRAME_-41014_CLASS              = 4
      FRAME_-41014_CLASS_ID           = -41014
      FRAME_-41014_CENTER             = -41
      TKFRAME_-41014_SPEC             = 'ANGLES'
      TKFRAME_-41014_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41014_ANGLES           = ( 0.0, 90.0, 0.0 )
      TKFRAME_-41014_AXES             = ( 3,    2,   1   )
      TKFRAME_-41014_UNITS            = 'DEGREES'

      FRAME_MEX_SA-Y                  = -41012
      FRAME_-41012_NAME               = 'MEX_SA-Y'
      FRAME_-41012_CLASS              =  3
      FRAME_-41012_CLASS_ID           = -41012
      FRAME_-41012_CENTER             = -41
      CK_-41012_SCLK                  = -41
      CK_-41012_SPK                   = -41

   \begintext


MEX High Gain Antenna Frame
--------------------------------------

   The MEX High Gain Antenna is rigidly attached to the -X side of the
   s/c bus.  Therefore, the MEX HGA frame, MEX_HGA, is defined as a fixed
   offset frame with its orientation given relative to the MEX_SPACECRAFT
   frame.

   The MEX_HGA frame is defined as follows:

      -  +Z axis is in the antenna boresight direction (nominally
         5 degrees off the s/c -X axis towards the s/c +Z axis);

      -  +Y axis is in the direction of the s/c +Y axis ;

      -  +X completes the right hand frame;

      -  the origin of the frame is located at the geometric center of the
         HGA dish outer rim circle.

   This diagram illustrates the MEX_HGA frame:

   +Z s/c side view:
   -----------------
                                    ^+Zhga
                                    |
                                    |
                                    |
                              +Xhga |       +Yhga
                               _____o------->
                               \         /
      .________________.     .__`._____.'__.     .________________.
      |                 \    |Beagle-2     |    /                 |
      |                  \   |     ___     |   /                  |
      |                   |  |   .'   `  +Ysc |                   |
      |                   |o=|   |  o------->o|                   |
      |                   |  |    `_|+Zsc  |  |                   |
      |                  /   |      |      |   \                  |
      ._________________/    .______|______.    \_________________.
       -Y Solar Array               |               +Y Solar Array
                                    V +Xsc


   Nominally a single rotation of -85 degrees about the +Y axis is needed to
   co-align the s/c frame with the HGA frame.

   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   \begindata

      FRAME_MEX_HGA                   =  -41020
      FRAME_-41020_NAME               = 'MEX_HGA'
      FRAME_-41020_CLASS              =  4
      FRAME_-41020_CLASS_ID           =  -41020
      FRAME_-41020_CENTER             =  -41
      TKFRAME_-41020_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41020_SPEC             = 'ANGLES'
      TKFRAME_-41020_UNITS            = 'DEGREES'
      TKFRAME_-41020_AXES             = ( 1,    2,   3   )
      TKFRAME_-41020_ANGLES           = ( 0.0, 85.0, 0.0 )

   \begintext


MEX Lander Communication Antenna Frames
--------------------------------------

   Both Mars Lander Communication (MELACOM) Antennas are rigidly  mounted
   on the instrument deck of the s/c bus. Therefore, the MELACOM antenna
   frames, MEX_MELACOM_1 and MEX_MELACOM_2, are defined as fixed offset
   frames with their orientation given relative to the MEX_SPACECRAFT frame.

   The MEX_MELACOM_1 and MEX_MELACOM_2 frames are defined as follows:

      -  +Z axis is in the direction of the antenna boresight (nominally
         along the s/c +Z axis);

      -  +Y axis is in the direction of the s/c +Y axis;

      -  X completes the right hand frame;

      -  the origin of the frame is located at the geometric center of the
         outer side of the antenna.

   This diagram illustrates the MEX_MELACOM_1 and MEX_MELACOM_2 frames:

   +X s/c side view:
   -----------------
                              ^ +Zm1      ^ +Zm2
                              |           |                 +Xm1, +Xm2
                              |           |                 are out of
                              |           |                  the page
                              |     +Ym1  |       +Ym2
                             .o------->  .o------->
                             | | .'   `. | |
                   MELACOM 1 | | \_____/ | | MELACOM 2
                             ._____________.
                             |Science Deck |
      =====================o |             | o=====================
      -Y SA                  |             |                  +Y SA
                             |   +Zsc      |
                             |      ^      |
                             |      |      |
                             |      |      |
                             .______|______.
                               |    |    |
                               .____o-------> +Ysc
                                +Xsc  \
                                 /_____\ Main Engine


   Nominally both antenna frames are co-aligned with the s/c frame.

   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   \begindata

      FRAME_MEX_MELACOM_1             =  -41031
      FRAME_-41031_NAME               = 'MEX_MELACOM_1'
      FRAME_-41031_CLASS              =  4
      FRAME_-41031_CLASS_ID           =  -41031
      FRAME_-41031_CENTER             =  -41
      TKFRAME_-41031_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41031_SPEC             = 'ANGLES'
      TKFRAME_-41031_UNITS            = 'DEGREES'
      TKFRAME_-41031_AXES             = ( 1,   2,   3   )
      TKFRAME_-41031_ANGLES           = ( 0.0, 0.0, 0.0 )

      FRAME_MEX_MELACOM_2             =  -41032
      FRAME_-41032_NAME               = 'MEX_MELACOM_2'
      FRAME_-41032_CLASS              =  4
      FRAME_-41032_CLASS_ID           =  -41032
      FRAME_-41032_CENTER             =  -41
      TKFRAME_-41032_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41032_SPEC             = 'ANGLES'
      TKFRAME_-41032_UNITS            = 'DEGREES'
      TKFRAME_-41032_AXES             = ( 1,   2,   3   )
      TKFRAME_-41032_ANGLES           = ( 0.0, 0.0, 0.0 )

   \begintext


MEX Low Gain Antenna Frame
--------------------------------------

   The MEX_LGA frame is a fixed offset frame with its orientation give
   relative to the MEX_SPACECRAFT frame.

   The MEX_LGA frame is defined as follows:

      -  Z axis is along <<TBD>> ;

      -  Y axis is along <<TBD>> ;

      -  X completes the right hand frame;

      -  the origin of the MEX_LGA frame is located at <<TBD>> .

   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   \begindata

      FRAME_MEX_LGA                   =  -41040
      FRAME_-41040_NAME               = 'MEX_LGA'
      FRAME_-41040_CLASS              =  4
      FRAME_-41040_CLASS_ID           =  -41040
      FRAME_-41040_CENTER             =  -41
      TKFRAME_-41040_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41040_SPEC             = 'ANGLES'
      TKFRAME_-41040_UNITS            = 'DEGREES'
      TKFRAME_-41040_AXES             = (   1,       2,       3     )
      TKFRAME_-41040_ANGLES           = ( 000.000, 000.000, 000.000 )

   \begintext

MEX Visual Monitoring Camera (VMC) Frame
----------------------------------------

   The MEX Visual Monitoring Camera is rigidly mounted on the instrument
   deck of the s/c bus, therefore the VMC camera frame, MEX_VMC, is defined
   as fixed offset frame with its orientation given relative to the
   MEX_SPACECRAFT frame, and is nominally rotated by -19 degrees about s/c
   +X, i.e. toward s/c +Y axis.   

   The MEX_VMC frame is defined by the camera design and its mounting
   on the s/c as follows:

      -  +Z axis is in the nominal direction of the VMC camera boresight;
         it points towards the s/c +Z axis, tilted 19 degrees towards the
         s/c +Y axis.
         
      -  +X axis is perpendicular to the CCD detector lines, and its nominally
         along the s/c +X axis.

      -  +Y completes the right hand frame;

      -  the origin of the frame is located at the VMC camera focal point.

   This diagram illustrates the VMC frame:

   +X s/c side view:
   -----------------
                                                
                                                            +Xvmc is  
                                                             out of
                                                             the page
                                     ^ +Zvmc          
                                   .'                 
                                 .'   `.    
                                '\_____/              
                         +Xvmc_o.__________.
                             |   `.        |
      =====================o |     `.>     | o=====================
      -Y SA                  |        +Yvmc|                  +Y SA
                             |   +Zsc      |
                             |      ^      |
                             |      |      |
                             |      |      |
                             .______|______.
                               |    |    |
                               .____o-------> +Ysc
                                +Xsc  \
                                 /_____\ Main Engine



   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   \begindata

      FRAME_MEX_VMC                   =  -41050
      FRAME_-41050_NAME               = 'MEX_VMC'
      FRAME_-41050_CLASS              =  4
      FRAME_-41050_CLASS_ID           =  -41050
      FRAME_-41050_CENTER             =  -41
      TKFRAME_-41050_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41050_SPEC             = 'ANGLES'
      TKFRAME_-41050_UNITS            = 'DEGREES'
      TKFRAME_-41050_AXES             = ( 3,   2,   1    )
      TKFRAME_-41050_ANGLES           = ( 0.0, 0.0, 19.0 )

   \begintext



ASPERA Frames
========================================================================

   This section of the file contains the definitions of the ASPERA
   instrument frames.


ASPERA Frame Tree
--------------------------------------

   The diagram below shows the ASPERA frame hierarchy.


                               "J2000" INERTIAL
           +-----------------------------------------------------+
           |                          |                          |
           |<-pck                     |                          |<-pck
           |                          |                          |
           V                          |                          V
       "IAU_MARS"                     |                     "IAU_EARTH"
     MARS BODY-FIXED                  |<-ck               EARTH BODY-FIXED
     ---------------                  |                   ----------------
                                      V
                               "MEX_SPACECRAFT"
           +-----------------------------------+
           |                                   |
           |<-fixed                            |<-fixed
           |                                   |
           V                                   V
    "MEX_ASPERA_URF"                 "MEX_ASPERA_IMA_URF"
    ----------------                 --------------------
           |                                   |
           |                                   |<-fixed
           |                                   |
           |                                   V
           |                           "MEX_ASPERA_IMA"
           |                           ----------------
           |                                   |
           |                                   |<-fixed
           |                                   |
           |                                   V
           |                           "MEX_ASPERA_IMAS"
           |                           -----------------
           |
           |
           |
           |          "MEX_ASPERA_SS1"   "MEX_ASPERA_SS2"
           |          ----------------   ----------------
           |                 |                 |
           |<-ck             |<-fixed          |<-fixed
           |                 |                 |
           V                 |                 | 
    "MEX_ASPERA_SAF"         |                 |
    -----------------------------------------------------------------+
           |                 |                 |                     |
           |<-fixed          |<-fixed          |<-fixed       fixed->|
           |                 |                 |                     |
           V                 V                 V                     V
    "MEX_ASPERA_ELS"   "MEX_ASPERA_NPI"   "MEX_ASPERA_NPD1"  "MEX_ASPERA_NPD2"
    ----------------   ----------------   -----------------  -----------------


ASPERA Main Unit URF and SAF Frames
--------------------------------------

   The ASPERA main unit base is rigidly mounted on the s/c science
   deck. Therefore, the frame associated with it -- the ASPERA main
   unit reference frame, MEX_ASPERA_URF, -- is a fixed offset frame
   with its orientation given relative to the MEX_SPACECRAFT frame.

   The MEX_ASPERA_URF frame is defined as follows:

      -  +Z axis is along the scanner rotation axis and points from the
         main unit mounting plate toward the scanner (nominally this
         axis is co-aligned with the s/c +Z axis);

      -  +X axis is parallel to the longer side of the main unit base
         and points from the NPD side towards the ELS side for scanner
         in "90 degrees" position (nominally this axis is co-aligned
         with the s/c -Y axis);

      -  +Y completes the right handed frame;

      -  the origin of the frame is located at the intersection of the
         mounting surface of the main unit base and the axis of the
         mounting screw hole located at NPD2 side for scanner in "90
         degrees" position.

   Nominally this frame is rotated by -90 degrees about Z axis from the
   s/c frame.

   Since ASPERA main unit scanner rotates with respect to its base with
   0..180 angle range clockwise about Z axis of the MEX_ASPERA_URF
   frame, the frame associated with it -- the sensor assembly frame,
   MEX_ASPERA_SAF, -- is defined as a CK frame with its orientation
   provided in a CK file relative to the MEX_ASPERA_URF frame.

   The MEX_ASPERA_SAF frame is defined as follows:

      -  +Z axis is along the scanner rotation axis and points from the
         main unit mounting plate toward the scanner; this axis is
         co-aligned with the +Z axis of the MEX_ASPERA_URF frame;

      -  +X axis is along the ASPERA sensor assembly central axis and
         points from the NPD sensor towards the ELS sensor; this axis
         is co-aligned with the +X axis of the MEX_ASPERA_URF frame
         when scanner is in "90 degrees" position.

      -  +Y axis completes the right handed frame;

      -  the origin of the MEX_ASPERA_SAF frame is located at the
         intersection of the scanner rotation axis and the bottom
         (mounting) surface of the scanner base.

   This diagram illustrates the MEX_ASPERA_URF and MEX_ASPERA_SAF
   frames for the scanner angle of 135 degrees:

   +Z s/c side view:
   -----------------
                          "180 deg"  
                          position
         "135 deg"            |
         position  `.         .
                     `        |
                                       HGA
                  +Xsaf ^.     ___________ 
      "90 deg"            `.   \         /
      position _________    `.__`._____.'__      _________________
          -.-           \     o +Zsaf      |    /                 |
      |           +Xurf <---.'--o +Zurf    |   /                  |
      |                   .' |  |          |  |                   |
      |                  V|o=|  |   o-------> +Ysc                |
      |            +Ysaf  |  |  |   | +Zsc |  .                   |
      |                  /   |  V   |      |   \                  |
      ._________________/     +Yurf |______.    \_________________.
       -Y Solar Array               V               +Y Solar Array
                                     +Xsc
                              |
                              .
                              | 
                           "0 deg"              +Zsc, +Zsaf and +Zurf
                          position            axes are out of the page

   In general the MEX_ASPERA_SAF frame is rotated with respect to the
   MEX_ASPERA_URF frame by ( 90-<angle> ) degrees about +Z axis for
   scanner in <angle> position. This rotation as a function of time is
   provided in the ASPERA scanner CK files.

   Thus, in "0 degrees" scanner position the SAF frame is co-aligned
   with the s/c frame; in "90 degrees" scanner position the SAF frame
   is co-aligned with the URF frame:

   "0 deg" position:                          "90 deg" position
   -----------------                          -----------------

              o-------> +Ysaf           +Xsaf <-------o
        <-----|-o                               <-----|-o
      +Xurf   | |                            +Xurf    | |
              | |   o-------> +Ysc                    | |   o--------> +Ysc
        +Xsaf V |   |                           +Ysaf V |   |
                V   |                                   V   |
             +Yurf  |                                +Yurf  |
                    V                                       V 
                  +Xsc                                    +Xsc

   These sets of keywords define the ASPERA URF and SAF frames:

   \begindata

      FRAME_MEX_ASPERA_URF            =  -41110
      FRAME_-41110_NAME               = 'MEX_ASPERA_URF'
      FRAME_-41110_CLASS              =  4
      FRAME_-41110_CLASS_ID           =  -41110
      FRAME_-41110_CENTER             =  -41
      TKFRAME_-41110_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41110_SPEC             = 'ANGLES'
      TKFRAME_-41110_UNITS            = 'DEGREES'
      TKFRAME_-41110_AXES             = ( 1,   2,    3   )
      TKFRAME_-41110_ANGLES           = ( 0.0, 0.0, 90.0 )

      FRAME_MEX_ASPERA_SAF            = -41111
      FRAME_-41111_NAME               = 'MEX_ASPERA_SAF'
      FRAME_-41111_CLASS              =  3
      FRAME_-41111_CLASS_ID           = -41111
      FRAME_-41111_CENTER             = -41
      CK_-41111_SCLK                  = -41
      CK_-41111_SPK                   = -41

   \begintext


ASPERA Main Unit Sensor Frames
--------------------------------------

   Because ASPERA main unit sensors are rigidly mounted on the scanner,
   their corresponding frames -- MEX_ASPERA_ELS, MEX_ASPERA_NPI,
   MEX_ASPERA_NPD1, and MEX_ASPERA_NPD2 -- are defined as fixed-offset
   frames with respect to the MEX_ASPERA_SAF frame.

   ELS and NPI sensor frames are defined such that their axes are
   co-aligned with the axes of the MEX_ASPERA_SAF frame and their
   origins are at the sensors' "focal points", located at the
   intersection of the sensor's aperture plane and the sensor's
   symmetry axis.

   NPD1 and NPD2 sensor frames are defined such that their +Y and +Z
   axes are in the sensor's aperture plane, with the +Z axis along the
   central axis of the aperture (view direction of the middle sector),
   and +X axis is perpendicular to the aperture plane and points in
   direction of the SAF frame +X axis. The origins of these frames are
   at the sensor "focal points", which are located in the sensor's
   aperture plane at the point here the sensor sector view directions
   intersect.

   In all cases the "focal point" of a particular sensor is the point
   from which all sensor view direction are emanating. 

   This diagram illustrates the ELS, NPI, NPD1, and NPD2 sensor frames
   orientation w.r.t to the SAF, URF and spacecraft frames (for scanner 
   at 90 degrees position:

                            +Xels
                             .--- ^ ---. ELS
                             |    |    |    
                       +Yels |<---o    | - - - - - - - - - - - - - -
                             |         |               ELS aperture
                            +Xnpi ^ -----. NPI             plane 
                          +Xss2 ^ |      | 
                        +Ynpi <---o      | - - - - - - - - - - - - -  
          ----------- +Yss2 <---o   o---> +Yss1 -.     NPI aperture
         /           .--------------|----------. |         plane
        |            |       +Xsaf^ V +Xss1    | |
         \           |            |            | |
         /           |  +Ysaf <---o    +Xnpd1  | |      YZ of "npd1" frame
        |            |                    ^   +Znpd1   are in the aperture
         \           |                   .-\--.^.|           plane
         /         +Xnpd2         +Ynpd1 <--o'  ||     +Xnpd1 is into the page
        |              ^                 `------'|     +Ynpd1 and +Znpd1 are
         \          .---\---.          NPD1    | |       out of the page
         /    +Ynpd2 <---o  |                  | |
        |           `--.'---' NPD2     Scanner | |      YZ of "npd2" frame
         \    +Znpd2 <'________________________. |     are in the aperture
         /             |                     |   |           plane
        |              |^ +Xurf              |   |
         \             ||              Base  |   |     +Ynpd1 is into the page
          \   +Yurf <---o____________________.   |     +Xnpd1 and +Znpd1 are
           \                                     |       out of the page
     +Xsc   \____                     Nadir deck |
       <---o     \____________                   |
           |                  \__________________|    +Z axes of all frames
           V +Ysc                                       are out of the page


   These sets of keywords define the MEX_ASPERA_ELS and MEX_ASPERA_NPI
   frames:

   \begindata

      FRAME_MEX_ASPERA_ELS            =  -41120
      FRAME_-41120_NAME               = 'MEX_ASPERA_ELS'
      FRAME_-41120_CLASS              =  4
      FRAME_-41120_CLASS_ID           =  -41120
      FRAME_-41120_CENTER             =  -41
      TKFRAME_-41120_RELATIVE         = 'MEX_ASPERA_SAF'
      TKFRAME_-41120_SPEC             = 'ANGLES'
      TKFRAME_-41120_UNITS            = 'DEGREES'
      TKFRAME_-41120_AXES             = ( 1,   2,   3   )
      TKFRAME_-41120_ANGLES           = ( 0.0, 0.0, 0.0 )

      FRAME_MEX_ASPERA_NPI            =  -41130
      FRAME_-41130_NAME               = 'MEX_ASPERA_NPI'
      FRAME_-41130_CLASS              =  4
      FRAME_-41130_CLASS_ID           =  -41130
      FRAME_-41130_CENTER             =  -41
      TKFRAME_-41130_RELATIVE         = 'MEX_ASPERA_SAF'
      TKFRAME_-41130_SPEC             = 'ANGLES'
      TKFRAME_-41130_UNITS            = 'DEGREES'
      TKFRAME_-41130_AXES             = ( 1,   2,   3   )
      TKFRAME_-41130_ANGLES           = ( 0.0, 0.0, 0.0 )

   \begintext

   The NPD1 aperture plane is tilted by 15 degrees towards +X axis of
   the SAF frame about +Y/+Z line of the SAF frame. Thus, to align the
   SAF frame with the NPD1 frame the SAF frame has to be rotated by +45
   degrees about X axis and then by +15 degrees about new position of Y
   axis.

   The NPD2 aperture plane is tilted by 15 degrees towards -X axis of
   the SAF frame about -Y/+Z line of the SAF frame. Thus, to align the
   SAF frame with the NPD2 frame the SAF frame has to be rotated by -45
   degrees about X axis and then by -15 degrees about new position of Y
   axis.

   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   These sets of keywords define the MEX_ASPERA_NPD1 and MEX_ASPERA_NPD2
   frames:

   \begindata

      FRAME_MEX_ASPERA_NPD1           =  -41141
      FRAME_-41141_NAME               = 'MEX_ASPERA_NPD1'
      FRAME_-41141_CLASS              =  4
      FRAME_-41141_CLASS_ID           =  -41141
      FRAME_-41141_CENTER             =  -41
      TKFRAME_-41141_RELATIVE         = 'MEX_ASPERA_SAF'
      TKFRAME_-41141_SPEC             = 'ANGLES'
      TKFRAME_-41141_UNITS            = 'DEGREES'
      TKFRAME_-41141_AXES             = (   1,     2,     1   )
      TKFRAME_-41141_ANGLES           = ( -45.0, -15.0,   0.0 )

      FRAME_MEX_ASPERA_NPD2           =  -41142
      FRAME_-41142_NAME               = 'MEX_ASPERA_NPD2'
      FRAME_-41142_CLASS              =  4
      FRAME_-41142_CLASS_ID           =  -41142
      FRAME_-41142_CENTER             =  -41
      TKFRAME_-41142_RELATIVE         = 'MEX_ASPERA_SAF'
      TKFRAME_-41142_SPEC             = 'ANGLES'
      TKFRAME_-41142_UNITS            = 'DEGREES'
      TKFRAME_-41142_AXES             = (   1,     2,     1   )
      TKFRAME_-41142_ANGLES           = (  45.0,  15.0,   0.0 )

   \begintext

   The ASPERA Solar Sensor 1 frame, MEX_ASPERA_SS1, is defined as follows:

      -  +X axis is co-aligned with +X axis of the MEX_ASPERA_SAF
         frame;

      -  +Z axis is 15 degrees off +Z axis of the MEX_ASPERA_SAF frame
         towards -Y axis of MEX_ASPERA_SAF frame;

      -  +Y completes the right handed frame and is along the sensor
         boresight direction;

      -  the origin of the frame is located at the sensor's FOV focal
         point.

   The ASPERA Solar Sensor 2 frame, MEX_ASPERA_SS2, is defined as follows:

      -  +X axis is co-aligned with -X axis of the MEX_ASPERA_SAF
         frame;

      -  +Z axis is 15 degrees off +Z axis of the MEX_ASPERA_SAF frame
         towards +Y axis of MEX_ASPERA_SAF frame;

      -  +Y completes the right handed frame and is along the sensor
         boresight direction;

      -  the origin of the frame is located at the sensor's FOV focal
         point.

   This diagram illustrates the MEX_ASPERA_SS1 and MEX_ASPERA_SS2
   frames:

                                  ^ +Zsaf
                                  |                       
                         +Zss1 ^  |  ^ +Zss2                
                              ..\.|./..                   
                           .'    \|/    `.       
               +Yss2 <-. .'       |       `. .-> +Yss1          
                        `-.      /|\      .-'             
                       .   `-.  / | \  .-'   .             
                       . +Xss2`x  |  o'+Xss1 .            
                       .          o--------------> +Ysaf  
                       .           +Xsaf     .            
                       .                     .            
                        .                   .             
                         .                 .              
                          `.             .'               
                            ` ......... '    


   Both frames are defined as fixed offset frames with respect to the
   MEX_ASPERA_SAF frame.

   To align the SAF frame with the SS1 frame the SAF frame has to be
   rotated by +15 degrees about X axis.

   To align the SAF frame with the SS2 frame the SAF frame has to be
   rotated by 180 degrees about Z axis and then by +15 degrees about
   new position of X axis. 

   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   These sets of keywords define the MEX_ASPERA_SS1 and MEX_ASPERA_SS2
   frames:

   \begindata

      FRAME_MEX_ASPERA_SS1            =  -41161
      FRAME_-41161_NAME               = 'MEX_ASPERA_SS1'
      FRAME_-41161_CLASS              =  4
      FRAME_-41161_CLASS_ID           =  -41161
      FRAME_-41161_CENTER             =  -41
      TKFRAME_-41161_RELATIVE         = 'MEX_ASPERA_SAF'
      TKFRAME_-41161_SPEC             = 'ANGLES'
      TKFRAME_-41161_UNITS            = 'DEGREES'
      TKFRAME_-41161_AXES             = (   3,   2,     1   )
      TKFRAME_-41161_ANGLES           = (   0.0, 0.0, -15.0 )

      FRAME_MEX_ASPERA_SS2            =  -41162
      FRAME_-41162_NAME               = 'MEX_ASPERA_SS2'
      FRAME_-41162_CLASS              =  4
      FRAME_-41162_CLASS_ID           =  -41162
      FRAME_-41162_CENTER             =  -41
      TKFRAME_-41162_RELATIVE         = 'MEX_ASPERA_SAF'
      TKFRAME_-41162_SPEC             = 'ANGLES'
      TKFRAME_-41162_UNITS            = 'DEGREES'
      TKFRAME_-41162_AXES             = (   3,   2,     1   )
      TKFRAME_-41162_ANGLES           = ( 180.0, 0.0, -15.0 )

   \begintext


ASPERA Ion Mass Analyzer Unit Frames
--------------------------------------

   The ASPERA IMA unit is rigidly mounted in the -X/-Y quadrant of the
   on the s/c -Z ("main engine") deck and has no moving parts.
   Therefore, the three ASPERA IMA frames -- IMA Unit Reference Frame 
   (MEX_ASPERA_IMA_URF), IMA sensor frame (MEX_ASPERA_IMA), and IMA
   sensor head frame (MEX_ASPERA_IMAS) -- are defined as fixed offset
   frames.

   The MEX_ASPERA_IMA_URF frame is defined as follows:

      -  +Z axis is normal to the UMA unit mounting plate and points
         from the the mounting plate toward the sensor (nominally this
         axis is co-aligned with the s/c -Z axis);

      -  +X axis is parallel to the longer side of the IMA unit base
         and sensor symmetry axis and points towards the sensors
         aperture side (nominally this axis is co-aligned with
         the s/c -Y axis);

      -  +Y completes the right handed frame;

      -  the origin of the frame is located at the intersection of the
         mounting surface of the IMA unit base and the axis of the
         mounting screw hole located far right from the sensor aperture
         (as seen on the top view with the sensor aperture at the bottom
         on the page)

   The MEX_ASPERA_IMA frame is defined such that its axes are
   co-aligned with the axes of the MEX_ASPERA_IMA_URF frame and its
   origin is at the sensor's "focal point", located at the intersection
   of the sensor's aperture symmetry plane and the sensor's symmetry
   axis.

   The MEX_ASPERA_IMAS frame is defined such that its axes are aligned
   with the axes of the MEX_ASPERA_IMA frame as follows:

      -  +Z-imas points along the +X-ima axis

      -  +X-imas axis points along the +Y-ima axis
 
      -  +Y-imas axis points along the +Z-ima axis

   The origin of the MEX_ASPERA_IMAS is also at the sensor's "focal point".

   These diagrams illustrate the MEX_ASPERA_IMA_URF, MEX_ASPERA_IMA, and
   MEX_ASPERA_IMAS frames:

   -Z s/c side view ("main engine" side):
   --------------------------------------

                                +Xsc
       -Y Solar Array               ^              +Y Solar Array
      ._________________     .______|______.     .________________.
      |                 \    |      |      |    /                 |
      |                  \   |      |      |   /                  |
      |                   |  |      |+Zsc  |  |                   |
      |           +Zimas  |+Yimas   x------->o|                   |
      |           +Xima   |+Zima         +Ysc .                   |
      |                <----o  +Zimau      |   \                  |
      ._________________/  <----o__________.    \_________________.
                     +Ximau |   |'     `. 
                            V  /|________\
                        +Ximas  V        HGA
                        +Yima   +Yimau

                                          +Zsc is into the page
                                        +Zima, +Zimau, and +Yimas
                                           are out of the page


   -Z s/c side view ("main engine" side) -- zoom in:
   -------------------------------------------------
 
                           --                         ^ +Xsc
                          |  \                        |
                          |   \__                     |
                          |      \_________           |
                          |                \          |
                          |                 \         x------->
                IMA       |._____________.   \     +Zsc      +Ysc
               Aperture   || o         o |    \
                   .___________________. |     \____
          +Zimas   | |  +Yimas         | |          \
          +Xima    | |  +Zima          | |           \
               <-------o |             | |            \
                   | | | |             | |   IMA       |
                   | | | |             | | Mounting     \
                   .___|_______________. |   plate      |
                       |  || o <-------o |              |
                +Ximas V  |. +Ximau ___|_.             /
                +Yima     |            |               | "Main Engine"
                          .____________|_______________/      Deck
                                       |
                                +Yimau V


   As seen in the diagram two rotations are needed to align the s/c
   frame (MEX_SPACECRAFT) with the IMA URF frame (MEX_ASPERA_IMA_URF)
   -- first rotation is by 180 degrees about Y axis, and then by -90
   degrees about new position of Z axis.

   No rotations are needed to align the IMA URF frame
   (MEX_ASPERA_IMA_URF) with the IMA sensor frame (MEX_ASPERA_IMA).

   Two rotations -- first by +90 degrees about X axis and then by +90
   degrees about Y axis -- are needed to align the IMA sensor frame
   (MEX_ASPERA_IMA) with the IMA sensor head frame (MEX_ASPERA_IMAS).

   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   \begindata

      FRAME_MEX_ASPERA_IMA_URF        =  -41150
      FRAME_-41150_NAME               = 'MEX_ASPERA_IMA_URF'
      FRAME_-41150_CLASS              =  4
      FRAME_-41150_CLASS_ID           =  -41150
      FRAME_-41150_CENTER             =  -41
      TKFRAME_-41150_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41150_SPEC             = 'ANGLES'
      TKFRAME_-41150_UNITS            = 'DEGREES'
      TKFRAME_-41150_AXES             = ( 1,     2,    3   )
      TKFRAME_-41150_ANGLES           = ( 0.0, 180.0, 90.0 )

      FRAME_MEX_ASPERA_IMA            =  -41151
      FRAME_-41151_NAME               = 'MEX_ASPERA_IMA'
      FRAME_-41151_CLASS              =  4
      FRAME_-41151_CLASS_ID           =  -41151
      FRAME_-41151_CENTER             =  -41
      TKFRAME_-41151_RELATIVE         = 'MEX_ASPERA_IMA_URF'
      TKFRAME_-41151_SPEC             = 'ANGLES'
      TKFRAME_-41151_UNITS            = 'DEGREES'
      TKFRAME_-41151_AXES             = ( 1,   2,   3   )
      TKFRAME_-41151_ANGLES           = ( 0.0, 0.0, 0.0 )

      FRAME_MEX_ASPERA_IMAS           =  -41152
      FRAME_-41152_NAME               = 'MEX_ASPERA_IMAS'
      FRAME_-41152_CLASS              =  4
      FRAME_-41152_CLASS_ID           =  -41152
      FRAME_-41152_CENTER             =  -41
      TKFRAME_-41152_RELATIVE         = 'MEX_ASPERA_IMA'
      TKFRAME_-41152_SPEC             = 'ANGLES'
      TKFRAME_-41152_UNITS            = 'DEGREES'
      TKFRAME_-41152_AXES             = ( 3  ,   1,     2   )
      TKFRAME_-41152_ANGLES           = ( 0.0, -90.0, -90.0 )

   \begintext


HRSC Frames
========================================================================

   This section of the file contains the definitions of the HRSC camera
   frames.


HRSC Frame Tree
--------------------------------------

   The diagram below shows the HRSC frame hierarchy.


                               "J2000" INERTIAL
           +-----------------------------------------------------+
           |                          |                          |
           |<-pck                     |                          |<-pck
           |                          |                          |
           V                          |                          V
       "IAU_MARS"                     |                     "IAU_EARTH"
     MARS BODY-FIXED                  |<-ck               EARTH BODY-FIXED
     ---------------                  |                   ----------------
                                      V
                               "MEX_SPACECRAFT"
           +-----------------------------------
           |
           |<-fixed
           |
           V
    "MEX_HRSC_BASE"
    -------------------------+
           |                 |
           |<-fixed          |<-fixed
           |                 |
           V                 V
    "MEX_HRSC_HEAD"    "MEX_HRSC_SRC"
    ---------------    --------------


HRSC Base Frame
--------------------------------------

   The HRSC camera base frame is defined by the camera design and its
   mounting on the s/c as follows:

      -  +Z axis is in the nominal direction of the HRSC main and SRC
         camera boresights; it nominally points in the direction of the
         s/c +Z axis;

      -  +Y axis is perpendicular to the nominal direction of HRSC main
         camera and SRC camera CCD lines and nominally points along the
         s/c +Y axis, in the direction of flight;

      -  +X completes the right hand frame and is parallel to the nominal
         CCD detector lines; it nominally points in the direction of the
         s/c +X axis;

      -  the origin of the frame is located at the HRSC main camera focal
         point.

   Because the HRSC camera is rigidly mounted on the s/c, the HRSC base
   frame is defined as a fixed-offset frame with its orientation given
   relative to the MEX_SPACECRAFT frame. Any misalignment between nominal
   and actual HRSC camera mounting alignment measured pre-launch should
   be incorporated into the definition of this frame.

   This diagram illustrates nominal MEX_HRSC_BASE frame with respect to the
   spacecraft frame.

   +Z s/c side view:
   -----------------
                                                         direction
                                                         of flight (+Ysc)
                               ___________ HGA            ---------->
                               \         /
      .________________.     .__`._____.'__.     .________________.
      |                 \    |             |    /                 |
      |                  \   |             |   /                  |
      |                   |  |   +Zsc    +Ysc |                   |
      |                   |o=       o------->o|                   |
      |                   |  |      |  .___|  .  +Yhbase          |
      |                  /   |      |  | o------->                |
      ._________________/    .______|__._|_.    \_________________.
       -Y Solar Array               |    |  HRSC    +Y Solar Array
                               +Xsc V    |
                                         |
                                         V +Xhbase

                                             +Zsc and +Zhbase are
                                                 out of page


   Nominally, the HRSC base frame is co-aligned with the s/c frame.

   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   \begindata

      FRAME_MEX_HRSC_BASE             =  -41200
      FRAME_-41200_NAME               = 'MEX_HRSC_BASE'
      FRAME_-41200_CLASS              =  4
      FRAME_-41200_CLASS_ID           =  -41200
      FRAME_-41200_CENTER             =  -41
      TKFRAME_-41200_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41200_SPEC             = 'ANGLES'
      TKFRAME_-41200_UNITS            = 'DEGREES'
      TKFRAME_-41200_AXES             = ( 1,   2,   3   )
      TKFRAME_-41200_ANGLES           = ( 0.0, 0.0, 0.0 )

   \begintext


HRSC Main Camera Frame
--------------------------------------

   The HRSC main camera frame, MEX_HRSC_HEAD, is defined exactly as, and
   is nominally co-aligned with, the HRSC camera base frame
   MEX_HRSC_BASE. This frame is introduced to allow incorporating into
   the HRSC frame chain any misalignment between the camera base and main
   camera measured prior to delivering the camera for installation on the
   s/c.

   The following in-flight calibrated misalignment angles were provided
   by Thomas Roatsch on December 15, 2003:

      HRSC: ( -0.3340, 0.0101, 0.0 )

   These values are included in the definition below.

   \begindata

      FRAME_MEX_HRSC_HEAD             =  -41210
      FRAME_-41210_NAME               = 'MEX_HRSC_HEAD'
      FRAME_-41210_CLASS              =  4
      FRAME_-41210_CLASS_ID           =  -41210
      FRAME_-41210_CENTER             =  -41
      TKFRAME_-41210_RELATIVE         = 'MEX_HRSC_BASE'
      TKFRAME_-41210_SPEC             = 'ANGLES'
      TKFRAME_-41210_UNITS            = 'DEGREES'
      TKFRAME_-41210_AXES             = (  1,      2,      3   )
      TKFRAME_-41210_ANGLES           = ( -0.3340, 0.0101, 0.0 )

   \begintext


HRSC Super Resolution Camera Frame
--------------------------------------

   The HRSC SRC camera frame, MEX_HRSC_SRC, frame is introduced to
   allow incorporating into the HRSC frame chain any misalignment
   between the camera base and the SRC camera. This frame is fixed with
   respect to the the HRSC camera base frame, MEX_HRSC_BASE, and is
   nominally rotated by -90 degrees about +Z from it, as shown on the 
   diagram:

   +Z s/c side view:
   -----------------
                                                         direction
                                                         of flight (+Ysc)
                               ___________ HGA            ---------->
                               \         /
      .________________.     .__`._____.'__.     .________________.
      |                 \    |             |    /                 |
      |                  \   |             |   /                  |
      |                   |  |   +Zsc    +Ysc |                   |
      |                   |o=       o------->o|                   |
      |                   |  |      |  .___|  .  +Yhbase          |
      |                  / +Xsrc <-------o------->                |
      ._________________/    .______|__._|_.    \_________________.
       -Y Solar Array               |    |  HRSC    +Y Solar Array
                               +Xsc V    |
                                         |
                                         V +Xhbase
                                           +Ysrc

                                             +Zsc and +Zhbase are
                                                 out of page


   The following in-flight calibrated misalignment angles were provided
   by Thomas Roatsch on December 15, 2003:

      SRC:  ( -0.0735, -0.0301, 90.0 )

   These values are included in the definition below.

   \begindata

      FRAME_MEX_HRSC_SRC              =  -41220
      FRAME_-41220_NAME               = 'MEX_HRSC_SRC'
      FRAME_-41220_CLASS              =  4
      FRAME_-41220_CLASS_ID           =  -41220
      FRAME_-41220_CENTER             =  -41
      TKFRAME_-41220_RELATIVE         = 'MEX_HRSC_BASE'
      TKFRAME_-41220_SPEC             = 'ANGLES'
      TKFRAME_-41220_UNITS            = 'DEGREES'
      TKFRAME_-41220_AXES             = (  1,       2,       3   )
      TKFRAME_-41220_ANGLES           = ( -0.0735, -0.0301, 90.0 )

   \begintext


MARSIS Frames
========================================================================

   This section of the file contains the definitions of the MARSIS
   antenna frames.


MARSIS Frame Tree
--------------------------------------

   The diagram below shows the MARSIS frame hierarchy.


                               "J2000" INERTIAL
           +-----------------------------------------------------+
           |                          |                          |
           |<-pck                     |                          |<-pck
           |                          |                          |
           V                          |                          V
       "IAU_MARS"                     |                     "IAU_EARTH"
     MARS BODY-FIXED                  |<-ck               EARTH BODY-FIXED
     ---------------                  |                   ----------------
                                      V
                               "MEX_SPACECRAFT"
           +---------------------------------------------------+
           |                          |                        |
           |<-fixed                   |<-fixed                 |<-fixed
           |                          |                        |
           V                          V                        V
    "MEX_MARSIS_DIPOLE_1"  "MEX_MARSIS_DIPOLE_2"  "MEX_MARSIS_MONOPOLE"
    ---------------------  ---------------------  ---------------------


MARSIS Antenna Frames
--------------------------------------

   Because all three MARSIS antennas are rigidly mounted on the s/c, the
   MARSIS antenna frames are defined as fixed-offset frames with their
   orientation given relative to the MEX_SPACECRAFT frame.

   DUE TO INSUFFICIENT MARSIS DOCUMENTATION AVAILABLE TO NAIF AT THE
   TIME WHEN THIS FILE WAS CREATED, THE MARSIS ANTENNA FRAMES DEFINED IN
   THIS FILE ARE PLACE-HOLDERS MAKING ALL MARSIS ANTENNA FRAMES CO-ALIGNED
   WITH THE S/C FRAME (BVS, 06/13/01)

   \begindata

      FRAME_MEX_MARSIS_DIPOLE_1       =  -41310
      FRAME_-41310_NAME               = 'MEX_MARSIS_DIPOLE_1'
      FRAME_-41310_CLASS              =  4
      FRAME_-41310_CLASS_ID           =  -41310
      FRAME_-41310_CENTER             =  -41
      TKFRAME_-41310_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41310_SPEC             = 'ANGLES'
      TKFRAME_-41310_UNITS            = 'DEGREES'
      TKFRAME_-41310_AXES             = (   1,       2,       3     )
      TKFRAME_-41310_ANGLES           = ( 000.000, 000.000, 000.000 )

      FRAME_MEX_MARSIS_DIPOLE_2       =  -41320
      FRAME_-41320_NAME               = 'MEX_MARSIS_DIPOLE_2'
      FRAME_-41320_CLASS              =  4
      FRAME_-41320_CLASS_ID           =  -41320
      FRAME_-41320_CENTER             =  -41
      TKFRAME_-41320_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41320_SPEC             = 'ANGLES'
      TKFRAME_-41320_UNITS            = 'DEGREES'
      TKFRAME_-41320_AXES             = (   1,       2,       3     )
      TKFRAME_-41320_ANGLES           = ( 000.000, 000.000, 000.000 )

      FRAME_MEX_MARSIS_MONOPOLE       =  -41330
      FRAME_-41330_NAME               = 'MEX_MARSIS_MONOPOLE'
      FRAME_-41330_CLASS              =  4
      FRAME_-41330_CLASS_ID           =  -41330
      FRAME_-41330_CENTER             =  -41
      TKFRAME_-41330_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41330_SPEC             = 'ANGLES'
      TKFRAME_-41330_UNITS            = 'DEGREES'
      TKFRAME_-41330_AXES             = (   1,       2,       3     )
      TKFRAME_-41330_ANGLES           = ( 000.000, 000.000, 000.000 )

   \begintext


OMEGA Frames
========================================================================

   This section of the file contains the definitions of the OMEGA
   frames.


OMEGA Frame Tree
--------------------------------------

   The diagram below shows the OMEGA frame hierarchy.


                               "J2000" INERTIAL
           +-----------------------------------------------------+
           |                          |                          |
           |<-pck                     |                          |<-pck
           |                          |                          |
           V                          |                          V
       "IAU_MARS"                     |                     "IAU_EARTH"
     MARS BODY-FIXED                  |<-ck               EARTH BODY-FIXED
     ---------------                  |                   ----------------
                                      V
                               "MEX_SPACECRAFT"
           +------------------------------------------------+
           |
           |<-fixed
           |
           V
    "MEX_OMEGA_SWIR_C"
    -------------------------+
           |                 |
           |<-fixed          |<-fixed
           |                 |
           V                 V
    "MEX_OMEGA_VNIR"  "MEX_OMEGA_SWIR_L"
    ----------------  -----------------


OMEGA Frames
--------------------------------------

   All three OMEGA frames -- MEX_OMEGA_SWIR_C, MEX_OMEGA_SWIR_L, and
   MEX_OMEGA_VNIR -- are defined by the instrument design and mounting
   on the s/c as follows:

      -  +Z axis is nominally in the direction of the OMEGA VNIR and SWIR
         channel boresights; it nominally points in the direction of the
         s/c +Z axis;

      -  +Y axis is perpendicular to the nominal OMEGA VNIR and SWIR image
         lines; it nominally points along the s/c +Y axis, in the
         direction of flight;

      -  +X completes the right hand frame and is parallel to the nominal
         VNIR and SWIR image lines; it nominally points along the s/c +X
         axis;

      -  the origins of the frames are located at the OMEGA SWIR and
         VNIR channel telescope focal points.

   Because OMEGA is rigidly mounted on the s/c and its telescopes are 
   rigidly mounted to the instrument enclosure, all three frames are 
   defined as a fixed-offset frames.

   This diagram illustrates nominal MEX_OMEGA_* frames with respect to the
   spacecraft frame.

   +Z s/c side view:
   -----------------
                                                         direction
                                                         of flight (+Ysc)
                               ___________ HGA            ---------->
                               \         /
      .________________.     .__`._____.'__.     .________________.
      |                 \    |             |    /                 |
      |                  \   |             |   /                  |
      |                   |  |   +Zsc    +Ysc |                   |
      |                   |o=       o------->o|                   |
      |                   |  |      |._.   |  +Yomega_*           |
      |                  /   |      ||o------->                   |
      ._________________/    .______|.|.___.   \__________________.
       -Y Solar Array               | | OMEGA       +Y Solar Array
                               +Xsc V |
                                      |
                                      V +Xomega_*

                                             +Zsc and +Zomega_* are
                                                out of the page


   Nominally, all three OMEGA frames are co-aligned with the s/c frame.

   Because of the way the alignment calibrations have been performed,
   the orientation of the MEX_OMEGA_SWIR_C frame is defined w.r.t. to 
   the MEX_SPACECRAFT frame while the orientations of the MEX_OMEGA_SWIR_L 
   and MEX_OMEGA_VNIR are given relative to it.

   The definitions below are based on the initial in-flight calibrated 
   values provided by N. Manaud on March 12, 2004 ([15]).

   \begindata

      FRAME_MEX_OMEGA_SWIR_C          =  -41421
      FRAME_-41421_NAME               = 'MEX_OMEGA_SWIR_C'
      FRAME_-41421_CLASS              =  4
      FRAME_-41421_CLASS_ID           =  -41421
      FRAME_-41421_CENTER             =  -41
      TKFRAME_-41421_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41421_SPEC             = 'ANGLES'
      TKFRAME_-41421_UNITS            = 'DEGREES'
      TKFRAME_-41421_AXES             = ( 1,      2,     3   )
      TKFRAME_-41421_ANGLES           = ( -0.030, 0.249, 0.0 )

      FRAME_MEX_OMEGA_SWIR_L          =  -41422
      FRAME_-41422_NAME               = 'MEX_OMEGA_SWIR_L'
      FRAME_-41422_CLASS              =  4
      FRAME_-41422_CLASS_ID           =  -41422
      FRAME_-41422_CENTER             =  -41
      TKFRAME_-41422_RELATIVE         = 'MEX_OMEGA_SWIR_C'
      TKFRAME_-41422_SPEC             = 'ANGLES'
      TKFRAME_-41422_UNITS            = 'DEGREES'
      TKFRAME_-41422_AXES             = ( 1,       2,      3   )
      TKFRAME_-41422_ANGLES           = ( 0.0715, -0.0135, 0.0 )

      FRAME_MEX_OMEGA_VNIR            =  -41410
      FRAME_-41410_NAME               = 'MEX_OMEGA_VNIR'
      FRAME_-41410_CLASS              =  4
      FRAME_-41410_CLASS_ID           =  -41410
      FRAME_-41410_CENTER             =  -41
      TKFRAME_-41410_RELATIVE         = 'MEX_OMEGA_SWIR_C'
      TKFRAME_-41410_SPEC             = 'ANGLES'
      TKFRAME_-41410_UNITS            = 'DEGREES'
      TKFRAME_-41410_AXES             = (  1,      2,    3   )
      TKFRAME_-41410_ANGLES           = ( -0.3067, 0.0,  0.0 )

   \begintext


PFS Frames
========================================================================

   This section of the file contains the definitions of the PFS frames.


PFS Frame Tree
--------------------------------------

   The diagram below shows the PFS frame hierarchy.

                               "J2000" INERTIAL
           +-----------------------------------------------------+
           |                          |                          |
           |<-pck                     |                          |<-pck
           |                          |                          |
           V                          |                          V
       "IAU_MARS"                     |                     "IAU_EARTH"
     MARS BODY-FIXED                  |<-ck               EARTH BODY-FIXED
     ---------------                  |                   ----------------
                                      V
                               "MEX_SPACECRAFT"
           +------------------------------------
           |
           |<-fixed
           |
           V
    "MEX_PFS_BASE"
    ---------------------------+
           .                   |
           .<-fixed            |<-ck
           .                   |
           .                   V
           .            "MEX_PFS_SCANNER"
           .           +-----------------+
           .           |                 |
           .           |<-fixed          |<-fixed
           .           |                 |
           .           V                 V
           .     "MEX_PFS_SWC"     "MEX_PFS_LWC"
           .     -------------     -------------
           .
           .
           .
           V
    "MEX_PFS_{NADIR,25_LEFT,12_LEFT,25_RIGHT,12_RIGHT,COLD_SPACE}"
    --------------------------------------------------------------


PFS Base and Scanner Frames
--------------------------------------

   The PFS instrument is rigidly mounted on the s/c science
   deck. Therefore, the PFS base frame, MEX_PFS_BASE, is a fixed offset
   frame with its orientation given relative to the MEX_SPACECRAFT frame.

   The MEX_PFS_BASE frame is defined by the instrument design and its
   mounting on the s/c as follows:

      -  +Y axis is along the nominal PFS 'S' module scanner rotation
         axis and nominal PFS 'O' module optical axis, and points from the
         PFS 'S' module toward the PFS 'O' module; nominally this axis
         is co-aligned with the s/c +Y axis;

      -  +Z axis is parallel to the nominal direction of the PFS 'S'
         scanner boresight in its 'nadir' (zero) position; it nominally
         points in the same direction as the s/c +Z axis;

      -  +X completes the right hand frame; it nominally points
         in the same direction as the s/c +X axis;

      -  the origin of this frame is located at the intersection of the
         PFS 'S' scanner rotation axis and the scanner cylinder central
         axis.

   Nominally this frame is co-aligned with the s/c frame. Any misalignment
   between nominal and actual PFS mounting alignment measured pre-launch
   can be incorporated into the definition of this frame.

   Since the PFS 'S' scanner rotates with respect to its base, the
   MEX_PFS_SCANNER frame is defined as a CK frame with its orientation
   provided in a CK file relative to the MEX_PFS_BASE frame.

   The MEX_PFS_SCANNER frame is defined as follows:

      -  +Y axis is along the nominal PFS 'S' module scanner rotation
         axis and nominal PFS 'O' module optical axis, and points from the
         PFS 'S' module toward PFS 'O' module; nominally this axis
         is co-aligned with the +Y axis of the MEX_PFS_BASE frame;

      -  +Z axis is parallel to the PFS 'S' scanner boresight; in 'nadir'
         scanner position it is co-aligned with the +Z axis of
         the MEX_PFS_BASE frame;

      -  +X completes the right hand frame;

      -  the origin of this frame is located at the intersection of the
         PFS 'S' scanner rotation axis and the scanner central axis.

   For an arbitrary scanner angle, the MEX_PFS_SCANNER frame is rotated by
   this angle about the +Y axis with respect to the MEX_PFS_BASE frame.

   This diagram illustrates the MEX_PFS_BASE and MEX_PFS_SCANNER
   frames for scanner angles of +25 degrees ('25 left') and -25 degrees
   ('25 right'). Both diagrams are +Y s/c side view:


   Scanner in '25 left' position       Scanner in '25 right' position
   -----------------------------       ------------------------------

                +Zbase                          +Zbase
       +Zscan   ^                                  ^     +Zscan
            ^   |                       +Xscan     |   ^
             \  |       Science              ^.    |  /     Science
              \ | PFS     Deck                 `.  | /        Deck
               \|___________.                    `.|/__________.
        <-------o |         |              <-------o |         |
    +Xbase    .'  | =o========         +Xbase    |   | =o========
            .'|___.       SA+Y                 PFS___.       SA+Y
          <'  |   +Zsc      |                    |   +Zsc      |
       +Xscan |      ^      |                    |      ^      |
              |      |      |                    |      |      |
              |      |      |                    |      |      |
              .______|______.                    .______|______.
                |    |    |                        |    |    |
             <-------o____.                     <-------o____.
            +Xsc   / +Ysc                      +Xsc   / +Ysc
                  /_____\                            /_____\
                 Main Engine                         Main Engine

                     +Ysc, +Ybase, and +Yscan are out of the page


   These sets of keywords define PFS base and scanner frames:

   \begindata

      FRAME_MEX_PFS_BASE              =  -41500
      FRAME_-41500_NAME               = 'MEX_PFS_BASE'
      FRAME_-41500_CLASS              =  4
      FRAME_-41500_CLASS_ID           =  -41500
      FRAME_-41500_CENTER             =  -41
      TKFRAME_-41500_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41500_SPEC             = 'ANGLES'
      TKFRAME_-41500_UNITS            = 'DEGREES'
      TKFRAME_-41500_AXES             = ( 1,   2,   3   )
      TKFRAME_-41500_ANGLES           = ( 0.0, 0.0, 0.0 )

      FRAME_MEX_PFS_SCANNER           = -41530
      FRAME_-41530_NAME               = 'MEX_PFS_SCANNER'
      FRAME_-41530_CLASS              =  3
      FRAME_-41530_CLASS_ID           = -41530
      FRAME_-41530_CENTER             = -41
      CK_-41530_SCLK                  = -41
      CK_-41530_SPK                   = -41

   \begintext


PFS Detector Frames
--------------------------------------

   Since both PFS detectors receive radiation through the scanner
   and both essentially have a single pixel, their frames, MEX_PFS_SWC
   and MEX_PFS_LWC, are defined to be nominally co-aligned with the PFS
   scanner frame, MEX_PFS_SCANNER. These frames are introduced to allow
   incorporating into the PFS frame chain any misalignment between the
   scanner boresight direction and the individual detector view directions
   measured prior to delivering the instrument for installation
   on the s/c.

   Currently no misalignment data are available, and, therefore, the set of
   keywords below makes these frames co-aligned with their reference.

   \begindata

      FRAME_MEX_PFS_SWC               =  -41510
      FRAME_-41510_NAME               = 'MEX_PFS_SWC'
      FRAME_-41510_CLASS              =  4
      FRAME_-41510_CLASS_ID           =  -41510
      FRAME_-41510_CENTER             =  -41
      TKFRAME_-41510_RELATIVE         = 'MEX_PFS_SCANNER'
      TKFRAME_-41510_SPEC             = 'ANGLES'
      TKFRAME_-41510_UNITS            = 'DEGREES'
      TKFRAME_-41510_AXES             = ( 1,   2,   3   )
      TKFRAME_-41510_ANGLES           = ( 0.0, 0.0, 0.0 )

      FRAME_MEX_PFS_LWC               =  -41520
      FRAME_-41520_NAME               = 'MEX_PFS_LWC'
      FRAME_-41520_CLASS              =  4
      FRAME_-41520_CLASS_ID           =  -41520
      FRAME_-41520_CENTER             =  -41
      TKFRAME_-41520_RELATIVE         = 'MEX_PFS_SCANNER'
      TKFRAME_-41520_SPEC             = 'ANGLES'
      TKFRAME_-41520_UNITS            = 'DEGREES'
      TKFRAME_-41520_AXES             = ( 1,   2,   3   )
      TKFRAME_-41520_ANGLES           = ( 0.0, 0.0, 0.0 )

   \begintext


PFS Scanner Fixed Positions Frames
--------------------------------------

   Because the PFS 'S' scanner can be rotated to only a limited number of
   positions for external observations -- 'nadir', '25 deg left', '12.5
   deg left', '25 deg right', '12.5 deg right', and 'cold_space' -- a
   fixed frame co-aligned with the scanner frame in each of these
   positions is defined to allow computing scanner orientation without
   needing to use CK.

   Each of these 'fixed-scanner-position' convenience frames is defined
   as a fixed offset frame with respect to the MEX_PFS_BASE frame as follows:

      -  +Y axis is along the nominal PFS 'S' module scanner rotation
         axis and nominal PFS 'O' module optical axis, and points from the
         PFS 'S' module toward PFS 'O' module; nominally this axis
         is co-aligned with the +Y axis of the MEX_PFS_BASE frame;

      -  +Z axis is parallel to the PFS 'S' scanner boresight at a particular
         angle;

      -  +X completes the right hand frame;

      -  the origin of this frame is located at the intersection of the
         PFS 'S' scanner rotation axis and scanner central axis.

   This diagram illustrates fixed PFS scanner pointing directions co-aligned
   with the +Z axis of the corresponding 'fixed-scanner-position' (fsp)
   frame:

   +Y s/c side view
   ----------------
                                 +Zbase
                                  ^
                                  |

                                nadir
                                  |
                        12.5 left |  12.5 right
                                . | .
                      25 left \ . | . / 25 right
                               \ .|. /
                                \.|./
                                 \|/__________. Science Deck
              <---    cold -------o |         |
            +Xbase    space     |   | =o========
                                |___.       SA+Y
                              PFS    +Zsc     |
                                |      ^      |
                                |      |      |
                                |      |      |
                                .______|______.
                                  |    |    |
                               <-------o____.
                              +Xsc   / +Ysc
                                    /_____\
                                   Main Engine

                     +Ysc, +Ybase, and +Yfsp are out of the page


   The 'fixed-scanner-position' frames are nominally rotated about the
   +Y axis of the MEX_PFS_BASE frames by the following angles:

      Frame name              Rotation Angle, deg
      ----------------------  -------------------
      MEX_PFS_25_RIGHT        -25.0
      MEX_PFS_12_RIGHT        -12.5
      MEX_PFS_NADIR             0.0
      MEX_PFS_12_LEFT          12.5
      MEX_PFS_25_LEFT          25.0
      MEX_PFS_COLD_SPACE       90.0

   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   \begindata

      FRAME_MEX_PFS_25_RIGHT          =  -41535
      FRAME_-41535_NAME               = 'MEX_PFS_25_RIGHT'
      FRAME_-41535_CLASS              =  4
      FRAME_-41535_CLASS_ID           =  -41535
      FRAME_-41535_CENTER             =  -41
      TKFRAME_-41535_RELATIVE         = 'MEX_PFS_BASE'
      TKFRAME_-41535_SPEC             = 'ANGLES'
      TKFRAME_-41535_UNITS            = 'DEGREES'
      TKFRAME_-41535_AXES             = ( 1,    2,   3   )
      TKFRAME_-41535_ANGLES           = ( 0.0, 25.0, 0.0 )

      FRAME_MEX_PFS_12_RIGHT          =  -41534
      FRAME_-41534_NAME               = 'MEX_PFS_12_RIGHT'
      FRAME_-41534_CLASS              =  4
      FRAME_-41534_CLASS_ID           =  -41534
      FRAME_-41534_CENTER             =  -41
      TKFRAME_-41534_RELATIVE         = 'MEX_PFS_BASE'
      TKFRAME_-41534_SPEC             = 'ANGLES'
      TKFRAME_-41534_UNITS            = 'DEGREES'
      TKFRAME_-41534_AXES             = ( 1,    2,   3   )
      TKFRAME_-41534_ANGLES           = ( 0.0, 12.5, 0.0 )

      FRAME_MEX_PFS_NADIR             =  -41533
      FRAME_-41533_NAME               = 'MEX_PFS_NADIR'
      FRAME_-41533_CLASS              =  4
      FRAME_-41533_CLASS_ID           =  -41533
      FRAME_-41533_CENTER             =  -41
      TKFRAME_-41533_RELATIVE         = 'MEX_PFS_BASE'
      TKFRAME_-41533_SPEC             = 'ANGLES'
      TKFRAME_-41533_UNITS            = 'DEGREES'
      TKFRAME_-41533_AXES             = ( 1,   2,   3   )
      TKFRAME_-41533_ANGLES           = ( 0.0, 0.0, 0.0 )

      FRAME_MEX_PFS_12_LEFT           =  -41532
      FRAME_-41532_NAME               = 'MEX_PFS_12_LEFT'
      FRAME_-41532_CLASS              =  4
      FRAME_-41532_CLASS_ID           =  -41532
      FRAME_-41532_CENTER             =  -41
      TKFRAME_-41532_RELATIVE         = 'MEX_PFS_BASE'
      TKFRAME_-41532_SPEC             = 'ANGLES'
      TKFRAME_-41532_UNITS            = 'DEGREES'
      TKFRAME_-41532_AXES             = ( 1,     2,   3   )
      TKFRAME_-41532_ANGLES           = ( 0.0, -12.5, 0.0 )

      FRAME_MEX_PFS_25_LEFT           =  -41531
      FRAME_-41531_NAME               = 'MEX_PFS_25_LEFT'
      FRAME_-41531_CLASS              =  4
      FRAME_-41531_CLASS_ID           =  -41531
      FRAME_-41531_CENTER             =  -41
      TKFRAME_-41531_RELATIVE         = 'MEX_PFS_BASE'
      TKFRAME_-41531_SPEC             = 'ANGLES'
      TKFRAME_-41531_UNITS            = 'DEGREES'
      TKFRAME_-41531_AXES             = ( 1,     2,   3   )
      TKFRAME_-41531_ANGLES           = ( 0.0, -25.0, 0.0 )

      FRAME_MEX_PFS_COLD_SPACE        =  -41536
      FRAME_-41536_NAME               = 'MEX_PFS_COLD_SPACE'
      FRAME_-41536_CLASS              =  4
      FRAME_-41536_CLASS_ID           =  -41536
      FRAME_-41536_CENTER             =  -41
      TKFRAME_-41536_RELATIVE         = 'MEX_PFS_BASE'
      TKFRAME_-41536_SPEC             = 'ANGLES'
      TKFRAME_-41536_UNITS            = 'DEGREES'
      TKFRAME_-41536_AXES             = ( 1,     2,   3   )
      TKFRAME_-41536_ANGLES           = ( 0.0, -90.0, 0.0 )

   \begintext


SPICAM Frames
========================================================================

   This section of the file contains the definitions of the SPICAM
   frames.


SPICAM Frame Tree
--------------------------------------

   The diagram below shows the SPICAM frame hierarchy.


                               "J2000" INERTIAL
           +-----------------------------------------------------+
           |                          |                          |
           |<-pck                     |                          |<-pck
           |                          |                          |
           V                          |                          V
       "IAU_MARS"                     |                     "IAU_EARTH"
     MARS BODY-FIXED                  |<-ck               EARTH BODY-FIXED
     ---------------                  |                   ----------------
                                      V
                               "MEX_SPACECRAFT"
           +-----------------------------------+
           |
           |<-fixed
           |
           V
    "MEX_SPICAM_BASE"
    -------------------------------------------+
           |           |           |           |
           |<-fixed    |<-fixed    |<--fixed   |<--fixed
           |           |           |           |
           V           |           V           |
    "MEX_SPICAM_SIR"   |    "MEX_SPICAM_SUV"   |
    ----------------   |    ----------------   |
                       V                       V
            "MEX_SPICAM_SIR_SOLAR"   "MEX_SPICAM_SUV_SOLAR"
            ----------------------   ----------------------


SPICAM Base Frame
--------------------------------------

   The SPICAM base frame is defined by the instrument design and its
   mounting on the s/c as follows:

      -  +Z axis is in the nominal direction of the SPICAM SUV/nadir and
         IR detector boresights; it nominally points in the direction of the
         s/c +Z axis;

      -  +X axis is parallel to the nominal direction of SPICAM SUV CCD
         columns; it is nominally along the s/c +X axis;

      -  +Y completes the right hand frame; it is nominally along
         the s/c +Y axis and points in the direction of flight; (*)

      -  the origin of the frame is located at the SPICAM SUV detector
         focal point.

   (*) SPICAM SUV spectral dimension is along CCD lines, which are parallel
   to Y,Z plane. SPICAM SIR has a single pixel.

   Because the SPICAM instrument is rigidly mounted to the s/c, the SPICAM
   base frame is defined as a fixed-offset frame with its orientation
   given relative to the MEX_SPACECRAFT frame. Any misalignment between the
   nominal and actual SPICAM mounting alignment measured pre-launch
   can be incorporated into the definition of this frame.

   This diagram illustrates the nominal MEX_SPICAM_BASE frame with respect
   to the spacecraft frame.

   +Z s/c side view:
   -----------------
                                                         direction
                                                         of flight (+Ysc)
                               ___________ HGA            ---------->
                               \         /
      .________________.     .__`._____.'__.     .________________.
      |                 \    |             |    /                 |
      |                  \   | +Ysbase     |   /                  |
      |                   |  .____.        |  |                   |
      |                   |o=|o---> o------->o|                   |
      |               SPICAM .|___. |+Zsc  |+Ysc                  |
      |                  /   ||     |      |   \                  |
      ._________________/    .|_____|______.    \_________________.
       -Y Solar Array         |     |                +Y Solar Array
                              V     V
                         +Xsbase   +Xsc

                                             +Zsc and +Zsbase are
                                                 out of the page


   Nominally, the SPICAM base frame is co-aligned the s/c frame.

   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   \begindata

      FRAME_MEX_SPICAM_BASE           =  -41600
      FRAME_-41600_NAME               = 'MEX_SPICAM_BASE'
      FRAME_-41600_CLASS              =  4
      FRAME_-41600_CLASS_ID           =  -41600
      FRAME_-41600_CENTER             =  -41
      TKFRAME_-41600_RELATIVE         = 'MEX_SPACECRAFT'
      TKFRAME_-41600_SPEC             = 'ANGLES'
      TKFRAME_-41600_UNITS            = 'DEGREES'
      TKFRAME_-41600_AXES             = (   1,       2,       3     )
      TKFRAME_-41600_ANGLES           = ( 000.000, 000.000, 000.000 )

   \begintext


SPICAM Detector Frames
--------------------------------------

   The SPICAM detector nadir port frames -- MEX_SPICAM_SUV and
   MEX_SPICAM_SIR -- are defined exactly as, and are nominally co-aligned
   with, the SPICAM base frame MEX_SPICAM_BASE. These frames are
   introduced to allow incorporating into the SPICAM frame chain any
   misalignment between the instrument base and detectors measured prior
   to delivering the instrument for installation on the s/c.

   The SPICAM SIR and SUV detector solar port frames -- MEX_SPICAM_SIR_SOLAR
   and MEX_SPICAM_SUV_SOLAR -- are defined in the same way as follows:

      -  +Z axis points along SIR/SUV solar port boresight;

      -  +X axis is parallel to the Sensor Unit assembly symmetry axis
         and points towards the SPICAM SUV/SIR opening side; it points
         nominally along the s/c +Z axis;

      -  +Y axis completes the right hand frame;

      -  the origin of the frame is located at the SPICAM SIR/SUV detector
         focal point.

   This diagram illustrates all three SPICAM detector port frames:

   +Z s/c side view:
   -----------------
                                                         direction
                                      HGA                of flight (+Ysc)
             +Zsir_solar       ___________               ---------->
             +Zsuv_solar ^     \    ^ +Ysir_solar
      .________________.  \  .__` .'  +Ysuv_solar.________________.
      |                 \  \ |  .'         |    /                 |
      |          +Xsir_solar\|.'     +Ybase|   /                  |
      |          +Xsuv_solar o____.  +Ysuv |  |           +Ysc    |
      |                   |o=|o------->    |=o|       ------->    |
      |               SPICAM .|__o-------> |  |                   |
      |                  /   ||  |      +Ysir  \                  |
      ._________________/    .|__|_________.    \_________________.
       -Y Solar Array         |  |                   +Y Solar Array
                       +Xbase V  |
                       +Xsuv     V +Xsir

                                 |            +Zsc, +Zbase, +Zsuv,
                                 |            +Zsir, +Xsir_solar,
                                 V +Xsc       and +Xsuv_solar
                                              are out of the page


   Nominally, the SPICAM SIR and SUV frames are co-aligned the SPICAM base
   frame while the SPICAM SIR and SUV solar port frames are first rotated
   from it by -90 degrees about +Y, and then rotated by +30 degrees about
   the new position of +X.

   Since the SPICE frames subsystem calls for specifying the reverse
   transformation--going from the instrument or structure frame to the
   base frame--as compared to the description given above, the order of
   rotations assigned to the TKFRAME_*_AXES keyword is also reversed
   compared to the above text, and the signs associated with the
   rotation angles assigned to the TKFRAME_*_ANGLES keyword are the
   opposite from what is written in the above text.

   \begindata

      FRAME_MEX_SPICAM_SIR            =  -41610
      FRAME_-41610_NAME               = 'MEX_SPICAM_SIR'
      FRAME_-41610_CLASS              =  4
      FRAME_-41610_CLASS_ID           =  -41610
      FRAME_-41610_CENTER             =  -41
      TKFRAME_-41610_RELATIVE         = 'MEX_SPICAM_BASE'
      TKFRAME_-41610_SPEC             = 'ANGLES'
      TKFRAME_-41610_UNITS            = 'DEGREES'
      TKFRAME_-41610_AXES             = ( 1,   2,   3   )
      TKFRAME_-41610_ANGLES           = ( 0.0, 0.0, 0.0 )


      FRAME_MEX_SPICAM_SUV            =  -41620
      FRAME_-41620_NAME               = 'MEX_SPICAM_SUV'
      FRAME_-41620_CLASS              =  4
      FRAME_-41620_CLASS_ID           =  -41620
      FRAME_-41620_CENTER             =  -41
      TKFRAME_-41620_RELATIVE         = 'MEX_SPICAM_BASE'
      TKFRAME_-41620_SPEC             = 'ANGLES'
      TKFRAME_-41620_UNITS            = 'DEGREES'
      TKFRAME_-41620_AXES             = ( 1,   2,   3   )
      TKFRAME_-41620_ANGLES           = ( 0.0, 0.0, 0.0 )


      FRAME_MEX_SPICAM_SIR_SOLAR      =  -41611
      FRAME_-41611_NAME               = 'MEX_SPICAM_SIR_SOLAR'
      FRAME_-41611_CLASS              =  4
      FRAME_-41611_CLASS_ID           =  -41611
      FRAME_-41611_CENTER             =  -41
      TKFRAME_-41611_RELATIVE         = 'MEX_SPICAM_BASE'
      TKFRAME_-41611_SPEC             = 'ANGLES'
      TKFRAME_-41611_UNITS            = 'DEGREES'
      TKFRAME_-41611_AXES             = ( 3,    2,     1   )
      TKFRAME_-41611_ANGLES           = ( 0.0, 90.0, -30.0 )


      FRAME_MEX_SPICAM_SUV_SOLAR      =  -41621
      FRAME_-41621_NAME               = 'MEX_SPICAM_SUV_SOLAR'
      FRAME_-41621_CLASS              =  4
      FRAME_-41621_CLASS_ID           =  -41621
      FRAME_-41621_CENTER             =  -41
      TKFRAME_-41621_RELATIVE         = 'MEX_SPICAM_BASE'
      TKFRAME_-41621_SPEC             = 'ANGLES'
      TKFRAME_-41621_UNITS            = 'DEGREES'
      TKFRAME_-41621_AXES             = ( 3,    2,     1   )
      TKFRAME_-41621_ANGLES           = ( 0.0, 90.0, -30.0 )

   \begintext


BEAGLE2 Frames
========================================================================

   This section of the file contains the definitions of the Beagle-2 local
   level, lander and instrument frames.


Beagle-2 Frame Tree
--------------------------------------

   The diagram below shows the Beagle-2 Lander and its instruments frame
   hierarchy.


                               "J2000" INERTIAL
           +-----------------------------------------------------+
           |                          |                          |
           |<-ck                      |                          |<-pck
           |                          |                          |
           V                          |                          V
    "MEX_SPACECRAFT"                  |                     "IAU_EARTH"
     ---------------                  |<-pck              EARTH BODY-FIXED
                                      |                   ----------------
                                      V
                                 "IAU_MARS"
                               MARS BODY-FIXED
                               ---------------
                                      |
                                      |<-fixed
                                      |
                                      V
                             "BEAGLE2_LOCAL_LEVEL"
                             --------------------
                                      |
                                      |<-fixed
                                      |
                                      V
                               "BEAGLE2_LANDER"
                               ----------------
                                      |
                                      |
                                      |
                                      V
                               (Beagle-2 instrument
                                    frames)


Beagle-2 Local Level frame
--------------------------------------

   This BEAGLE2_LOCAL_LEVEL frame at the landing site is defined as
   follows:

      -  +Z axis is the normal outward at the landing site;
      -  +X axis points at local north;
      -  +Y completes the right hand frame;
      -  the origin of this frame is located on the surface under
         the geometric center of the lander capsule;

   The orientation of the frame is fixed relative to the Mars fixed
   rotating frame 'IAU_MARS' and is determined by the landing site
   coordinates.

   The target Beagle-2 landing site selected on December 20, 2000 is
   located at:

            270.0 degrees west  planetographic longitude, and
             10.6 degrees north planetographic latitude

   which is in Isidis Basin of Utopia Planitia.

   More accurate coordinates used by the project during summer/fall
   2003 are:

            269.250 degrees west  planetographic longitude, and
             11.734 degrees north planetographic latitude

   The transformation from 'BEAGLE2_LOCAL_LEVEL' frame to 'IAU_MARS'
   frame is a 3-2-3 rotation with angles defined as the negative of the
   site longitude, the negative of the site colatitude, 180 degrees.

   \begindata

      FRAME_BEAGLE2_LOCAL_LEVEL       =  -44900
      FRAME_-44900_NAME               = 'BEAGLE2_LOCAL_LEVEL'
      FRAME_-44900_CLASS              =  4
      FRAME_-44900_CLASS_ID           =  -44900
      FRAME_-44900_CENTER             =  -44
      TKFRAME_-44900_RELATIVE         = 'IAU_MARS'
      TKFRAME_-44900_SPEC             = 'ANGLES'
      TKFRAME_-44900_UNITS            = 'DEGREES'
      TKFRAME_-44900_AXES             = (   3,       2,       3   )
      TKFRAME_-44900_ANGLES           = ( 269.250, -78.266, 180.0 )

   \begintext


Beagle-2 Lander Frame
--------------------------------------

   The BEAGLE2 lander frame, BEAGLE2_LANDER, is defined by the lander
   design as follows:

      -  +Z axis is parallel to the lander capsule symmetry axis and
         points up;
      -  +X axis point from +Z toward robotic arm;
      -  +Y axis completes the right hand frame;
      -  the origin of the frame is located at the intersection of the lander
         main capsule outer rim plane and the lander main capsule symmetry
         axis.

   Once landed the lander orientation is constant with respect to the
   surface and, therefore, this frame is defined as a fixed-offset frame
   with its orientation provided with respect to the BEAGLE2_LOCAL_LEVEL
   frame. Should the lander orientation change due to sliding and/or science
   instrument operations, BEAGLE2_LANDER frame should be re-defined as
   a CK based frame.

   For nominal applications, the BEAGLE2_LANDER frame is assumed to be
   co-aligned with the BEAGLE2_LOCAL_LEVEL frame.

   \begindata

      FRAME_BEAGLE2_LANDER            =  -44000
      FRAME_-44000_NAME               = 'BEAGLE2_LANDER'
      FRAME_-44000_CLASS              =  4
      FRAME_-44000_CLASS_ID           =  -44000
      FRAME_-44000_CENTER             =  -44
      TKFRAME_-44000_RELATIVE         = 'BEAGLE2_LOCAL_LEVEL'
      TKFRAME_-44000_SPEC             = 'ANGLES'
      TKFRAME_-44000_UNITS            = 'DEGREES'
      TKFRAME_-44000_AXES             = ( 1,   2,   3   )
      TKFRAME_-44000_ANGLES           = ( 0.0, 0.0, 0.0 )

   \begintext


Beagle-2 Instrument Frames
--------------------------------------

   Beagle-2 instrument frames are TDB.


Mars Express Mission NAIF ID Codes -- Definition Section
========================================================================

   This section contains name to NAIF ID mappings for the MEX mission.


Mars Express Spacecraft (MEX) spacecraft and instruments IDs:
-------------------------------------------------------------

   This table summarizes MEX Spacecraft IDs:

            Name                   ID       Synonyms
            ---------------------  -------  ---------------------------
            MEX                    -41      MARS EXPRESS, MARS-EXPRESS,
                                            MARS_EXPRESS
   Notes:

      -- 'MEX', 'MARS EXPRESS', 'MARS-EXPRESS', and 'MARS_EXPRESS' are
         synonyms and all map to the official MEX s/c ID (-41);

   Name-ID Mapping keywords:

         \begindata

            NAIF_BODY_NAME += ( 'MEX'                    )
            NAIF_BODY_CODE += ( -41                      )

            NAIF_BODY_NAME += ( 'MARS EXPRESS'           )
            NAIF_BODY_CODE += ( -41                      )

            NAIF_BODY_NAME += ( 'MARS-EXPRESS'           )
            NAIF_BODY_CODE += ( -41                      )

            NAIF_BODY_NAME += ( 'MARS_EXPRESS'           )
            NAIF_BODY_CODE += ( -41                      )

         \begintext


ASPERA IDs
--------------------------------------

   This table summarizes ASPERA IDs:


            Name                   ID
            ---------------------  -------
            MEX_ASPERA             -41100
            MEX_ASPERA_URF         -41110
            MEX_ASPERA_SAF         -41111
            MEX_ASPERA_ELS         -41120
            MEX_ASPERA_NPI         -41130
            MEX_ASPERA_NPD1        -41141
            MEX_ASPERA_NPD2        -41142
            MEX_ASPERA_IMA_URF     -41150
            MEX_ASPERA_IMA         -41151
            MEX_ASPERA_IMAS        -41152
            MEX_ASPERA_SS1         -41161
            MEX_ASPERA_SS2         -41162

   Name-ID Mapping keywords:

         \begindata

            NAIF_BODY_NAME += ( 'MEX_ASPERA'             )
            NAIF_BODY_CODE += ( -41100                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_URF'         )
            NAIF_BODY_CODE += ( -41110                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_SAF'         )
            NAIF_BODY_CODE += ( -41111                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_ELS'         )
            NAIF_BODY_CODE += ( -41120                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_NPI'         )
            NAIF_BODY_CODE += ( -41130                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_NPD1'        )
            NAIF_BODY_CODE += ( -41141                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_NPD2'        ) 
            NAIF_BODY_CODE += ( -41142                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_IMA_URF'     )
            NAIF_BODY_CODE += ( -41150                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_IMA'         )
            NAIF_BODY_CODE += ( -41151                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_IMAS'        )
            NAIF_BODY_CODE += ( -41152                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_SS1'         )
            NAIF_BODY_CODE += ( -41161                   )

            NAIF_BODY_NAME += ( 'MEX_ASPERA_SS2'         )
            NAIF_BODY_CODE += ( -41162                   )

         \begintext

HRSC IDs
--------------------------------------

   This table summarizes HRSC IDs:

            Name                   ID
            ---------------------  -------
            MEX_HRSC               -41200
            MEX_HRSC_HEAD          -41210
            MEX_HRSC_S2            -41211
            MEX_HRSC_RED           -41212
            MEX_HRSC_P2            -41213
            MEX_HRSC_BLUE          -41214
            MEX_HRSC_NADIR         -41215
            MEX_HRSC_GREEN         -41216
            MEX_HRSC_P1            -41217
            MEX_HRSC_IR            -41218
            MEX_HRSC_S1            -41219
            MEX_HRSC_SRC           -41220

   Notes:

      -- each CCD line detector has its own ID to allow individual
         CCD detector FOV definitions in the Instrument Kernel (IK).

   Name-ID Mapping keywords:

         \begindata

            NAIF_BODY_NAME += ( 'MEX_HRSC'               )
            NAIF_BODY_CODE += ( -41200                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_HEAD'          )
            NAIF_BODY_CODE += ( -41210                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_S2'            )
            NAIF_BODY_CODE += ( -41211                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_RED'           )
            NAIF_BODY_CODE += ( -41212                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_P2'            )
            NAIF_BODY_CODE += ( -41213                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_BLUE'          )
            NAIF_BODY_CODE += ( -41214                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_NADIR'         )
            NAIF_BODY_CODE += ( -41215                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_GREEN'         )
            NAIF_BODY_CODE += ( -41216                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_P1'            )
            NAIF_BODY_CODE += ( -41217                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_IR'            )
            NAIF_BODY_CODE += ( -41218                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_S1'            )
            NAIF_BODY_CODE += ( -41219                   )

            NAIF_BODY_NAME += ( 'MEX_HRSC_SRC'           )
            NAIF_BODY_CODE += ( -41220                   )

         \begintext


MARSIS IDs:
--------------------------------------

   This table summarizes MARSIS IDs:

            Name                   ID
            ---------------------  -------
            MEX_MARSIS             -41300
            MEX_MARSIS_DIPOLE_1    -41310
            MEX_MARSIS_DIPOLE_2    -41320
            MEX_MARSIS_MONOPOLE    -41330

   Name-ID Mapping keywords:

         \begindata

            NAIF_BODY_NAME += ( 'MEX_MARSIS'             )
            NAIF_BODY_CODE += ( -41300                   )

            NAIF_BODY_NAME += ( 'MEX_MARSIS_DIPOLE_1'    )
            NAIF_BODY_CODE += ( -41310                   )

            NAIF_BODY_NAME += ( 'MEX_MARSIS_DIPOLE_2'    )
            NAIF_BODY_CODE += ( -41320                   )

            NAIF_BODY_NAME += ( 'MEX_MARSIS_MONOPOLE'    )
            NAIF_BODY_CODE += ( -41330                   )

         \begintext


OMEGA IDs
--------------------------------------

   This table summarizes OMEGA IDs:

            Name                   ID
            ---------------------  -------
            MEX_OMEGA              -41400
            MEX_OMEGA_VNIR         -41410
            MEX_OMEGA_SWIR         -41420
            MEX_OMEGA_SWIR_C       -41421
            MEX_OMEGA_SWIR_L       -41422

   Name-ID Mapping keywords:

         \begindata

            NAIF_BODY_NAME += ( 'MEX_OMEGA'              )
            NAIF_BODY_CODE += ( -41400                   )

            NAIF_BODY_NAME += ( 'MEX_OMEGA_VNIR'         )
            NAIF_BODY_CODE += ( -41410                   )

            NAIF_BODY_NAME += ( 'MEX_OMEGA_SWIR'         )
            NAIF_BODY_CODE += ( -41420                   )

            NAIF_BODY_NAME += ( 'MEX_OMEGA_SWIR_C'       )
            NAIF_BODY_CODE += ( -41421                   )

            NAIF_BODY_NAME += ( 'MEX_OMEGA_SWIR_L'       )
            NAIF_BODY_CODE += ( -41422                   )

         \begintext


PFS IDs
--------------------------------------

   This table summarizes PFS IDs:


            Name                   ID
            ---------------------  -------
            MEX_PFS                -41500
            MEX_PFS_SWC            -41510
            MEX_PFS_LWC            -41520
            MEX_PFS_SCANNER        -41530

   Name-ID Mapping keywords:

         \begindata

            NAIF_BODY_NAME += ( 'MEX_PFS'                )
            NAIF_BODY_CODE += ( -41500                   )

            NAIF_BODY_NAME += ( 'MEX_PFS_SWC'            )
            NAIF_BODY_CODE += ( -41510                   )

            NAIF_BODY_NAME += ( 'MEX_PFS_LWC'            )
            NAIF_BODY_CODE += ( -41520                   )

            NAIF_BODY_NAME += ( 'MEX_PFS_SCANNER'        )
            NAIF_BODY_CODE += ( -41530                   )

         \begintext

SPICAM IDs
--------------------------------------

   This table summarizes SPICAM IDs:

            Name                   ID
            ---------------------  -------
            MEX_SPICAM             -41600
            MEX_SPICAM_SIR         -41610
            MEX_SPICAM_SIR_SOLAR   -41611
            MEX_SPICAM_SUV         -41620
            MEX_SPICAM_SUV_SOLAR   -41621


   Name-ID Mapping keywords:

         \begindata

            NAIF_BODY_NAME += ( 'MEX_SPICAM'             )
            NAIF_BODY_CODE += ( -41600                   )

            NAIF_BODY_NAME += ( 'MEX_SPICAM_SIR'         )
            NAIF_BODY_CODE += ( -41610                   )

            NAIF_BODY_NAME += ( 'MEX_SPICAM_SIR_SOLAR'   )
            NAIF_BODY_CODE += ( -41611                   )

            NAIF_BODY_NAME += ( 'MEX_SPICAM_SUV'         )
            NAIF_BODY_CODE += ( -41620                   )

            NAIF_BODY_NAME += ( 'MEX_SPICAM_SUV_SOLAR'   )
            NAIF_BODY_CODE += ( -41621                   )

         \begintext


MEX Spacecraft Structures IDs
--------------------------------------

   This table summarizes MEX Spacecraft Structure IDs:

            Name                   ID       Synonyms
            ---------------------  -------  -------------------------
            MEX_SPACECRAFT         -41000   MEX_SC
            MEX_SA+Y               -41011
            MEX_SA-Y               -41012
            MEX_SA+Y_GIMBAL        -41013
            MEX_SA-Y_GIMBAL        -41014

            MEX_HGA                -41020
            MEX_MELACOM_1          -41031
            MEX_MELACOM_2          -41032
            MEX_LGA                -41040

            MEX_VMC                -41050

   Notes:

      -- 'MEX_SC' and 'MEX_SPACECRAFT' are synonyms and all map to the MEX
         s/c bus structure ID (-41000);

   Name-ID Mapping keywords:

         \begindata

            NAIF_BODY_NAME += ( 'MEX_SPACECRAFT'         )
            NAIF_BODY_CODE += ( -41000                   )

            NAIF_BODY_NAME += ( 'MEX_SC'                 )
            NAIF_BODY_CODE += ( -41000                   )

            NAIF_BODY_NAME += ( 'MEX_SA+Y'               )
            NAIF_BODY_CODE += ( -41011                   )

            NAIF_BODY_NAME += ( 'MEX_SA-Y'               )
            NAIF_BODY_CODE += ( -41012                   )

            NAIF_BODY_NAME += ( 'MEX_SA+Y_GIMBAL'        )
            NAIF_BODY_CODE += ( -41013                   )

            NAIF_BODY_NAME += ( 'MEX_SA-Y_GIMBAL'        )
            NAIF_BODY_CODE += ( -41014                   )

            NAIF_BODY_NAME += ( 'MEX_HGA'                )
            NAIF_BODY_CODE += ( -41020                   )

            NAIF_BODY_NAME += ( 'MEX_MELACOM_1'          )
            NAIF_BODY_CODE += ( -41031                   )

            NAIF_BODY_NAME += ( 'MEX_MELACOM_2'          )
            NAIF_BODY_CODE += ( -41032                   )

            NAIF_BODY_NAME += ( 'MEX_LGA'                )
            NAIF_BODY_CODE += ( -41040                   )

            NAIF_BODY_NAME += ( 'MEX_VMC'                )
            NAIF_BODY_CODE += ( -41050                   )

         \begintext


Mars Express Beagle-2 (BEAGLE2) Lander and its instruments IDs
--------------------------------------------------------------

   This table summarizes Beagle-2 lander and its instrument IDs:

            Name                   ID       Synonyms
            ---------------------  -------  -------------------------
            BEAGLE2                -44      BEAGLE 2, BEAGLE-2,
                                            BEAGLE_2
            BEAGLE2_LANDER         -44000
            BEAGLE2_GAP            -44100
            BEAGLE2_PAW            -44200
            BEAGLE2_LANDING_SITE   -44900   BEAGLE2_LS, BEAGLE2_SITE

   Notes:

      -- 'BEAGLE2', 'BEAGLE 2', 'BEAGLE-2', and 'BEAGLE_2' are
         synonyms and all map to the official Beagle-2 s/c ID (-44);

      -- 'BEAGLE2_LANDING_SITE', 'BEAGLE2_LS' and 'BEAGLE2_SITE' are
         synonyms and all map to Beagle-2 landing site ID (-44000);

   Name-ID Mapping keywords:

         \begindata

            NAIF_BODY_NAME += ( 'BEAGLE2'                 )
            NAIF_BODY_CODE += ( -44                       )

            NAIF_BODY_NAME += ( 'BEAGLE 2'                )
            NAIF_BODY_CODE += ( -44                       )

            NAIF_BODY_NAME += ( 'BEAGLE-2'                )
            NAIF_BODY_CODE += ( -44                       )

            NAIF_BODY_NAME += ( 'BEAGLE_2'                )
            NAIF_BODY_CODE += ( -44                       )

            NAIF_BODY_NAME += ( 'BEAGLE2_LANDER'          )
            NAIF_BODY_CODE += ( -44000                    )

            NAIF_BODY_NAME += ( 'BEAGLE2_GAP'             )
            NAIF_BODY_CODE += ( -44100                    )

            NAIF_BODY_NAME += ( 'BEAGLE2_PAW'             )
            NAIF_BODY_CODE += ( -44200                    )

            NAIF_BODY_NAME += ( 'BEAGLE2_LANDING_SITE'    )
            NAIF_BODY_CODE += ( -44900                    )

            NAIF_BODY_NAME += ( 'BEAGLE2_LS'              )
            NAIF_BODY_CODE += ( -44900                    )

            NAIF_BODY_NAME += ( 'BEAGLE2_SITE'            )
            NAIF_BODY_CODE += ( -44900                    )

         \begintext


