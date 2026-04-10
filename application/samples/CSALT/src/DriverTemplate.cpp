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
	// ocfObjects.push_back(new OcfObj1("OcfObj1"));
	// ocfObjects.push_back(new OcfObj2("OcfObj2")); // Note: example. not included above

	// Set CSALT and SNOPT-related parameters in this method
	// See CsaltDriver.hpp for some of the available options
	maxMeshRefinementCount = 25;
	feasibilityTolerances(0) = 1.0e-6;
	optimizationMode[0] = "Minimize";
}

void Driver::SetupPhases() {
	//Set mesh properties
	RadauPhase* phase1 = new RadauPhase();
	// Initial guess modes: "LinearNoControl", "LinearUnityControl",
	// "LinearCoast", "OCHFile", "GuessArrays"
	std::string initialGuessMode =
	Rvector meshIntervalFractions(
	IntegerArray meshIntervalNumPoints =

	//Set time properties
	Real timeLowerBound =
	Real timeUpperBound =
	Real initialGuessTime =
	Real finalGuessTime =

	//Set state properties
	Integer numStateVars =
	Rvector stateLowerBound(
	Rvector stateUpperBound(
	Rvector initialGuessState(
	Rvector finalGuessState(

	//Set control properties
	Integer numControlVars =
	Rvector controlUpperBound(
	Rvector controlLowerBound(

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

	// Continue if there are more phases
}