//
//  CustomSensorTestClass.hpp
//  
//
//  Created by Stark, Michael E. (GSFC-5850) on 4/18/17.
//
//

#ifndef CustomSensorTestClass_hpp
#define CustomSensorTestClass_hpp

#include <stdio.h>
#include "CustomSensor.hpp"

class CustomSensorTestClass: public CustomSensor
{
public:
   // constructor, call through to parent constructor
   CustomSensorTestClass (const Rvector &coneAngleVec,
                          const Rvector &clockAngleVec);
   
   // make rVector utilities visible
   void Sort(Rvector &v, bool Ascending = true);
   void Sort(Rvector &v, IntegerArray &indices, bool Ascending=true);
   Real Max(const Rvector &v);
   Real Min(const Rvector &v);
   
   // make coordinate conversion utilities visible
   void ConeClocktoRADEC(Real coneAngle, Real clockAngle,
                         Real &RA, Real &dec);
   Rvector3 RADECtoUnitVec(Real RA, Real dec);
   void UnitVecToStereographic(const Rvector3 &u,
                               Real &xCoord, Real &yCoord);
   void ConeClockToStereographic(Real coneAngle, Real clockAngle,
                                 Real &xCoord, Real &yCoord);
   void ConeClockArraysToStereographic(const Rvector &coneAngleVec,
                                       const Rvector &clockAngleVec,
                                       Rvector &xArray, Rvector &yArray);
   
   // make class hidden functions visible, pointsToSegments() and
   // computeExternalPoints() are tested in constructor
   //bool CheckTargetMaxExcursionCoordinates(Real xCoord, Real yCoord);
   
   // print functions here to show protected class state data
   void PrintConeAndClockAngles();
   void PrintStereographicProjections();
   void PrintSegmentArray();
   void PrintExternalPointArray();
   void PrintMinMaxExcursions();
   void PrintState();
};
   
#endif /* CustomSensorTestClass_hpp */