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
                map[year][mag] = std::make_pair(-1e6, -1e6);
            }
        }
    }

    return map;
}


// =============================================
// Function to average over years and polarities
// =============================================
std::pair<double, double> getAverage(valMap map, int run = -1) {

    // Check if calculation is for one run only
    std::map<int, bool> runs_to_use = {{1, true}, {2, true}};
        std::map<int, std::vector<std::string>> run_years = {
            {1, {"2011", "2012"}}, 
            {2, {"2015", "2016"}}};
    if (run < 0) {
        if (map["2011"]["down"].first < -1e5 && map["2012"]["down"].first < -1e5) {
            runs_to_use[1] = false;
        }
        if (map["2015"]["down"].first < -1e5 && map["2016"]["down"].first < -1e5) {
            runs_to_use[2] = false;
        }
    } else {
        if (run == 1) {
            runs_to_use[2] = false;
        } else if (run == 2) {
            runs_to_use[1] = false;
        } else {
            std::cout << "Error: " << run << " not a valid run number (-1, 1 or 2)" 
                << std::endl;
        }
    }

    // Check if sharing any values across 2011/2012 and 2015/2012
    for (auto run : run_years) {
        if (!runs_to_use[run.first]) continue;
        for (std::string mag : {"up", "down"}) {
            if (map[run.second[0]][mag].first < 0) {
                map[run.second[0]][mag] = map[run.second[1]][mag];
            }
        }
    }

    // Read in luminosities and Run 1 vs. Run 2 ratio
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Parameters/");
    pr->ReadParameters("run2_vs_run1.param", "1v2");
    pr->ReadParameters("lumi_calculated.param", "lumi");

    // Factor to account for more events in Run 2
    double ratio_1v2 = pr->GetValue("run2_vs_run1", "1v2");
    double ratio_1v2_err = pr->GetError("run2_vs_run1", "1v2");
    std::map<int, std::pair<double, double>> run_factors;
    run_factors[1].first = runs_to_use[2] ? 1/(1 + ratio_1v2) : 1;
    run_factors[1].second = run_factors[1].first * sqrt(2) * (runs_to_use[2] ? ratio_1v2_err : 0)/ratio_1v2;
    run_factors[2].first = runs_to_use[1] ? ratio_1v2/(1 + ratio_1v2) : 1;
    run_factors[2].second = run_factors[2].first * sqrt(2) * (runs_to_use[1] ? ratio_1v2_err : 0)/ratio_1v2;

    // Luminosity factors
    std::map<std::string, std::map<std::string, std::pair<double, double>>> lumis;
    for (auto run : run_years) {

        // Calculate total luminosity for run
        double lumi_sum = 0;
        double sq_lumi_err = 0;
        for (auto year : run.second) {
            for (std::string mag : {"up", "down"}) {
                lumi_sum += pr->GetValue(year + "_" + mag, "lumi");
                sq_lumi_err += pow(pr->GetError(year + "_" + mag, "lumi"), 2);
            }
        }

        // Calculate luminosity fraction for year
        for (auto year : run.second) {
            for (std::string mag : {"up", "down"}) {
                double lumi_val = pr->GetValue(year + "_" + mag, "lumi");
                double lumi_error = pr->GetError(year + "_" + mag, "lumi");
                double lumi_factor = lumi_val/lumi_sum;
                double lumi_factor_error = lumi_factor * sqrt(pow(lumi_error/lumi_val, 2) +
                        pow(sqrt(sq_lumi_err)/lumi_sum, 2));
                lumis[year][mag] = std::make_pair(lumi_factor, lumi_factor_error);
            }
        }
    }

    // Calculate weighted efficiency
    double sum = 0;
    double sq_err_sum = 0;
    for (auto run : run_years) {
        if (!runs_to_use[run.first]) continue;
        for (auto year : run.second) {
            for (std::string mag : {"up", "down"}) {

                // Calculate contribution from this polarity/year
                double contrib = map[year][mag].first * run_factors[run.first].first * 
                    lumis[year][mag].first;
                sum += contrib;

                // Calculate error for this year
                double frac_err_sum = 0;
                // frac_err_sum += pow(lumis[year][mag].second/lumis[year][mag].first, 2);
                // frac_err_sum += pow(run_factors[run.first].second/run_factors[run.first].first, 2);
                frac_err_sum += pow(map[year][mag].second/map[year][mag].first, 2);
                sq_err_sum += pow(contrib, 2) * frac_err_sum;

            } // End polarity loop
        } // End year loop within run
    } // End run loop

    return std::make_pair(sum, sqrt(sq_err_sum));
}

// ===============================================================
// Script to average the efficiency over years and magnet polarity
// ===============================================================
int main(int argc, char * argv[]) {

    // Check input args
    if (argc < 5) {
        std::cout << "Usage: ./AverageEfficiency <output-file> -i <mode-name> <file> ..."
            " -c <combined-name> <file1> <file2> ... <--run1/--run2>" << std::endl;
        return -1;
    }

    // Get input args
    std::string outfile = argv[1];
    std::map<std::string, std::string> input_files;
    std::map<std::string, std::pair<std::string, std::string>> combined_input_files;
    bool run1_only = false;
    bool run2_only = false;
    int i = 2;
    bool combined = false;
    while (i < argc) {
        if (std::string(argv[i]) == "--run1") {
            run1_only = true;
        } else if (std::string(argv[i]) == "--run2") {
            run2_only = true;
        } else if (std::string(argv[i]) == "-i") {
            combined = true;
        } else if (std::string(argv[i]) == "-c") {
            combined = false;
        } else {
            if (combined) {
                std::string mode = argv[i];
                i++;
                std::string filename = argv[i];
                input_files.emplace(mode, filename);
            } else {
                std::string mode = argv[i];
                i++;
                std::string comb1 = argv[i];
                i++;
                std::string comb2 = argv[i];
                std::cout << "AverageEfficiency: Will combine values from " << 
                    comb1 << " and " << comb2 << " as " << mode << std::endl;
                combined_input_files.emplace(mode, std::make_pair(comb1, comb2));
            }
        }
        i++;
    }

    // Whether to use run 1 or run 2 only
    int run = -1; 
    if (run1_only) run = 1;
    else if (run2_only) run = 2;

    // Make output file
    std::ofstream out(outfile);

    // Loop through and get values
    for (auto file : input_files) {
        auto map = readValues(file.second);
        std::pair<double, double> av = getAverage(map, run);
        out << std::fixed << file.first << " " << av.first << " " << av.second << std::endl;
    }
    for (auto comb : combined_input_files) {
        auto map1 = readValues(comb.second.first);
        auto map2 = readValues(comb.second.second);
        std::pair<double, double> av1 = getAverage(map1, run);
        std::pair<double, double> av2 = getAverage(map2, run);
        double av_comb = 0.5 * (av1.first + av2.first);
        double av_err = sqrt(pow(0.5 * av1.second, 2) + pow(0.5 * av2.second, 2));
        out << comb.first << " " << av_comb << " " << av_err << std::endl;
    }
    out.close();

}
