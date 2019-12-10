// ===========================================================
// Macro to compare the B->Drho double CB with the B->D*rho CB
// ===========================================================
void compare_rho_with_lowMass_CB() {

    // Make Drho parameters
    RooRealVar m("m", "", 5200, 5500);
    RooRealVar alpha_L("alpha_L", "", 2.2007);
    RooRealVar alpha_R("alpha_R", "", -0.50275);
    RooRealVar frac("frac", "", 0.287588);
    RooRealVar mean("mean", "", 5321.48);
    RooRealVar n_L("n_L", "", 1);
    RooRealVar n_R("n_R", "", 9.0308);
    RooRealVar sigma_L("sigma_L", "", 17.7197);
    RooRealVar sigma_R("sigma_R", "", 13.5428);

    // Make Drho shapes
    RooCBShape CB_L("CB_L", "", m, mean, sigma_L, alpha_L, n_L);
    RooCBShape CB_R("CB_R", "", m, mean, sigma_R, alpha_R, n_R);
    RooAddPdf CB_Drho("CB_Drho", "", RooArgList(CB_L, CB_R), RooArgList(frac));

    // Make D*rho parameters
    RooRealVar alpha("alpha", "", -0.732378);
    RooRealVar n("n", "", 8.08288);
    RooRealVar sigma("sigma", "", 31.9791);

    // Make D*rho shape
    RooCBShape CB_DstarRho("CB_DstarRho", "", m, mean, sigma, alpha, n);

    // Plot the two shapes
    gROOT->ForceStyle();
    RooPlot * plot = m.frame();
    CB_Drho.plotOn(plot, RooFit::LineColor(kRed));
    CB_DstarRho.plotOn(plot, RooFit::LineColor(kBlue));
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    plot->Draw();
    canvas->SaveAs("../Plots/Comparisons/rho_vs_lowMass.pdf");

}
