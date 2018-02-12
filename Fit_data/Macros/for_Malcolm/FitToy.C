// =========================================================================
// Macro to generate and fit a simple toy with and without a piK signal peak
// =========================================================================
void FitToy(std::string run_many_string = "N", int number = 0) {

    // =====
    // Setup
    // =====
    // Settings (change these to try different type of toy)
    bool free_Kpi_yield = true;
    bool free_expo_yield = true;
    if (!free_Kpi_yield && !free_expo_yield) {
        std::cout << "Must free at least one yield (Kpi signal or piK background"
            << std::endl;
        return;
    }
    
    // Name of directories to save to
    TString dirname; 
    if (free_Kpi_yield && free_expo_yield) dirname = "free_Kpi_free_expo";
    else if (free_Kpi_yield && !free_expo_yield) dirname = "free_Kpi_fixed_expo";
    else if (!free_Kpi_yield && free_expo_yield) dirname = "fixed_Kpi_free_expo";

    // Settings for mutliple toys; run 4 at a time if option is selected
    bool run_many = (run_many_string == "Y");
    int n_toys = 1;
    if (run_many) n_toys = 4;
    
    // Make fit variable
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);

    // Set binning
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);

    // Make fit categories: Kpi and piK
    RooCategory cat("category", "");
    cat.defineType("Kpi");
    cat.defineType("piK");

    // Make tree for saving results to a file
    TFile * results_file = TFile::Open("Results/" + dirname + "/results_" +
            number + ".root", "RECREATE");
    TTree * toy_tree = new TTree("toy_tree", "");

    // Set tree branches to store initial values and fit values of free parameters
    double init_value_n_signal_Kpi;
    double final_value_n_signal_Kpi;
    double null_value_n_signal_Kpi;
    double init_value_n_expo;
    double final_value_n_expo;
    double null_value_n_expo;
    double init_value_R_piK_vs_Kpi;
    double final_value_R_piK_vs_Kpi;
    double null_value_R_piK_vs_Kpi = 0;
    toy_tree->Branch("init_value_R_piK_vs_Kpi", &init_value_R_piK_vs_Kpi);
    toy_tree->Branch("final_value_R_piK_vs_Kpi", &final_value_R_piK_vs_Kpi);
    toy_tree->Branch("null_value_R_piK_vs_Kpi", &null_value_R_piK_vs_Kpi);
    if (free_Kpi_yield) {
        toy_tree->Branch("init_value_n_signal_Kpi", &init_value_n_signal_Kpi);
        toy_tree->Branch("final_value_n_signal_Kpi", &final_value_n_signal_Kpi);
        toy_tree->Branch("null_value_n_signal_Kpi", &null_value_n_signal_Kpi);
    }
    if (free_expo_yield) {
        toy_tree->Branch("init_value_n_expo", &init_value_n_expo);
        toy_tree->Branch("final_value_n_expo", &final_value_n_expo);
        toy_tree->Branch("null_value_n_expo", &null_value_n_expo);
    }

    // Make branches for significance and status
    double significance;
    int status;
    toy_tree->Branch("signifiance", &signifiance);
    toy_tree->Branch("status", &status, "status/I");

    // ---------------------------------------------------------------------------- 

    // ===================================
    // Loop through desired number of toys
    // ===================================
    for (int i = 0; i < n_toys; i++) {

        // ============
        // Make toy PDF
        // ============
        // Signal shape fixed parameters (from my fit to Monte Carlo)
        RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 1.90);
        RooRealVar * alpha_R = new RooRealVar("alpha_R", "", -3.00);
        RooRealVar * n_L = new RooRealVar("n_L", "", 2.48);
        RooRealVar * n_R = new RooRealVar("n_R", "", 0.76);
        RooRealVar * ratio = new RooRealVar("ratio", "", 0.545);
        RooRealVar * frac = new RooRealVar("frac", "", 0.275);

        // Signal mean and width (from my fit to data)
        RooRealVar * mean = new RooRealVar("mean", "", 5278.2);
        RooRealVar * sigma_L = new RooRealVar("sigma_L", "", 20.67);
        RooFormulaVar * sigma_R = new RooFormulaVar("sigma_R", "@0 * @1", 
                RooArgList(*sigma_L, *ratio));

        // Make double crystal ball shape for signal
        RooCBShape * CB_L = new RooCBShape("CB_L", "", *Bd_M, 
                *mean, *sigma_L, *alpha_L, *n_L);
        RooCBShape * CB_R = new RooCBShape("CB_R", "", *Bd_M, 
                *mean, *sigma_R, *alpha_R, *n_R);
        RooAddPdf * signal_shape = new RooAddPdf("signal_shape", "",
                RooArgList(*CB_L, *CB_R), RooArgList(*frac));

        // Make exponential shape for background (slope from fit to data)
        RooRealVar * slope = new RooRealVar("slope", "", -0.00519);
        RooExponential * expo = new RooExponential("expo", "", *Bd_M, *slope);

        // Yields (from previous analysis)
        RooRealVar * toy_n_signal_Kpi = new RooRealVar("toy_n_signal_Kpi", "", 774.9);
        RooRealVar * toy_n_expo_piK = new RooRealVar("toy_n_expo_piK", "", 922.6);
        RooRealVar * toy_R_piK_vs_Kpi = new RooRealVar("toy_R_piK_vs_Kpi", "", 0.06);
        RooFormulaVar * toy_n_signal_piK = new RooFormulaVar("toy_n_signal_piK", 
                "@0 * @1", RooArgList(*toy_n_signal_Kpi, *toy_R_piK_vs_Kpi));

        // Make total fit PDF
        RooAddPdf * toy_Kpi = new RooAddPdf("toy_Kpi", "", 
                RooArgList(*signal_shape), RooArgList(*toy_n_signal_Kpi));
        RooAddPdf * toy_piK = new RooAddPdf("toy_piK", "", 
                RooArgList(*signal_shape, *expo), 
                RooArgList(*toy_n_signal_piK, *toy_n_expo_piK));
        RooSimultaneous * toy_pdf = new RooSimultaneous("toy_pdf", "", cat);
        toy_pdf->addPdf(*toy_Kpi, "Kpi");
        toy_pdf->addPdf(*toy_piK, "piK");


        // ====================
        // Generate toy dataset
        // ====================
        // Set random number seed
        TRandom * rand = new TRandom;
        rand->SetSeed();
        RooRandom::setRandomGenerator(rand);

        // Generate binned dataset
        RooDataHist * data = toy_pdf->generateBinned(RooArgList(*Bd_M, cat),
                toy_pdf->expectedEvents(cat));


        // ============
        // Make fit PDF
        // ============
        // Make yields
        RooRealVar * n_signal_Kpi;
        RooRealVar * n_expo_piK;
        RooRealVar * R_piK_vs_Kpi = new RooRealVar("R_piK_vs_Kpi", "", 0.06, 0, 10);
        if (free_Kpi_yield) {
            n_signal_Kpi = new RooRealVar("n_signal_Kpi", "", 100, 0, 8000);
        } else {
            n_signal_Kpi = new RooRealVar("n_signal_Kpi", "", toy_n_signal_Kpi->getVal());
        }
        if (free_expo_yield) {
            n_expo_piK = new RooRealVar("n_expo_piK", "", 1000, 0, 8000);
        } else {
            n_expo_piK = new RooRealVar("n_expo_piK", "", toy_n_expo_piK->getVal());
        }
        RooFormulaVar * n_signal_piK = new RooFormulaVar("n_signal_piK", "@0 * @1",
                RooArgList(*n_signal_Kpi, *R_piK_vs_Kpi));

        // Make PDFs
        RooAddPdf * fit_Kpi = new RooAddPdf("fit_Kpi", "",
                RooArgList(*signal_shape), RooArgList(*n_signal_Kpi));
        RooAddPdf * fit_piK = new RooAddPdf("fit_piK", "",
                RooArgList(*signal_shape, *expo),
                RooArgList(*n_signal_piK, *n_expo_piK));
        RooSimultaneous * fit_pdf = new RooSimultaneous("fit_pdf", "", cat);
        fit_pdf->addPdf(*fit_Kpi, "Kpi");
        fit_pdf->addPdf(*fit_piK, "piK");
        fit_pdf->Print("v");


        // ===========
        // Perform fit
        // ===========
        RooFitResult * result = fit_pdf->fitTo(*data, RooFit::Save(),
                RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
        result->Print("v");
        double NLL = result->minNll();


        // =========================================
        // Make fit PDF without piK signal component
        // =========================================
        // Make yields
        RooRealVar * null_n_signal_Kpi;
        RooRealVar * null_n_expo_piK;
        if (free_Kpi_yield) {
            null_n_signal_Kpi = new RooRealVar("null_n_signal_Kpi", "", 100, 0, 8000);
        } else {
            null_n_signal_Kpi = new RooRealVar("null_n_signal_Kpi", "",
                    toy_n_signal_Kpi->getVal());
        }
        if (free_expo_yield) {
            null_n_expo_piK = new RooRealVar("null_n_expo_piK", "", 1000, 0, 8000);
        } else {
            null_n_expo_piK = new RooRealVar("null_n_expo_piK", "",
                    toy_n_expo_piK->getVal());
        }

        // Make PDFs: piK has exponential background only
        RooAddPdf * null_fit_Kpi = new RooAddPdf("null_fit_Kpi", "",
                RooArgList(*signal_shape), RooArgList(*null_n_signal_Kpi));
        RooAddPdf * null_fit_piK = new RooAddPdf("null_fit_piK", "",
                RooArgList(*expo), RooArgList(*null_n_expo_piK));
        RooSimultaneous * null_fit_pdf = new RooSimultaneous("null_fit_pdf", "", cat);
        null_fit_pdf->addPdf(*null_fit_Kpi, "Kpi");
        null_fit_pdf->addPdf(*null_fit_piK, "piK");


        // ================================
        // Perform fit with null hypothesis
        // ================================
        RooFitResult * result_null = null_fit_pdf->fitTo(*data, RooFit::Save(),
                RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
        result_null->Print("v");
        double NLL_null = result_null->minNll();


        // ======================
        // Calculate significance
        // ======================
        double significance = sqrt(2 * (NLL_null - NLL));
        std::cout << "Significance: " << significance << std::endl;


        // -------------------------------------------------------------------------


        // ========
        // Plotting
        // ========
        // Only plot if this is a single-toy run
        if (run_many) return;

        // Get TH1s of data points
        RooDataHist * data_Kpi = (RooDataHist*)data->reduce("category==category::Kpi");
        RooDataHist * data_piK = (RooDataHist*)data->reduce("category==category::piK");
        TH1F * data_hist_Kpi = (TH1F*)data_Kpi->createHistogram("data_hist_Kpi", *Bd_M,
                RooFit::Binning(10 * nBins));
        TH1F * data_hist_piK = (TH1F*)data_piK->createHistogram("data_hist_piK", *Bd_M,
                RooFit::Binning(10 * nBins));

        // Make TH1s of fit PDFs
        TH1F * fit_hist_Kpi = (TH1F*)fit_Kpi->createHistogram("fit_hist_Kpi", *Bd_M,
                RooFit::Binning(10 * nBins));
        TH1F * fit_hist_piK = (TH1F*)fit_piK->createHistogram("fit_hist_piK", *Bd_M,
                RooFit::Binning(10 * nBins));
        TH1F * null_hist_Kpi = (TH1F*)null_fit_Kpi->createHistogram("null_hist_Kpi", 
                *Bd_M, RooFit::Binning(10 * nBins));
        TH1F * null_hist_piK = (TH1F*)null_fit_piK->createHistogram("null_hist_piK", 
                *Bd_M, RooFit::Binning(10 * nBins));

        // Make TH1s of fit components
        TH1F * expo_hist = (TH1F*)expo->createHistogram("expo_hist", *Bd_M,
                RooFit::Binning(10 * nBins));
        TH1F * null_expo_hist = (TH1F*)expo->createHistogram("null_expo_hist", *Bd_M,
                RooFit::Binning(10 * nBins));
        TH1F * signal_hist = (TH1F*)signal_shape->createHistogram("signal_hist", *Bd_M,
                RooFit::Binning(10 * nBins));

        // Scale histograms
        fit_hist_Kpi->Scale(10 * n_signal_Kpi->getVal()/fit_hist_Kpi->Integral());
        fit_hist_piK->Scale(10 * (n_signal_piK->getVal() + n_expo_piK->getVal())
                /fit_hist_piK->Integral());
        expo_hist->Scale(10 * n_expo_piK->getVal()/expo_hist->Integral());
        signal_hist->Scale(10 * n_signal_piK->getVal()/signal_hist->Integral());
        null_hist_Kpi->Scale(10 * null_n_signal_Kpi->getVal()/null_hist_Kpi->Integral());
        null_hist_piK->Scale(10 * null_n_expo_piK->getVal()/null_hist_piK->Integral());
        null_expo_hist->Scale(10 * null_n_expo_piK->getVal()/null_hist_piK->Integral());

        // Formatting
        data_hist_Kpi->SetStats(kFALSE);
        data_hist_Kpi->SetLineWidth(1);
        data_hist_piK->SetStats(kFALSE);
        data_hist_piK->SetLineWidth(1);
        fit_hist_Kpi->SetLineWidth(2);
        fit_hist_Kpi->SetLineColor(kBlack);
        fit_hist_piK->SetLineWidth(2);
        fit_hist_piK->SetLineColor(kBlack);
        signal_hist->SetLineWidth(2);
        signal_hist->SetLineColor(kRed + 2);
        signal_hist->SetMarkerColor(kRed + 2);
        expo_hist->SetLineWidth(0);
        expo_hist->SetFillStyle(1001);
        expo_hist->SetFillColor(862);
        null_expo_hist->SetLineWidth(0);
        null_expo_hist->SetFillStyle(1001);
        null_expo_hist->SetFillColor(862);

        // Get pulls
        RooPlot * frame_Kpi = Bd_M->frame();
        data_Kpi->plotOn(frame_Kpi);
        fit_Kpi->plotOn(frame_Kpi);
        RooHist * pulls_Kpi = frame_Kpi->pullHist();
        RooPlot * frame_piK = Bd_M->frame();
        data_piK->plotOn(frame_piK);
        fit_piK->plotOn(frame_piK);
        RooHist * pulls_piK = frame_piK->pullHist();
        RooPlot * null_frame_Kpi = Bd_M->frame();
        data_Kpi->plotOn(null_frame_Kpi);
        null_fit_Kpi->plotOn(null_frame_Kpi);
        RooHist * null_pulls_Kpi = null_frame_Kpi->pullHist();
        RooPlot * null_frame_piK = Bd_M->frame();
        data_piK->plotOn(null_frame_piK);
        null_fit_piK->plotOn(null_frame_piK);
        RooHist * null_pulls_piK = null_frame_piK->pullHist();

        // Make legends
        TLegend * leg_Kpi = new TLegend(0.55, 0.45, 0.85, 0.9);
        leg_Kpi->AddEntry(data_hist_Kpi, "Toy data");
        leg_Kpi->AddEntry(fit_hist_Kpi, "Fit");
        TLegend * leg_piK = new TLegend(0.55, 0.45, 0.85, 0.9);
        leg_piK->AddEntry(data_hist_piK, "Toy data");
        leg_piK->AddEntry(fit_hist_piK, "Fit");
        leg_piK->AddEntry(signal_hist, "Signal");
        leg_piK->AddEntry(expo_hist, "Combinatorial");

        // Plot and save
        // Kpi fit
        TCanvas * canvas = new TCanvas("canvas", "", 1.25 * 700, 1000);
        TPad * pad1 = new TPad("fit_pad", "", 0.0, 0.3, 1.0, 1.0);
        pad1->cd();
        data_hist_Kpi->Draw();
        fit_hist_Kpi->Draw("C SAME");
        data_hist_Kpi->Draw("E SAME");
        leg_Kpi->Draw();
        TPad * pad2 = new TPad("pull_pad", "", 0.0, 0.0, 1.0, 0.3);
        pad2->cd();
        RooPlot * frame = Bd_M->frame();
        frame->addPlotable(pulls_Kpi, "BEX0");
        frame->Draw();
        canvas->cd();
        pad1->Draw();
        pad2->Draw();
        canvas->SaveAs("Plots/Fit_withSignal_Kpi.pdf");

        // piK fit
        pad1->cd();
        data_hist_piK->Draw();
        expo_hist->Draw("C SAME");
        signal_hist->Draw("C SAME");
        fit_hist_piK->Draw("C SAME");
        data_hist_piK->Draw("E SAME");
        leg_piK->Draw();
        pad2->cd();
        frame->remove();
        frame->addPlotable(pulls_piK, "BEX0");
        frame->Draw();
        canvas->cd();
        pad1->Draw();
        pad2->Draw();
        canvas->SaveAs("Plots/Fit_withSignal_piK.pdf");

        // Kpi null fit
        pad1->cd();
        data_hist_Kpi->Draw();
        fit_hist_Kpi->Draw("C SAME");
        data_hist_Kpi->Draw("E SAME");
        leg_Kpi->Draw();
        pad2->cd();
        frame->remove();
        frame->addPlotable(null_pulls_Kpi, "BEX0");
        frame->Draw();
        canvas->cd();
        pad1->Draw();
        pad2->Draw();
        canvas->SaveAs("Plots/Fit_null_Kpi.pdf");

        // piK null fit
        pad1->cd();
        data_hist_piK->Draw();
        null_hist_piK->Draw("C SAME");
        data_hist_piK->Draw("E SAME");
        leg_Kpi->Draw();
        pad2->cd();
        frame->remove();
        frame->addPlotable(null_pulls_piK, "BEX0");
        frame->Draw();
        canvas->cd();
        pad1->Draw();
        pad2->Draw();
        canvas->SaveAs("Plots/Fit_null_piK.pdf");
    }

    } // End loop over toys

    // =================
    // Save results file
    // =================
    results_file->cd();
    toy_tree->Write();
    results_file->Close();

}
