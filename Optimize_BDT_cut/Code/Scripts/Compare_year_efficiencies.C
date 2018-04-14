// =====================================================
// Script to compare efficiency curves for different years
// =====================================================
void Compare_year_efficiencies(int year1, int year2) {

    for (std::string type : {"signal", "background"}) {
        for (std::string mode : {"Kpi", "KK", "pipi", "Kpipipi"}) {

            // Exclude modes without correct MC
            if (mode != "Kpi" && (year1 == 2011 || year2 == 2011)) continue;
            if (mode == "Kpipipi" && (year1 == 2015 || year2 == 2015)) continue;

            std::string year1_str = std::to_string(year1);
            std::string year2_str = std::to_string(year2);

            TFile * file1 = TFile::Open(("../../Efficiencies/" + year1_str
                    + "_down/" + type + "_" + mode + ".root").c_str(), "READ");
            TFile * file2 = TFile::Open(("../../Efficiencies/" + year2_str
                    + "_down/" + type + "_" + mode + ".root").c_str(), "READ");
            TGraphErrors * graph1 = (TGraphErrors*)file1->Get("graph");
            TGraphErrors * graph2 = (TGraphErrors*)file2->Get("graph");

            graph1->SetLineColor(kRed);
            graph2->SetLineColor(kBlue);

            TLegend leg(0.15, 0.2, 0.4, 0.35);
            leg.AddEntry(graph1, year1_str.c_str(), "L");
            leg.AddEntry(graph2, year2_str.c_str(), "L");

            graph1->GetXaxis()->SetTitle((mode + " BDT cut").c_str());
            graph1->GetYaxis()->SetTitle(("Relative efficiency (" + type + ")").c_str());

            TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
            graph1->Draw();
            graph2->Draw("SAME");
            leg.Draw();
            canvas->SaveAs(("../../Plots/" + year1_str + "_vs_" + year2_str + "_"
                        + type + "_" + mode + ".pdf").c_str());
        }
    }
}
