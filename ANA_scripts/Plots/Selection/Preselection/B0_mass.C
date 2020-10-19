// =============================================
// Plot B0 mass distribution after preselection 
// =============================================
void B0_mass(std::string run = "") {

    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    std::map<std::string, std::string> files = {
        {"preselection", "Kpi.root"},
        {"selected", "Kpi_selected.root"}
    };
    for (auto f : files) {

        // Open data files
        TString dir = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
        TChain * tree = new TChain("DecayTree");
        std::vector<TString> years = {};
        if (run == "" || run == "run1") {
           years.push_back("2011");
           years.push_back("2012");
        } 
        if (run == "" || run == "run2") {
            years.push_back("2015");
            years.push_back("2016");
        }
        for (TString year : years) {
            for (TString mag : {"_up", "_down"}) {
                tree->Add(dir + year + mag + "/" + f.second);
            }
        }

        // Draw B mass
        TH1F * hist = new TH1F("hist", "", 200, 5000, 5800);
        tree->Draw("Bd_ConsD_M>>hist");
        int bin_width = hist->GetBinWidth(1);

        // Set maximum
        double y_max = hist->GetMaximum() * 1.05;
        hist->GetYaxis()->SetRangeUser(0, y_max);

        // Draw hist
        canvas->Clear();
        hist->SetStats(false);
        hist->SetLineWidth(1);
        hist->SetLineColor(kBlack);
        hist->GetXaxis()->SetTitle("#it{m}([K#pi]_{D}K^{*0}) [MeV/#it{c}^{2}]");
        hist->GetYaxis()->SetTitle(("Candidates / (" + std::to_string(bin_width) + 
                    " MeV/#it{c}^{2})").c_str());
        hist->Draw("E");

        // Add line at 5800
        TLine line(5800, 0, 5800, y_max);
        line.SetLineStyle(2);
        line.SetLineColor(kRed);
        line.Draw();
        gPad->RedrawAxis();

        // Save
        std::string year_str = (run == "") ? "" : "_" + run;
        canvas->SaveAs(("../../../../ANA_resources/Plots/Selection/Preselection/B0_mass_" + f.first + year_str + ".pdf").c_str());
    }
}
