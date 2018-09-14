// ============================================================
// Get number of free parameters in the fit and print to a file
// ============================================================
void GetNParameters() {

    // Get fit result
    TFile * file = TFile::Open("../../../Fit_data_new/Results/twoAndFourBody_data_split.root", "READ");
    RooFitResult * result = (RooFitResult*)file->Get("fit_result");

    // Get number of params
    RooArgList args = result->floatParsFinal();
    int n_pars = args.getSize();

    // Write to a file
    std::ofstream outfile("../../../ANA_resources/Tables/Fit_setup/Values/n_params.tex");
    outfile << n_pars << std::endl;
    outfile.close();
}
