#!/usr/bin/env mpython

#=======================================================================
# Filename:     GmatFunctions.py
# Description:  A collection of GMAT API functions of general utility
#
# Date         Author               Notes
# -----------  -------------------  -----------------------------------
# 10/02/2020   Darrel J. Conway     Initial Version
#
#=======================================================================

#=======================================================================
# Load modules and set preferences
#=======================================================================

# Load the GMAT Modules
from load_gmat import *


# Calculates the time difference between two Gregorian epochs
#
# Inputs:
#
#    startTime  The first epoch
#    endTime    The second epoch
#
# Returns:
#    The time difference, in seconds
def FindTimeDifference(startTime, endTime):
   timeconverter = gmat.TimeSystemConverter.Instance()
   start = timeconverter.ConvertGregorianToMjd(startTime)
   end   = timeconverter.ConvertGregorianToMjd(endTime)

   return (end - start) * 86400.0


# Propagation for a fixed time, updating the spacecraft at the end
#
# Inputs:
#
#    sat        A GMAT Spacecraft object
#    prop       A GMAT Propagator
#    dt         The time step to take, in seconds
#
# Returns:
#    A boolean indicating success or failure
def PropForInterval(sat, prop, dt, showEnd = False, showStart = False):

   # Setup the spacecraft that is propagated
   prop.AddPropObject(sat)
   prop.PrepareInternals()

   # Refresh the 'gator reference
   gator = prop.GetPropagator()

   # Showing the starting state
   if showStart:
      print("T = 0:  ", gator.GetState())

   retval = gator.Step(dt)
   
   '''
   timeleft = dt
   deltat = 0.0

   while timeleft > 0.0:
      retval = gator.Step(timeleft)
      deltat = gator.GetStepTaken()
      timeleft -= deltat
      print(timeleft, "  ", deltat, gator.GetState())

   if showEnd:
      print("        ", gator.GetState())
   '''

   prop.GetODEModel().UpdateSpaceObject()
   return retval 


# Propagation to an apsides, updating the spacecraft at the end
#
# Inputs:
#
#    sat        A GMAT Spacecraft object
#    prop       A GMAT Propagator
#    dt         The time step to take, in seconds
#
# Returns:
#    A boolean indicating success or failure
def PropToApsides(sat, prop, showStates = False, showStart = False):

   # Setup the spacecraft that is propagated
   prop.AddPropObject(sat)
   prop.PrepareInternals()

   # Refresh the 'gator reference
   gator = prop.GetPropagator()

   state0 = gator.GetState()
   time = 0.0
   if showStart:
      print("T = ", time, ":  ", state0)

   dot0 = RdotV(state0)

   searching = True
   while searching:
      retval = gator.Step()
      state = gator.GetState()
      dot = RdotV(state)

      print(dot, dot0)

      if dot * dot0 <= 0.0:
         searching = False
         
         dt = TimeToStop(sat, prop, RdotV)
         gator.Step(dt)
         time += gator.GetStepTaken()

         if showStates:
            print("    ", time, ":  ", state)
         prop.GetODEModel().UpdateSpaceObject()

      else:
         time += gator.GetStepTaken()
         if showStates:
            print("    ", time, ":  ", state)
         prop.GetODEModel().UpdateSpaceObject()

   if showStates:
      print("TF  ", time, ":  ", state)

   return retval


# Computes R dot V from a state vector.  
# 
# This is a basic example of a function used for stopping conditions.  When R*V
# passes through zero, the spacecraft passes an apse.
#
# Inputs:
# 
#    state     The Cartesian state used for evaluation
#
# Returns:
#    The dot product between R and V  
def RdotV(state):
   return state[0] * state[3] + state[1] * state[4] + state[2] * state[5]  


# An API approach to stopping conditions
# 
# The API propagation calls stop on time.  Sometimes the user wants to stop on
# a different parameter.  This function provides that capability.
#
# Note: the current implementation uses linear interpolation to stop, so it 
# isn't very accurate.  The implementation would benefit from some iteration to
# improve the solution.
#
# Inputs:
# 
#    sat          The spacecraft that is being propagated.  The spacecraft state
#                 should be set so that a propagation step will bracket the 
#                 stopping point.
#    prop         The propagator used for the spacecraft
#    stopFunction A function used to locate the stopping point.  This function
#                 needs to evaluate to zero at the stopping point, must change 
#                 signs on either side of the point, and needs to be at least 
#                 continuous.  C1 continuity or better is preferred and may be
#                 required if the stopping algorithm is improved.
#
# Returns:
#    The time required to propagate to the stopping condition
def TimeToStop(sat, prop, stopFunction):
   gator = prop.GetPropagator()
   statef = gator.GetState()

   tf = gator.GetStepTaken()
   ti = 0.0
   yf = stopFunction(statef)

   prop.GetODEModel().UpdateFromSpaceObject()
   state = gator.GetState()
   yi =  stopFunction(state)

   stopt = - yi * (tf - ti) / (yf - yi)

   return tf # stopt
