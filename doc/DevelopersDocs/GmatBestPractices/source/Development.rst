*****************************
Development Workflow for GMAT
*****************************

GMAT development is performed in cycles, typically building a core capability, and then iterating on it until the feature under development is ready for use.  Implemented features may then be found to need updating, either to add new capabilities or to correct defects uncovered during testing or use.  The general approach is this:

* Determine what needs to be done, either formally or informally
	* The sections below describe this process in several categories.
* Open a ticket in GMAT's issue and project tracking software (currently JIRA)
	* Follow GMAT's standard issue tracking templates as closely as possible
	* Mark the ticket as open and assign it a priority
	* Assign the ticket to the appropriate sprint
* When work begins 
	* Assign the ticket to the person responsible for working on it
	* Mark the ticket as "In Progress" once active development begins
* Update the ticket as work progresses, checking off work items as they are completed
	* Code management for the work is described in :ref:'Repositories'.
	* Developer testing is described in :ref:`Testing`.
	* Completion includes:
		* Updating documentation as needed
		* Performing developer tests
		* Adding the code to the GMAT repositories used to build the system for the nightly test runs
* Upon completion, mark the ticket as "Resolved" and assign it to a colleague to confirm that work is complete
	* The "colleague" might be a tester, the user requesting the work, the reporter of the issue, or another interested party.
	* Tell the colleague that the update is ready for review.
* After review, the ticket is closed by the reviewer.


New Feature Development
=======================
A typical development cycle for new GMAT code follows the cycle

#. Requirements are defined for a feature by the users that need the feature.
#. A design is developed that addresses these requirements.
#. The users that generated the requirements review the design for completeness and usability.


Feature Updates and Bug Fixes
=============================



Tips
====

* GMAT copies objects repeatedly when running a script.  Be sure to check the following elements of any new classes or class members:
	* Are all data members initialized in the default constructor?
	* Are members copied or reset in the copy constructor and assignment operator?
	