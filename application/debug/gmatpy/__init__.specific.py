# __init__.py for GMAT Python API, used in version-specific Python folders
# e.g. bin/gmatpy/_py39

import os
import sys

# Add current directory to path so Python can find modules that use local imports
filePath = os.path.dirname(os.path.abspath(__file__))
sys.path.append(filePath)

from .gmat_py import *
from .station_py import *
from .navigation_py import *
