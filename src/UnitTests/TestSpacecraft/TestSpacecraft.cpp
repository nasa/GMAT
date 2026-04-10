//------------------------------------------------------------------------------
//                              TestSpacecraft
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/01/16
//
/**
 * Unit test program for spaceraft.
 */
//------------------------------------------------------------------------------

#include "TestFunction.hpp"
#include "SpacePoint.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateSystemException.hpp"
#include "MJ2000EqAxes.hpp"
#include "SolarSystem.hpp"
#include "Planet.hpp"
#include <iomanip>

int main(int argc, char **argv)
{
   std::cout << "************************************************\n"
             << "*** Spacecraft Unit Test Program\n"
             << "************************************************\n\n";

   std::cout << std::setprecision(8);

   try
   {
      SolarSystem *solarSystem = new SolarSystem("MySolarSystem");
      std::cout << "\nSolarSystem's name: " << solarSystem->GetName();

      Planet *earth = (Planet *)solarSystem->GetBody(SolarSystem::EARTH_NAME);
      std::cout << "\nEarth's name: " << earth->GetName()
                << ", and type: " << earth->GetTypeName() << std::endl;

      SpacePoint *spacePoint = NULL;
      spacePoint = earth;

      earth->SetJ2000BodyName("Earth");
      earth->SetJ2000Body(spacePoint);

      std::cout << "\nCreating MJ2000EqAxes...\n";
      MJ2000EqAxes *m2000eq = new MJ2000EqAxes("SC_MJ2000Eq");

      std::cout << "\nCreating Coordinate System...\n";
      CoordinateSystem *mj2000 = new CoordinateSystem("","CS_MJ2000");
      std::cout << "\n---- nbr of objects instantiated = " 
                << GmatBase::GetInstanceCount() << std::endl; 

      std::cout << "\nSetting mj2000's solar system ...\n";
      mj2000->SetSolarSystem(solarSystem);

      std::cout << "\nSetting mj2000's Origin to \"Earth\"...\n";
//      mj2000->SetStringParameter("OriginName","Earth");
      mj2000->SetStringParameter("Origin","Earth");

      std::cout << "\nSetting mj2000's J2000Body to \"MJ2000\"...\n";
//      mj2000->SetStringParameter("J2000BodyName","MJ2000"); 
      mj2000->SetStringParameter("J2000Body","MJ2000"); 

      std::cout << "\nSetting SetRefObject for Coordinate System...\n";
      std::cout << "\nm2000eq->GetName() = " << m2000eq->GetName() << std::endl;
      if (!mj2000->SetRefObject(earth,Gmat::SPACE_POINT,"Earth") ||
          !mj2000->SetRefObject(spacePoint,Gmat::SPACE_POINT,"MJ2000") ||
          !mj2000->SetRefObject(m2000eq,Gmat::AXIS_SYSTEM,m2000eq->GetName()))
      {
         std::cout << "\nError: failure of setting coordinate system\n";
         return -1;
      } 

      std::cout << "\nInitializing  Coordinate System...\n";
      mj2000->Initialize();

//      mj2000 = NULL;
      // Create Spacecraft
      std::cout << "\nStarting to create Spacecraft...\n";
      Spacecraft *sc = new Spacecraft();

      std::cout << "\nSetting Spacecraft's coordinate system...\n";
      sc->SetRefObject(mj2000,Gmat::COORDINATE_SYSTEM,"CoordinateSysName");

      
      std::cout << "\nInitializing Spacecraft ...\n";
      sc->Initialize();

      MessageInterface::ShowMessage(
                        "\nGetRefObjectName = %s\n"
                        "\nGetStringParameter(CoordinateSystem) = %s\n",
                        sc->GetRefObjectName(Gmat::COORDINATE_SYSTEM).c_str(),
                        sc->GetStringParameter("CoordinateSystem").c_str());

      MessageInterface::ShowMessage(
                        "\nGetParamCount = %d\n", sc->GetParameterCount());

      for (Integer i=0; i < sc->GetParameterCount(); i++)
          MessageInterface::ShowMessage(
                            "\n%d = %s\n", i, sc->GetParameterText(i).c_str());

      MessageInterface::ShowMessage("\n\nChanging StateType to Keplerian\n");
      sc->SetStringParameter("StateType","Keplerian");
      for (Integer i=0; i < sc->GetParameterCount(); i++)
          MessageInterface::ShowMessage(
                            "\n%d = %s\n", i, sc->GetParameterText(i).c_str());
 
      MessageInterface::ShowMessage("\n\nAfter Copy constructorn");
      // Spacecraft *newSC( = (Spacecraft*)sc->Clone();
      Spacecraft *newSC(sc);
      for (Integer i=0; i < newSC->GetParameterCount(); i++)
          MessageInterface::ShowMessage("\n%d = %s\n", i, 
                                        newSC->GetParameterText(i).c_str());

      PropState propState = sc->GetState();
      Real* instate = propState.GetState();
      printState("Refreshing after GetState(Real*)", instate);
      printState("Refreshing after GetState(Spacecraft *sc)",sc);
      
      Rvector6 newStateVector = sc->GetStateVector();
      printState("Here's newStateVector...", newStateVector);
   

      MessageInterface::ShowMessage("\nGetting SMA: %f\n",
                            sc->GetRealParameter("SMA"));

      MessageInterface::ShowMessage("\nGetting RadPer: %f\n",
                            sc->GetRealParameter("RadPer"));
      MessageInterface::ShowMessage("\nGetting X: %f\n",
                            sc->GetRealParameter("X"));

      MessageInterface::ShowMessage("\nGetting TA: %f\n",
                            sc->GetRealParameter("TA"));
      MessageInterface::ShowMessage("\nGetting MA: %f\n",
                            sc->GetRealParameter("MA"));
      MessageInterface::ShowMessage("\nGetting EA: %f\n",
                            sc->GetRealParameter("EA"));

      MessageInterface::ShowMessage("\nGetting Text from 7: %s\n",
                            sc->GetParameterText(7).c_str());


      sc->SetStringParameter("StateType","Keplerian");
      MessageInterface::ShowMessage("\nGetting Text from 7 after "
                                    "stateTtype changed: %s and value: %f\n",
                            sc->GetParameterText(7).c_str(),
                            sc->GetRealParameter(7));

      Rvector6 stateVector = sc->GetStateVector("Cartesian"); 
      printState("StateVector in Cartesian",stateVector);

      MessageInterface::ShowMessage("\nGetting state:%s and anomaly(%d): "
                        "%s -> %f\n",
                        sc->GetStringParameter("StateType").c_str(),
                        sc->GetParameterID("AnomalyType"),
                        sc->GetStringParameter("AnomalyType").c_str(),
                        sc->GetRealParameter(sc->GetStringParameter(
                                             "AnomalyType")));

      sc->SetStringParameter("AnomalyType","EA");
      MessageInterface::ShowMessage("\nAfter setting new anomaly type,  "
                        "state: %s and anomaly(%d): %s -> %f\n",
                        sc->GetStringParameter("StateType").c_str(),
                        sc->GetParameterID("AnomalyType"),
                        sc->GetStringParameter("AnomalyType").c_str(),
                        sc->GetRealParameter(sc->GetStringParameter(
                                             "AnomalyType")));

      MessageInterface::ShowMessage("\nGetRealParameter(\"EA\"): %f and (\"TA\"): %f\n",
                                    sc->GetRealParameter("EA"), 
                                    sc->GetRealParameter("TA"));

      printState("StateVector in Keplerian", sc->GetStateVector("Keplerian"));

   
#if 0    // just testing for settting value
      sc->SetRealParameter("SMA",8000.0);
      sc->SetRealParameter("ECC",8000.0);
      sc->SetRealParameter("X",7321.0);
      MessageInterface::ShowMessage("\nGetting W: %f\n",
                                    sc->GetRealParameter("W"));
#endif

      printEpoch(sc);

      MessageInterface::ShowMessage("\nSetting 21546.0 to \"Epoch\"....\n");
      sc->SetStringParameter("Epoch","21546.0");
      printEpoch(sc);
   
      MessageInterface::ShowMessage("\nGetEpoch(): %f\n",sc->GetEpoch());
      
      MessageInterface::ShowMessage("\nSetting 21544.999629232 to "
                                    "\"Epoch\".UTCModJulian\"....\n");
      sc->SetStringParameter("Epoch.UTCModJulian","21544.999629232");
      printEpoch(sc);


      MessageInterface::ShowMessage("\nSetEpoch(21550.0) ---------->\n");
      sc->SetEpoch(21550.0);
      printEpoch(sc);

      MessageInterface::ShowMessage("\nSetEpoch(21545.0)\n");
      sc->SetEpoch(21545.0);
      printEpoch(sc);
      MessageInterface::ShowMessage("\nGetEpoch()-> %f\n",sc->GetEpoch());


      return 0;

      sc->SetStringParameter("Epoch.UTCGregorian","01 Jan 2000 11:59:27.966");
//      sc->SetStringParameter("Epoch","UTCGregorian");
      MessageInterface::ShowMessage(
                        "\nsc->GetStringParameter(\"Epoch\"): %s\n",
                        sc->GetStringParameter("Epoch").c_str());

      MessageInterface::ShowMessage(
                        "\nsc->GetParameterText(1): %s\n",
                        sc->GetParameterText(1).c_str());

      MessageInterface::ShowMessage(
                        "\nsc->GetStringParameter(\"TAIModJulian\"): %s\n",
                        sc->GetStringParameter("TAIModJulian").c_str());

      return 0;

      printState("StateVector in Keplerian", sc->GetStateVector("Keplerian"));
//       printState("StateVector in Cartesian",stateVector);

#if 0
      std::cout << "\nEpoch: " 
                << sc->GetRealParameter("Epoch") << std::endl;

      sc->SetRealParameter("Epoch",22800.589);
      std::cout << "\nEpoch after setting: " 
                << sc->GetRealParameter("Epoch") << std::endl;

      std::cout << "\nGet SMA: " << sc->GetRealParameter("SMA") << std::endl;
#endif

#if 0 
   std::cout << "\nDisplayCoordType: " << sc->GetDisplayCoordType(); 
   printState("Here it goes...", sc->GetDisplayState());

   std::cout << "\nChange to Keplerian...\n";
   sc->SetDisplayCoordType("Keplerian");
   sc->SaveDisplay();
   std::cout << "\nDisplayCoordType: " << sc->GetDisplayCoordType(); 
   printState("Here it goes...", sc->GetDisplayState());
                        
   std::cout << "\nGetting Anomaly type-> "
             << sc->GetStringParameter("AnomalyType")
             << ": " 
             << sc->GetRealParameter(sc->GetStringParameter("AnomalyType"))
             << std::endl;

   sc->SetRealParameter(sc->GetStringParameter("AnomalyType"),99.5);
   std::cout << "\nAfter Setting Anomaly value-> "
             << sc->GetStringParameter("AnomalyType")
             << ": " 
             << sc->GetRealParameter(sc->GetStringParameter("AnomalyType"))
             << std::endl;
   printState("Here it goes...", sc->GetDisplayState());

   std::cout << "\nChange back to Cartesian...\n";
   sc->SetDisplayCoordType("Cartesian");
   sc->SaveDisplay();
   std::cout << "\nDisplayCoordType: " << sc->GetDisplayCoordType(); 
   printState("Here it goes...", sc->GetDisplayState());

   std::cout << "\nChange to MA...\n";
   sc->SetStringParameter("AnomalyType","MA");
   printState("Here it goes...", sc->GetDisplayState());


   std::cout << "\nChange to EA...\n";
   sc->SetStringParameter("AnomalyType","EA");
   std::cout << "\nGetting Anomaly type-> "
             << sc->GetStringParameter("AnomalyType")
             << ": " 
             << sc->GetRealParameter(sc->GetStringParameter("AnomalyType"))
             << std::endl;
   printState("Here it goes...", sc->GetDisplayState());

   std::cout << "\nChange back to Keplerian again...\n";
   sc->SetDisplayCoordType("Keplerian");
   sc->SaveDisplay();
   std::cout << "\nDisplayCoordType: " << sc->GetDisplayCoordType(); 
   printState("Here it goes...", sc->GetDisplayState());


   std::cout << "\nChange back to TA...\n";
   sc->SetStringParameter("AnomalyType","TA");
   printState("Here it goes...", sc->GetDisplayState());

   sc->SetRealParameter(sc->GetStringParameter("AnomalyType"),99.887749332048);
   std::cout << "\nAfter Setting Anomaly value-> "
             << sc->GetStringParameter("AnomalyType")
             << ": " 
             << sc->GetRealParameter(sc->GetStringParameter("AnomalyType"))
             << std::endl;
   printState("Here it goes...", sc->GetDisplayState());

   std::cout << "\nChange back to Cartesian for final...\n";
   sc->SetDisplayCoordType("Cartesian");
   sc->SaveDisplay();
   std::cout << "\nDisplayCoordType: " << sc->GetDisplayCoordType(); 
   printState("Here it goes...", sc->GetDisplayState());

   instate = propState.GetState();
   printState("Go back to Internal Cartesian", instate);
#endif

#if 0    // hold this below for debug..
   sc->SetStringParameter(sc->GetParameterID("StateType"),"SphericalAZFPA");
   sc->SetRealParameter("RMAG",11456.5716205502);
   sc->SetRealParameter("RA",46.44641685679);
   sc->SetRealParameter("DEC",34.2529104782205);
   sc->SetRealParameter("VMAG",7.65188771328657);
   sc->SetRealParameter("AZI",177.421725219839);
   sc->SetRealParameter("FPA",49.2586292443246);
  
   std::cout << "\nAfter setting new data, StateType: " 
             << sc->GetStringParameter(sc->GetParameterID("StateType")); 
   std::cout << "\nRMAG: " << sc->GetRealParameter("RMAG"); 
   std::cout << "\nRA: " << sc->GetRealParameter("RA"); 
   std::cout << "\nDEC: " << sc->GetRealParameter("DEC"); 
   std::cout << "\nVMAG: " << sc->GetRealParameter("VMAG"); 
   std::cout << "\nAZI: " << sc->GetRealParameter("AZI"); 
   std::cout << "\nFPA: " << sc->GetRealParameter("FPA") << std::endl; 
   
   std::cout << "\nDisplayCoordType: " << sc->GetDisplayCoordType(); 
   printState("Here it goes...", sc->GetDisplayState());

#endif   

#if 0  // for Keplerian state  
   std::cout << "\nSetting Keplerian values";
   // sc->SetStringParameter(sc->GetParameterID("StateType"),"Keplerian");
   sc->SetStringParameter("StateType","Keplerian");
   sc->SetStringParameter("AnomalyType","MA");
   sc->SetRealParameter("SMA",36127.3377639748);
   sc->SetRealParameter("ECC",0.832853399083688);
   sc->SetRealParameter("INC",87.8691261770264);
   sc->SetRealParameter("RAAN",227.898260357274);
   sc->SetRealParameter("AOP",53.3849306701938);
   sc->SetRealParameter("TA",92.3351567104033);

   std::cout << "\nAfter setting new data, StateType: " 
             << sc->GetStringParameter(sc->GetParameterID("StateType")); 
   printState("\nAfter setting new data",sc);

//   std::cout << "\nSMA: " << sc->GetRealParameter("SMA"); 
//   std::cout << "\nECC: " << sc->GetRealParameter("ECC"); 
//   std::cout << "\nINC: " << sc->GetRealParameter("INC"); 
//   std::cout << "\nRAAN: " << sc->GetRealParameter("RAAN"); 
//   std::cout << "\nAOP: " << sc->GetRealParameter("AOP"); 
//   std::cout << "\nTA: " << sc->GetRealParameter("TA") << std::endl; 

   std::cout << "\nDisplayCoordType: " << sc->GetDisplayCoordType(); 
//   sc->SetDisplayCoordType("Keplerian");
   printState("Here it goes...", sc->GetDisplayState());
   instate = sc->GetDisplayState();
   printState("Here it goes again...", instate);

   std::cout << "GetRealParameter(\"SMA\"): " << sc->GetRealParameter("SMA") 
             << std::endl;

#endif
#if 0 // for Modified Keplerian
   std::cout << "\nSetting Modified Keplerian values";
   sc->SetStringParameter("StateType","ModifiedKeplerian");
//   sc->SetStringParameter("AnomalyType","TA");
   sc->SetRealParameter("RadPer",6038.56170740389);
   sc->SetRealParameter("RadApo",66216.1138205456);
   sc->SetRealParameter("INC",87.8691261770264);
   sc->SetRealParameter("RAAN",227.898260357274);
   sc->SetRealParameter("AOP",53.3849306701938);
   sc->SetRealParameter("TA",92.3351567104033);

   std::string elementName[] = {"Element1","Element2","Element3",
                              "Element4","Element5","Element6"};

   for (Integer i=0; i < 6; i++)
   {
      Integer id = sc->GetParameterID(elementName[i]);
      std::cout << "\nElement" << (i+1) << ": " 
                << sc->GetParameterText(id);
   }
   std::cout << std::endl;

   std::cout << "\nDisplayCoordType: " << sc->GetDisplayCoordType(); 
   printState("Here it goes...", sc->GetDisplayState());
   instate = sc->GetDisplayState();
   printState("Here it goes again...", instate);


#endif

   }
   catch(SpaceObjectException &soe)
   {
      std::cout << "\n" << soe.GetMessage() << std::endl;
   }
   catch(CoordinateSystemException &cse)
   {
      std::cout << "\n" << cse.GetMessage() << std::endl;
   }

   catch(GmatBaseException &bge)
   {
      std::cout << "\n" << bge.GetMessage() << std::endl;
   }
   return 0;
}
