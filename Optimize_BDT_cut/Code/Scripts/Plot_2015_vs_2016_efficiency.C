// =====================================================
// Script to compare efficiency curves for 2015 and 2016
// =====================================================
void Plot_2015_vs_2016_efficiency() {

    for (TString type : {"signal", "background"}) {
        for (TString mode : {"Kpi", "KK", "pipi"}) {

            TFile * file_2015 = TFile::Open("../../Efficiencies/2015_down/" + type + 
                    "_" + mode + ".root", "READ");
            TFile * file_2016 = TFile::Open("../../Efficiencies/2016_down/" + type + 
                    "_" + mode + ".root", "READ");
            TGraphErrors * graph_2015 = (TGraphErrors*)file_2015->Get("graph");
            TGraphErrors * graph_2016 = (TGraphErrors*)file_2016->Get("graph");

            graph_2015->SetLineColor(kRed);
            graph_2016->SetLineColor(kBlue);

            TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
            graph_2015->Draw();
            graph_2016->Draw("SAME");
            canvas->SaveAs("../../Plots/2015_vs_2016_" + type + "_" + mode + ".pdf");
        }
    }
}
