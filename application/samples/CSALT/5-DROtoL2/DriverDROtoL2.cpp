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
    maxMeshRefinementCount = 6;
    majorOptimalityTolerances = Rvector(1, 1.0e-2);
    feasibilityTolerances = Rvector(1, 1.0e-5);
}

void Driver::SetupPhases() {
    //Set mesh properties
    RadauPhase* phase1 = new RadauPhase();
    // Initial guess modes: "LinearNoControl", "LinearUnityControl",
    // "LinearCoast", "OCHFile", "GuessArrays"
    std::string initialGuessMode = "GuessArrays";
    Rvector meshIntervalFractions(10);
    for (int i = 0; i < 10; ++i)
        meshIntervalFractions(i) = -1.0 + 2.0 / 9.0 * i;    // linspace equivalent

    IntegerArray meshIntervalNumPoints{ 5, 5, 5, 5, 5, 5, 5, 5, 5 };

    //Set time properties
    Real timeLowerBound = 0.0;
    Real timeUpperBound = 100.0;
    Real initialGuessTime = 0.0;
    Real finalGuessTime = 4.50492518;

    //Set state properties
    Integer numStateVars = 7;
    Rvector stateLowerBound(7, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, 1.0);
    Rvector stateUpperBound(7, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 1.5);
    Rvector initialGuessState(7, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 1.2);
    Rvector finalGuessState(7, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.2);


    //Set control properties
    Integer numControlVars = 3;
    Rvector controlUpperBound(3, 1.0, 1.0, 1.0);
    Rvector controlLowerBound(3, -1.0, -1.0, -1.0);

    // Obtain initial guess for time, state, and controls from data files
    Integer n = 150;
    Rvector timeArray(n);
    Rmatrix stateArray(n, 7);
    Rmatrix controlArray(n, 3);
    FILE* fidt, * fidx, * fidu;
    fidt = fopen("t_guess.dat", "r");
    fidx = fopen("x_guess.dat", "r");
    fidu = fopen("u_guess.dat", "r");
    for (int i = 0; i < n; ++i) {
        (void)fscanf(fidt, "%lf", &timeArray(i));
        (void)fscanf(fidx, "%lf %lf %lf %lf %lf %lf %lf", &stateArray(i, 0),
            &stateArray(i, 1), &stateArray(i, 2), &stateArray(i, 3),
            &stateArray(i, 4), &stateArray(i, 5), &stateArray(i, 6));
        (void)fscanf(fidu, "%lf %lf %lf", &controlArray(i, 0),
            &controlArray(i, 1), &controlArray(i, 2));
    }
    fclose(fidt);
    fclose(fidx);
    fclose(fidu);

    // Set phase options
    phase1->SetInitialGuessMode(initialGuessMode);
    phase1->SetInitialGuessArrays(timeArray, stateArray, controlArray);
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