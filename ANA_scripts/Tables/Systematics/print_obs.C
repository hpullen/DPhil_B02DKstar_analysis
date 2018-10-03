// ============================================================
// Script to make table of floating parameters from fit to data
// ============================================================
void print_obs() {

    // Open RooFitResult
    TFile * result_file = TFile::Open("/home/pullen/analysis/B02DKstar/Fit_data/Results/twoAndFourBody_data_split.root", "READ");
    RooFitResult * r = (RooFitResult*)result_file->Get("fit_result");

    // Open output file
    std::ofstream file("raw_obs.param");

    // Loop through parameters of interest
    std::ifstream infile("pars_list.txt");
    std::string line;
    while (std::getline(infile, line)) {

        // Normal: get from RooFitResult
        if (line.find("R_ds_") == std::string::npos) {
            std::cout << "Looking for " << "pdf_params_" << line << std::endl;
            RooRealVar * var = (RooRealVar*)r->floatParsFinal().find(("pdf_params_" + line).c_str());
            file << line << " " << var->getVal() << " " << var->getError() << std::endl;
        }

        // Otherwise, get formulavar and propagate error
        else {
            std::cout << "Looking for " << line << std::endl;
            RooFormulaVar * var = (RooFormulaVar*)result_file->Get(line.c_str());
            file << line << " " << var->getVal() << " " << var->getPropagatedError(*r) << std::endl;
        }

    }

    // Close
    file.close();
    result_file->Close();
}
