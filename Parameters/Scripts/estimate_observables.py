from uncertainties import ufloat
from uncertainties.umath import *
from numpy import pi

def radians(val):
    return val * pi / 180.0



# List of physics parameters
gamma = ufloat(radians(74.0), radians(6.4))
r_B = ufloat(0.221, 0.047)
delta_B = ufloat(radians(187.0), radians(23))
r_D = ufloat(0.0591, 0.004)
delta_D = ufloat(radians(188.0), radians(11.2))
kappa = ufloat(0.95, 0.03)
r_D_K3pi = ufloat(0.0549, 0.0006)
delta_D_K3pi = ufloat(radians(128.0), radians(28))
kappa_K3pi = ufloat(0.43, 0.17)
F_4pi = ufloat(0.769, 0.023)
r_B_DKpipi = ufloat(0.081, 0.027)
delta_B_DKpipi = ufloat(radians(351.0), radians(37))
#  r_B_s = 0.057


# Branching fractions
BF_B0 = ufloat(4.5e-5, 0.6e-5)
BF_Bs = ufloat(4.4e-4, 0.6e-4)


# Calculate my parameters of interest
def prval(name, value):
    print("{0} {1} {2}".format(name, value.nominal_value, value. std_dev))

# ADS ratios
prval("R_plus", (r_B**2 + r_D**2 + 2*kappa*r_B*r_D*cos(delta_B + delta_D + gamma)) / (1 + r_B**2 * r_D**2 + 2*kappa*r_B*r_D*cos(delta_B - delta_D + gamma)))
prval("R_minus", (r_B**2 + r_D**2 + 2*kappa*r_B*r_D*cos(delta_B + delta_D - gamma)) / (1 + r_B**2 * r_D**2 + 2*kappa*r_B*r_D*cos(delta_B - delta_D - gamma)))


# GLW parameters
#  prval("R_CP", 1 + r_B**2 + 2*kappa*r_B*cos(delta_B)*cos(gamma))
prval("R_CP", (1 + r_B**2 + 2*kappa*r_B*cos(delta_B)*cos(gamma)) / (1 + r_B**2 * r_D**2 + 2*kappa*r_B*r_D*cos(delta_B - delta_D)*cos(gamma)))
#  prval("A_CP", 2*kappa*r_B*sin(delta_B)*sin(gamma))
prval("A_CP", (2*kappa*r_B*sin(delta_B)*sin(gamma)) / (1 + r_B**2 + 2*kappa*r_B*cos(delta_B)*cos(gamma)))

# 4-body ADS
prval("R_plus_K3pi", (r_B**2 + r_D_K3pi**2 + 2*kappa*kappa_K3pi*r_B*r_D_K3pi*cos(delta_B + delta_D_K3pi + gamma)) / (1 + r_B**2 * r_D_K3pi**2 + 2*kappa*kappa_K3pi*r_B*r_D_K3pi*cos(delta_B - delta_D_K3pi + gamma)))
#  prval("R_plus_K3pi without denominator", r_B**2 + r_D_K3pi**2 + 2*kappa*kappa_K3pi*r_B*r_D_K3pi*cos(delta_B + delta_D_K3pi + gamma))
prval("R_minus_K3pi", (r_B**2 + r_D_K3pi**2 + 2*kappa*kappa_K3pi*r_B*r_D_K3pi*cos(delta_B + delta_D_K3pi - gamma)) / (1 + r_B**2 * r_D_K3pi**2 + 2*kappa*kappa_K3pi*r_B*r_D_K3pi*cos(delta_B - delta_D_K3pi - gamma)))

# 4-body GLW
prval("R_CP_4pi", 1 + r_B**2 + 2*kappa*(2*F_4pi - 1)*r_B*cos(delta_B)*cos(gamma))
prval("A_CP_4pi", 2*kappa*(2*F_4pi - 1)*r_B*sin(delta_B)*sin(gamma))

# Bs ratios
prval("R_ds", (BF_B0 / BF_Bs) * (1 + r_B**2 + 2*kappa*r_B*cos(delta_B)*cos(gamma)))
prval("R_ds_4pi", (BF_B0 / BF_Bs) * (1 + r_B**2 + 2*kappa*(2*F_4pi - 1)*r_B*cos(delta_B)*cos(gamma)))

#  # Back-calculate kappa_DKpipi
#  R_plus_DKpipi = 107*10e-4
#  R_minus_DKpipi = 53*10e-4
#  prval("kappa_DKpipi from R+", (R_plus_DKpipi - r_B_DKpipi**2 - r_D**2) / (2*r_B_DKpipi*r_D*cos(delta_B_DKpipi + delta_D + gamma)))
#  prval("kappa_DKpipi from R-", (R_minus_DKpipi - r_B_DKpipi**2 - r_D**2) / (2*r_B_DKpipi*r_D*cos(delta_B_DKpipi + delta_D - gamma)))
#  print("")

#  # Estimate ADS ratios for Bs
#  prval("R_plus_Bs", (r_B_s**2 + r_D**2 + 2*kappa*r_B_s*r_D*cos(delta_B + delta_D + gamma)) / (1 + r_B_s**2 * r_D**2 + 2*kappa*r_B_s*r_D*cos(delta_B - delta_D + gamma)))
#  prval("R_minus_Bs", (r_B_s**2 + r_D**2 + 2*kappa*r_B_s*r_D*cos(delta_B + delta_D - gamma)) / (1 + r_B_s**2 * r_D**2 + 2*kappa*r_B_s*r_D*cos(delta_B - delta_D - gamma)))
#  print("")

