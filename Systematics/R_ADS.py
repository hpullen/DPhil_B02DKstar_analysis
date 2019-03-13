# Calculate R_ADS and A_ADS
from uncertainties import ufloat
from uncertainties.umath import *

# For one set of R+, R-
def print_results(name, R_plus, R_minus):
    print "\n{}:".format(name)
    R_ADS = 0.5 * (R_plus + R_minus)
    print "R_ADS = {} +/- {}".format(R_ADS.nominal_value, R_ADS.std_dev)
    A_ADS = (R_minus - R_plus)/(R_minus + R_plus)
    print "A_ADS = {} +/- {}".format(A_ADS.nominal_value, A_ADS.std_dev)

# Two body
R_plus_piK = ufloat(0.065, 0.021)
R_minus_piK = ufloat(0.098, 0.021)
print_results("piK", R_plus_piK, R_minus_piK)
    
# Four body
R_plus_piKpipi = ufloat(0.073, 0.026)
R_minus_piKpipi = ufloat(0.072, 0.025)
print_results("piKpipi", R_plus_piKpipi, R_minus_piKpipi)
