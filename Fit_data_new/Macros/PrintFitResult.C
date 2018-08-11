// ====================
// Print a RooFitResult
// ====================
void PrintFitResult(TString filename) {

    TFile * file = TFile::Open(filename, "READ");
    RooFitResult * result = (RooFitResult*)file->Get("fit_result");
    result->Print("v");


}

