// =================================================
// Script to plot all modes together to compare size
// =================================================
void PlotAllModes() {

    // List of modes
    std::vector<std::string> modes = {
        "1_Kstar_1680/Kstar",
        "1_Kstar_1680/rho",
        "2_Kstar_892",
        "3_K1_1400/Kstar",
        "3_K1_1400/rho",
        "3_K1_1400/non_res",
        "4_Kstar_1430",
        "5_Kstar_1410/Kstar",
        "5_Kstar_1410/rho",
        "6_rho",
        "7_K1_1270/Kstar",
        "7_K1_1270/rho",
        "10_nonres"
    };

    // Scaling factors for each mode
    std::map<std::string, double> scales = {
        {"1_Kstar_1680/Kstar", 0.175 * 0.67},
        {"1_Kstar_1680/rho", 0.175 * 0.33}, 
        {"2_Kstar_892", 0.175},
        {"3_K1_1400/Kstar", 0.14 * 0.63 / (0.63 + 0.1 + 0.13)},
        {"3_K1_1400/rho", 0.14 * 0.1 / (0.63 + 0.1 + 0.13)},
        {"3_K1_1400/non_res", 0.14 * 0.13 / (0.63 + 0.1 + 0.13)},
        {"4_Kstar_1430", 0.13},
        {"5_Kstar_1410/Kstar", 0.12 * 0.9},
        {"5_Kstar_1410/rho", 0.12 * 0.1},
        {"6_rho", 0.11},
        {"7_K1_1270/Kstar", 0.05 * 0.14 / (0.14 + 0.071)}, 
        {"7_K1_1270/rho", 0.05 * 0.071 / (0.14 + 0.071)}, 
        {"10_nonres", 0.03}
    };

    // Settings for each mode
    std::map<std::string, int> line_style = {
        {"1_Kstar_1680/Kstar", 1},
        {"1_Kstar_1680/rho", 2}, 
        {"2_Kstar_892", 1},
        {"3_K1_1400/Kstar", 1},
        {"3_K1_1400/rho", 2},
        {"3_K1_1400/non_res", 3},
        {"4_Kstar_1430", 1},
        {"5_Kstar_1410/Kstar", 1},
        {"5_Kstar_1410/rho", 2},
        {"6_rho", 1},
        {"7_K1_1270/Kstar", 1}, 
        {"7_K1_1270/rho", 2}, 
        {"10_nonres", 1}
    };
    std::map<std::string, int> colors = {
        {"1_Kstar_1680/Kstar", kRed},
        {"1_Kstar_1680/rho", kRed}, 
        {"2_Kstar_892", kBlue},
        {"3_K1_1400/Kstar", kOrange},
        {"3_K1_1400/rho", kOrange},
        {"3_K1_1400/non_res", kOrange},
        {"4_Kstar_1430", kYellow},
        {"5_Kstar_1410/Kstar", kCyan},
        {"5_Kstar_1410/rho", kCyan},
        {"6_rho", kViolet},
        {"7_K1_1270/Kstar", kMagenta}, 
        {"7_K1_1270/rho", kMagenta}, 
        {"10_nonres", kGreen}
    };
    std::map<std::string, TString> leg_entries = {
        {"1_Kstar_1680/Kstar", "B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}"},
        {"1_Kstar_1680/rho", "B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}"},
        {"2_Kstar_892", "B^{+} #rightarrow K^{*0} #bar{D}^{0} #pi^{+}"},
        {"3_K1_1400/Kstar", "B^{+} #rightarrow (K_{1}(1400)^{+} #rightarrow K^{*0} "
            "#pi^{+}) #bar{D}^{0}"},
        {"3_K1_1400/rho", "B^{+} #rightarrow (K_{1}(1400)^{+} #rightarrow #rho^{0} "
            "#pi^{+}) #bar{D}^{0}"},
        {"3_K1_1400/non_res", "B^{+} #rightarrow (K_{1}(1400)^{+} #rightarrow "
            "K^{+} #pi^{-} #pi^{+}) #bar{D}^{0}"},
        {"4_Kstar_1430", "B^{+} #rightarrow K_{0}^{*}(1430)^{0} #pi^{+} "
            "#bar{D}^{0}"},
        {"5_Kstar_1410/Kstar", "B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}"},
        {"5_Kstar_1410/rho", "B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}"},
        {"6_rho", "B^{+} #rightarrow K^{+} #rho^{0} #bar{D}^{0}"},
        {"7_K1_1270/Kstar", "B^{+} #rightarrow (K_{1}(1270)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}"},
        {"7_K1_1270/rho", "B^{+} #rightarrow (K_{1}(1270)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}"},
        {"10_nonres", "B^{+} #rightarrow K^{+} #pi^{-} #pi^{+} #bar{D}^{0}"}
    };

    // Cut to apply to masses
    TString cut = "abs(Kstar_M - 0.8955) < 0.05 && abs(D0_M - 1.86484) < 0.025" 
        " && abs(helicity_angle) > 0.4";

    // Map to store yields
    std::map<std::string, double> yields;
    std::map<std::string, TH1F*> hists;

    // Canvas to plot on
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    TLegend * leg = new TLegend(0.55, 0.2, 0.85, 0.9);

    // Histogram and tree to hold overall shape
    TH1F * hist_all = new TH1F("hist_all", "", 200, 5000, 5400);
    TH1F * hist_hel = new TH1F("hist_hel", "", 100, -1, 1);
    TH1F * hist_Kstar = new TH1F("hist_Kstar", "", 100, 840, 960);
    TH1F * hist_D0 = new TH1F("hist_D0", "", 100, 1800, 1900);

    // Loop through modes
    for (auto mode : modes) {

        // Make histogram
        hists[mode] = new TH1F(("mass_hist_" + mode).c_str(), "", 200, 5000, 
                5400);

        // Open tree
        TFile * file = TFile::Open(("/data/lhcb/users/pullen/RapidSim/DKpipi/" + 
                mode + "/tree.root").c_str(), "READ");
        TTree * tree = (TTree*)file->Get("SelectedTree");

        // Apply mass cuts
        tree->Draw(">> elist", cut);
        TEventList * elist = (TEventList*)gDirectory->Get("elist");

        // Mass branch
        double Bd_M;
        tree->SetBranchAddress("Bd_M", &Bd_M);
        double D0_M;
        double Kstar_M;
        double helicity_angle;
        tree->SetBranchAddress("D0_M", &D0_M);
        tree->SetBranchAddress("Kstar_M", &Kstar_M);
        tree->SetBranchAddress("helicity_angle", &helicity_angle);

        // Calculate angle and fill histogram
        for (int i = 0; i < elist->GetN(); i++) {
            int evt = elist->GetEntry(i);
            tree->GetEntry(evt);
            hists[mode]->Fill(Bd_M * 1000);
            hist_all->Fill(Bd_M * 1000, scales[mode]);
            hist_D0->Fill(D0_M * 1000, scales[mode]);
            hist_Kstar->Fill(Kstar_M * 1000, scales[mode]);
            hist_hel->Fill(helicity_angle, scales[mode]);
        }

        // Draw histogram
        hists[mode]->Scale(scales[mode]);
        hists[mode]->SetLineWidth(1);
        hists[mode]->SetLineStyle(line_style[mode]);
        hists[mode]->SetLineColor(colors[mode]);
        hists[mode]->GetXaxis()->SetTitle("Reconstructed B^{0} mass [MeV/#it{c}^{2}]");
        hists[mode]->GetYaxis()->SetTitle("Selected candidates / (4 Mev/#it{c}^{2})");
        leg->AddEntry(hists[mode], leg_entries[mode], "L");

        // Add to yield map
        yields[mode] = hists[mode]->Integral();
    }

    // Plot tallest hist
    double max = 0;
    for (auto hist : hists) {
        if (hist.second->GetMaximum() > max) {
            max = hist.second->GetMaximum();
            hist.second->Draw("HIST"); 
        }
    }

    // Draw all others
    for (auto hist : hists) {
        hist.second->Draw("HIST SAME");
    }
    leg->Draw();
    gPad->RedrawAxis();
    canvas->SaveAs("../Plots/Components/all.pdf");

    // Print yields
    double total;
    double total_error;
    for (auto yield : yields) {
        total += yield.second;
        total_error = sqrt(total);
    }
    std::ofstream outfile("../Results/yield_fractions.param");
    for (auto yield : yields) {
        double yield_error = yield.second/total * sqrt(1/yield.second + 1/total);
        outfile << yield.first << ": " << yield.second/total << " " <<
            yield_error << std::endl;
    }
    outfile.close();

    // Plot overall shape
    canvas->Clear();
    hist_all->GetXaxis()->SetTitle("Reconstructed B^{0} mass [MeV/#it{c}^{2}]");
    hist_all->GetYaxis()->SetTitle("Selected candidates / (4 Mev/#it{c}^{2})");
    hist_all->SetLineColor(kBlack);
    hist_all->SetLineWidth(1);
    hist_all->Draw("HIST");
    canvas->SaveAs("../Plots/Components/overall_shape.pdf");

    // Save histograms
    TFile * hist_file = TFile::Open("../Results/histograms.root", "RECREATE");
    hist_all->Write("Bd_M");
    hist_hel->Write("helicity_angle");
    hist_Kstar->Write("Kstar_M");
    hist_D0->Write("D0_M");
    hist_file->Close();

}
