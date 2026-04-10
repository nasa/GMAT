# __init__.py for GMAT Python API
# Standard use: "import gmatpy as gmat"
# Alternate use: "from gmatpy import *" (Python does not recommend this usage)

import os, platform, sys

filePath = os.path.dirname(os.path.abspath(__file__))
binPath = os.path.dirname(filePath)
sys.path.append(filePath)

# On Windows, load GMAT dependencies before importing API
if platform.system() == "Windows":
  from ctypes import cdll

  cdll.LoadLibrary(os.path.normpath(os.path.join(binPath, "libGmatUtil")))
  cdll.LoadLibrary(os.path.normpath(os.path.join(binPath, "libGmatBase")))
  cdll.LoadLibrary(os.path.normpath(os.path.join(binPath, "../plugins/libStation")))
  cdll.LoadLibrary(os.path.normpath(os.path.join(binPath, "../plugins/libGmatEstimation")))

  del cdll
del platform

# Define modules to import if user does "from gmatpy import *"
__all__ = ["gmat_py", "station_py", "navigation_py"]

# Programmatically import symbols from GMAT API into this module
# This is the equivalent of "from _pyXY import *"
# https://stackoverflow.com/questions/41990900/what-is-the-function-form-of-star-import-in-python-3
try:
  # Try to import the GMAT API implementation for the current Python version
  gmatpyver = f"_py{sys.version_info.major}{sys.version_info.minor}"
  gmatpy_impl = __import__(gmatpyver, fromlist=['*'])
except ImportError:
  print(f"GMAT API for Python {sys.version_info.major}.{sys.version_info.minor} not found or could not be loaded.")
  raise

if hasattr(gmatpy_impl, '__all__'):
  all_names = gmatpy_impl.__all__
else:
  all_names = [name for name in dir(gmatpy_impl) if not name.startswith('_')]
globals().update({name: getattr(gmatpy_impl, name) for name in all_names})

# Initialize GMAT's FileManager
fileManager = FileManager.Instance()
fileManager.SetBinDirectory("gmat_startup_file.txt", binPath + os.sep)
