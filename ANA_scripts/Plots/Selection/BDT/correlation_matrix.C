// =====================================================
// Plot response curves for training and testing samples
// =====================================================
void correlation_matrix() {

    TString mode = "Kpi";
    TString BDT_name = mode + "_run2";
    TCanvas * canvas = new TCanvas("canvas", "", 6000, 6000);
    canvas->Divide(1, 2);
    gROOT->ForceStyle();

    // Get histograms
    TFile * file = TFile::Open("/home/pullen/analysis/B02DKstar/BDT/Outputs/"
            "Classification_" + BDT_name + ".root", "READ");
    std::map<TString, TH2F*> hists;
    hists["Signal"] = (TH2F*)file->Get("CorrelationMatrixS");
    hists["Background"] = (TH2F*)file->Get("CorrelationMatrixB");

    // Draw
    int count = 1;
    for (std::pair<TString, TH2F*> hist : hists) {
        TPad * current = (TPad*)canvas->cd(count);
        current->SetLeftMargin(0.3);
        current->SetBottomMargin(0.25);
        hist.second->SetTitle(hist.first + " linear correlation coefficients in %");
        hist.second->SetMarkerSize(2);
        hist.second->SetMarkerColor(kWhite);
        hist.second->Draw("TEXT COLZ");
        gPad->RedrawAxis();
        count++;
        std::cout << "Palette: " << hist.second->GetColorPalette() << std::endl;
    }

    // Save
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/ANA_plots/Plots/Selection/"
            "BDT/correlation_matrix.pdf");


}
