#include <string>
#include <map>
#include <fstream>
#include <cmath>
#include <iostream>

// =========================================================================
// Script to weight efficiencies by year and polarity, and write to new file
// =========================================================================
int main (int argc, char * argv[]) {

    // Input args
    if (argc != 2) {
        std::cout << "Usage: ./WeightEfficiencies <file (without .txt)>" << std::endl;
        return -1;
    }
    std::string file = argv[1];

    // Map to hold inputs
    std::map<std::string, std::map<std::string, std::map<std::string, std::pair<double, double>>>> input_eff;

    // Open input file
    std::ifstream in_file(file + ".txt");

    // Loop through and fill map of efficiencies
    std::string in_mode;
    std::string in_year;
    std::string in_mag;
    std::string in_eff;
    std::string in_error;
    in_file >> in_mode;
    while (!in_file.eof()) {
        in_file >> in_year;
        in_file >> in_mag;
        in_file >> in_eff;
        in_file >> in_error;
        input_eff[in_mode][in_year][in_mag] = std::make_pair(std::stof(in_eff), 
                std::stof(in_error));
        in_file >> in_mode;
    }
    in_file.close();

    // File to hold output
    std::ofstream out_file(file + "_average.txt");

    // Weighting factors
    const double lumi_2015 = 0.33;
    const double lumi_2016 = 1.67;
    std::map<std::string, const double> year_weights = {
        {"2015", lumi_2015 / (lumi_2015 + lumi_2016)},
        {"2016", lumi_2016 / (lumi_2015 + lumi_2016)}};
    std::map<std::string, std::map<std::string, double>> mag_weights;
    mag_weights["2015"].emplace("up", 0.5);
    mag_weights["2015"].emplace("down", 0.5);
    mag_weights["2016"].emplace("up", 0.5);
    mag_weights["2016"].emplace("down", 0.5);

    // Loop through modes and calculate average efficiency and error
    for (auto mode : input_eff) {
       double eff_sum = 0;
       double error_sq_sum = 0;

       // Loop through years
       for (auto year : mode.second) {

           // Loop through polarities
           for (auto mag : year.second) {
               eff_sum += mag.second.first * year_weights[year.first] * mag_weights[year.first][mag.first];
               error_sq_sum += pow(mag.second.second * year_weights[year.first] * mag_weights[year.first][mag.first], 2);

           } // End polarity loop
       } // End year loop

       // Write to file
       out_file << mode.first << " " << eff_sum << " " << sqrt(error_sq_sum) << std::endl;
       
    } // End mode loop

    out_file.close();
    return 0;
}




    
