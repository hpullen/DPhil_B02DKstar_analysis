#ifndef LOW_MASS_PARAMETERS_H
#define LOW_MASS_PARAMETERS_H

#include <iostream>
#include <fstream>
#include <string>

namespace LowMassParameters {

    namespace ParameterCategories {

        struct KinematicEndpoint {
            double gamma;
            double pi;
        };

        struct Parameter {
            double low;
            double high;
            double start;
        };

        struct InputCsi {
            Parameter horns;
            Parameter hill;
            Parameter littlehorns;
        };

        struct InputSigma {
            Parameter gamma_010;
            Parameter gamma_100;
            Parameter pi_010;
            Parameter pi_100;
        };

        struct OutputCsi {
            double horns;
            double hill;
            double littlehorns;
        }; 

        struct OutputSigma {
            double gamma_010;
            double gamma_100;
            double pi_010;
            double pi_100;
        };
    }

    // Struct to hold input parameters
    struct InputParameters {
        ParameterCategories::KinematicEndpoint a;
        ParameterCategories::KinematicEndpoint b;
        ParameterCategories::Parameter f;
        ParameterCategories::Parameter ratio;
        ParameterCategories::InputCsi csi;
        ParameterCategories::InputSigma sigma;
    };

    // Struct for output parameters
    struct OutputParameters {
        ParameterCategories::KinematicEndpoint a;
        ParameterCategories::KinematicEndpoint b;
        double f;
        double ratio;
        ParameterCategories::OutputCsi csi;
        ParameterCategories::OutputSigma sigma;
    };

    // Function to read input parameters from file
    inline InputParameters readInputParameters(std::string filepath) {

        // Open file
        std::ifstream file(filepath);
        if (!file.good()) {
            std::cout << "No parameters file found: " << filepath << std::endl;
        }
        std::string word;
        InputParameters param;

        // Read in parameters
        // a and b
        file >> word; file >> word;
        param.a.gamma = std::stod(word);
        file >> word; file >> word;
        param.a.pi = std::stod(word);
        file >> word; file >> word;
        param.b.gamma = std::stod(word);
        file >> word; file >> word;
        param.b.pi = std::stod(word);

        // csi
        file >> word; file >> word;
        param.csi.horns.start = std::stod(word);
        file >> word;
        param.csi.horns.low = std::stod(word);
        file >> word;
        param.csi.horns.high = std::stod(word);
        file >> word; file >> word;
        param.csi.hill.start = std::stod(word);
        file >> word;
        param.csi.hill.low = std::stod(word);
        file >> word;
        param.csi.hill.high = std::stod(word);
        file >> word; file >> word;
        param.csi.littlehorns.start = std::stod(word);
        file >> word;
        param.csi.littlehorns.low = std::stod(word);
        file >> word;
        param.csi.littlehorns.high = std::stod(word);

        // f and ratio
        file >> word; file >> word;
        param.f.start = std::stod(word);
        file >> word;
        param.f.low = std::stod(word);
        file >> word;
        param.f.high = std::stod(word);
        file >> word; file >> word;
        param.ratio.start = std::stod(word);
        file >> word;
        param.ratio.low = std::stod(word);
        file >> word;
        param.ratio.high = std::stod(word);

        // sigma
        file >> word; file >> word;
        param.sigma.gamma_010.start = std::stod(word);
        file >> word;
        param.sigma.gamma_010.low = std::stod(word);
        file >> word;
        param.sigma.gamma_010.high = std::stod(word);
        file >> word; file >> word;
        param.sigma.gamma_100.start = std::stod(word);
        file >> word;
        param.sigma.gamma_100.low = std::stod(word);
        file >> word;
        param.sigma.gamma_100.high = std::stod(word);
        file >> word; file >> word;
        param.sigma.pi_010.start = std::stod(word);
        file >> word;
        param.sigma.pi_010.low = std::stod(word);
        file >> word;
        param.sigma.pi_010.high = std::stod(word);
        file >> word; file >> word;
        param.sigma.pi_100.start = std::stod(word);
        file >> word;
        param.sigma.pi_100.low = std::stod(word);
        file >> word;
        param.sigma.pi_100.high = std::stod(word);

        // Return parameters
        std::cout << "Successfully read parameters from " << filepath << std::endl;
        return param;
    }

    // Struct to hold input parameters for 

}

#endif // LOW_MASS_PARAMETERS_H
