// Macro to plot B0 and B0-bar PDFs overlaid
void PlotFlavourDifference() {

    // Open file
    gROOT->ForceStyle();
    TFile * file = TFile::Open("../Histograms/twoAndFourBody_data_split.root", "READ");

    // Lists of modes and runs
    std::vector<TString> modes = {
        "Kpi",
        "piK",
        "KK",
        "pipi",
        "Kpipipi",
        "piKpipi",
        "pipipipi"
    };
    std::vector<TString> runs = {"_run1", "_run2"};

    // Loop through modes and runs
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    for (auto mode : modes) {
        for (auto run : runs) {

            // Check if it exists
            if (!file->GetListOfKeys()->Contains("fit_" + mode + run + "_plus")) {
                continue;
            }

            // Get fit histograms for both
            TH1F * hist_plus = (TH1F*)file->Get("fit_" + mode + run + "_plus");
            TH1F * hist_minus = (TH1F*)file->Get("fit_" + mode + run + "_minus");

            // Set styles
            gROOT->ForceStyle();
            hist_plus->SetLineColor(kRed);
            hist_plus->SetLineWidth(1);
            hist_minus->SetLineColor(ANABlue);
            hist_minus->SetLineWidth(1);

            // Make legend
            TLegend * leg = new TLegend(0.65, 0.7, 0.85, 0.9);
            leg->AddEntry(hist_plus, "B^{0}", "L");
            leg->AddEntry(hist_minus, "#bar{B}^{0}", "L");

            // Plot them
            canvas->Clear();
            hist_minus->Draw("C");
            hist_plus->Draw("C SAME");
            leg->Draw();
            canvas->SaveAs("../Plots/flavour_comparison/" + mode + run + ".pdf");
        }
    }




}
