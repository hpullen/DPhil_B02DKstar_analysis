// Quickly fit to D meson mass to find the width
void quick_fit_D() {

    std::string filename = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/2016_down/Kpi_selected.root";
    TFile *file = TFile::Open(filename.c_str(), "READ");
    TTree *tree = (TTree*)file->Get("DecayTree");
    tree->Draw("D0_M>>hist");
    TH1F *hist = (TH1F*)gDirectory->Get("hist");
    hist->Fit("gaus");

    TCanvas *canv = new TCanvas("canv", "", 900, 600);
    hist->Draw("E");
    TF1 *fit = hist->GetFunction("gaus");
    fit->Draw("C SAME");
    canv->SaveAs("~/D_mass_fit.pdf");




}
