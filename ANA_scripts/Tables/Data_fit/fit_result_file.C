// ============================================================
// Script to make table of floating parameters from fit to data
// ============================================================
void fit_result_file() {

    // Open RooFitResult
    TFile * result_file = TFile::Open("/home/pullen/analysis/B02DKstar/"
            "Fit_data_new/Results/twoAndFourBody_data_split.root", "READ");
    RooFitResult * r = (RooFitResult*)result_file->Get("fit_result");

    // Open output file
    std::ofstream file("raw_fit_result.param");

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
