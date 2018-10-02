"""This file is an example of a user-defined binning scheme file, which """ \
"""can be passed as an argument to the multi-track calibration scripts.
The methods for constructing binning schema are defined in """ \
"""$PIDPERFSCRIPTSROOT/python/PIDPerfScripts/binning.py."""

from PIDPerfScripts.Binning import *
from PIDPerfScripts.Definitions import *

for trType in ["K", "Pi"]:
    # momentum                                                                                                                         
    AddBinScheme(trType, 'P', 'AltBinning', 3000, 100000)
    AddUniformBins(trType, 'P', 'AltBinning', 30, 3000, 100000)
    AddBinScheme(trType, 'Brunel_P', 'AltBinning', 3000, 100000)
    AddUniformBins(trType, 'Brunel_P', 'AltBinning', 30, 3000, 100000)

    # eta                                                                                                                              
    AddBinScheme(trType, 'ETA', 'AltBinning', 1.5, 5.0)
    AddUniformBins(trType, 'ETA', 'AltBinning', 8, 1.5, 5.0)
    AddBinScheme(trType, 'Brunel_ETA', 'AltBinning', 1.5, 5.0)
    AddUniformBins(trType, 'Brunel_ETA', 'AltBinning', 8, 1.5, 5.0)
