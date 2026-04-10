# stateTypes.py

# Holds state type enumeration values.

# Revision history:
# 2019-03-06; Noble Hatten; Created. Class was moved out of StateConversions.py


from enum import Enum # requires python 3.4+

class stateTypes(Enum):
    Cartesian = 1
    ClassicalKeplerian = 2
    SphericalAzFPA = 4