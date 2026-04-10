#!/usr/python3

# Script used to parse a TLE file and construct a GMAT include file for a 
# collection of spacecraft, grouped in a formation

def FindSatellites(searchString, tleFile):
   sats = []
   sats.append("STARLINK-31")             
   sats.append("STARLINK-22")             
   sats.append("STARLINK-23")             
   sats.append("STARLINK-24")             
   sats.append("STARLINK-25")             
   sats.append("STARLINK-26")             
   sats.append("STARLINK-27")             
   sats.append("STARLINK-28")             
   sats.append("STARLINK-29")             
   sats.append("STARLINK-30")

   return sats

def WriteIncludeFile(filename, sats, epoch, tleFile):
   print("Writing to file ", filename)

   for i in range(len(sats)):
      print ("Create Spacecraft ", sats[i], ";", sep='')
      print (sats[i], ".DateFormat = UTCGregorian;", sep='')
      print (sats[i], ".Epoch = '", epoch, "';", sep='');
      print (sats[i], ".EphemerisName = '", tleFile, "';", sep='')
      print (sats[i], ".Id = '", sats[i], "';\n", sep='')

   print ("Create Formation SatForm")
   for i in range(len(sats)):
      print ("SatForm.Add = ", sats[i], sep='')

   print ("\nCreate Propagator tleProp;")
   print ("tleProp.Type = TLE;")

   print ("\nBeginMissionSequence")
   print ("Propagate tleProp(SatForm) {", sats[0], ".ElapsedDays = 1.0;", sep = '')


tleFile = "Active.txt"
sats = FindSatellites("Starlink", tleFile)
WriteIncludeFile("Starlink.formation", sats, "28 Nov 2019 12:00:00.000", tleFile)

