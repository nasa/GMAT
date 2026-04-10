**STM and Covariance Propagation**
==================================

**Problem**
-----------
Spacecraft propagation advances the position and velocity of the spacecraft through time, and models mass flow during finite burn maneuvers.  Users may also need to propagate the spacecraft's orbital state transition matrix (STM) and the orbital covariance matrix.  This chapter shows how to add the STM and covariance propagation to the propagation example shown in :ref:`SpacecraftPropagation`.  

**Solution**
------------
The GMAT Spacecraft and propagation models work together to build and propagate the state transition and covariance matrices.

Example STM Propagation
***********************

**Step 1**: Configure the Spacecraft

We'll need a spacecraft to propagate.  :numref:`BasicSpacecraftSnippet` shows an abbreviated block of the spacecraft configuration from :ref:`SpacecraftPropagation`.

.. _BasicSpacecraftSnippet:
.. code-block:: python
    :caption: Abbreviated Spacecraft configuration (see :numref:`SpacecraftConfig` for full version)

    sat = gmat.Construct("Spacecraft", "LeoSat")
    sat.SetField("DateFormat", "UTCGregorian")
    ...
    sat.SetField("Cr", 1.8)
    sat.SetField("DragArea", 1.5)
    sat.SetField("SRPArea", 1.2)

The spacecraft initializes the state transition matrix to an identity matrix, and the covariance matrix to a 6x6 matrix with large values set nonzero along its diagonal.  The STM setting is required so that the propagated and unpropagated STM requirements are met

.. math::

    r(t1) = \Phi(t0, t1) r(t0) 

    \Phi(t0, t0) = I 

The covariance matrix is generally set to more reasonable numbers before proceeding with covariance propagation, either through an estimation process or by direct user intervention.  :numref:`CovarianceInitialization` shows the latter approach.

.. _CovarianceInitialization:
.. code-block:: python
    :caption: Covariance initialization of sat using the API

    # Initialize the covariance matrix
    sat.SetRealParameter("Covariance", 0.000001, 0, 0)
    sat.SetRealParameter("Covariance", 0.0000015, 1, 1)
    sat.SetRealParameter("Covariance", 0.000001, 2, 2)
    sat.SetRealParameter("Covariance", 2.5e-11, 3, 3)
    sat.SetRealParameter("Covariance", 2.5e-11, 4, 4)
    sat.SetRealParameter("Covariance", 2.5e-11, 5, 5)


**Step 2**: Update the propagation subsystem

GMAT's propagators configure the data that is integrated using a component called the propagation state manager (PSM).  The PSM assembles the data that is integrated into a vector that the numerical integrator uses to advance the GMAT simulation through time.  The STM and covariance matrix are set to participated in the integration through calls to the PSM, shown in :numref:`PSMSetupForSTMCovProp`.

.. _PSMSetupForSTMCovProp:
.. code-block:: python
    :caption: Propagation State Manager Configuration for Covariance Propagation

    # Setup STM and covariance propagation
    psm = pdprop.GetPropStateManager()
    psm.SetProperty("Covariance",sat)
    psm.SetProperty("STM",sat)


**Step 3**: Spacecraft Updating

The propagation chapter of this document glosses over an object synchronization item that is required for successful covariance propagation.  After a propagation step has been performed, the propagation vector managed by the PSM needs to be passed to the spacecraft object so that the internal spacecraft data is in sync with the propagation.  This synchronization is performed by directing the initialized propagator to pass the state vector into the spacecraft.  The propagator performs this action by calling methods on the PSM.  :numref:`StmCovSync` shows the calls that accomplish this data synchronization.

.. _StmCovSync:
.. code-block:: python
    :caption: Data synchronization between the propagator and the spacecraft

    # Perform top level and integrator initialization
    gmat.Initialize()
    pdprop.PrepareInternals()
    propagator = pdprop.GetPropagator()

    # Push the data into the spacecraft runtime elements
    propagator.UpdateSpaceObject()

Note, in particular, the last line in this block.  The call to UpdateSpaceObjects() is new in this chapter, and is the call that feeds the data from the propagation subsystem into the spacecraft object.

**Step 4**: Accessing the STM and Covariance

The steps shown above configure the GMAT API for STM and covariance propagation.  The data is viewed by accessing it from the spacecraft object.  The code in :numref:`StmCovarianceAccess` shows the access methods needed to use the matrix data.  The code shown here accesses the data for printing.

.. _StmCovarianceAccess:
.. code-block:: python
    :caption: Code that displays the STM and covariance to the user

    print(sat.GetName(), " State and matrix data:\n")
    print("Epoch:  ", sat.GetEpoch())
    print("Position:  ", sat.GetField("X"), ", ", sat.GetField("Y"), ", ", 
        sat.GetField("Z"))
    print("Velocity:  ", sat.GetField("VX"), ", ", sat.GetField("VY"), ", ", 
        sat.GetField("VZ"))

    # Access the state transition matrix data
    stm = sat.GetRmatrixParameter("STM")
    print("\nState Transition Matrix:\n")
    for i in range(6):
        cstr = ""
        for j in range(6):
            cstr = cstr + "    " + str(stm.GetElement(i,j))
        print(cstr)
    print()

    # Access the covariance matrix data
    cov = sat.GetCovariance().GetCovariance()
    print("\nCovariance:\n")
    for i in range(6):
        cstr = ""
        for j in range(6):
            cstr = cstr + "    " + str(cov.GetElement(i,j))
        print(cstr)

**Discussion**
--------------
This example has provides an overview of the methods used to access the STM and Covariance matrices using the GMAT API.  The appendix to this chapter contains a complete working of these features.

**Appendix: A Complete Example**
--------------------------------
.. literalinclude:: code/STMPropagation.py
