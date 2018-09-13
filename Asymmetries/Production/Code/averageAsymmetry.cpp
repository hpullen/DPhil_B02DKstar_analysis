#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "ParameterReader.hpp"

typedef std::map<std::string, std::map<std::string, std::pair<double, double>>> valMap;


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
        for (std::string mag : {"up", "down"}) {
            std::string name = year + "_" + mag;
            if (pr->Contains(name)) {
                map[year][mag] = std::make_pair(pr->GetValue(name), pr->GetError(name));
            } else {
                map[year][mag] = std::make_pair(-1, -1);
            }
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

    // Factor to account for more events in Run 2
    double ratio_1v2 = pr->GetValue("run2_vs_run1", "1v2");
    double ratio_1v2_err = pr->GetError("run2_vs_run1", "1v2");

    // Luminosity factors
    std::map<std::string, std::map<std::string, std::pair<double, double>>> lumis;

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
        for (std::string mag : {"up", "down"}) {
            double lumi_val = pr->GetValue(year + "_" + mag, "lumi");
            double lumi_factor = lumi_val/lumi_sum;
            double lumi_factor_error = 0;
            lumis[year][mag] = std::make_pair(lumi_factor, lumi_factor_error);
        }
    }

    // Calculate weighted efficiency
    double sum = 0;
    double sq_err_sum = 0;
    for (std::string year : {"2011", "2012"}) {
        for (std::string mag : {"up", "down"}) {

            // Calculate contribution from this polarity/year
            double contrib = map[year][mag].first * lumis[year][mag].first;
            sum += contrib;

            // Calculate error for this year
            double frac_err_sum = 0;
            frac_err_sum += pow(lumis[year][mag].second/lumis[year][mag].first, 2);
            frac_err_sum += pow(map[year][mag].second/map[year][mag].first, 2);
            sq_err_sum += pow(contrib, 2) * frac_err_sum;

        } // End polarity loop
    } // End year loop within run

    // Combine Run 1 and Run 2 (Run 2: same value, double uncertainty)
    double A_err_run1 = sqrt(sq_err_sum);
    double A_err_run2 = 2 * sqrt(sq_err_sum);
    double contrib_run1 = sum / (1 + ratio_1v2);
    double err_run1 = fabs(contrib_run1 * sqrt(pow(A_err_run1/sum, 2) + 
            2 * pow(ratio_1v2_err/ratio_1v2, 2)));
    double contrib_run2 = ratio_1v2 * sum / (1 + ratio_1v2);
    double err_run2 = contrib_run2 * sqrt(pow(A_err_run2/sum, 2) + 
            2 * pow(ratio_1v2_err/ratio_1v2, 2));

    if (run == -1) {
        return std::make_pair(sum, sqrt(pow(err_run1, 2) + pow(err_run2, 2)));
    } else if (run == 1) {
        return std::make_pair(sum, err_run1);
    } else {
        return std::make_pair(sum, 2 * err_run1);
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
