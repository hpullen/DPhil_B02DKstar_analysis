// ============================================
// Function to return numeric integral of a PDF
// ============================================
double numericIntegral(RooAbsPdf * pdf, RooRealVar * x, double mean) {

    // Convert to histogram
    TH1F * hist = (TH1F*)pdf->createHistogram("hist", *x, 
            RooFit::Binning(1000));

    // Integrate
    double integ = 0;
    double min = mean - 25;
    double max = mean + 25;
    for (int bin = 0; bin <= hist->GetNbinsX(); bin++) {
        double bin_center = hist->GetBinCenter(bin);
        if (bin_center > min && bin_center < max) {
            integ += hist->GetBinContent(bin);
        }
    }

    // Return
    delete hist;
    return integ;
}



// ==================================================================================
// Script to compare signal and background in B0 mass region for old and new analyses
// ==================================================================================
void compare_with_old_analysis() {

    gROOT->ForceStyle();

    // Mass
    RooRealVar Bd_M("Bd_ConsD_MD",  "#it{m}(DK^{*0}) [MeV/#it{c}^{2}]", 5000, 5800);

    // Make previous analysis signal shape
    RooRealVar f_core("f_core", "", 0.88);
    RooRealVar kappa("kappa", "", 2.67);
    RooRealVar mu("mu", "", 5369.3 - 87.19);
    RooRealVar sigma("sigma", "", 12.7);
    RooFormulaVar sigma2("sigma2", "@0 * @1", RooArgList(sigma, kappa));
    RooGaussian gauss_core("gauss_core", "", Bd_M, mu, sigma);
    RooGaussian gauss2("gauss2", "", Bd_M, mu, sigma2);
    RooAddPdf gauss("gauss", "", RooArgList(gauss_core, gauss2), RooArgList(f_core));

    // Make previous analysis combinatorial shape
    RooRealVar slope("slope", "", -0.0049);
    RooExponential expo("expo", "", Bd_M, slope);
    RooRealVar n_expo_prev("n_expo_prev", "", 1768.9);
    RooRealVar n_signal_prev("n_signal_prev", "", 774.9);
    RooAddPdf shape_prev("shape_prev", "", RooArgList(gauss, expo), 
            RooArgList(n_signal_prev, n_expo_prev));

    // Plot previous analysis shapes
    RooPlot * plt = Bd_M.frame(RooFit::Title("Previous analysis"));
    shape_prev.plotOn(plt, RooFit::DrawOption("C"), RooFit::LineColor(kBlack),
            RooFit::LineWidth(2));
    shape_prev.plotOn(plt, RooFit::Components("expo"), RooFit::DrawOption("CF"), 
            RooFit::LineWidth(0), RooFit::FillColor(ANABlue));
    shape_prev.plotOn(plt, RooFit::Components("gauss"), RooFit::DrawOption("C"), 
            RooFit::LineWidth(2), RooFit::LineColor(kRed + 2));
    TCanvas * canv_prev = new TCanvas("canv_prev", "canv", 900, 600);
    plt->GetYaxis()->SetTitle("");
    plt->Draw();

    // Get integrals
    double integ_signal = numericIntegral(&gauss, &Bd_M, mu.getVal()) * n_signal_prev.getVal();
    double integ_expo = numericIntegral(&expo, &Bd_M, mu.getVal()) * n_expo_prev.getVal();
    std::stringstream ss_sig;
    std::stringstream ss_expo;
    ss_sig << std::setprecision(3) << "Signal: " << integ_signal << " +/- " 
        << std::setprecision(2) << sqrt(integ_signal);
    ss_expo << std::setprecision(3) << "Combinatorial: " << integ_expo << " +/- "
        << std::setprecision(2) << sqrt(integ_expo);

    // Draw integrals
    TPaveText * text1 = new TPaveText(0.5, 0.75, 0.82, 0.875, "NDC");
    text1->AddText(ss_sig.str().c_str());
    text1->AddText(ss_expo.str().c_str());
    text1->Draw();

    // Draw lines at +/- 25 MeV
    TLine line_low(mu.getVal() - 25, 0, mu.getVal() - 25, plt->GetMaximum());
    TLine line_high(mu.getVal() + 25, 0, mu.getVal() + 25, plt->GetMaximum());
    line_low.SetLineWidth(1);
    line_low.SetLineStyle(2);
    line_low.SetLineColor(ANAGreen);
    line_high.SetLineWidth(1);
    line_high.SetLineStyle(2);
    line_high.SetLineColor(ANAGreen);
    line_low.Draw();
    line_high.Draw();
    gPad->RedrawAxis();
    canv_prev->SaveAs("../Plots/previous_analysis_Kpi.pdf");

    // Load RooFitResult for my analysis (Run 1 only)
    TFile * file = TFile::Open("../Results/twoAndFourBody_data_run1.root", "READ"); 
    RooFitResult * r = (RooFitResult*)file->Get("fit_result");

    // Get constant parameters
    RooArgList const_pars = r->constPars();
    RooRealVar * alpha_L = (RooRealVar*)const_pars.find("pdf_params_signal_alpha_L");
    RooRealVar * alpha_R = (RooRealVar*)const_pars.find("pdf_params_signal_alpha_R");
    RooRealVar * n_L = (RooRealVar*)const_pars.find("pdf_params_signal_n_L");
    RooRealVar * n_R = (RooRealVar*)const_pars.find("pdf_params_signal_n_R");
    RooRealVar * ratio = (RooRealVar*)const_pars.find("pdf_params_signal_sigma_ratio");
    RooRealVar * frac = (RooRealVar*)const_pars.find("pdf_params_signal_frac");
    RooRealVar * mean_before_shift = (RooRealVar*)const_pars.find("pdf_params_signal_mean_before_shift");

    // Get floating parameter results
    RooArgList float_pars = r->floatParsFinal();
    RooRealVar * shift = (RooRealVar*)float_pars.find("pdf_params_shift");
    RooRealVar * n_expo_Kpi = (RooRealVar*)float_pars.find("pdf_params_n_expo_Kpi");
    RooRealVar * n_signal_Kpi = (RooRealVar*)float_pars.find("pdf_params_n_signal_Kpi");
    RooRealVar * slope_Kpi = (RooRealVar*)float_pars.find("pdf_params_slope_Kpi");
    RooRealVar * sigma_L = (RooRealVar*)float_pars.find("pdf_params_signal_sigma_L");
    RooFormulaVar * sigma_R = new RooFormulaVar("sigma_R", "@0 * @1", 
            RooArgList(*sigma_L, *ratio));
    RooFormulaVar * mean = new RooFormulaVar("mean", "@0 + @1", 
            RooArgList(*mean_before_shift, *shift));

    // Make double crystal ball
    RooCBShape cb_L("cb_L", "", Bd_M, *mean, *sigma_L, *alpha_L, *n_L);
    RooCBShape cb_R("cb_R", "", Bd_M, *mean, *sigma_R, *alpha_R, *n_R);
    RooAddPdf cb("cb", "", RooArgList(cb_L, cb_R), RooArgList(*frac));

    // Make my analysis combinatorial shape
    RooExponential expo_Kpi("expo_Kpi", "", Bd_M, *slope_Kpi);
    RooAddPdf shape_current("shape_current", "", RooArgList(cb, expo_Kpi),
            RooArgList(*n_signal_Kpi, *n_expo_Kpi));

    // Plot my analysis shapes
    RooPlot * plt2 = Bd_M.frame();
    shape_current.plotOn(plt2, RooFit::DrawOption("C"), RooFit::LineColor(kBlack),
            RooFit::LineWidth(2));
    shape_current.plotOn(plt2, RooFit::Components("expo_Kpi"), RooFit::DrawOption("CF"), 
            RooFit::LineWidth(0), RooFit::FillColor(ANABlue));
    shape_current.plotOn(plt2, RooFit::Components("cb"), RooFit::DrawOption("C"), 
            RooFit::LineWidth(2), RooFit::LineColor(kRed + 2));
    TCanvas * canv_current = new TCanvas("canv_current", "", 900, 600);
    plt2->GetYaxis()->SetTitle("");
    plt2->Draw();

    // Get integrals
    double integ_signal_current = numericIntegral(&cb, &Bd_M, mean->getVal()) * n_signal_Kpi->getVal();
    double integ_expo_current = numericIntegral(&expo_Kpi, &Bd_M, mean->getVal()) * n_expo_Kpi->getVal();
    std::stringstream ss_sig_current;
    std::stringstream ss_expo_current;
    ss_sig_current << std::setprecision(3) << "Signal: " << integ_signal_current << 
        " +/- " << std::setprecision(2) << sqrt(integ_signal_current);
    ss_expo_current << std::setprecision(2) << "Combinatorial: " << 
        integ_expo_current << " +/- " << std::setprecision(1) << sqrt(integ_expo_current);

    // Draw integrals
    TPaveText * text2 = new TPaveText(0.5, 0.75, 0.82, 0.875, "NDC");
    text2->AddText(ss_sig_current.str().c_str());
    text2->AddText(ss_expo_current.str().c_str());
    text2->Draw();

    // Draw lines
    line_low.SetX1(mean->getVal() - 25);
    line_low.SetX2(mean->getVal() - 25);
    line_high.SetX1(mean->getVal() + 25);
    line_high.SetX2(mean->getVal() + 25);
    line_low.SetY2(plt2->GetMaximum());
    line_high.SetY2(plt2->GetMaximum());
    line_low.Draw();
    line_high.Draw();
    canv_current->SaveAs("../Plots/current_analysis_Kpi.pdf");

    // Print integrals
    std::cout << "Previous analysis: signal = " << integ_signal << std::endl;
    std::cout << "Previous analysis: expo = " << integ_expo << std::endl;
    std::cout << "Current analysis: signal = " << integ_signal_current << std::endl;
    std::cout << "Current analysis: expo = " << integ_expo_current << std::endl;
}
