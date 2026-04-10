.. _Repositories:

*********************************
Committing and Synchronizing Code
*********************************
Source code for GMAT is maintained in a set of repositories, used to manage the core system code, plugins components, and proprietary add on pieces used at GSFC.  The core code and plugins are managed in the main GMAT repository, and are used for the public releases of the system.  The components in the proprietary - or internal - repository cannot be released to the public, either because of licensing issues or because the contents of those components are controlled under International Traffic in Arms Regulations (ITAR) or related release restrictions.  The OpenFramesInterface component is managed external to GSFC by Emergent Space Technologies, and stored in a third repository.



Branch Management
=================
The GMAT system code is managed in the Dev branches of the main and internal GMAT repositories located on the GitLab server at GSFC.  These branches are used to build the system for the script and GUI test runs, and contain the production code for GMAT daily and release builds.  Developers clone these repositories and work in their local clones, synchronizing their local code by pulling from the GitLab servers periodically.  Code is pushed from the developer's machine to the GitLab servers less frequently, as described below.

New GMAT code is developed in a branch made from the Dev branches.  Branches used for feature development are only pushed to the central repository at GitLab when the code needs to be shared between developers or when the development cycle is long and a backup of the new code is desired.  Once development is complete, the new code is merged into the developer's local Dev branch, tested, and then pushed to the central repositories.

The typical repository workflow for GMAT development for a developer with a configured system [#]_ is

#. The developer updates the local Dev branches on the development workstation by pulling from the central GMAT repositories.
#. The developer creates a branch of the code that will be updated based on the current Dev branch.  Development then proceeds as follows:
	
	#. The developer makes code changes.  As development proceeds, commits are made to the local branch so that the developer can recover from inadvertent changes, and so that the team can track the source of side effects at a granular level if they are not caught during development.
	#. The developer runs tests from time to time as needed, either informally or more formally in the test system.
	#. If code sharing is needed, the developer pushes the local branch to the central repository.
	#. For long development cycles, the developer pulls the Dev branch from the central repository and merges it into the new branch so that code remains synchronized across the team.
	#. This process repeats until the code is ready to be incorporated into the central repository.

#. When the code is ready for incorporation into the central repository, the developer proceeds as follows [#]_ :

	#. The developer pulls the Dev branch from the central repository.
	#. The developer merges the updated Dev branch code into the the feature branch.
	#. The developer builds and tests the merged code.

		#. The developer runs smoke tests for the local build.
		#. The developer runs additional tests impacted by the new feature to ensure that there are no unexpected side effects from the code changes.

	#. If testing shows that the code is ready for incorporation into the central repository:

		#. The developer merges the branch for the new feature into the local Dev branch(es).  This should be a "fast forward" merge at this point.
		#. The developer pulls the Dev branch from the central repository.  If there are code changes from that pull, the developer builds and tests (running smoke tests as a minimum) to ensure that the code is still ready for release to the central repository.
		#. If there are not code updates, or if tests pass as expected, teh Dev branch(es) are pushed to the central repository at GSFC, completing the development cycle.

.. [#] Here a configured system is a workstation with clones of the repositories in place.  The system should be set up for development, though the actual coding and building steps are outside of the scope of the repository management tasks described here.

.. [#] The procedure outlined here is the rigorous process for features that occur while other changes are made to the system.  They may be shortened based on current circumstances - for example, the merge of Dev to local branch, test, merge local to Dev, test, then push to GSFC can be shortened to a single test cycle if changes from other developers are small or if they do not affect the changes made on the developer's system.

Minor/Cosmetic Updates
----------------------
Minor bug fixes may be made directly to the Dev branch when the fix has cosmetic or very limited potential impact on the system.  An example of a minor fix is one that requires less time to implement than the time taken to create a branch, make changes, and synchronize everything following the procedure outlined above.
