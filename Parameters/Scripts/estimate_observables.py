import numpy as np

def radians(val):
    return val * np.pi / 180.0



# List of physics parameters
gamma = radians(74.0)
r_B = 0.221
delta_B = radians(187.0)
r_D = 0.0591
delta_D = radians(188.0)
kappa = 0.95
r_D_K3pi = 0.0549
delta_D_K3pi = radians(128.0)
kappa_K3pi = 0.43
F_4pi = 0.769
r_B_DKpipi = 0.081
delta_B_DKpipi = radians(351.0)
r_B_s = 0.057


# Calculate my parameters of interest
def prval(name, value):
    print("{0}: {1}".format(name, value))


# ADS ratios
prval("R_plus", (r_B**2 + r_D**2 + 2*kappa*r_B*r_D*np.cos(delta_B + delta_D + gamma)) / (1 + r_B**2 * r_D**2 + 2*kappa*r_B*r_D*np.cos(delta_B - delta_D + gamma)))
prval("R_plus without denominator", r_B**2 + r_D**2 + 2*kappa*r_B*r_D*np.cos(delta_B + delta_D + gamma))
prval("R_minus", (r_B**2 + r_D**2 + 2*kappa*r_B*r_D*np.cos(delta_B + delta_D - gamma)) / (1 + r_B**2 * r_D**2 + 2*kappa*r_B*r_D*np.cos(delta_B - delta_D - gamma)))
print("")

# GLW parameters
prval("R_CP", 1 + r_B**2 + 2*kappa*r_B*np.cos(delta_B)*np.cos(gamma))
prval("R_CP with denominator", (1 + r_B**2 + 2*kappa*r_B*np.cos(delta_B)*np.cos(gamma)) / (1 + r_B**2 * r_D**2 + 2*kappa*r_B*r_D*np.cos(delta_B - delta_D)*np.cos(gamma)))
prval("A_CP", 2*kappa*r_B*np.sin(delta_B)*np.sin(gamma))
prval("A_CP with denominator", (2*kappa*r_B*np.sin(delta_B)*np.sin(gamma)) / (1 + r_B**2 + 2*kappa*r_B*np.cos(delta_B)*np.cos(gamma)))
print("")

# 4-body ADS
prval("R_plus_K3pi", (r_B**2 + r_D_K3pi**2 + 2*kappa*kappa_K3pi*r_B*r_D_K3pi*np.cos(delta_B + delta_D_K3pi + gamma)) / (1 + r_B**2 * r_D_K3pi**2 + 2*kappa*kappa_K3pi*r_B*r_D_K3pi*np.cos(delta_B - delta_D_K3pi + gamma)))
prval("R_plus_K3pi without denominator", r_B**2 + r_D_K3pi**2 + 2*kappa*kappa_K3pi*r_B*r_D_K3pi*np.cos(delta_B + delta_D_K3pi + gamma)) 
prval("R_minus_K3pi", (r_B**2 + r_D_K3pi**2 + 2*kappa*kappa_K3pi*r_B*r_D_K3pi*np.cos(delta_B + delta_D_K3pi - gamma)) / (1 + r_B**2 * r_D_K3pi**2 + 2*kappa*kappa_K3pi*r_B*r_D_K3pi*np.cos(delta_B - delta_D_K3pi - gamma)))
print("")

# 4-body GLW
prval("R_CP_4pi", 1 + r_B**2 + 2*kappa*(2*F_4pi - 1)*r_B*np.cos(delta_B)*np.cos(gamma))
prval("R_CP_4pi", 1 + r_B**2 + 2*kappa*(2*F_4pi - 1)*r_B*np.cos(delta_B)*np.cos(gamma))
prval("A_CP_4pi", 2*kappa*(2*F_4pi - 1)*r_B*np.sin(delta_B)*np.sin(gamma))
print("")

# Back-calculate kappa_DKpipi
R_plus_DKpipi = 107*10e-4
R_minus_DKpipi = 53*10e-4
prval("kappa_DKpipi from R+", (R_plus_DKpipi - r_B_DKpipi**2 - r_D**2) / (2*r_B_DKpipi*r_D*np.cos(delta_B_DKpipi + delta_D + gamma)))
prval("kappa_DKpipi from R-", (R_minus_DKpipi - r_B_DKpipi**2 - r_D**2) / (2*r_B_DKpipi*r_D*np.cos(delta_B_DKpipi + delta_D - gamma)))
print("")

# Estimate ADS ratios for Bs
prval("R_plus_Bs", (r_B_s**2 + r_D**2 + 2*kappa*r_B_s*r_D*np.cos(delta_B + delta_D + gamma)) / (1 + r_B_s**2 * r_D**2 + 2*kappa*r_B_s*r_D*np.cos(delta_B - delta_D + gamma)))
prval("R_minus_Bs", (r_B_s**2 + r_D**2 + 2*kappa*r_B_s*r_D*np.cos(delta_B + delta_D - gamma)) / (1 + r_B_s**2 * r_D**2 + 2*kappa*r_B_s*r_D*np.cos(delta_B - delta_D - gamma)))
print("")

