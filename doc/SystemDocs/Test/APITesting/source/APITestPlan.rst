.. _ApiFunctionTests:

******************
GMAT API Functions
******************
The API provides a set of high-level functions designed to make the access and use of GMAT objects simple for people working in the API platforms.  These functions provide the core interface into GMAT for users that want to make GMAT objects and work with them for analysis.  As a central new feature of the GMAT API, these functions need to be exercised extensively in the API tests.

This chapter describes the new API specific functions.


API Functions
=============
The GMAT API adds functions designed to facilitate and simplify use of the GMAT code from the target platforms.  :numref:`APIFunctions` lists functions intended to assist API users that need help starting new projects.  


.. _APIFunctions:
.. table:: Top level functions supplied by the GMAT API
   :class: longtable
   :widths: 15 15 25 45

   ============== =============== ======================= ========================================================
   Function       Fields          Examples                Description/Items to Check
   ============== =============== ======================= ========================================================
   | Help         | *none*        | Help()                Provides help to the user for select items
                  | Commands      | Help("Commands")        
                  | Groups        | Help("MySat")         #. Does help text display?
                  | Objects                               #. Is the text correct?
                  | ScriptUsage                           #. Is the grammar/spelling correct and professional?
                  | Object name                           #. Is there additional information that would improve
                                                             the contents?
   -------------- --------------- ----------------------- --------------------------------------------------------
   | ShowClasses  | *none*        | ShowClasses()         Lists all available API classes
                  | Class name    | ShowClasses("Solver")   
                                                          #. Is the list complete?
                                                          #. Are subclasses from plugins listed?
                                                          #. Are there intermediate subclasses that should be 
                                                             listed but are missing?
                                                          #. Is there a better way to present this information?
                                                          #. Is the behavior for unknown class types acceptable?
   -------------- --------------- ----------------------- --------------------------------------------------------
   | ShowObjects  | *none*        | ShowObjects()           Shows the objects that exist in the GMAT API
                  | Class name    | ShowObjects("Burn")     
                                                          #. Are all objects listed if the type is empty?
                                                          #. Are subclass categories handled correctly?
   ============== =============== ======================= ========================================================

API users familiar with the system use the commands in :numref:`ObjectFunctions` to create the objects they need for their analysis problem, and to prepare those objects for use.

.. _ObjectFunctions:
.. table:: Functions used to work with GMAT Objects
   :class: longtable
   :widths: 15 15 25 45

   =========== =============== ========================== =================================================
   Function    Fields          Example(s)                 Description/Items to Check
   =========== =============== ========================== =================================================
   Setup       *none*          Setup()                    *Deprecated* Prepares the API for use.  
                                                          Setup occurs automatically now.

                                                          *Not sure there is anything to do here*
   ----------- --------------- -------------------------- -------------------------------------------------
   Construct   Class name,     Construct("Spacecraft",    Builds a GMAT object with a name
               instance name             "MySat")
                                                          #. Is the object created?
                                                          #. Are incorrect entries handled gracefully? 
                                                             (e.g. If the class name is misspelled)?
                                                          #. Are repeated names handles gracefully? (e.g. 
                                                             what happens if an object is constructed with 
                                                             the same name as an existing object?) 
   ----------- --------------- -------------------------- -------------------------------------------------
   Initialize  *none*          Initialize()               Prepares the created GMAT objects for use

                                                          #. Are internal references set up as expected?
   ----------- --------------- -------------------------- -------------------------------------------------
   Copy                        Copy("MySat", "Sat2")      Makes a copy of a GMAT object

                                                          #. Is the copy created?
                                                          #. Does the copy have the same data as the 
                                                             original?
                                                          #. Are naming issues handled gracefully?
   ----------- --------------- -------------------------- -------------------------------------------------
   GetObject   Instance name   GetObject("MySat")         Retrieves a GMAT object

                                                          #. Is the expected object returned?
                                                          #. Can the returned object be changed?
                                                          #. Are the changes saved for subsequent use?
   ----------- --------------- -------------------------- -------------------------------------------------
   Clear       Instance name   Clear()                    Removes selected or all GMAT objects
                               Clear("Spacecraft")         
                               Clear("MySat")             #. Are objects removed from the configuration
                                                             as expected?
                                                          #. If a user tries to use a cleared object, what 
                                                             happens?
   =========== =============== ========================== =================================================

