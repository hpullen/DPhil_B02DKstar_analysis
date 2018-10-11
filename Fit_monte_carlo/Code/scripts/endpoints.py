import numpy as np

# Meson masses
m_B = 5279.63
m_D = 1864.83
m_Dstar = 2006.85
m_K = 895.65
m_pi = 134.98
m_gamma = 0

# Function to get endpoints for a missing particle
def get_endpoints(m_miss):

    # B0 REST FRAME
    # Calculate energies of Dstar and K
    E_K = (m_B**2 + m_K**2 -m_Dstar**2)/(2*m_B)
    E_Dstar = (m_B**2 + m_Dstar**2 -m_K**2)/(2*m_B)

    # Calculate momenta of Dstar and K
    p_K = np.sqrt(E_K**2 - m_K**2)
    p_Dstar = np.sqrt(E_Dstar**2 - m_Dstar**2)

    # DSTAR REST FRAME
    # Calculate energies of D0 and pi
    E_D = (m_Dstar**2 + m_D**2 - m_miss**2)/(2*m_Dstar)
    E_pi = (m_Dstar**2 + m_miss**2 -m_D**2)/(2*m_Dstar)

    # Calculate momenta of D0 and pi
    p_D = np.sqrt(E_D**2 - m_D**2)
    p_pi = np.sqrt(E_pi**2 - m_miss**2)

    # BOOST BACK TO B0 REST FRAME
    gamma = E_Dstar/m_Dstar
    v = p_Dstar/E_Dstar

    # Case 1: D produced in same direction to D*
    p_D_max = gamma * (p_D + v * E_D)
    E_D_max = np.sqrt(p_D_max**2 + m_D**2)

    # Case 2: D produced in opposite direction to D*
    p_D_min = gamma * (-p_D + v * E_D)
    E_D_min = np.sqrt(p_D_min**2 + m_D**2)

    # Calculate measured B0 mass in each case
    m_B_max = np.sqrt((E_D_max + E_K)**2 - (p_D_max - p_K)**2)
    m_B_min = np.sqrt((E_D_min + E_K)**2 - (p_D_min - p_K)**2)

    # Print results
    print 'Max B mass: ', m_B_max
    print 'Min B mass: ', m_B_min


# Do for gamma and pi
get_endpoints(m_pi)
get_endpoints(m_gamma)
