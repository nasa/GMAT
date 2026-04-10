#!/usr/bin/env mpython

#=======================================================================
# Filename:     GmatMonte.py
# Description:  A collection of functions used to connect GMAT and Monte 
#
# Date         Author               Notes
# -----------  -------------------  -----------------------------------
# 10/08/2020   Darrel J. Conway     Initial Version
#=======================================================================

#=======================================================================
# Load modules and set preferences
#=======================================================================

# Load a basic set of Monte modules to use 
import Monte as M
import mpy.io.data as defaultData
import mpy.traj.force.grav.basic as basicGrav
from mpy.units import *
import cristo

# Load the GMAT Modules
from load_gmat import *

# Python imports for capturing console output
from contextlib import redirect_stdout
import io


# Convert a GMAT VNB impulsive burn to a Monte BVN impulsive burn
#
# Inputs:
#
#   gmatBurn   The GMAT burn
#   epoch      The burn epoch, in UTC Gregorian format
#   theBoa     The Monte binary object archive receiving the burn
#   satName    The Monte string associated with the spacecraft that maneuvers
#
# Outputs:
#   A Monte impulsive burn set to match the GMAT burn
def G2M_VnbBurn(gmatBurn, utcEpoch, theBoa, satName):
   burnName = gmatBurn.GetName() + "_gmat"

   # Define Monte frame for the burn
   frameName = burnName + " Frame"

   # Define the VNB frame - for Monte, this is a BVN frame
   origin    = gmatBurn.GetField("Origin")
   if origin == "Luna":
      orign = "Moon"

   M.BodyVelDirFrame( theBoa, frameName, "EME2000", M.TimeInterval(),
                    satName, origin )

   isp       = gmatBurn.GetNumber("Isp") * sec
   burnEpoch = M.Epoch(utcEpoch + " UTC")
   burnDV    = M.Unit3Vec( ( gmatBurn.GetNumber("Element3") * km/s, 
                           gmatBurn.GetNumber("Element1") * km/s, 
                           gmatBurn.GetNumber("Element2") * km/s ) )
   monteBurn = M.ImpulseBurn(burnName, burnEpoch, frameName, burnDV, isp)

   return monteBurn


# Convert a Monte BVN impulsive burn to a GMAT VNB impulsive burn
#
# Inputs:
#
#   monteBurn  The Monte burn
#   boa        The binary object archive for the Monte burn
#   gmatTank   Name of a GMAT tank for mass depletion; empty for no mass flow
#
# Outputs:
#   A GMAT impulsive burn set to match the Monte burn
def M2G_BvnBurn(monteBurn, boa, gmatTank = ""):
   gmatBurn = gmat.Construct("ImpulsiveBurn", monteBurn.name() + "_monte")

   # Retrieve needed data from the burn
   deltaV    = monteBurn.dvel()

   # Dig into the boa to find the origin for the burn
   theframe = M.CoordFrameBoa.getFrames(boa, monteBurn.frame())
   origin = theframe[0].center()

   gmatBurn.SetField("CoordinateSystem", "Local")
   # ToDo: Pick up the origin setting from the Monte frame
   gmatBurn.SetField("Origin", origin)
   gmatBurn.SetField("Axes", "VNB")

   if gmatTank != "":
      gmatBurn.SetField("DecrementMass", True)
      gmatBurn.SetField("GravitationalAccel", 9.81)
      gmatBurn.SetField("Tank", gmatTank)
      gmatBurn.SetField("Isp", monteBurn.isp().value())

   gmatBurn.SetField("Element1", deltaV[1])
   gmatBurn.SetField("Element2", deltaV[2])
   gmatBurn.SetField("Element3", deltaV[0])

   return gmatBurn


# Impulsive buns in Monte have an associated epoch.  This function retrieves 
# that epoch in a GMAT UTCGregorian string.
#
# Inputs:
#
#    boa         The BOA that has the burn.  Note that propagation needs to have 
#                occurred.
#    burnName    Name of the Monte impulsive burn
#
# Outputs:
#    A GMAT compatible UTC Gregorian epoch
#
# ToDo:  Check the return string for required leading zeros, etc
def M2G_BurnEpoch(boa, burnName):
   events = CaptureOutput(cristo.events(boa))

   # todo: Write parser for the events to find the epoch for burnName
   # print(events)  

   return "02 Oct 2020 18:09:30.927"  #epoch.format()


# Function used to set a GMAT spacecraft state using a Monte state
#
# Inputs:
#
#    monteState     The Monte State object containing the state data
#    monteEpoch     Epoch of the state data
#    gmatSpacecraft A GMAT Spacecraft object that receives the data
#
# Outputs:
#    None.  On return, the Spacecraft data has changed.
def M2G_CopySpacecraft(monteState, monteEpoch, gmatSpacecraft):
   monteEpoch.setFormat("$DD $Mon $YYYY $HR:$MN:$SC(###)$UTC")
   gmatEpoch = monteEpoch.format()
   origin = monteState.center()
   pos = monteState.pos()
   vel = monteState.vel()

   if (origin != "Earth"):
      raise Exception("M2G_CopySpacecraft() is only implemented for Earth centered states for now")

   # Fill in the GMAT settings
   gmatSpacecraft.SetField("DateFormat","UTCGregorian")
   gmatSpacecraft.SetField("CoordinateSystem","EarthMJ2000Eq")
   gmatSpacecraft.SetField("DisplayStateType","Cartesian")

   # Set the state data
   gmatSpacecraft.SetField("Epoch", gmatEpoch)
   gmatSpacecraft.SetField("X", pos[0])
   gmatSpacecraft.SetField("Y", pos[1])
   gmatSpacecraft.SetField("Z", pos[2])
   gmatSpacecraft.SetField("VX", vel[0])
   gmatSpacecraft.SetField("VY", vel[1])
   gmatSpacecraft.SetField("VZ", vel[2])


# Function used to capture output from stdout
#
# Inputs:
#
#    thecall     The command that generates the output
#
# Outputs:
#    The captured output
def CaptureOutput(thecall):
   f = io.StringIO()
   with redirect_stdout(f):
       thecall
   return f.getvalue()
