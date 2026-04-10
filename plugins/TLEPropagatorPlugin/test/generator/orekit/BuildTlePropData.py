#!/opt/anaconda/bin/python3

# Import the code we use
import orekit
orekit.initVM()
from orekit.pyhelpers import setup_orekit_curdir
setup_orekit_curdir()

from org.orekit.data import DataProvidersManager, ZipJarCrawler
from org.orekit.frames import FramesFactory, TopocentricFrame
from org.orekit.bodies import OneAxisEllipsoid, GeodeticPoint
from org.orekit.time import TimeScalesFactory, AbsoluteDate, DateComponents, TimeComponents
from org.orekit.utils import IERSConventions, Constants
from org.orekit.propagation.analytical.tle import TLE, TLEPropagator
from java.io import File
from math import radians, pi

def GetTleData(fromFile, forSpacecraft):
   tleLines = []
   tleLines.append("ExampleSat")
   tleLines.append("1 27421U 02021A   02124.48976499 -.00021470  00000-0 -89879-2 0    20")
   tleLines.append("2 27421  98.7490 199.5121 0001333 133.9522 226.1918 14.26113993    62")

   return tleLines


lines = GetTleData("SomeFile.txt", "forSpacecraft")

mytle = TLE(lines[1],lines[2])
print (mytle)
print ('Epoch :',mytle.getDate())

ITRF = FramesFactory.getITRF(IERSConventions.IERS_2010, True)
earth = OneAxisEllipsoid(Constants.WGS84_EARTH_EQUATORIAL_RADIUS, 
                         Constants.WGS84_EARTH_FLATTENING, 
                         ITRF)

#longitude = radians(21.063)
#latitude  = radians(67.878)
#altitude  = 341.0
#station1 = GeodeticPoint(latitude, longitude, altitude)
#sta1Frame = TopocentricFrame(earth, station1, "Esrange")

initialDate = AbsoluteDate(DateComponents(2002, 5, 7),
                           TimeComponents.H00,
                           TimeScalesFactory.getUTC())

inertialFrame = FramesFactory.getEME2000()

propagator = TLEPropagator.selectExtrapolator(mytle)

extrapDate = initialDate;
finalDate = extrapDate.shiftedBy(86400.0) #seconds

epoch  = []
posvel = []

while (extrapDate.compareTo(finalDate) <= 0.0):  
    pv = propagator.getPVCoordinates(extrapDate, inertialFrame)
    pos_tmp = pv.getPosition()
    vel_tmp = pv.getVelocity()

    epoch.append(extrapDate.toString())
    posvel.append((pos_tmp.getX()/1000.0,pos_tmp.getY()/1000.0,pos_tmp.getZ()/1000.0,
                   vel_tmp.getX()/1000.0,vel_tmp.getY()/1000.0,vel_tmp.getZ()/1000.0))
    
    extrapDate = extrapDate.shiftedBy(300.0)

for i in range(len(epoch)):
    print(epoch[i], posvel[i])
