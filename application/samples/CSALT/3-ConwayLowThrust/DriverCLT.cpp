#include "Driver.hpp"
#include "OcfObj1.hpp"
#include "OcfObj2.hpp"

// SetPointPathAndProperties() method is where the Path, Point, and
// OptimalControlFunction classes will be instantiated.
void Driver::SetPointPathAndProperties() {
	pathObject = new PathObj();
	pointObject = new PointObj();

	// Set CSALT and SNOPT-related parameters in this method
	// See CsaltDriver.hpp for some of the available options
	maxMeshRefinementCount = 25;
	feasibilityTolerances(0) = 1.0e-6;
	optimizationMode[0] = "Maximize";
}

void Driver::SetupPhases() {
	//Set mesh properties
	RadauPhase* phase1 = new RadauPhase();
	// Initial guess modes: "LinearNoControl", "LinearUnityControl",
	// "LinearCoast", "OCHFile", "GuessArrays"
	std::string initialGuessMode = "LinearUnityControl";
	Rvector meshIntervalFractions(5, -1.0, -0.5, 0.0, 0.5, 1.0);

	IntegerArray meshIntervalNumPoints = { 12,12,12,12 };

	//Set time properties
	Real timeLowerBound = 0.0;
	Real timeUpperBound = 100.0;
	Real initialGuessTime = 0.0;
	Real finalGuessTime = 50.0;

	//Set state properties
	Integer numStateVars = 4;
	Rvector stateLowerBound(4, 0.0, -6.0 * 2.0 * GmatMathConstants::PI, -10.0, -10.0);
	Rvector initialGuessState(4, 1.1, 0.0, 0.0, 1.0 / sqrt(1.1));
	Rvector finalGuessState(4, 5.0, 3.0 * 2.0 * GmatMathConstants::PI, 1.0, 1.0);
	Rvector stateUpperBound(4, 10.0, 6.0 * 2.0 * GmatMathConstants::PI, 10.0, 10.0);

	//Set control properties
	Integer numControlVars = 1;
	Rvector controlUpperBound(1, 10.0);
	Rvector controlLowerBound(1, -10.0);

	// Set phase options
	phase1->SetInitialGuessMode(initialGuessMode);
	phase1->SetNumStateVars(numStateVars);
	phase1->SetNumControlVars(numControlVars);
	phase1->SetMeshIntervalFractions(meshIntervalFractions);
	phase1->SetMeshIntervalNumPoints(meshIntervalNumPoints);
	phase1->SetStateLowerBound(stateLowerBound);
	phase1->SetStateUpperBound(stateUpperBound);
	phase1->SetStateInitialGuess(initialGuessState);
	phase1->SetStateFinalGuess(finalGuessState);
	phase1->SetTimeLowerBound(timeLowerBound);
	phase1->SetTimeUpperBound(timeUpperBound);
	phase1->SetTimeInitialGuess(initialGuessTime);
	phase1->SetTimeFinalGuess(finalGuessTime);
	phase1->SetControlLowerBound(controlLowerBound);
	phase1->SetControlUpperBound(controlUpperBound);

	// Set phase to phaseList vector data type
	phaseList.push_back(phase1);
}

void Driver::PostProcess(Trajectory* traj) {
}