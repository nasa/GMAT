

%==========================================================================
%==========================================================================
%----   Intialize the Object Store
%==========================================================================
%==========================================================================

- Add objects to object store
- Create mapping of names to objects
- Create mapping of object types

%==========================================================================
%==========================================================================
%----   Intialize the sand box
%==========================================================================
%==========================================================================

%-----  Estimator_Initialize
- Set pointers to participants
- Determine objects that we will need to propagate and create pointer list
  list of parameter Ids for each parameter to be propagated on each object
- Determine objects that we will need to estimate and create pointer list
  list of parameter Ids for each parameter to be propagated on each object
- Determine size of state vector and dimenion ESV and STM

%----- Initialize the RunEstimator Command
- Call ESM on the estimator to get objects to get PropObject pointer
  list and parameter Ids for each prop object
- Determine size of state vector and deminion it accordingly.
- Configure derivative map

%==========================================================================
%==========================================================================
%----   Intializing the RunEstimator command
%==========================================================================
%==========================================================================

%--------------------------
%  Estimator_PreparetoSolve
%--------------------------

-  Populate the ESV with data


%--------------------------
%  Propagator_PreparetoPropagate
%--------------------------

-  Populate the PSV with data