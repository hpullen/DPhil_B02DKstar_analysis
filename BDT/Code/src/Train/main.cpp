#include <iostream>
#include <string>
#include <vector>

#include "MVATrainer.hpp"

int main(int argc, char * argv[]) {

    // Check number of input args
    if (argc < 6) {
        std::cout << "Not enough input args provided." << std::endl;
        std::cout << "Usage: <mva-name> -s <signal-files> -b "
            "<background-files> (-v <arg-file>)" << std::endl;
        return -1;
    }

    // Get name
    std::string mva_name = argv[1];

    // Get signal files
    bool sigFound = false;
    std::vector<std::string> sigFiles;
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-s") {
            if (i == argc - 1) break;
            for (int j = i + 1; j < argc; j++) {
                if (std::string(argv[j]) == "-b" || std::string(argv[j]) == "-v") {
                    break;
                }
                sigFiles.push_back(std::string(argv[j]));
                sigFound = true;
            }
            break;
        }
    }
    if (!sigFound) {
        std::cout << "Please supply at least one signal file with -s." << std::endl;
        return -1;
    }

    // Get background files
    bool backgroundFound = false;
    std::vector<std::string> backgroundFiles;
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-b") {
            if (i == argc - 1) break;
            for (int j = i + 1; j < argc; j++) {
                if (std::string(argv[j]) == "-s" || std::string(argv[j]) == "-v") {
                    break;
                }
                backgroundFiles.push_back(std::string(argv[j]));
                backgroundFound = true;
            }
            break;
        }
    }
    if (!backgroundFound) {
        std::cout << "Please supply at least one background file with -b." << 
            std::endl;
        return -1;
    }

    // Check for an arg file
    std::string varFileName = "../VariableLists/" + mva_name + ".txt";
    std::string cutFileName = "../VariableLists/Cuts/" + mva_name + ".cut";
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-v") {
            if (i + 1 == argc) {
                std::cout << "Warning: option -v was given but no file was "
                    "specified. Using default." << std::endl;
                break;
            }
            varFileName = "../VariableLists/" + std::string(argv[i + 1]) + ".txt";
            // cutFileName = "../VariableLists/Cuts/" + std::string(argv[i + 1]) +
                // ".cut";
            break;
        }
    }

    // Get cut file
    // std::string cutFileName = "../VariableLists/Cuts/Kpi_run2.cut";
    // if (mva_name.find("KK") != std::string::npos) {
        // cutFileName = "../VariableLists/Cuts/KK_run2.cut";
    // } else if (mva_name.find("pipi") != std::string::npos) {
        // cutFileName = "../VariableLists/Cuts/pipi_run2.cut";
    // }

    // Print info
    std::cout << "Training BDTG " << mva_name << " with " << sigFiles.size() << 
        " signal file(s) and " << backgroundFiles.size() << " background file(s)."
        << std::endl;
    std::cout << "Using variable list: " << varFileName << std::endl;
    std::cout << "Using cuts from: " << cutFileName << std::endl;

    // Make MVA trainer and train MVAs
    MVATrainer * trainer = new MVATrainer();
    trainer->train(mva_name, sigFiles, backgroundFiles, varFileName, cutFileName);
    delete trainer;

}
