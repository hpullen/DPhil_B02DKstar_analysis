#ifndef LOW_MASS_COMPONENTS_H
#define LOW_MASS_COMPONENTS_H

#include <map>
#include <string>

#include "../AllParameters.hpp"

class TH1F;

// Enums describing background components
enum Component {pi_010, pi_100, gamma_010, gamma_100, Bs_pi_010, Bs_pi_100,
        Bs_gamma_100, Bs_gamma_010};
enum Parent {Bd, Bs};

// Struct containing component of background
struct BackgroundComponent {
    std::string name;
    int events;
    TH1F * histogram;
    AllParameters::LowMassOutput parameters;
};

#endif // LOW_MASS_COMPONENTS_H
