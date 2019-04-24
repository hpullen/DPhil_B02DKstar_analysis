// Plot zoomed-in signal region
void PlotZoomed() {

    // Open file and get invariant mass
    TFile * file = TFile::Open("../Histograms/twoAndFourBody_data_binned_RooFit.root", "READ");
    RooRealVar * Bd_M = (RooRealVar*)file->Get("Bd_M");

    // Get data and plot

    // Get fit PDF



}
