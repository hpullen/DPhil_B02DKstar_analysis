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

# Two body SYS
R_plus_piK = ufloat(0.0642, 0.0045)
R_minus_piK = ufloat(0.0950, 0.0028)
print_results("piK", R_plus_piK, R_minus_piK)
    
# Four body SYS
R_plus_piKpipi = ufloat(0.0738, 0.0031)
R_minus_piKpipi = ufloat(0.0723, 0.0026)
print_results("piKpipi", R_plus_piKpipi, R_minus_piKpipi)
