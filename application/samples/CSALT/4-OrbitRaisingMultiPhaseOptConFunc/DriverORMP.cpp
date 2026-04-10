#include "Driver.hpp"
#include "OcfObj1.hpp"
#include "OcfObj2.hpp"

// SetPointPathAndProperties() method is where the Path, Point, and
// OptimalControlFunction classes will be instantiated.
void Driver::SetPointPathAndProperties() {
	pathObject = new PathObj();
	pointObject = new PointObj();
	// Add any optional OptimalControlFunction classes to the ocfObjects vector
	// variable type. Order does not matter
	ocfObjects.push_back(new OcfObj1("OcfObj1"));

	// Set CSALT and SNOPT-related parameters in this method
	// See CsaltDriver.hpp for options
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
	Rvector meshIntervalFractions(2, -1.0, 1.0);
	IntegerArray meshIntervalNumPoints = { 7 };

	//Set time properties
	Real timeLowerBound = 0.0;
	Real timeUpperBound = 3.32;
	Real initialGuessTime = 0.0;
	Real finalGuessTime = 1.0;

	//Set state properties
	Real cpi = GmatMathConstants::PI;
	Integer numStateVars = 5;
	Rvector stateLowerBound(5, 0.5, 0.0, -10.0, -10.0, 0.1);
	Rvector stateUpperBound(5, 5.0, 4.0 * cpi, 10.0, 10.0, 1.0);
	Rvector initialGuessState(5, 1.0, 0.0, 0.0, 1.0, 1.0);
	Rvector finalGuessState(5, 1.0, 0.5, 0.0, 1.0, 1.0);

	//Set control properties
	Integer numControlVars = 2;
	Rvector controlUpperBound(2, 10.0, 10.0);
	Rvector controlLowerBound(2, -10.0, -10.0);

	// Set phase options
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

	RadauPhase* phase2 = new RadauPhase();
	// Initial guess modes: "LinearNoControl", "LinearUnityControl",
	// "LinearCoast", "OCHFile", "GuessArrays"
	std::string initialGuessMode2 = "LinearUnityControl";
	Rvector meshIntervalFractions2(2, -1.0, 1.0);
	IntegerArray meshIntervalNumPoints2 = { 7 };

	//Set time properties
	Real timeLowerBound2 = 0.0;
	Real timeUpperBound2 = 3.32;
	Real initialGuessTime2 = 1.0;
	Real finalGuessTime2 = 3.32;

	//Set state properties
	Integer numStateVars2 = 5;
	Rvector stateLowerBound2(5, 0.5, 0.0, -10.0, -10.0, 0.1);
	Rvector stateUpperBound2(5, 5.0, 4.0 * cpi, 10.0, 10.0, 1.0);
	Rvector initialGuessState2(5, 1.0, 0.5, 0.0, 1.0, 1.0);
	Rvector finalGuessState2(5, 1.0, cpi, 0.0, 1.0, 1.0);

	//Set control properties
	Integer numControlVars2 = 2;
	Rvector controlUpperBound2(2, 10.0, 10.0);
	Rvector controlLowerBound2(2, -10.0, -10.0);

	// Set phase options
	phase2->SetInitialGuessMode(initialGuessMode2);
	// Needed if initialGuessMode is "GuessArrays"
	// phase2->SetInitialGuessArrays(
	phase2->SetNumStateVars(numStateVars2);
	phase2->SetNumControlVars(numControlVars2);
	phase2->SetMeshIntervalFractions(meshIntervalFractions2);
	phase2->SetMeshIntervalNumPoints(meshIntervalNumPoints2);
	phase2->SetStateLowerBound(stateLowerBound2);
	phase2->SetStateUpperBound(stateUpperBound2);
	phase2->SetStateInitialGuess(initialGuessState2);
	phase2->SetStateFinalGuess(finalGuessState2);
	phase2->SetTimeLowerBound(timeLowerBound2);
	phase2->SetTimeUpperBound(timeUpperBound2);
	phase2->SetTimeInitialGuess(initialGuessTime2);
	phase2->SetTimeFinalGuess(finalGuessTime2);
	phase2->SetControlLowerBound(controlLowerBound2);
	phase2->SetControlUpperBound(controlUpperBound2);

	// Set phase to phaseList vector data type
	phaseList.push_back(phase2);
}

void Driver::PostProcess(Trajectory* traj) {
}