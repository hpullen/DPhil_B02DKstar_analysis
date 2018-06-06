void fit_run1_with_run2() {

    // Read in Run 1 data
    TChain * tree = new TChain("DecayTree");
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/";
    tree->Add((path + "twoBody/Kpi/2011_down/Kpi_selected.root").c_str());
    tree->Add((path + "twoBody/Kpi/2011_up/Kpi_selected.root").c_str());
    tree->Add((path + "twoBody/Kpi/2012_down/Kpi_selected.root").c_str());
    tree->Add((path + "twoBody/Kpi/2012_up/Kpi_selected.root").c_str());

    // Make dataset
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5160, 5400, "MeV/c^{2}");
    int binWidth = 2;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);
    RooArgList args;
    args.add(Bd_M);
    RooDataSet * data = new RooDataSet("data", "", tree, args);

    // Make Run 2 shape
    RooRealVar * mean = new RooRealVar("mean", "", 5279.92, 5200, 5300);
    RooRealVar * sigma_L = new RooRealVar("sigma_L", "", 20.56);
    RooRealVar * sigma_ratio = new RooRealVar("sigma_ratio", "", 0.547);
    RooFormulaVar * sigma_R = new RooFormulaVar("sigma_R", "@0 * @1", 
            RooArgList(*sigma_L, *sigma_ratio));
    RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 1.85);
    RooRealVar * alpha_R = new RooRealVar("alpha_R", "", -3.048);
    RooRealVar * n_L = new RooRealVar("n_L", "", 2.828);
    RooRealVar * n_R = new RooRealVar("n_R", "", 0.694);
    RooRealVar * frac = new RooRealVar("frac", "", 0.259);
    RooCBShape * signal_L = new RooCBShape("signal_L", "", Bd_M, *mean, *sigma_L,
            *alpha_L, *n_L);
    RooCBShape * signal_R = new RooCBShape("signal_R", "", Bd_M, *mean, *sigma_R,
            *alpha_R, *n_R);
    RooAddPdf * signal = new RooAddPdf("signal", "", RooArgList(*signal_L, 
                *signal_R), RooArgList(*frac));

    // Fit to the dataset
    std::cout << "Dataset entries: " << data->sumEntries() << std::endl;
    RooFitResult * r = signal->fitTo(*data, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), /* RooFit::Offset(true),  */
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Plot the fit 
    TCanvas * canvas = new TCanvas("canvas", "", 900, 900);
    RooPlot * plot = Bd_M.frame();
    data->plotOn(plot, RooFit::DrawOption("PZ"));
    signal->plotOn(plot, RooFit::ProjWData(*data), RooFit::LineColor(ANABlue), 
                RooFit::LineWidth(2), RooFit::DrawOption("C"));
    TPad * pad1 = new TPad("fit", "", 0.0, 1.0/3.0, 1.0, 1.0);
    pad1->cd();
    plot->Draw();
    canvas->cd();
    pad1->Draw();

    // Plot pulls
    RooHist * pulls = plot->pullHist();
    RooPlot * pull_frame = Bd_M.frame();
    pull_frame->addPlotable(pulls, "BEX0");
    TPad * pad2 = new TPad("pullPad", "", 0.0, 0.0, 1.0, 1.0/3.0);
    pad2->cd();
    pull_frame->Draw();
    canvas->cd();
    pad2->Draw();

    // Save
    canvas->SaveAs("../Plots/run1_fit_with_run2.pdf");

}
