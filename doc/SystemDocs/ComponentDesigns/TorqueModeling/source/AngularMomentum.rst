*************************
Angular Momentum Modeling
*************************

:ref:`TorqueModeling` describes the design of GMAT's torque model implementation.  GMAT does not integrate torques in its dynamics modeling at this writing.  However, the torques computed in GMAT can be written to reports and processed externally to compute the angular momentum change due to torques acting on a spacecraft.  This chapter documents one approach to this calculation, implemented in Python.

Background
==========
The angular momentum :math:`textbf{h}` of a spacecraft in the absence of torques is a conserved quantity.  Torques impart a change in the angular momentum through the relationship

.. math::
    :label: hDot

    \dot{\textbf{h}} = \boldsymbol{\tau}_T

Integrating produces the angular momentum change across a given time interval:

.. math::
    :label: DeltaH

    \Delta \textbf{h} = \int_{ti}^{tf} \boldsymbol{\tau}_T(t) \,dt

The scripting provided in this chapter implements this relationship by generating a time history of torque data in GMAT, and then integrating through this history in Python.

GMAT Scripting
==============
**To be written during the torque model coding cycle**

Python Post Processing
======================
**To be written during the torque model coding cycle**
