//------------------------------------------------------------------------------
//                           TestOptCtrl
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Claire R. Conway, Thinking Systems, Inc.
/**
 * INSTRUCTIONS FOR ADDING A NEW DRIVER
 *  - For each of the following steps, replace "Shell" with the name of your
 *      new driver
 *  - Add the following line to the include statements in testcases.hpp:
 *        #include "ShellDriver.hpp"
 *  - Add the following line to msg, numbered appropriately:
 *        msg += "#.  Shell\n";
 *  - Add the following lines to the "all" option, where appropriate:
 *        ShellDriver *driver# = new ShellDriver();
 *        driver#->Run();
 *        delete driver#;
 *  - Copy an existing driver from the numbered driver options, and modify it
 *      to work with your new driver.
 */
//------------------------------------------------------------------------------

#include "TestOptCtrl.hpp"
#include "csalt.hpp"
#include "testcases.hpp"
#include <ctime>
#include <chrono>
#include "StateConversionUtil.hpp"

//#define AUTONGC_TEST_CASE
#ifdef AUTONGC_TEST_CASE
   #include "AutoNGCDriver.hpp"
#endif

/**
 * Test case driver program for CSALT test cases
 */
int main(int argc, char *argv[])
{
	std::string msg = "";
	std::string outputDirectory = "../test/run/";
	std::string test = "";
	std::string inputString;
	std::string::size_type sz;
	int input;
	bool testing = true;

	CsaltTestDriver *driver = NULL;

	//std::cout << "argc: " << argc << "   argv[1]: " << argv[1] << "\n";

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			std::string argument = argv[i];
			if (argument == "-run")
			{
				//make sure i != argc - 1; that'd be an error
				if (i == (argc - 1))
				{
					std::cout << "Error: No test specified for -run argument."
						<< std::endl;
					return 1;
				}
				test = argv[i + 1];
				i++;

				std::cout << "Running '" << test << "'\n";

				if (test == "All")
				{
					std::clock_t starttime = std::clock();

					BrachistochroneDriver                *driver1 = new BrachistochroneDriver();
					BrachistochroneDriver				 *driver101 = new BrachistochroneDriver("BrachistochroneDefaultSnoptPrint", 1); // manually tell to print default snopt display to console
					BrachistochroneDriver				 *driver102 = new BrachistochroneDriver("BrachistochroneNoSnoptPrint", 0); // silence snopt display to console
					BrachistochroneDriver				 *driver103 = new BrachistochroneDriver("BrachistochroneInvalidSnoptPrint", -5); // invalid input that should be changed to 1
					HyperSensitiveDriver                 *driver2 = new HyperSensitiveDriver();
					RayleighDriver                       *driver3 = new RayleighDriver();
					RayleighControlStateConstraintDriver *driver4 = new RayleighControlStateConstraintDriver();
					ConwayOrbitExampleDriver             *driver5 = new ConwayOrbitExampleDriver();
					ConwayOrbitExampleRKDriver           *driver6 = new ConwayOrbitExampleRKDriver();
					SchwartzDriver                       *driver7 = new SchwartzDriver();
					MoonLanderDriver                     *driver8 = new MoonLanderDriver();
					GoddardRocketDriver                  *driver9 = new GoddardRocketDriver();
					GoddardRocketThreePhaseDriver        *driver10 = new GoddardRocketThreePhaseDriver();
					RauAutomaticaDriver                  *driver11 = new RauAutomaticaDriver();
					BrysonDenhamDriver                   *driver12 = new BrysonDenhamDriver();
					BrysonMaxRangeDriver                 *driver13 = new BrysonMaxRangeDriver();
					InteriorPointDriver                  *driver14 = new InteriorPointDriver();
					LinearTangentSteeringDriver          *driver15 = new LinearTangentSteeringDriver();
					ObstacleAvoidanceDriver              *driver16 = new ObstacleAvoidanceDriver();
					Hull95Driver                         *driver17 = new Hull95Driver();
					BangBangDriver                       *driver18 = new BangBangDriver();
					CatalyticGasOilCrackerDriver         *driver19 = new CatalyticGasOilCrackerDriver();
					LinearTangentSteeringStaticVarDriver *driver20 = new LinearTangentSteeringStaticVarDriver();
					HohmannTransferDriver                *driver21 = new HohmannTransferDriver();
					TutorialDriver *driver22 = new TutorialDriver;

					driver1->Run();
					driver101->Run();
					driver102->Run();
					driver103->Run();
					driver2->Run();
					driver3->Run();
					driver4->Run();
					driver5->Run();
					driver6->Run();
					driver7->Run();
					driver8->Run();
					driver9->Run();
					driver10->Run();
					driver11->Run();
					driver12->Run();
					driver13->Run();
					driver14->Run();
					driver15->Run();
					driver16->Run();
					driver17->Run();
					driver18->Run();
					driver19->Run();
					driver20->Run();
					driver21->Run();
					driver22->Run();

					delete driver1;
					delete driver101;
					delete driver102;
					delete driver103;
					delete driver2;
					delete driver3;
					delete driver4;
					delete driver5;
					delete driver6;
					delete driver7;
					delete driver8;
					delete driver9;
					delete driver10;
					delete driver11;
					delete driver12;
					delete driver13;
					delete driver14;
					delete driver15;
					delete driver16;
					delete driver17;
					delete driver18;
					delete driver19;
					delete driver20;
					delete driver21;
					delete driver22;

					Real duration = (std::clock() - starttime) / (double)CLOCKS_PER_SEC;
					std::cout << "Total run time: " << duration << " sec\n\n";
				}
				else
				{
					if (test == "Brachistochrone")
						driver = new BrachistochroneDriver();
					else if (test == "BrachistochroneDefaultSnoptPrint")
						driver = new BrachistochroneDriver(test, 1);
					else if (test == "BrachistochroneNoSnoptPrint")
						driver = new BrachistochroneDriver(test, 0);
					else if (test == "BrachistochroneInvalidSnoptPrint")
						driver = new BrachistochroneDriver(test, -5);
					else if (test == "Hypersensitive")
						driver = new HyperSensitiveDriver();
					else if (test == "Rayleigh")
						driver = new RayleighDriver();
					else if (test == "RayleighControlStateConstraint")
						driver = new RayleighControlStateConstraintDriver();
					else if (test == "ConwayOrbitExample")
						driver = new ConwayOrbitExampleDriver();
					else if (test == "ConwayOrbitExampleRK")
						driver = new ConwayOrbitExampleRKDriver();
					else if (test == "Schwartz")
						SchwartzDriver *driver = new SchwartzDriver();
					else if (test == "Moonlander")
						MoonLanderDriver *driver = new MoonLanderDriver();
					else if (test == "GoddardRocket")
						GoddardRocketDriver *driver = new GoddardRocketDriver();
					else if (test == "GoddardRocketThreePhase")
						GoddardRocketThreePhaseDriver *driver = new GoddardRocketThreePhaseDriver();
					else if (test == "RauAutomatica")
						RauAutomaticaDriver *driver = new RauAutomaticaDriver();
					else if (test == "BrysonDenham")
						BrysonDenhamDriver *driver = new BrysonDenhamDriver();
					else if (test == "BrysonMaxRange")
						BrysonMaxRangeDriver *driver = new BrysonMaxRangeDriver();
					else if (test == "InteriorPoint")
						InteriorPointDriver *driver = new InteriorPointDriver();
					else if (test == "LinearTangentSteering")
						LinearTangentSteeringDriver *driver = new LinearTangentSteeringDriver();
					else if (test == "ObstacleAvoidance")
						ObstacleAvoidanceDriver *driver = new ObstacleAvoidanceDriver();
					else if (test == "HullProblem95")
						Hull95Driver *driver = new Hull95Driver();
					else if (test == "BangBang")
						BangBangDriver *driver = new BangBangDriver();
					else if (test == "CatalyticGasOilCracker")
						CatalyticGasOilCrackerDriver *driver = new CatalyticGasOilCrackerDriver();
					else if (test == "LinearTangentSteeringStaticVar")
						LinearTangentSteeringStaticVarDriver *driver = new LinearTangentSteeringStaticVarDriver();
					else if (test == "HohmannTransfer")
						HohmannTransferDriver *driver = new HohmannTransferDriver();
					else if (test == "Tutorial")
						TutorialDriver *driver = new TutorialDriver();
					else
					{
						std::cout << "Error: Invalid test entered for -run command."
							<< std::endl;
						return 3;
					}
					RunTest(&driver/*, outputDirectory */);
				}
			}
			else if (argument == "-exit")
			{
				if (i != (argc - 1))
				{
					std::cout << "Error: -exit used, but is not the final command line argument."
						<< std::endl;
					return 2;
				}
				else
				{
					return 0;
				}
			}
			else if (argument == "-help")
			{
				msg = "";
				msg += "\n********************************************\n";
				msg += "TestOptCtrl\n";
				msg += "********************************************\n\n";
				msg += "This program runs various test problems. If used\n";
				msg += "without any command line arguments, it will offer\n";
				msg += "a list of problems to choose from.\n";
				msg += "\n";
				msg += "The following command line arguments are \n";
				msg += "available:\n";
				msg += "\n";

				msg += "-run [Test]: Runs the specified test problem.\n";
				msg += "Currently, the following test problems are \n";
				msg += "available:\n";
				msg += "   Brachistochrone\n";
				msg += "   BrachistochroneDefaultSnoptPrint\n";
				msg += "   BrachistochroneNoSnoptPrint\n";
				msg += "   BrachistochroneInvalidSnoptPrint\n";
				msg += "   HyperSensitive\n";
				msg += "   Rayleigh\n";
				msg += "   RayleighControlStateConstraint\n";
				msg += "   ConwayOrbitExample\n";
				msg += "   ConwayOrbitExampleRK\n";
				msg += "   Schwartz\n";
				msg += "   MoonLander\n";
				msg += "   GoddardRocket\n";
				msg += "   GoddardRocketThreePhase\n";
				msg += "   RauAutomatica\n";
				msg += "   BrysonDenham\n";
				msg += "   BrysonMaxRange\n";
				msg += "   InteriorPoint\n";
				msg += "   LinearTangentSteering\n";
				msg += "   ObstacleAvoidance\n";
				msg += "   HullProblem95\n";
				msg += "   BangBang\n";
				msg += "   CatalyticGasOilCracker\n";
				msg += "   LinearTangentSteeringStaticVar\n";
				msg += "   HohmannTransfer\n";
				msg += "   Tutorial\n";
				msg += "\n";

				msg += "-exit: Exits after the prior arguments are \n";
				msg += "   processed, rather than continuing to the \n";
				msg += "   multiple choice list. Must be final command\n";
				msg += "   line argument, if used.\n";
				msg += "\n";

				msg += "-help: Displays this help menu.\n";

				std::cout << msg
					<< std::endl;
			}
		}
	}

	msg = "";
	msg += "\n********************************************\n";
	msg += "TestOptCtrl\n";
	msg += "********************************************\n\n";
	msg += "Select a test case:\n";
	msg += "0.  Exit TestOptCtrl\n";
	msg += "A.  Run all test cases\n";
	msg += "1.  Brachistochrone\n";
	msg += "101.  Brachistochrone Default Snopt Print\n";
	msg += "102.  Brachistochrone No Snopt Print\n";
	msg += "103.  Brachistochrone Invalid Snopt Print\n";
	msg += "2.  HyperSensitive\n";
	msg += "3.  Rayleigh\n";
	msg += "4.  RayleighControlStateConstraint\n";
	msg += "5.  ConwayOrbitExample\n";
	msg += "6.  ConwayOrbitExample, RK Transcription\n";
	msg += "7.  Schwartz\n";
	msg += "8.  MoonLander\n";
	msg += "9.  GoddardRocket\n";
	msg += "10. GoddardRocket Three Phase\n";
	msg += "11. RauAutomatica\n";
	msg += "12. BrysonDenham\n";
	msg += "13. BrysonMaxRange\n";
	msg += "14. InteriorPoint\n";
	msg += "15. LinearTangentSteeringDriver\n";
	msg += "16. ObstacleAvoidanceDriver\n";
	msg += "17. HullProblem95\n";
	msg += "18. BangBang\n";
	msg += "19. CatalyticGasOilCracker\n";
	msg += "20. LinearTangentSteeringStaticVarDriver\n";
	msg += "21. HohmannTransferDriver\n";
	msg += "22. Tutorial\n";

   #ifdef AUTONGC_TEST_CASE
      msg += "23. AutoNGCDriver\n";
   #endif


	while (testing)
	{
		std::cout << msg
			<< std::endl;

		std::cout << "Input test case: ";
		std::cin >> inputString;

		if (inputString == "A")
		{
			std::clock_t starttime = std::clock();

			BrachistochroneDriver                *driver1 = new BrachistochroneDriver();
			BrachistochroneDriver				 *driver101 = new BrachistochroneDriver("BrachistochroneDefaultSnoptPrint", 1); // manually tell to print default snopt display to console
			BrachistochroneDriver				 *driver102 = new BrachistochroneDriver("BrachistochroneNoSnoptPrint", 0); // silence snopt display to console
			BrachistochroneDriver				 *driver103 = new BrachistochroneDriver("BrachistochroneInvalidSnoptPrint", -5); // invalid input that should be changed to 1
			HyperSensitiveDriver                 *driver2 = new HyperSensitiveDriver();
			RayleighDriver                       *driver3 = new RayleighDriver();
			RayleighControlStateConstraintDriver *driver4 = new RayleighControlStateConstraintDriver();
			ConwayOrbitExampleDriver             *driver5 = new ConwayOrbitExampleDriver();
			ConwayOrbitExampleRKDriver           *driver6 = new ConwayOrbitExampleRKDriver();
			SchwartzDriver                       *driver7 = new SchwartzDriver();
			MoonLanderDriver                     *driver8 = new MoonLanderDriver();
			GoddardRocketDriver                  *driver9 = new GoddardRocketDriver();
			GoddardRocketThreePhaseDriver        *driver10 = new GoddardRocketThreePhaseDriver();
			RauAutomaticaDriver                  *driver11 = new RauAutomaticaDriver();
			BrysonDenhamDriver                   *driver12 = new BrysonDenhamDriver();
			BrysonMaxRangeDriver                 *driver13 = new BrysonMaxRangeDriver();
			InteriorPointDriver                  *driver14 = new InteriorPointDriver();
			LinearTangentSteeringDriver          *driver15 = new LinearTangentSteeringDriver();
			ObstacleAvoidanceDriver              *driver16 = new ObstacleAvoidanceDriver();
			Hull95Driver                         *driver17 = new Hull95Driver();
			BangBangDriver                       *driver18 = new BangBangDriver();
			CatalyticGasOilCrackerDriver         *driver19 = new CatalyticGasOilCrackerDriver();
			LinearTangentSteeringStaticVarDriver *driver20 = new LinearTangentSteeringStaticVarDriver();
			HohmannTransferDriver                *driver21 = new HohmannTransferDriver();
			TutorialDriver               *driver22 = new TutorialDriver();

			driver1->Run();
			driver101->Run();
			driver102->Run();
			driver103->Run();
			driver2->Run();
			driver3->Run();
			driver4->Run();
			driver5->Run();
			driver6->Run();
			driver7->Run();
			driver8->Run();
			driver9->Run();
			driver10->Run();
			driver11->Run();
			driver12->Run();
			driver13->Run();
			driver14->Run();
			driver15->Run();
			driver16->Run();
			driver17->Run();
			driver18->Run();
			driver19->Run();
			driver20->Run();
			driver21->Run();
			driver22->Run();

			delete driver1;
			delete driver101;
			delete driver102;
			delete driver103;
			delete driver2;
			delete driver3;
			delete driver4;
			delete driver5;
			delete driver6;
			delete driver7;
			delete driver8;
			delete driver9;
			delete driver10;
			delete driver11;
			delete driver12;
			delete driver13;
			delete driver14;
			delete driver15;
			delete driver16;
			delete driver17;
			delete driver18;
			delete driver19;
			delete driver20;
			delete driver21;
			delete driver22;

			Real duration = (std::clock() - starttime) / (double)CLOCKS_PER_SEC;
			std::cout << "Total run time: " << duration << " sec\n\n";
		}
		else
		{
			try
			{
				input = std::stoi(inputString, &sz);
			}
			catch (const std::invalid_argument& ia)
			{
				input = -1;
			}

			switch (input)
			{
			case 0:
				testing = false;
				break;

			case 1:
				test = "Brachistochrone";
				driver = new BrachistochroneDriver();
				break;

			case 101:
				test = "BrachistochroneDefaultSnoptPrint";
				driver = new BrachistochroneDriver(test, 1);
				break;

			case 102:
				test = "BrachistochroneNoSnoptPrint";
				driver = new BrachistochroneDriver(test, 0);
				break;

			case 103:
				test = "BrachistochroneInvalidSnoptPrint";
				driver = new BrachistochroneDriver(test, -5);
				break;

			case 2:
				test = "HyperSensitive";
				driver = new HyperSensitiveDriver();
				break;

			case 3:
				test = "Rayleigh";
				driver = new RayleighDriver();
				break;

			case 4:
				test = "RayleighControlStateConstraint";
				driver = new RayleighControlStateConstraintDriver();
				break;

			case 5:
				test = "ConwayOrbitExample";
				driver = new ConwayOrbitExampleDriver();
				break;
			case 6:
				test = "ConwayOrbit - RK Transcription";
				driver = new ConwayOrbitExampleRKDriver();
				break;

			case 7:
				test = "Schwartz";
				driver = new SchwartzDriver();
				break;

			case 8:
				test = "MoonLander";
				driver = new MoonLanderDriver();
				break;

			case 9:
				test = "GoddardRocket";
				driver = new GoddardRocketDriver();
				break;

			case 10:
				test = "GoddardRocketThreePhase";
				driver = new GoddardRocketThreePhaseDriver();
				break;

			case 11:
				test = "RauAutomatica";
				driver = new RauAutomaticaDriver();
				break;

			case 12:
				test = "BrysonDenham";
				driver = new BrysonDenhamDriver();
				break;

			case 13:
				test = "BrysonMaxRange";
				driver = new BrysonMaxRangeDriver();
				break;

			case 14:
				test = "InteriorPoint";
				driver = new InteriorPointDriver();
				break;

			case 15:
				test = "LinearTangentSteeringDriver";
				driver = new LinearTangentSteeringDriver();
				break;

			case 16:
				test = "ObstacleAvoidance";
				driver = new ObstacleAvoidanceDriver();
				break;

			case 17:
				test = "HullProblem9_5";
				driver = new Hull95Driver();
				break;

			case 18:
				test = "BangBang";
				driver = new BangBangDriver();
				break;

			case 19:
				test = "CatalyticGasOilCracker";
				driver = new CatalyticGasOilCrackerDriver();
				break;

			case 20:
				test = "LinearTangentSteeringStaticVarDriver";
				driver = new LinearTangentSteeringStaticVarDriver();
				break;

			case 21:
				test = "HohmannTransferDriver";
				driver = new HohmannTransferDriver();
				break;

			case 22:
				test = "TutorialDriver";
				driver = new TutorialDriver();
				break;

         #ifdef AUTONGC_TEST_CASE
            case 23:
            {
               Rvector6 kepState, cartState;
               Real initMass = 1000.0;
               AutoNGCDriver *testNGC;
               test = "AutoNGCDriver";
               kepState(0) = 7000.0 + 400.0 * 1.8339;
               kepState(1) = 0.7559 / 100.0;
               kepState(2) = GmatMathUtil::RadToDeg(GmatMathConstants::PI / 2.0 * 0.5298);
               kepState(3) = GmatMathUtil::RadToDeg(2.0 * GmatMathConstants::PI * 0.2298);
               kepState(4) = GmatMathUtil::RadToDeg(2.0 * GmatMathConstants::PI * 0.8722);
               kepState(5) = GmatMathUtil::RadToDeg(2.0 * GmatMathConstants::PI * 0.3490);
               cartState = StateConversionUtil::Convert(kepState, "Keplerian", "Cartesian");
               testNGC = new AutoNGCDriver();
               testNGC->InitializeProblemData(35000.0, cartState, initMass);
               testNGC->Initialize();
               Trajectory *traj = testNGC->GetTrajectory();
               Rvector  dv2 = traj->GetDecisionVector();
               Rvector  C = traj->GetCostConstraintFunctions();
               Rvector z = dv2;
               Rvector F(C.GetSize());
               Rvector xmul(dv2.GetSize());
               Rvector Fmul(C.GetSize());
               Integer exitFlag;
               testNGC->Optimize(z, F, xmul, Fmul, exitFlag);
               delete testNGC;
               break;
            }
         #endif

			default:
				std::cout << "Invalid test case. Try again.\n";
				break;
			}

         // Note: The AutoNGC test is run differently, so do not use RunTest 
         // here for that case (test 23)
			if (input >= 0 && input != 23)
				RunTest(&driver/*, outputDirectory */);
		}
	}
	std::cout << "Exiting TestOptCtrl." << std::endl;
	return 0;
}

bool RunTest(CsaltTestDriver **driver, const std::string &outputPath)
{
   bool retval = false;

   if (*driver)
   {
      std::clock_t starttime = std::clock();

      if (outputPath != "")
         ;     // Set output path

      (*driver)->Run();

      // Should test the return from Run, but for now...
      retval = true;

      Real duration = (std::clock() - starttime) / (double)CLOCKS_PER_SEC;
      std::cout << "Total run time: " << duration << " sec\n\n";

      delete *driver;
      *driver = NULL;
   }

   return retval;
}
