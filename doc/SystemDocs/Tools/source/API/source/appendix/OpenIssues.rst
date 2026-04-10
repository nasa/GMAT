Open issues
===========

*  Should the util library be wrapped separately from the base library?

   **Resolution**: The util code is included in the core GMAT API packaging.

*  When will base be made into smaller components?

   * May be useful for on board project
   * Helps ensure modularity

   **Resolution**: There is no repackaging plan at this

*  The CMake files need some work, so that the wrappers are written into the 
   build folder (application/bin by default).

   **Resolution**: The current best practice is to build a GMAT release to get
   the API package set.

*  Is there a tie-in between GMAT/CSALT on board and the GMAT API?

   **Resolution**: Not at this time.

*  Are there export issues between the languages?  (e.g. Java supports x but not 
   Python)

   **Resolution**: Language differences are addressed in this document.

*  Is there a way to identify "approved" API features from those not yet tested?

   * Tell SWIG to expose only some pieces of a class without a lot of hand edits?
   * Tell Doxygen to make an API specific build?
   * Identify API pieces in the full Doxygen build?

   **Resolution**: This item is still under consideration.  There is no API 
   marking of features at this time.

*  GMAT already has a ``Create`` object, producing a name conflict with the API 
   ``Create`` function.  For now, I'm using ``Construct``. 

   **Resolution**: Construct is the API creation mechanism.
  
*  We need to report when things fail. Example: Setup("ThisIsntAStartupFile.txt")
   fails to initialize the Moderator.

   **Resolution**: This is currently ad hoc: issues GMAT encountered that throw
   exceptions are reported, but general failures are not.