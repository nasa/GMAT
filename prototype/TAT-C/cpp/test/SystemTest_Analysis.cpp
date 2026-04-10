//------------------------------------------------------------------------------
//                               SystemTest_Analysis
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2016.05.31
//
/**
 * System tester for analysis
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "AbsoluteDate.hpp"
#include "Spacecraft.hpp"
#include "Earth.hpp"
#include "KeyValueStatistics.hpp"
#include "VisiblePOIReport.hpp"
#include "OrbitState.hpp"
#include "PointGroup.hpp"
#include "Propagator.hpp"
#include "ConicalSensor.hpp"
#include "RectangularSensor.hpp"
#include "CustomSensor.hpp"
#include "CoverageChecker.hpp"
#include "NadirPointingAttitude.hpp"
#include "LagrangeInterpolator.hpp"
#include "TimeTypes.hpp"

//#define DEBUG_A_LOT


using namespace std;
using namespace GmatMathUtil;
using namespace GmatMathConstants;

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   // **************************************************************************
   // -------------------------- Begin User Config -----------------------------
   // **************************************************************************
   //
   //  Set the case flag
   //   caseFlag 1 uses conical sensor with 60 second step size
   //   caseFlag 2 uses custom sensor configured to mimic conical with 
   //              10 second step size
   //   caseFlag 3 uses TROPICS sensor with 2.0 second step size
   //   caseFlag 4 uses rectangular sensor with TBD step size
   Integer caseFlag = 2;
   //
   // whichTest 1 = accumulate as we propagate;
   // whichTest 2 = propagate first, then interpolate for coverage
   Integer whichTest = 2;
   //
   // Set the number of iterations
   Integer numIter = 1000; // 10000;
   //  Set the stepsize, dependent on the caseFlag
   Real stepSize = 0;
   if (caseFlag == 1)
   {
      stepSize = 60.0;           // recommended value
   }
   else if (caseFlag == 2)
   {
      stepSize = 10.0;           // recommended value
   }
   else if (caseFlag == 3)
   {
      stepSize = 2.0;            // recommended value
   }
   else if (caseFlag == 4)
   {
      stepSize = 60.0;           // recommended value   TBD!!!!
   }
   // Set the duration of the while (propagation) loop (in days)
   Real whileLoopDuration = 0.1;
   // Set the interpolation stepsize
   Real interpolationStepSize = 1.0;
   //
   bool showPlots = false;        // ******currently unused******
   //
   Real        tolerance = 1e-15; // ******currently unused******
   //
   // NOTE: setting up the actual objects and their default values (e.g. cone
   // and clock angles) is done below
   // **************************************************************************
   // -------------------------- End User Config -------------------------------
   // **************************************************************************

   /// ******** Set up the messaging and output
   std::string outFormat = "%16.9f ";
   
   // Set up the message receiver and log file
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   MessageInterface::ShowMessage("%s\n",
                                 GmatTimeUtil::FormatCurrentTime().c_str());
   
   // Set the global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);
   
   // Check the OS (note that this does not work correctly for Mac)
   char *buffer = NULL;
   buffer = getenv("OS");
   if (buffer  != NULL)
   {
      MessageInterface::ShowMessage("Current OS is %s\n", buffer);
   }
   else
   {
      MessageInterface::ShowMessage("Buffer is NULL\n");
   }
   
   /// ******** Begin setting up the test
   MessageInterface::ShowMessage("*** START TEST ***\n");
  
   try
   {
      // This is a usage example that drives O-C code and computes standard
      // statistical products typical of O-C analysis.  This script is an
      // example of how R-M might use the O-C data.
      
      // These are the objects needed
      AbsoluteDate             *date;
      OrbitState               *state;
      ConicalSensor            *conicalSensor;
      Spacecraft               *sat1;
      Propagator               *prop;
      CoverageChecker          *covChecker;
	   Earth                    *earth;
      CustomSensor             *customSensor;
      CustomSensor             *tropicsSensor;
      NadirPointingAttitude    *attitude;
      /// Create the interpolator and the PointGroup once!
      /// (i.e. not in the loop)
      LagrangeInterpolator     *interp = new LagrangeInterpolator(
                                             "TATCLagrangeInterpolator", 6, 7);
      PointGroup               *pGroup = new PointGroup();
      
      // Set up the points
      pGroup->AddHelicalPointsByNumPoints(200);

      // Create the container to hold the coverage events
      std::vector<IntervalEventReport> coverageEvents;
      
      clock_t t0 = clock(); // for timing
      /// Set up the PointGroup for the whole run
      // Try to set the lower and upper bounds for the longitude
//      pGroup->SetLatLonBounds((PI/2.0), (-PI/2.0), PI, -PI);
      
      /// Run the test numIter times
      for (Integer ii = 0; ii < numIter; ii++)
      {
         #ifdef DEBUG_A_LOT
            MessageInterface::ShowMessage("**** iter ii = %d **************\n",
                                          ii);
         #endif
         // Create an Earth model
         earth = new Earth();
         
         // Create the epoch object and set the initial epoch
         date = new AbsoluteDate();
         date->SetGregorianDate(2017, 1, 15, 22, 30, 20.111);
         
         // Create the spacecraft state object and set Keplerian elements
         state = new OrbitState();
         state->SetKeplerianState(6900.0, 0.000, 90.0*RAD_PER_DEG,
             0, 0, 90.0*RAD_PER_DEG);
         
         #ifdef DEBUG_A_LOT
            MessageInterface::ShowMessage("**** date and state OK "
                                          "**************\n");
         #endif
         // Create a conical sensor with FOV of coneAngle
         Real coneAngle = 30.0*PI/180.0;
         conicalSensor = new ConicalSensor(coneAngle);
//         conicalSensor->SetSensorBodyOffsetAngles(180.0,0.0,0.0);

         // Create a custom sensor with FOV of coneAngle (so output
         // should match that for conicalSensor)
         Rvector coneAngleVec(361,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle,
                     coneAngle,coneAngle,coneAngle,coneAngle,coneAngle);
//                     coneAngle);
         Rvector clockAngleVec(361,0.0,
                         0.017453292519943,0.034906585039887,0.05235987755983,
                         0.069813170079773,0.087266462599716,
                         0.10471975511966,0.1221730476396,0.13962634015955,
                         0.15707963267949,0.17453292519943,
                         0.19198621771938,0.20943951023932,0.22689280275926,
                         0.24434609527921,0.26179938779915,
                         0.27925268031909,0.29670597283904,0.31415926535898,
                         0.33161255787892,0.34906585039887,
                         0.36651914291881,0.38397243543875,0.4014257279587,
                         0.41887902047864,0.43633231299858,
                         0.45378560551853,0.47123889803847,0.48869219055841,
                         0.50614548307836,0.5235987755983,
                         0.54105206811824,0.55850536063819,0.57595865315813,
                         0.59341194567807,0.61086523819802,
                         0.62831853071796,0.6457718232379,0.66322511575785,
                         0.68067840827779,0.69813170079773,
                         0.71558499331768,0.73303828583762,0.75049157835756,
                         0.7679448708775,0.78539816339745,
                         0.80285145591739,0.82030474843733,0.83775804095728,
                         0.85521133347722,0.87266462599716,
                         0.89011791851711,0.90757121103705,0.92502450355699,
                         0.94247779607694,0.95993108859688,
                         0.97738438111682,0.99483767363677,1.0122909661567,
                         1.0297442586767,1.0471975511966,
                         1.0646508437165,1.0821041362365,1.0995574287564,
                         1.1170107212764,1.1344640137963,
                         1.1519173063163,1.1693705988362,1.1868238913561,
                         1.2042771838761,1.221730476396,
                         1.239183768916,1.2566370614359,1.2740903539559,
                         1.2915436464758,1.3089969389957,
                         1.3264502315157,1.3439035240356,1.3613568165556,
                         1.3788101090755,1.3962634015955,
                         1.4137166941154,1.4311699866354,1.4486232791553,
                         1.4660765716752,1.4835298641952,
                         1.5009831567151,1.5184364492351,1.535889741755,
                         1.553343034275,1.5707963267949,
                         1.5882496193148,1.6057029118348,1.6231562043547,
                         1.6406094968747,1.6580627893946,
                         1.6755160819146,1.6929693744345,1.7104226669544,
                         1.7278759594744,1.7453292519943,
                         1.7627825445143,1.7802358370342,1.7976891295542,
                         1.8151424220741,1.832595714594,
                         1.850049007114,1.8675022996339,1.8849555921539,
                         1.9024088846738,1.9198621771938,
                         1.9373154697137,1.9547687622336,1.9722220547536,
                         1.9896753472735,2.0071286397935,
                         2.0245819323134,2.0420352248334,2.0594885173533,
                         2.0769418098733,2.0943951023932,
                         2.1118483949131,2.1293016874331,2.146754979953,
                         2.164208272473,2.1816615649929,
                         2.1991148575129,2.2165681500328,2.2340214425527,
                         2.2514747350727,2.2689280275926,
                         2.2863813201126,2.3038346126325,2.3212879051525,
                         2.3387411976724,2.3561944901923,
                         2.3736477827123,2.3911010752322,2.4085543677522,
                         2.4260076602721,2.4434609527921,
                         2.460914245312,2.4783675378319,2.4958208303519,
                         2.5132741228718,2.5307274153918,
                         2.5481807079117,2.5656340004317,2.5830872929516,
                         2.6005405854716,2.6179938779915,
                         2.6354471705114,2.6529004630314,2.6703537555513,
                         2.6878070480713,2.7052603405912,
                         2.7227136331112,2.7401669256311,2.757620218151,
                         2.775073510671,2.7925268031909,
                         2.8099800957109,2.8274333882308,2.8448866807508,
                         2.8623399732707,2.8797932657906,
                         2.8972465583106,2.9146998508305,2.9321531433505,
                         2.9496064358704,2.9670597283904,
                         2.9845130209103,3.0019663134302,3.0194196059502,
                         3.0368728984701,3.0543261909901,
                         3.07177948351,3.08923277603,3.1066860685499,
                         3.1241393610699,3.1415926535898,
                         3.1590459461097,3.1764992386297,3.1939525311496,
                         3.2114058236696,3.2288591161895,
                         3.2463124087095,3.2637657012294,3.2812189937493,
                         3.2986722862693,3.3161255787892,
                         3.3335788713092,3.3510321638291,3.3684854563491,
                         3.385938748869,3.4033920413889,
                         3.4208453339089,3.4382986264288,3.4557519189488,
                         3.4732052114687,3.4906585039887,
                         3.5081117965086,3.5255650890285,3.5430183815485,
                         3.5604716740684,3.5779249665884,
                         3.5953782591083,3.6128315516283,3.6302848441482,
                         3.6477381366681,3.6651914291881,
                         3.682644721708,3.700098014228,3.7175513067479,
                         3.7350045992679,3.7524578917878,
                         3.7699111843078,3.7873644768277,3.8048177693476,
                         3.8222710618676,3.8397243543875,
                         3.8571776469075,3.8746309394274,3.8920842319474,
                         3.9095375244673,3.9269908169872,
                         3.9444441095072,3.9618974020271,3.9793506945471,
                         3.996803987067,4.014257279587,
                         4.0317105721069,4.0491638646268,4.0666171571468,
                         4.0840704496667,4.1015237421867,
                         4.1189770347066,4.1364303272266,4.1538836197465,
                         4.1713369122664,4.1887902047864,
                         4.2062434973063,4.2236967898263,4.2411500823462,
                         4.2586033748662,4.2760566673861,
                         4.2935099599061,4.310963252426,4.3284165449459,
                         4.3458698374659,4.3633231299858,
                         4.3807764225058,4.3982297150257,4.4156830075457,
                         4.4331363000656,4.4505895925855,
                         4.4680428851055,4.4854961776254,4.5029494701454,
                         4.5204027626653,4.5378560551853,
                         4.5553093477052,4.5727626402251,4.5902159327451,
                         4.607669225265,4.625122517785,
                         4.6425758103049,4.6600291028249,4.6774823953448,
                         4.6949356878647,4.7123889803847,
                         4.7298422729046,4.7472955654246,4.7647488579445,
                         4.7822021504645,4.7996554429844,
                         4.8171087355043,4.8345620280243,4.8520153205442,
                         4.8694686130642,4.8869219055841,
                         4.9043751981041,4.921828490624,4.939281783144,
                         4.9567350756639,4.9741883681838,
                         4.9916416607038,5.0090949532237,5.0265482457437,
                         5.0440015382636,5.0614548307836,
                         5.0789081233035,5.0963614158234,5.1138147083434,
                         5.1312680008633,5.1487212933833,
                         5.1661745859032,5.1836278784232,5.2010811709431,
                         5.218534463463,5.235987755983,
                         5.2534410485029,5.2708943410229,5.2883476335428,
                         5.3058009260628,5.3232542185827,
                         5.3407075111026,5.3581608036226,5.3756140961425,
                         5.3930673886625,5.4105206811824,
                         5.4279739737024,5.4454272662223,5.4628805587423,
                         5.4803338512622,5.4977871437821,
                         5.5152404363021,5.532693728822,5.550147021342,
                         5.5676003138619,5.5850536063819,
                         5.6025068989018,5.6199601914217,5.6374134839417,
                         5.6548667764616,5.6723200689816,
                         5.6897733615015,5.7072266540215,5.7246799465414,
                         5.7421332390613,5.7595865315813,
                         5.7770398241012,5.7944931166212,5.8119464091411,
                         5.8293997016611,5.846852994181,
                         5.8643062867009,5.8817595792209,5.8992128717408,
                         5.9166661642608,5.9341194567807,
                         5.9515727493007,5.9690260418206,5.9864793343406,
                         6.0039326268605,6.0213859193804,
                         6.0388392119004,6.0562925044203,6.0737457969403,
                         6.0911990894602,6.1086523819802,
                         6.1261056745001,6.14355896702,6.16101225954,
                         6.1784655520599,6.1959188445799,
                         6.2133721370998,6.2308254296198,6.2482787221397,
                         6.2657320146596,6.2831853071796,
                         2*PI);
         customSensor = new CustomSensor(coneAngleVec,clockAngleVec);
         
         // Create a rectangular sensor (TBD)
         RectangularSensor *rectangularSensor =
                           new RectangularSensor(0.5, 0.5); // ????? TBD ?????

         // Create a custom sensor to match the rectangular sensor
         Real tropicsConeAngle = 0.9949922073045261;
         Rvector tropicsConeAngleVec(5, tropicsConeAngle, tropicsConeAngle,
                                     tropicsConeAngle, tropicsConeAngle,
                                     tropicsConeAngle);
         Rvector tropicsClockAngleVec(5, 0.02181442636896036, 3.119778227220833, 3.163407079958754,
             -0.02181442636896036, 0.02181442636896036);
         tropicsSensor = new CustomSensor(tropicsConeAngleVec,
                                          tropicsClockAngleVec);
         #ifdef DEBUG_A_LOT
            MessageInterface::ShowMessage("**** sensors OK **************\n");
         #endif
         
         // Create a spacecraft giving it a state and epoch
         attitude = new NadirPointingAttitude();

         #ifdef DEBUG_A_LOT
            MessageInterface::ShowMessage(
                              "*** About to create Spacecraft!!!!\n");
         #endif
         sat1     = new Spacecraft(date, state, attitude, interp); //,0.0, 0.0,
                                                                   // 180.0);
            
         #ifdef DEBUG_A_LOT
            MessageInterface::ShowMessage("*** DONE creating Spacecraft!!!!\n");
            MessageInterface::ShowMessage("**** attitude and sat1 OK "
                                          "**************\n");
         #endif

         //  Add the sensor to the spacecraft

         if (caseFlag == 1)
         {
            sat1->AddSensor(conicalSensor);
         }
         else if (caseFlag == 2)
         {
             sat1->AddSensor(customSensor);
         }
         else if (caseFlag == 3)
         {
            sat1->AddSensor(tropicsSensor);
         }
         else if (caseFlag == 4)
            sat1->AddSensor(rectangularSensor);
         
         // Create the propagator
         prop = new Propagator(sat1);
         
         #ifdef DEBUG_A_LOT
            MessageInterface::ShowMessage("**** prop OK **************\n");
            MessageInterface::ShowMessage("*** DONE creating Propagator!!!!\n");
         #endif
         
         // Initialize the coverage checker
         covChecker = new CoverageChecker(pGroup,sat1);
         covChecker->SetComputePOIGeometryData(true);
         #ifdef DEBUG_A_LOT
            MessageInterface::ShowMessage("*** Coverage Checker created!!!!\n");
         #endif
         
         // Propagate for a duration and collect data
         Real           startDate   = date->GetJulianDate();
         IntegerArray   loopPoints;
         #ifdef DEBUG_A_LOT
            MessageInterface::ShowMessage("**** checking whichTest "
                                          "**************\n");
         #endif
         
         if (whichTest == 1)
         {
         // TEST 1: accumulate coverage data as we go
            #ifdef DEBUG_A_LOT
               MessageInterface::ShowMessage("*** About to Propagate!!!!\n");
            #endif
            // Propagate to the initial time first
            prop->Propagate(*date);
            while (date->GetJulianDate() < ((Real)startDate +
                                            whileLoopDuration))
            {
               // Compute points in view at time zero!
               loopPoints = covChecker->AccumulateCoverageData();

               // Propagate
               date->Advance(stepSize);
               prop->Propagate(*date);
            
               // Compute lat., lon., and height of s/c w/r/t the ellipsoid
               Real     jDate        = sat1->GetJulianDate();
               Rvector6 cartState    = sat1->GetCartesianState();
               Rvector3 inertialPosVec(cartState(0), cartState(1),
                                       cartState(2));
               Rvector3 latLonHeight = earth->InertialToBodyFixed(
                                             inertialPosVec,
                                             jDate, "Ellipsoid");
            }
            #ifdef DEBUG_A_LOT
               MessageInterface::ShowMessage(" --- propagation completed\n");
            #endif
            // END TEST 1: accumulate coverage data as we go
         }

         else // whichTest = 2
         {
            // TEST 2: propagate first, then accumulate coverage data:
            //         interpolate data when needed (number of points is
            //         sufficient and we are not about to fall off the
            //         time range
            Real           interpTime = startDate;
            Real           midRange   = 0.0;
            Real           propTime   = date->GetJulianDate();
            #ifdef DEBUG_A_LOT
               MessageInterface::ShowMessage(" --- propTime = %12.10f\n",
                                             propTime);
            #endif
            // Propagate to the initial time first
            #ifdef DEBUG_A_LOT
               MessageInterface::ShowMessage(" --- ABOUT to propagate\n");
            #endif
            prop->Propagate(*date);
            #ifdef DEBUG_A_LOT
               MessageInterface::ShowMessage(" --- propagation completed\n");
            #endif
            while (date->GetJulianDate() < ((Real) startDate +
                                            whileLoopDuration))
            {
               date->Advance(stepSize);
               prop->Propagate(*date);
               #ifdef DEBUG_A_LOT
                  MessageInterface::ShowMessage(
                                    " ------- propagation completed\n");
               #endif
               propTime = date->GetJulianDate();
               #ifdef DEBUG_A_LOT
                  MessageInterface::ShowMessage(" ------- propTime = %12.10f\n",
                                                propTime);
               #endif
               // Interpolate when and if needed
               if (sat1->TimeToInterpolate(propTime, midRange))
               {
                  while (interpTime < (propTime - midRange))
                  {
                     loopPoints = covChecker->
                                  AccumulateCoverageData(interpTime);
                     interpTime += interpolationStepSize/
                                   GmatTimeConstants::SECS_PER_DAY;
                  }
               }
            }
            #ifdef DEBUG_A_LOT
               MessageInterface::ShowMessage(" --- propagation completed\n");
            #endif

            // Interpolate to the end, if necessary
            propTime = date->GetJulianDate();
            while (interpTime <= propTime)
            {
               loopPoints = covChecker->
                            AccumulateCoverageData(interpTime);
               interpTime += interpolationStepSize/
                             GmatTimeConstants::SECS_PER_DAY;
            }
            
            #ifdef DEBUG_A_LOT
               MessageInterface::ShowMessage(" --- interpolation completed\n");
            #endif
            // END TEST 2: propagate first, then accumulate coverage data
         }
         
         // Compute coverage data
         coverageEvents = covChecker->ProcessCoverageData();
         MessageInterface::ShowMessage(" --- ProcessCoverageData completed "
                                       "(ii = %d) with numEvents = %d\n",
                                       ii, (int) coverageEvents.size());
         if (coverageEvents.empty())
         {
            MessageInterface::ShowMessage("--- ERROR!!!!! No events!!!\n");
            exit(0);
         }
         
         //End loop here, so delete all the objects (which are
         // recreated each loop)
         delete    covChecker;
         delete    prop;
//         delete    sat1;  // deletes date, state, attitude, interp - so DON'T
         delete    date;
         delete    state;
         delete    attitude;
         delete    tropicsSensor;
         delete    customSensor;
         delete    conicalSensor;
         delete    earth;

         #ifdef DEBUG_A_LOT
            MessageInterface::ShowMessage(" --- Done deleting old pointers\n");
         #endif

      } // ***** for loop
         
      // check timing
      Real timeSpent = ((Real) (clock() - t0)) / CLOCKS_PER_SEC;
      MessageInterface::ShowMessage(
                        "TIME SPENT in %d iterations is %12.10f seconds\n",
                        numIter,timeSpent);
      
      RealArray lonVec;
      RealArray latVec;
      // Compute coverage stats.  Shows how R-M might use data for coverage
      // analysis
      // Create Lat\Lon Grid
      for (Integer pointIdx = 0; pointIdx < pGroup->GetNumPoints(); pointIdx++)
      {
         Rvector3 *vec = pGroup->GetPointPositionVector(pointIdx);
         lonVec.push_back(ATan(vec->GetElement(1),vec->GetElement(0))
                          *DEG_PER_RAD);
         latVec.push_back(ASin(vec->GetElement(2)/vec->GetMagnitude())
                          *DEG_PER_RAD);
      }
      
      MessageInterface::ShowMessage(" --- lat/long set-up completed\n");
      
      // Compute total coverage statistics from all coverage events
      Rvector totalCoverageDuration(pGroup->GetNumPoints());
      IntegerArray numPassVec;  // (pGroup->GetNumPoints());
      for (Integer ii = 0; ii < pGroup->GetNumPoints(); ii++)
         numPassVec.push_back(0);
      Rvector minPassVec(pGroup->GetNumPoints());
      Rvector maxPassVec(pGroup->GetNumPoints());
      for (UnsignedInt eventIdx = 0; eventIdx < coverageEvents.size();
           eventIdx++)
      {
         IntervalEventReport currEvent = coverageEvents.at(eventIdx);
         std::vector<VisiblePOIReport> discreteEvents =
                                                       currEvent.GetPOIEvents();
         Integer          poiIndex       = currEvent.GetPOIIndex();
         Real             eventDuration  =
                                      (currEvent.GetEndDate().GetJulianDate() -
                                      currEvent.GetStartDate().GetJulianDate())
                                      * 24.0;

         totalCoverageDuration(poiIndex) = totalCoverageDuration(poiIndex) + eventDuration;
      
         // Save the maximum duration if necessary
         if (eventDuration > maxPassVec(poiIndex))
            maxPassVec(poiIndex) = eventDuration;

      
         if (minPassVec(poiIndex) == 0 || (eventDuration< maxPassVec(poiIndex)))
            minPassVec(poiIndex) = eventDuration;

         numPassVec.at(poiIndex) = numPassVec.at(poiIndex) + 1;
      }
      
      // *********** display stuff ***********
      // Write the simple coverage report to the MATLAB command window
      MessageInterface::ShowMessage("       =======================================================================\n");
      MessageInterface::ShowMessage("       ==================== Brief Coverage Analysis Report ===================\n");
      MessageInterface::ShowMessage("       lat (deg): Latitude of point in degrees                  \n");
      MessageInterface::ShowMessage("       lon (deg): Longitude of point in degrees                  \n");
      MessageInterface::ShowMessage("       numPasses: Number of total passes seen by a point                           \n");
      MessageInterface::ShowMessage("       totalDur : Total duration point was observed in hours                         \n");
      MessageInterface::ShowMessage("       minDur   : Duration of the shortest pass in minutes                         \n");
      MessageInterface::ShowMessage("       maxDur   : Duration of the longest pass in hours                            \n");
      MessageInterface::ShowMessage("       =======================================================================\n");
      MessageInterface::ShowMessage("       =======================================================================\n");
      MessageInterface::ShowMessage("  ");
      
//      data = [latVec,lonVec, numPassVec, totalCoverageDuration, minPassVec, maxPassVec];  <<<<<<<<<
      Integer headerCount = 1;
      Integer dataEnd     = 0;
      for (Integer passIdx = 0; passIdx < pGroup->GetNumPoints(); passIdx+= 10)
      {
         MessageInterface::ShowMessage("       lat (deg)     lon (deg)       numPasses  totalDur    minDur      maxDur\n");
         dataEnd = passIdx + 10;  // 9;
         for (Integer ii = 0; ii < 10; ii++)  // 9
            MessageInterface::ShowMessage(
                           "       %le    %le    %d    %le    %le    %le \n",
                           latVec.at(passIdx+ii),
                           lonVec.at(passIdx+ii),
                           numPassVec.at(passIdx+ii),
                           totalCoverageDuration(passIdx+ii),
                           minPassVec(passIdx+ii),
                           maxPassVec(passIdx+ii));
      }

      if (dataEnd + 1 < pGroup->GetNumPoints())
      {
         MessageInterface::ShowMessage(
            "       lat (deg)    lon (deg)     numPasses   totalDur    minDur      maxDur\n");
         for (Integer ii = dataEnd; ii < pGroup->GetNumPoints(); ii++)
            MessageInterface::ShowMessage(
                           "       %le    %le    %d    %le    %le    %le \n",
                           latVec.at(ii),
                           lonVec.at(ii),
                           numPassVec.at(ii),
                           totalCoverageDuration(ii),
                           minPassVec(ii),
                           maxPassVec(ii));
      }
      
      MessageInterface::ShowMessage("*** END TEST ***\n");
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n",
                                    be.GetFullMessage().c_str());
   }
   
}
