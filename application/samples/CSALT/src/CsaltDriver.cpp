
#include "CsaltDriver.hpp"

const Real CsaltDriver::INF = std::numeric_limits<Real>::infinity();

//------------------------------------------------------------------------------
// CsaltDriver()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
CsaltDriver::CsaltDriver(const std::string& name = "Problem") :
    testName                   (name),
    verbosity                  (VERBOSE),
    traj                       (NULL),
    pathObject                 (NULL),
    pointObject                (NULL),
    costLowerBound             (-INF),
    costUpperBound             (INF),
    maxMeshRefinementCount     (0),
    generateOptimizationOutput (true),
    writeControlHistory        (true),
    snoptConsoleOutputLevel    (1)
{
	optimizationOutputFile = testName + "Data.txt";
	controlHistoryFile = testName + ".och";
	majorOptimalityTolerances = Rvector(1, 1.0E-4);
	majorIterationsLimits = IntegerArray(1,3000);
	totalIterationsLimits = IntegerArray(1,300000);
	feasibilityTolerances = Rvector(1, 1.0E-6);
	optimizationMode = StringArray(1, "Minimize");
}

//------------------------------------------------------------------------------
// CsaltDriver()
//------------------------------------------------------------------------------
/**
 * Constructor that sets value of snoptConsoleOutputLevel via user input
 */
 //------------------------------------------------------------------------------
CsaltDriver::CsaltDriver(const std::string &name, const Integer snoptConsoleOutputLevel) :
	testName					(name),
	verbosity					(VERBOSE),
	traj						(NULL),
	pathObject					(NULL),
	pointObject					(NULL),
	costLowerBound				(-INF),
	costUpperBound				(INF),
	maxMeshRefinementCount		(0),
	generateOptimizationOutput	(true),
	writeControlHistory			(true),
	snoptConsoleOutputLevel		(snoptConsoleOutputLevel)
{
	optimizationOutputFile = testName + "Data.txt";
	controlHistoryFile = testName + ".och";
	majorOptimalityTolerances = Rvector(1, 1.0E-4);
	majorIterationsLimits = IntegerArray(1, 3000);
	totalIterationsLimits = IntegerArray(1, 300000);
	feasibilityTolerances = Rvector(1, 1.0E-6);
	optimizationMode = StringArray(1, "Minimize");
}

//------------------------------------------------------------------------------
// ~CsaltDriver()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
CsaltDriver::~CsaltDriver()
{
   // Path and point objects are no longer deleted in trajectory to avoid 
   // double delete issues, delete them here
   if (traj)
       delete traj;
   if (pathObject)
       delete pathObject;
   if (pointObject)
       delete pointObject;
   for (Integer ii = 0; ii < phaseList.size(); ++ii)
	   if (phaseList.at(ii))
		   delete phaseList.at(ii);
   for (Integer ii = 0; ii < ocfObjects.size(); ++ii)
	   if (ocfObjects.at(ii))
		   delete ocfObjects.at(ii);
}

//------------------------------------------------------------------------------
// CsaltDriver& CsaltDriver::operator =(const CsaltDriver& driver)
//------------------------------------------------------------------------------
/**
 * Assignment operator (Likely unused)
 *
 * @param driver The object providing data for this one
 *
 * @return This object, set to match driver
 */
//------------------------------------------------------------------------------
CsaltDriver& CsaltDriver::operator=(const CsaltDriver& driver)
{
   // Currently nothing to do here
   return *this;
}

//------------------------------------------------------------------------------
// Integer CsaltDriver::Run()
//------------------------------------------------------------------------------
/**
 * The entry point for the test case
 *
 * @return A code indicating teh state of the run
 */
//------------------------------------------------------------------------------
Integer CsaltDriver::Run() {
	Integer retval = 0;
	
	std::string outFormat = "%16.9f ";
	
	ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
	MessageInterface::SetMessageReceiver(consoleMsg);
	std::string outPath = "./";
	MessageInterface::SetLogFile(outPath + "CsaltLog.txt");

	// Set global format setting
	GmatGlobal *global = GmatGlobal::Instance();
	global->SetActualFormat(false, false, 16, 1, false);

	if (verbosity != SILENT)
	{
	  MessageInterface::ShowMessage("%s\n",
			GmatTimeUtil::FormatCurrentTime().c_str());
	  MessageInterface::ShowMessage("\n*** Running the %s CSALT problem ***\n",
			testName.c_str());
	}

	try
	{
	  // Create the trajectory and set core settings
	  traj = new Trajectory();
	  
	  // Get the path and point properties as well as any OptimalControlFunction(s)
	  SetPointPathAndProperties();
	  
	  if (pathObject)
		 traj->SetUserPathFunction(pathObject);
	  if (pointObject) {
		  if (ocfObjects.size() > 0)
			  pointObject->AddFunctions(ocfObjects);
		  traj->SetUserPointFunction(pointObject);
	  }

	  traj->SetMajorIterationsLimit(majorIterationsLimits);
	  traj->SetOptimalityTolerances(majorOptimalityTolerances);
	  traj->SetFeasibilityTolerances(feasibilityTolerances);
	  traj->SetTotalIterationsLimit(totalIterationsLimits);
	  traj->SetOptimizationMode(optimizationMode);
	  traj->SetCostLowerBound(costLowerBound);
	  traj->SetCostUpperBound(costUpperBound);
	  traj->SetMaxMeshRefinementCount(maxMeshRefinementCount);
	  traj->SetFailedMeshOptimizationAllowance(false);
	  traj->SetMeshRefinementGuessMode("LastSolutionMostRecentMesh");
	  traj->SetSnoptConsoleOutputLevel(this->snoptConsoleOutputLevel);
	  
	  // Get the phase information
	  SetupPhases();

	  if (phaseList.size() == 0)
		 retval = -1;
	  else
	  {
		 traj->SetPhaseList(phaseList);

		 if (verbosity == VERBOSE)
			MessageInterface::ShowMessage("*** TEST *** initializing the Trajectory\n");
		 traj->Initialize();

		 if (verbosity == VERBOSE)
			MessageInterface::ShowMessage("*** TEST *** setting up the call to Optimize!!\n");

		 Rvector  dv2      = traj->GetDecisionVector();
		 Rvector  C        = traj->GetCostConstraintFunctions(); // nonDim);

		 if (verbosity == VERBOSE_DEBUG)
		 {
			RSMatrix conSp    = phaseList[0]->GetConSparsityPattern();

			MessageInterface::ShowMessage("*** TEST *** Con sparsity pattern from Phase1:\n");
			Integer rJac = conSp.size1();
			Integer cJac = conSp.size2();
			for (Integer cc = 0; cc < cJac; cc++)
			{
			   for (Integer rr = 0; rr < rJac; rr++)
			   {
				  Real jacTmp = conSp(rr,cc);
				  if (jacTmp != 0.0)
				  MessageInterface::ShowMessage(" (%d, %d)   %12.10f\n", rr+1, cc+1, jacTmp);
			   }
			}
		 }

		 // ------------------ Optimizing --------------------------------------------
		 Rvector z = dv2;
		 Rvector F(C.GetSize());
		 Rvector xmul(dv2.GetSize());
		 Rvector Fmul(C.GetSize());
		 Integer exitFlag;

		 if (verbosity == VERBOSE)
			MessageInterface::ShowMessage("*** TEST *** Optimizing!!\n");

		 if (generateOptimizationOutput)
			traj->Optimize(z, F, xmul, Fmul, exitFlag, optimizationOutputFile);
		 else
			traj->Optimize(z, F, xmul, Fmul, exitFlag);

		 if (writeControlHistory)
			traj->WriteToFile(controlHistoryFile);


			#ifdef DEBUG_SHOWRESULTS
			   MessageInterface::ShowMessage("*** TEST *** z:\n%s\n",
											 z.ToString(12).c_str());
			   MessageInterface::ShowMessage("*** TEST *** F:\n%s\n",
											 F.ToString(12).c_str());
			   MessageInterface::ShowMessage("*** TEST *** xmul:\n%s\n",
											 xmul.ToString(12).c_str());
			   MessageInterface::ShowMessage("*** TEST *** Fmul:\n%s\n",
											 Fmul.ToString(12).c_str());
			   MessageInterface::ShowMessage("*** TEST *** Optimization complete!!\n");

			   Phase *phase1 = traj->GetPhaseList()[0];
			   Rvector dvP1 = phase1->GetDecVector();
			   MessageInterface::ShowMessage("*** TEST *** dvP1:\n%s\n",
											 dvP1.ToString(12).c_str());

			   // Interpolate solution
			   Rvector timeVector = phase1->GetTimeVector();
			   DecisionVector *dv = phase1->GetDecisionVector();
			   Rmatrix stateSol   = dv->GetStateArray();
			   Rmatrix controlSol = dv->GetControlArray();
			   Rvector staticSol = dv->GetStaticVector();
			#endif

		 if (verbosity != SILENT)
			MessageInterface::ShowMessage("*** END %s TEST ***\n", testName.c_str());
	  } // Phase[0] exists

	}
	catch (BaseException &ex)
	{
	  MessageInterface::ShowMessage("Caught a CSALT Exception:\n\n%s\n\n",
			ex.GetFullMessage().c_str());
	}

	return retval;
}