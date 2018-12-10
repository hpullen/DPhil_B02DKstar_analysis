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
    file << std::fixed;

    // Print parameters to file
    RooArgList vars = r->floatParsFinal();
    RooRealVar * var;
    TIterator * it = vars.createIterator();
    while ((var = (RooRealVar*)it->Next())) {
        file << var->GetName() << " " << var->getVal() << " " 
            << var->getError() << std::endl;
    }
    file.close();

    // Also print R_ds values
    std::ofstream R_ds_file("R_ds_fit_result" + 
            extra_name_out + ".param");
    std::vector<std::string> R_ds_vars = {
        "R_ds_KK_run1_blind",
        "R_ds_KK_run2_blind",
        "R_ds_pipi_run1_blind",
        "R_ds_pipi_run2_blind",
        "R_ds_pipipipi_run2_blind"
    };
    RooWorkspace * ws = (RooWorkspace*)result_file->Get("wspace");
    for (auto var_name : R_ds_vars) {
        RooFormulaVar * var = (RooFormulaVar*)ws->arg(("pdf_params_" + var_name).c_str());
        double stat = var->getPropagatedError(*r);
        R_ds_file << var->GetName() << " " << var->getVal() << " " <<
            stat << std::endl;
    }

    // Close
    R_ds_file.close();
    result_file->Close();
}
