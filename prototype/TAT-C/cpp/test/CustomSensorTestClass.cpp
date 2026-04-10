//
//  CustomSensorTestClass.cpp
//  
//
//  Created by Stark, Michael E. (GSFC-5850) on 4/18/17.
//
//

#include "CustomSensorTestClass.hpp"
#include "MessageInterface.hpp"

// call superclass constructor
CustomSensorTestClass::CustomSensorTestClass(const Rvector &coneAngleVec,
                                             const Rvector &clockAngleVec)
: CustomSensor(coneAngleVec,clockAngleVec)

{
   MessageInterface::ShowMessage("successful call to parent constructor\n\n");
}

//------------------------------------------------------------------------------
// rVector utilities
//------------------------------------------------------------------------------
void CustomSensorTestClass::Sort(Rvector &v, bool Ascending)
{
   CustomSensor::Sort(v,Ascending);
}

//------------------------------------------------------------------------------
void CustomSensorTestClass::Sort(Rvector &v,
                                 IntegerArray &indices, bool Ascending)
{
   CustomSensor::Sort(v, indices, Ascending);
}

//------------------------------------------------------------------------------
Real CustomSensorTestClass::Max(const Rvector &v)
{
   return CustomSensor::Max(v);
}

//------------------------------------------------------------------------------
Real CustomSensorTestClass::Min(const Rvector &v)
{
   return CustomSensor::Min(v);
}

//------------------------------------------------------------------------------
// end rVector utilities

//------------------------------------------------------------------------------
// coordinate conversion utilities
//------------------------------------------------------------------------------
void CustomSensorTestClass::ConeClocktoRADEC(Real coneAngle, Real clockAngle,
                      Real &RA, Real &dec)
{
   CustomSensor::ConeClocktoRADEC(coneAngle,clockAngle,RA,dec);
}

//------------------------------------------------------------------------------
Rvector3 CustomSensorTestClass::RADECtoUnitVec(Real RA, Real dec)
{
   return CustomSensor::RADECtoUnitVec(RA,dec);
}

//------------------------------------------------------------------------------
void CustomSensorTestClass::UnitVecToStereographic(const Rvector3 &u,
                            Real &xCoord, Real &yCoord)
{
   CustomSensor::UnitVecToStereographic(u,xCoord,yCoord);
}

//------------------------------------------------------------------------------
void CustomSensorTestClass::ConeClockToStereographic(Real coneAngle, Real clockAngle,
                              Real &xCoord, Real &yCoord)
{
   CustomSensor::ConeClockToStereographic(coneAngle,clockAngle,xCoord,yCoord);
}

//------------------------------------------------------------------------------
void CustomSensorTestClass::ConeClockArraysToStereographic
   (const Rvector &coneAngleVec, const Rvector &clockAngleVec,
                                 Rvector &xArray, Rvector &yArray)
{
   CustomSensor::ConeClockArraysToStereographic(coneAngleVec, clockAngleVec,
                                                xArray, yArray);
}

//------------------------------------------------------------------------------
// end coordinate conversion utilities

//------------------------------------------------------------------------------
// bounds checking
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// end bounds checking

//------------------------------------------------------------------------------
// functions to print member data, must invoke constructor first
//------------------------------------------------------------------------------
void CustomSensorTestClass::PrintConeAndClockAngles()
{
   MessageInterface::ShowMessage("%d points define the FOV\n",numFOVPoints);
   MessageInterface::ShowMessage("Stored Cone & Clock Angles\n");
   for (int i=0; i<numFOVPoints; i++)
      MessageInterface::ShowMessage("Point %2d = (%13.10f %13.10f)\n",
                                    i+1,
                                    this->coneAngleVec[i],
                                    this->clockAngleVec[i]);
   MessageInterface::ShowMessage("\n");
}

//------------------------------------------------------------------------------
void CustomSensorTestClass::PrintStereographicProjections()
{
   MessageInterface::ShowMessage(
      "Stereographic Projection of Cone & Clock angles\n");
   for (int i=0; i<numFOVPoints; i++)
      MessageInterface::ShowMessage("Point %2d = (%15.10f %15.10f)\n",
                                    i+1,
                                    this->xProjectionCoordArray[i],
                                    this->yProjectionCoordArray[i]);
   MessageInterface::ShowMessage("\n");
}

//------------------------------------------------------------------------------
void CustomSensorTestClass::PrintSegmentArray()
{
   MessageInterface::ShowMessage("FOV Defining Line Segments\n");
   for (int i=0; i<numFOVPoints; i++)
      MessageInterface::ShowMessage(
         "Segment %2d start = (%12.9f %12.9f), end=(%12.9f, %12.9f)\n",
                                    i+1,
                                    this->segmentArray.GetElement(i,0),
                                    this->segmentArray.GetElement(i,1),
                                    this->segmentArray.GetElement(i,2),
                                    this->segmentArray.GetElement(i,3) );
                                    
   MessageInterface::ShowMessage("\n");
}

//------------------------------------------------------------------------------
void CustomSensorTestClass::PrintExternalPointArray()
{
   MessageInterface::ShowMessage("ExternalPointArray\n");
   Integer nRows = this->externalPointArray.GetNumRows();
   for (int i=0;i<nRows; i++)
      MessageInterface::ShowMessage("External Point %2d = (%15.10f %15.10f)\n",
                                    i+1,
                                    this->externalPointArray.GetElement(i,0),
                                    this->externalPointArray.GetElement(i,1));
   
   MessageInterface::ShowMessage("\n");
}

//------------------------------------------------------------------------------
void CustomSensorTestClass::PrintMinMaxExcursions()
{
   
}

//------------------------------------------------------------------------------
void CustomSensorTestClass::PrintState()
{
   // PrintMaxExcursions();
   PrintConeAndClockAngles();
   PrintStereographicProjections();
   PrintSegmentArray();
   PrintExternalPointArray();
   
}
//------------------------------------------------------------------------------
// end print functions

// end CustomSensorTestClass