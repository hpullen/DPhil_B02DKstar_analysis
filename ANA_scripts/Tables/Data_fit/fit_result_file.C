// ============================================================
// Script to make table of floating parameters from fit to data
// ============================================================
void fit_result_file(bool combined = false) {

    // Open RooFitResult
    TString extra_name = combined ? "" : "_split";
    TFile * result_file = TFile::Open("/home/pullen/analysis/B02DKstar/Fit_data/Results/twoAndFourBody_data" + extra_name + ".root", "READ");
    RooFitResult * r = (RooFitResult*)result_file->Get("fit_result");

    // Open output file
    std::string extra_name_out = combined ? "_combined" : "";
    std::ofstream file("raw_fit_result" + extra_name_out + ".param");

    // Print parameters to file
    RooArgList vars = r->floatParsFinal();
    RooRealVar * var;
    TIterator * it = vars.createIterator();
    while ((var = (RooRealVar*)it->Next())) {
        file << var->GetName() << " " << var->getVal() << " " 
            << var->getError() << std::endl;
    }

    // Close
    file.close();
    result_file->Close();
}
