#include <iostream>

#include "MVAApplicator.hpp"

int main(int argc, char * argv[]) {

    // Check number of input arguments
    if (argc < 5) {
        std::cout << "Not enough args given." << std::endl;
        std::cout << "Usage: ./ApplyMVA <mva-name> <input-file> <output-file> " 
           "<tree-name> (-v <variable-file>)" << std::endl;
        return -1;
    }
    
    // Get arguments
    std::string mvaName = argv[1];
    std::string inputPath = argv[2];
    std::string outputPath = argv[3];
    std::string treename = argv[4];

    // Get optional variable file
    std::string varFile = "../VariableLists/" + mvaName + ".txt";
    if (argc > 5) {
        if (std::string(argv[5]) == "-v") {
            varFile = "../VariableLists/" + std::string(argv[6]) + ".txt"; 
        } else {
            std::cout << "Ambigious variable file definition. Use correct syntax." 
                << std::endl;
            return -1;
        }
    } 

    // Print some info
    std::cout << "Applying MVA " << mvaName << " to file " << inputPath << "." <<
        std::endl;
    std::cout << "Using variable file: " << varFile << std::endl;
    std::cout << "Output will be saved to " << outputPath << std::endl;

    // Make applicator and apply MVA
    MVAApplicator * applicator = new MVAApplicator();
    applicator->apply(inputPath, outputPath, mvaName, treename, varFile);
    delete applicator;
    
}
