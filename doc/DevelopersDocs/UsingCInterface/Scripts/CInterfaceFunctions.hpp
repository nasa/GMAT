const char *getLastMessage();
Retrieves the most recent message from the GMAT engine

int StartGmat();
Loads GMAT into memory and initializes the system

int LoadScript(const char* scriptName);
Loads a script into the GMAT engine

int RunScript();
Populates a GMAT Sandbox with the scripted objects,
establishes interconnects between those objects, and 
executes the Mission Control Sequence (unless a 
PrepareMissionSequence command is used to start the 
timeline).

int LoadAndRunScript(const char* scriptName);
Combines the LoadScript and RunScript commands into a 
single function.

int FindOdeModel(const char* modelName);
Retrieves an initialized ODEModel from GMAT's Sandbox 
for use by the interface. 

int GetStateSize();
Determines the size of the state vector in a retreived
ODEModel.

const char *GetStateDescription();
Gets a listing of the elements in the state vector.

int SetState(double epoch, double state[], int stateDim);
Sets the first stateDim elements of the state vector at the 
input epoch.

double *GetState();
Retrieves the state vector.

double *GetDerivativesForState(double epoch, double state[], 
      int stateDim, double dt, int order, int *pdim);
Retrieves the derivative data for an input state vector, as 
defined for the differential equations in the ODEModel.

double *GetDerivatives(double dt, int order, int *pdim);
Retrieves the derivative data for the internal state vector, 
as defined for the differential equations in the ODEModel.

int CountObjects();
Retrieves the number of scripted objects in the GMAT engine.

const char *GetObjectName(int which);
Retrieves the name of an object given its index.

const char *GetRunSummary();
Retrieves a listing of the results of a run.