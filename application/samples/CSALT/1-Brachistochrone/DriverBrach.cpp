#include "Driver.hpp"
#include "OcfObj1.hpp"
#include "OcfObj2.hpp"

// SetPointPathAndProperties() method is where the Path, Point, and
// OptimalControlFunction classes will be instantiated.
void Driver::SetPointPathAndProperties() {
	pathObject = new PathObj();
	pointObject = new PointObj();

	// Set CSALT and SNOPT-related parameters in this method
	// See CsaltDriver.hpp for some of the available
	maxMeshRefinementCount = 25;
	feasibilityTolerances(0) = 1.0e-6;
	optimizationMode[0] = "Minimize";
}

void Driver::SetupPhases() {
	//Set mesh properties
	RadauPhase* phase1 = new RadauPhase();
	// Initial guess modes: "LinearNoControl", "LinearUnityControl",
	// "LinearCoast", "OCHFile", "GuessArrays"
	std::string initialGuessMode = "LinearNoControl";
	Rvector meshIntervalFractions(3, -1.0, 0.0, 1.0);

	IntegerArray meshIntervalNumPoints = { 5, 5 };

	//Set time properties
	Real timeLowerBound = 0.0;
	Real timeUpperBound = 100.0;
	Real initialGuessTime = 0.0;
	Real finalGuessTime = 0.3;

	//Set state properties
	Integer numStateVars = 3;
	Rvector stateLowerBound(3, -10.0, -10.0, -10.0);
	Rvector stateUpperBound(3, 10.0, 0.0, 0.0);
	Rvector initialGuessState(3, 0.0, 0.0, 0.0);
	Rvector finalGuessState(3, 2.0, -1.0, -1.0);

	//Set control properties
	Integer numControlVars = 1;
	Rvector controlUpperBound(1, 10.0);
	Rvector controlLowerBound(1, -10.0);

	phase1->SetInitialGuessMode(initialGuessMode);
	// Needed if initialGuessMode is "GuessArrays"
	// phase1->SetInitialGuessArrays(
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