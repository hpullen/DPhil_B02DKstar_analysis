#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "ParameterReader.hpp"

typedef std::map<std::string, std::pair<double, double>> valMap;


// ============================
// Function to read from a file
// ============================
valMap readValues(std::string file) {

    // Read parameters
    ParameterReader * pr = new ParameterReader();
    pr->ReadParameters(file);

    // Make map; default value = 0
    valMap map;
    for (std::string year : {"2011", "2012", "2015", "2016"}) {
        if (pr->Contains(year)) {
            map[year] = std::make_pair(pr->GetValue(year), pr->GetError(year));
        } else {
            map[year] = std::make_pair(-1, -1);
        }
    }

    return map;
}


// =============================================
// Function to average over years and polarities
// =============================================
std::pair<double, double> getAverage(valMap map, int run = -1) {

    // Read in luminosities and Run 1 vs. Run 2 ratio
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Parameters/");
    pr->ReadParameters("run2_vs_run1.param", "1v2");
    pr->ReadParameters("lumi_calculated.param", "lumi");

    // Luminosity factors
    std::map<std::string, std::pair<double, double>> lumis;

    // Calculate total luminosity for Run 1
    double lumi_sum = 0;
    double sq_lumi_err = 0;
    for (std::string year : {"2011", "2012"}) {
        for (std::string mag : {"up", "down"}) {
            lumi_sum += pr->GetValue(year + "_" + mag, "lumi");
            sq_lumi_err += pow(pr->GetError(year + "_" + mag, "lumi"), 2);
        }
    }

    // Calculate luminosity fraction for year
    for (std::string year : {"2011", "2012"}) {
        double year_sum = 0;
        for (std::string mag : {"up", "down"}) {
            year_sum += pr->GetValue(year + "_" + mag, "lumi");
        }
        double lumi_factor = year_sum/lumi_sum;
        double lumi_factor_error = 0;
        lumis[year] = std::make_pair(lumi_factor, lumi_factor_error);
    }

    // Calculate weighted efficiency
    double sum = 0;
    double sq_err_sum = 0;
    for (std::string year : {"2011", "2012"}) {

            // Calculate contribution from this year
            double contrib = map[year].first * lumis[year].first;
            sum += contrib;

            // Calculate error for this year
            double lumi_times_err = lumis[year].first * map[year].second;
            sq_err_sum += lumi_times_err * lumi_times_err;

    } // End year loop within run

    // Print result
    double A_err_run1 = sqrt(sq_err_sum);
    if (run == 1) {
        return std::make_pair(sum, A_err_run1);
    } else {
        return std::make_pair(sum, 2 * A_err_run1);
    }
}

// ===============================================================
// Script to average the efficiency over years and magnet polarity
// ===============================================================
int main(int argc, char * argv[]) {

    // Check input args
    if (argc < 4) {
        std::cout << "Usage: ./AverageEfficiency <output-file> <mode-name> <file> ..."
            " (--run1/--run2)" << std::endl;
        return -1;
    }

    // Get input args
    std::string outfile = argv[1];
    std::map<std::string, std::string> input_files;
    int run = -1;
    int i = 2;
    while (i < argc) {
        if (std::string(argv[i]) == "--run1") {
            run = 1;
            i++;
        } else if (std::string(argv[i]) == "--run2"){
            run = 2;
            i++;
        } else {
            std::string mode = argv[i];
            i++;
            std::string filename = argv[i];
            input_files.emplace(mode, filename);
            i++;
        }
    }

    // Make output file
    std::ofstream out(outfile);

    // Loop through and get values
    for (auto file : input_files) {
        auto map = readValues(file.second);
        std::pair<double, double> av = getAverage(map, run);
        out << file.first << " " << av.first << " " << av.second << std::endl;
    }
    out.close();

}
