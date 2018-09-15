// Script to print highest correlations
void PrintCorrelation(TString filename = "../Results/twoAndFourBody_data.root") {

    // Get fit result
    TFile * file = TFile::Open(filename, "READ");
    RooFitResult * r = (RooFitResult*)file->Get("fit_result");

    // Get list of parameters
    RooArgList pars = r->floatParsFinal();

    // Loop through and get correlations
    for (int i = 0; i < pars.getSize(); i++) {

        // Get correlation list
        TString parname = pars[i].GetName();
        const RooArgList * corrs = r->correlation(parname);

        // Loop through and print if large
        std::string corr_string = "";
        for (int j = 0; j < corrs->getSize(); j++) {
            if (i == j) continue;
            double corr = ((RooRealVar*)corrs->at(j))->getVal();
            if (std::abs(corr) > 0.5) {
                corr_string += "    ";
                corr_string += pars[j].GetName();
                corr_string +=  ": ";
                corr_string += std::to_string(corr);
                corr_string += "\n";
            }
        }

        // Print if there were large correlations
        if (corr_string != "") {
            std::cout << parname << ": " << std::endl;
            for (int l = 0; l < parname.Length() + 1; l++) {
                std::cout << "=";
            }
            std::cout << std::endl;
            std::cout << corr_string << std::endl;
            std::cout << std::endl;
        }
    }

    file->Close();
}

