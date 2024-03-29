// ===================================================
// Plot all components of DKpipi background normalized
// ===================================================
void PlotDalitz() {

    // Open files and get trees
    TString dir = "/data/lhcb/users/pullen/RapidSim/B_DKpipi_study/";

    // 1. B+ -> K*(1680)+ D0
    TFile * file_1a = TFile::Open(dir + "1_K_1680/Kstar/tree_full.root", "READ");
    TTree * tree_1a = (TTree*)file_1a->Get("DecayTree");
    TFile * file_1b = TFile::Open(dir + "1_K_1680/rho_K/tree_full.root", "READ");
    TTree * tree_1b = (TTree*)file_1b->Get("DecayTree");

    // 2. B+ -> K*0 D0 pi+
    TFile * file_2 = TFile::Open(dir + "2_Kstar/tree_full.root", "READ");
    TTree * tree_2 = (TTree*)file_2->Get("DecayTree");

    // 3. B+ -> K1(1400)+ D0
    TFile * file_3 = TFile::Open(dir + "3_K1_1400/tree_full.root", "READ");
    TTree * tree_3 = (TTree*)file_3->Get("DecayTree");

    // 4. B+ -> K*0(1430)0 D0
    TFile * file_4 = TFile::Open(dir + "4_K0_1430/tree_full.root", "READ");
    TTree * tree_4 = (TTree*)file_4->Get("DecayTree");

    // 5. B+ -> K*(1410)+ D0
    TFile * file_5a = TFile::Open(dir + "5_K_1410/Kstar/tree_full.root", "READ");
    TTree * tree_5a = (TTree*)file_5a->Get("DecayTree");
    TFile * file_5b = TFile::Open(dir + "5_K_1410/rho_K/tree_full.root", "READ");
    TTree * tree_5b = (TTree*)file_5b->Get("DecayTree");

    // 6. B+ -> rho0 D0 K+
    TFile * file_6 = TFile::Open(dir + "6_K_rho/tree_full.root", "READ");
    TTree * tree_6 = (TTree*)file_6->Get("DecayTree");

    // 7. B+ -> K1(1270)+ D0
    TFile * file_7a = TFile::Open(dir + "7_K1_1270/Kstar/tree_full.root", "READ");
    TTree * tree_7a = (TTree*)file_7a->Get("DecayTree");
    TFile * file_7b = TFile::Open(dir + "7_K1_1270/rho_K/tree_full.root", "READ");
    TTree * tree_7b = (TTree*)file_7b->Get("DecayTree");

    // Ignore 8 and 9 for now: opposite sign decays

    // 10. Non-resonant B+ -> K+ pi- pi+ D0
    TFile * file_10 = TFile::Open(dir + "10_nonres/tree_full.root", "READ");
    TTree * tree_10 = (TTree*)file_10->Get("DecayTree");

    // Make cut
    // TCut cut = "abs(Kstar_M - 0.8955) < 0.05 && Bd_M > 5";
    TCut cut = "nEvent < 50000";

    // Make histograms
    double min = 0;
    double max_D = 30;
    double max_K = 15;
    TH2F * hist_1a = new TH2F("hist_1a", "", 100, min, max_K, 100, min, max_D);
    TH2F * hist_1b = new TH2F("hist_1b", "", 100, min, max_K, 100, min, max_D);
    TH2F * hist_2 = new TH2F("hist_2", "", 100, min, max_K, 100, min, max_D);
    TH2F * hist_3 = new TH2F("hist_3", "", 100, min, max_K, 100, min, max_D);
    TH2F * hist_4 = new TH2F("hist_4", "", 100, min, max_K, 100, min, max_D);
    TH2F * hist_5a = new TH2F("hist_5a", "", 100, min, max_K, 100, min, max_D);
    TH2F * hist_5b = new TH2F("hist_5b", "", 100, min, max_K, 100, min, max_D);
    TH2F * hist_6 = new TH2F("hist_6", "", 100, min, max_K, 100, min, max_D);
    TH2F * hist_7a = new TH2F("hist_7a", "", 100, min, max_K, 100, min, max_D);
    TH2F * hist_7b = new TH2F("hist_7b", "", 100, min, max_K, 100, min, max_D);
    TH2F * hist_10 = new TH2F("hist_10", "", 100, min, max_K, 100, min, max_D);

    // Fill histograms
    tree_1a->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_1a", cut);
    tree_1b->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_1b", cut);
    tree_2->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_2", cut);
    tree_3->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_3", cut);
    tree_4->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_4", cut);
    tree_5a->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_5a", cut);
    tree_5b->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_5b", cut);
    tree_6->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_6", cut);
    tree_7a->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_7a", cut);
    tree_7b->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_7b", cut);
    tree_10->Draw("D0_Kstar_M2_TRUE:Kstar_pi_M2_TRUE>>hist_10", cut);

    // Set colours
    hist_1a->SetMarkerColor(kRed);
    hist_1b->SetMarkerColor(kGray);
    hist_2->SetMarkerColor(kBlue);
    hist_3->SetMarkerColor(kOrange);
    hist_4->SetMarkerColor(kYellow);
    hist_5a->SetMarkerColor(kCyan);
    hist_5b->SetMarkerColor(kSpring);
    hist_6->SetMarkerColor(kViolet);
    hist_7a->SetMarkerColor(kMagenta);
    hist_7b->SetMarkerColor(kPink);
    hist_10->SetMarkerColor(kGreen);

    // Small markers
    hist_1a->SetMarkerSize(1);
    hist_1b->SetMarkerSize(1);
    hist_2->SetMarkerSize(1);
    hist_3->SetMarkerSize(1);
    hist_4->SetMarkerSize(1);
    hist_5a->SetMarkerSize(1);
    hist_5b->SetMarkerSize(1);
    hist_6->SetMarkerSize(1);
    hist_7a->SetMarkerSize(1);
    hist_7b->SetMarkerSize(1);
    hist_10->SetMarkerSize(1);
    hist_1a->SetMarkerStyle(1);
    hist_1b->SetMarkerStyle(1);
    hist_2->SetMarkerStyle(1);
    hist_3->SetMarkerStyle(1);
    hist_4->SetMarkerStyle(1);
    hist_5a->SetMarkerStyle(1);
    hist_5b->SetMarkerStyle(1);
    hist_6->SetMarkerStyle(1);
    hist_7a->SetMarkerStyle(1);
    hist_7b->SetMarkerStyle(1);
    hist_10->SetMarkerStyle(1);

    // No lines
    hist_1a->SetLineColor(0);
    hist_1b->SetLineColor(0);
    hist_2->SetLineColor(0);
    hist_3->SetLineColor(0);
    hist_4->SetLineColor(0);
    hist_5a->SetLineColor(0);
    hist_5b->SetLineColor(0);
    hist_6->SetLineColor(0);
    hist_7a->SetLineColor(0);
    hist_7b->SetLineColor(0);
    hist_10->SetLineColor(0);

    // // Set labels
    // hist_1a->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_1a->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");
    // hist_1b->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_1b->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");
    // hist_2->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_2->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");
    // hist_3->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_3->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");
    // hist_4->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_4->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");
    // hist_5a->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_5a->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");
    // hist_5b->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_5b->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");
    // hist_6->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_6->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");
    // hist_7a->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_7a->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");
    // hist_7b->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_7b->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");
    // hist_10->GetXaxis()->SetTitle("m^{2}(K^{*0}#pi^{+})");
    // hist_10->GetYaxis()->SetTitle("m^{2}(D^{0}K^{*0})");

    // Set labels
    hist_1a->GetXaxis()->SetTitle("B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    hist_1b->GetXaxis()->SetTitle("B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}");
    hist_2->GetXaxis()->SetTitle("B^{+} #rightarrow K^{*0} #bar{D}^{0} #pi^{+}");
    hist_3->GetXaxis()->SetTitle("B^{+} #rightarrow (K_{1}(1400)^{+} #rightarrow K^{*0} "
            "#pi^{+}) #bar{D}^{0}");
    hist_4->GetXaxis()->SetTitle("B^{+} #rightarrow K_{0}^{*}(1430)^{0} #pi^{+} "
            "#bar{D}^{0}");
    hist_5a->GetXaxis()->SetTitle("B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    hist_5b->GetXaxis()->SetTitle("B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}");
    hist_6->GetXaxis()->SetTitle("B^{+} #rightarrow K^{+} #rho^{0} #bar{D}^{0}");
    hist_7a->GetXaxis()->SetTitle("B^{+} #rightarrow (K_{1}(1270)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    hist_7b->GetXaxis()->SetTitle("B^{+} #rightarrow (K_{1}(1270)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}");
    hist_10->GetXaxis()->SetTitle("B^{+} #rightarrow K^{+} #pi^{-} #pi^{+} #bar{D}^{0}");

    // // Set ranges
    // hist_1a->SetMinimum(3);
    // hist_1b->SetMinimum(3);
    // hist_2->SetMinimum(3);
    // hist_3->SetMinimum(3);
    // hist_4->SetMinimum(3);
    // hist_5a->SetMinimum(3);
    // hist_5b->SetMinimum(3);
    // hist_6->SetMinimum(3);
    // hist_7a->SetMinimum(3);
    // hist_7b->SetMinimum(3);
    // hist_10->SetMinimum(3);


    // Draw and save each histogram
    TCanvas * canvas = new TCanvas("canvas", "", 800, 600);
    canvas->Divide(4, 3);
    canvas->cd(1);
    hist_1a->SetTitle("B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    hist_1a->Draw("COLZ");
    TLine line_1a(2.9515, 0, 2.9515, 30);
    line_1a.SetLineWidth(1);
    line_1a.SetLineStyle(2);
    line_1a.SetLineColor(kRed);
    line_1a.Draw("SAME");
    canvas->cd(2);
    hist_1b->SetTitle("B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}");
    hist_1b->Draw("COLZ");
    line_1a.Draw("SAME");
    canvas->cd(3);
    hist_2->SetTitle("B^{+} #rightarrow K^{*0} #bar{D}^{0} #pi^{+}");
    hist_2->Draw("COLZ");
    canvas->cd(4);
    hist_3->SetTitle("B^{+} #rightarrow (K_{1}(1400)^{+} #rightarrow K^{*0} "
            "#pi^{+}) #bar{D}^{0}");
    hist_3->Draw("COLZ");
    TLine line_3 = line_1a;
    line_3.SetX1(1.9684);
    line_3.SetX2(1.9684);
    line_3.Draw("SAME");
    canvas->cd(5);
    hist_4->SetTitle("B^{+} #rightarrow K_{0}^{*}(1430)^{0} #pi^{+} "
            "#bar{D}^{0}");
    hist_4->Draw("COLZ");
    canvas->cd(6);
    hist_5a->SetTitle("B^{+} #rightarrow K_{0}^{*}(1430)^{0} #pi^{+} "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    hist_5a->Draw("COLZ");
    TLine line_5a = line_1a;
    line_5a.SetX1(1.9994);
    line_5a.SetX2(1.9994);
    line_5a.Draw("SAME");
    canvas->cd(7);
    hist_5b->SetTitle("B^{+} #rightarrow K_{0}^{*}(1430)^{0} #pi^{+} "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    hist_5b->Draw("COLZ");
    line_5a.Draw("SAME");
    canvas->cd(8);
    hist_6->SetTitle("B^{+} #rightarrow K^{+} #rho^{0} #bar{D}^{0}");
    hist_6->Draw("COLZ");
    canvas->cd(9);
    hist_7a->SetTitle("B^{+} #rightarrow (K_{1}(1270)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    hist_7a->Draw("COLZ");
    TLine line_7a = line_1a;
    line_7a.SetX1(1.61798);
    line_7a.SetX2(1.61798);
    line_7a.Draw("SAME");
    canvas->cd(10);
    hist_7b->SetTitle("B^{+} #rightarrow (K_{1}(1270)^{+} #rightarrow "
            "#rho^{0} K^{+}) #bar{D}^{0}");
    hist_7b->Draw("COLZ");
    line_7a.Draw("SAME");
    canvas->cd(11);
    hist_10->SetTitle("B^{+} #rightarrow K^{+} #pi^{-} #pi^{+} #bar{D}^{0}");
    hist_10->Draw("COLZ");

    // // Make histograms with bigger markers for legend
    // TH2F * temp_1a = new TH2F(*hist_1a);
    // TH2F * temp_1b = new TH2F(*hist_1b);
    // TH2F * temp_2 = new TH2F(*hist_2);
    // TH2F * temp_3 = new TH2F(*hist_3);
    // TH2F * temp_4 = new TH2F(*hist_4);
    // TH2F * temp_5a = new TH2F(*hist_5a);
    // TH2F * temp_5b = new TH2F(*hist_5b);
    // TH2F * temp_6 = new TH2F(*hist_6);
    // TH2F * temp_7a = new TH2F(*hist_7a);
    // TH2F * temp_7b = new TH2F(*hist_7b);
    // TH2F * temp_10 = new TH2F(*hist_10);
    // temp_1a->SetMarkerStyle(8);
    // temp_1b->SetMarkerStyle(8);
    // temp_2->SetMarkerStyle(8);
    // temp_3->SetMarkerStyle(8);
    // temp_4->SetMarkerStyle(8);
    // temp_5a->SetMarkerStyle(8);
    // temp_5b->SetMarkerStyle(8);
    // temp_6->SetMarkerStyle(8);
    // temp_7a->SetMarkerStyle(8);
    // temp_7b->SetMarkerStyle(8);
    // temp_10->SetMarkerStyle(8);

    // Make legend
    // TLegend * leg = new TLegend(0, 0, 1, 1);
    // leg->AddEntry(temp_1a, "B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            // "K^{*0}#pi^{+}) #bar{D}^{0}");
    // leg->AddEntry(temp_1b, "B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            // "#rho^{0} K^{+}) #bar{D}^{0}");
    // leg->AddEntry(temp_2, "B^{+} #rightarrow K^{*0} #bar{D}^{0} #pi^{+}");
    // leg->AddEntry(temp_3, "B^{+} #rightarrow (K_{1}(1400)^{+} #rightarrow K^{*0} "
            // "#pi^{+}) #bar{D}^{0}");
    // leg->AddEntry(temp_4, "B^{+} #rightarrow K_{0}^{*}(1430)^{0} #pi^{+} "
            // "#bar{D}^{0}");
    // leg->AddEntry(temp_5a, "B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            // "K^{*0}#pi^{+}) #bar{D}^{0}");
    // leg->AddEntry(temp_5b, "B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            // "#rho^{0} K^{+}) #bar{D}^{0}");
    // leg->AddEntry(temp_6, "B^{+} #rightarrow K^{+} #rho^{0} #bar{D}^{0}");
    // leg->AddEntry(temp_7a, "B^{+} #rightarrow (K_{1}(1270)^{+} #rightarrow "
            // "K^{*0}#pi^{+}) #bar{D}^{0}");
    // leg->AddEntry(temp_7b, "B^{+} #rightarrow (K_{1}(1270)^{+} #rightarrow "
            // "#rho^{0} K^{+}) #bar{D}^{0}");
    // leg->AddEntry(temp_10, "B^{+} #rightarrow K^{+} #pi^{-} #pi^{+} #bar{D}^{0}");
    // canvas->cd(12);
    // leg->Draw();
    canvas->SaveAs("../Plots/Dalitz_all.pdf");

}
