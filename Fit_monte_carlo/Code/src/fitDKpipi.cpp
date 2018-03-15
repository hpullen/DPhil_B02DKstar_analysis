#include <fstream>
#include <iostream>

#include "TApplication.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TString.h"

#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooCBShape.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooHILLdini.h"
#include "RooHist.h"
#include "RooHORNSdini.h"
#include "RooPlotable.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "PlotStyle.hpp"
#include "Plotter.hpp"

// Program for fitting to DKpipi MC with a double crystal ball shape
int main(int argc, char * argv[]) {

    // Path to DKpipi MC
    TString path = "/data/lhcb/users/pullen/RapidSim/B_DKpipi_study/";

    // Load the top 4 contributions
    TFile * file_1a = TFile::Open(path + "1_K_1680/Kstar/tree_full.root", "READ");
    TTree * tree_1a = (TTree*)file_1a->Get("DecayTree");
    TFile * file_2 = TFile::Open(path + "2_Kstar/tree_full.root", "READ");
    TTree * tree_2 = (TTree*)file_2->Get("DecayTree");
    TFile * file_3 = TFile::Open(path + "3_K1_1400/tree_full.root", "READ");
    TTree * tree_3 = (TTree*)file_3->Get("DecayTree");
    TFile * file_5a = TFile::Open(path + "5_K_1410/Kstar/tree_full.root", "READ");
    TTree * tree_5a = (TTree*)file_5a->Get("DecayTree");

    // Get entries in each tree for normalising
    int n_1a = tree_1a->GetEntries();
    int n_2 = tree_2->GetEntries();
    int n_3 = tree_3->GetEntries();
    int n_5a = tree_5a->GetEntries();

    // Make cut on daughter masses
    std::string cut = "(abs(D0_M - 1.86484) < 0.025 && abs(Kstar_M - 0.8955) < 0.05)";

    // Set up mass
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5000, 5150, "MeV/c^{2}");
    // RooRealVar Bd_M("Bd_ConsD_MD_reduced", "", 4500, 5300, "MeV/c^{2}");
    int binWidth = 2;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    double nBins_reduced = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);
    Bd_M.setBins(nBins_reduced);

    // Draw weighted trees into histogram
    setPlotStyle();
    TH1F * hist = new TH1F("hist", "", nBins, Bd_M.getMin(), Bd_M.getMax());
    TH1F * hist_1a = new TH1F("hist_1a", "", nBins, Bd_M.getMin(), Bd_M.getMax());
    TH1F * hist_2 = new TH1F("hist_2", "", nBins, Bd_M.getMin(), Bd_M.getMax());
    TH1F * hist_3 = new TH1F("hist_3", "", nBins, Bd_M.getMin(), Bd_M.getMax());
    TH1F * hist_5a = new TH1F("hist_5a", "", nBins, Bd_M.getMin(), Bd_M.getMax());
    tree_1a->Draw("(Bd_M * 1000)>>hist", ("(0.175 * 0.67)/" + std::to_string(n_1a) + " * " + cut).c_str());
    tree_1a->Draw("(Bd_M * 1000)>>hist_1a", ("(0.175 * 0.67)/" + std::to_string(n_1a) + " * " + cut).c_str());
    tree_2->Draw("(Bd_M * 1000)>>+hist", ("0.175/" + std::to_string(n_2) + " * " + cut).c_str());
    tree_2->Draw("(Bd_M * 1000)>>hist_2", ("0.175/" + std::to_string(n_2) + " * " + cut).c_str());
    tree_3->Draw("(Bd_M * 1000)>>+hist", ("0.14/" + std::to_string(n_3) + " * " + cut).c_str());
    tree_3->Draw("(Bd_M * 1000)>>hist_3", ("0.14/" + std::to_string(n_3) + " * " + cut).c_str());
    tree_5a->Draw("(Bd_M * 1000)>>+hist", ("(0.12 * 0.9)/" + std::to_string(n_5a) + " * " + cut).c_str());
    tree_5a->Draw("(Bd_M * 1000)>>hist_5a", ("(0.12 * 0.9)/" + std::to_string(n_5a) + " * " + cut).c_str());

    // Check entries
    std::cout << "Total: " << hist->Integral() << std::endl;
    std::cout << "Hist 1a: " << hist_1a->Integral() << std::endl;
    std::cout << "Hist 2: " << hist_2->Integral() << std::endl;
    std::cout << "Hist 3: " << hist_3->Integral() << std::endl;
    std::cout << "Hist 5a: " << hist_5a->Integral() << std::endl;

    // Save histogram with components
    // TCanvas * canvas = new TCanvas("canvas", "", 800, 400);
    // hist->SetLineColor(kBlack);
    // hist->SetLineWidth(1);
    // hist->SetStats(kFALSE);
    // hist->GetXaxis()->SetTitle("B^{0} mass / MeV");
    // hist->GetYaxis()->SetTitle("Candidates");
    // hist->Draw("HIST");
    // hist_1a->SetLineColor(kRed);
    // hist_1a->SetLineWidth(1);
    // hist_1a->Draw("HIST SAME");
    // hist_2->SetLineColor(kBlue);
    // hist_2->SetLineWidth(1);
    // hist_2->Draw("HIST SAME");
    // hist_3->SetLineColor(kOrange);
    // hist_3->SetLineWidth(1);
    // hist_3->Draw("HIST SAME");
    // hist_5a->SetLineColor(kCyan);
    // hist_5a->SetLineWidth(1);
    // hist_5a->Draw("HIST SAME");
    // hist->Draw("HIST SAME");
    // TLegend * leg = new TLegend(0.65, 0.5, 0.85, 0.9);
    // leg->AddEntry(hist, "Overall shape");
    // leg->AddEntry(hist_1a, "B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            // "K^{*0}#pi^{+}) #bar{D}^{0}");
    // leg->AddEntry(hist_2, "B^{+} #rightarrow K^{*0} #bar{D}^{0} #pi^{+}");
    // leg->AddEntry(hist_3, "B^{+} #rightarrow (K_{1}(1400)^{+} #rightarrow K^{*0} "
            // "#pi^{+}) #bar{D}^{0}");
    // leg->AddEntry(hist_5a, "B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            // "K^{*0}#pi^{+}) #bar{D}^{0}");
    // leg->Draw();
    // canvas->SaveAs("../Plots/DKpipi_shape.pdf");

    // Make RooDataHists
    RooDataHist * dHist = new RooDataHist("dHist", "", Bd_M, RooFit::Import(*hist));
    RooDataHist * dHist_1a = new RooDataHist("dHist_1a", "", Bd_M,
            RooFit::Import(*hist_1a));
    RooDataHist * dHist_2 = new RooDataHist("dHist_2", "", Bd_M, 
            RooFit::Import(*hist_2));
    RooDataHist * dHist_3 = new RooDataHist("dHist_3", "", Bd_M,
            RooFit::Import(*hist_3));
    RooDataHist * dHist_5a = new RooDataHist("dHist_5a", "", Bd_M,
            RooFit::Import(*hist_5a));

    // Fit parameters: non-resonant
    RooRealVar a_2("a_2", "", 2000, 1500, 3000);
    RooRealVar b_2("b_2", "", 5000, 4200, 5200);
    RooRealVar csi_2("csi", "", 0);
    RooRealVar sigma_2("sigma_2", "", 5, 0, 50);
    RooRealVar ratio_2("ratio_2", "", 8, 2, 15);
    RooRealVar frac_2("frac_2", "", 0.5, 0, 1);
    RooRealVar shift("shift", "", 0);

    // Fit parameters: other components
    // 1a
    RooRealVar a_1a("a_1a", "", -4000, -10000, -10);
    RooRealVar b_1a("b_1a", "", 5000, 4200, 5200);
    RooRealVar csi_1a("csi_1a", "", 0);
    RooRealVar sigma_1a("sigma_1a", "", 5, 0, 50);
    RooRealVar ratio_1a("ratio_1a", "", 8, 2, 15);
    RooRealVar frac_1a("frac_1a", "", 0.5, 0, 1);
    // 3
    RooRealVar a_3("a_3", "", -4000, -10000, -10);
    RooRealVar b_3("b_3", "", 5000, 4200, 5200);
    RooRealVar csi_3("csi_3", "", 0);
    RooRealVar sigma_3("sigma_3", "", 5, 0, 50);
    RooRealVar ratio_3("ratio_3", "", 8, 2, 15);
    RooRealVar frac_3("frac_3", "", 0.5, 0, 1);
    // 5
    RooRealVar a_5a("a_5a", "", -4000, -10000, -10);
    RooRealVar b_5a("b_5a", "", 5000, 4200, 5200);
    RooRealVar csi_5a("csi_5a", "", 0);
    RooRealVar sigma_5a("sigma_5a", "", 5, 0, 50);
    RooRealVar ratio_5a("ratio_5a", "", 8, 2, 15);
    RooRealVar frac_5a("frac_5a", "", 0.5, 0, 1);

    // Make RooHILL shape
    RooHILLdini * fit_2 = new RooHILLdini("fit_2", "", Bd_M, a_2, b_2, csi_2, shift,
            sigma_2, ratio_2, frac_2);

    // Make RooHORNS shapes
    RooHORNSdini * fit_1a = new RooHORNSdini("fit_1a", "", Bd_M, a_1a, b_1a, csi_1a, 
            shift, sigma_1a, ratio_1a, frac_1a);
    RooHORNSdini * fit_3 = new RooHORNSdini("fit_3", "", Bd_M, a_3, b_3, csi_3, 
            shift, sigma_3, ratio_3, frac_3);
    RooHORNSdini * fit_5a = new RooHORNSdini("fit_5a", "", Bd_M, a_5a, b_5a, csi_5a, 
            shift, sigma_5a, ratio_5a, frac_5a);

    // Create catgeories
    RooCategory category("cat", "");
    category.defineType("1a");
    category.defineType("2");
    category.defineType("3");
    category.defineType("5a");

    // Make combined dataset
    std::map<std::string, RooDataHist*> dataMap = {
        {"1a", dHist_1a},
        {"2", dHist_2},
        {"3", dHist_3},
        {"5a", dHist_5a}
    };
    RooDataHist combData("combData", "", Bd_M, RooFit::Index(category),
            RooFit::Import(dataMap));

    // Make simultaneous PDF
    RooSimultaneous * simPdf = new RooSimultaneous("simPdf", "", category);
    simPdf->addPdf(*fit_1a, "1a");
    simPdf->addPdf(*fit_2, "2");
    simPdf->addPdf(*fit_3, "3");
    simPdf->addPdf(*fit_5a, "5a");

    // Fit to the dataset
    RooFitResult * r_2 = fit_2->fitTo(*dHist_2, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    RooFitResult * r_1a = fit_1a->fitTo(*dHist_1a, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    RooFitResult * r_3 = fit_3->fitTo(*dHist_3, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    RooFitResult * r_5a = fit_5a->fitTo(*dHist_5a, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r_2->Print("v");
    r_1a->Print("v");
    r_3->Print("v");
    r_5a->Print("v");
    // RooFitResult * r = simPdf->fitTo(combData, RooFit::Save(), RooFit::NumCPU(8, 2),
            // RooFit::Optimize(false), RooFit::Offset(true),
            // RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    // r->Print("v");

    // Convert PDFs to TH1s
    TFile * outfile_1a = TFile::Open("../Histograms/DKpipi_1a_Kpi.root", "RECREATE");
    TH1F * h_fit_1a = (TH1F*)fit_1a->createHistogram("fit_hist_1a", Bd_M,
            RooFit::Binning(nBins * 10));
    TFile * outfile_2 = TFile::Open("../Histograms/DKpipi_2_Kpi.root", "RECREATE");
    TH1F * h_fit_2 = (TH1F*)fit_2->createHistogram("fit_hist_2", Bd_M,
            RooFit::Binning(nBins * 10));
    TFile * outfile_3 = TFile::Open("../Histograms/DKpipi_3_Kpi.root", "RECREATE");
    TH1F * h_fit_3 = (TH1F*)fit_3->createHistogram("fit_hist_3", Bd_M,
            RooFit::Binning(nBins * 10));
    TFile * outfile_5a = TFile::Open("../Histograms/DKpipi_5a_Kpi.root", "RECREATE");
    TH1F * h_fit_5a = (TH1F*)fit_5a->createHistogram("fit_hist_5a", Bd_M,
            RooFit::Binning(nBins * 10));

    // Scale histograms
    h_fit_1a->Scale(hist_1a->Integral() * 10);
    h_fit_2->Scale(hist_2->Integral() * 10);
    h_fit_3->Scale(hist_3->Integral() * 10);
    h_fit_5a->Scale(hist_5a->Integral() * 10);

    // Plot the fit and display it in TApplication
    RooPlot * frame_1a = Bd_M.frame(RooFit::Title(""));
    dHist_1a->plotOn(frame_1a);
    fit_1a->plotOn(frame_1a);
    RooHist * pulls_1a = frame_1a->pullHist();
    RooPlot * frame_2 = Bd_M.frame(RooFit::Title(""));
    dHist_2->plotOn(frame_2);
    fit_2->plotOn(frame_2);
    RooHist * pulls_2 = frame_2->pullHist();
    RooPlot * frame_3 = Bd_M.frame(RooFit::Title(""));
    dHist_3->plotOn(frame_3);
    fit_3->plotOn(frame_3);
    RooHist * pulls_3 = frame_3->pullHist();
    RooPlot * frame_5a = Bd_M.frame(RooFit::Title(""));
    dHist_5a->plotOn(frame_5a);
    fit_5a->plotOn(frame_5a);
    RooHist * pulls_5a = frame_5a->pullHist();

    // Save histograms and pulls to file
    outfile_1a->cd();
    hist_1a->Write("data");
    h_fit_1a->Write("fit");
    pulls_1a->Write("pulls");
    outfile_1a->Close();
    outfile_2->cd();
    hist_2->Write("data");
    h_fit_2->Write("fit");
    pulls_2->Write("pulls");
    outfile_2->Close();
    outfile_3->cd();
    hist_3->Write("data");
    h_fit_3->Write("fit");
    pulls_3->Write("pulls");
    outfile_3->Close();
    outfile_5a->cd();
    hist_5a->Write("data");
    h_fit_5a->Write("fit");
    pulls_5a->Write("pulls");
    outfile_5a->Close();

    // Plot the results nicely
    Plotter * plotter = new Plotter("Kpi", "DKpipi_1a");
    plotter->plotFit();
    plotter = new Plotter("Kpi", "DKpipi_2");
    plotter->plotFit();
    plotter = new Plotter("Kpi", "DKpipi_3");
    plotter->plotFit();
    plotter = new Plotter("Kpi", "DKpipi_5a");
    plotter->plotFit();

    // Combine into total PDF
    RooRealVar coeff_1a("coeff_1a", "", 0.175 * 0.67);
    RooRealVar coeff_2("coeff_2", "", 0.175);
    RooRealVar coeff_3("coeff_3", "", 0.14);
    RooRealVar coeff_5a("coeff_5a", "", 0.12 * 0.9);
    RooAddPdf * fit_total = new RooAddPdf("fit_total", "",
            RooArgList(*fit_1a, *fit_2, *fit_3, *fit_5a),
            RooArgList(coeff_1a, coeff_2, coeff_3, coeff_5a));
    TH1F * fit_hist = (TH1F*)fit_total->createHistogram("fit_total_hist", Bd_M,
            RooFit::Binning(nBins * 10));
    fit_hist->Scale(hist->Integral() * 10/fit_hist->Integral());

    // Get pulls
    RooPlot * frame_total = Bd_M.frame();
    dHist->plotOn(frame_total);
    fit_total->plotOn(frame_total);
    RooHist * pulls = frame_total->pullHist();

    // Save
    TFile * outfile = TFile::Open("../Histograms/DKpipi_Kpi.root", "RECREATE");
    outfile->cd();
    hist->Write("data");
    fit_hist->Write("fit");
    pulls->Write("pulls");
    outfile->Close();

    // Plot 
    plotter = new Plotter("Kpi", "DKpipi");
    plotter->plotFit();

    // Save output to a file
    std::ofstream params("../Results/DKpipi.param");
    params << "a_1a " << a_1a.getVal() << " " << a_1a.getError() << std::endl;
    params << "b_1a " << b_1a.getVal() << " " << b_1a.getError() << std::endl;
    params << "csi_1a " << csi_1a.getVal() << " " << csi_1a.getError() << std::endl;
    params << "sigma_1a " << sigma_1a.getVal() << " " << sigma_1a.getError() << std::endl;
    params << "ratio_1a " << ratio_1a.getVal() << " " << ratio_1a.getError() << std::endl;
    params << "frac_1a " << frac_1a.getVal() << " " << frac_1a.getError() << std::endl;
    params << "coeff_1a " << coeff_1a.getVal() << " " << coeff_1a.getError() << std::endl;
    params << "a_2 " << a_2.getVal() << " " << a_2.getError() << std::endl;
    params << "b_2 " << b_2.getVal() << " " << b_2.getError() << std::endl;
    params << "csi_2 " << csi_2.getVal() << " " << csi_2.getError() << std::endl;
    params << "sigma_2 " << sigma_2.getVal() << " " << sigma_2.getError() << std::endl;
    params << "ratio_2 " << ratio_2.getVal() << " " << ratio_2.getError() << std::endl;
    params << "frac_2 " << frac_2.getVal() << " " << frac_2.getError() << std::endl;
    params << "coeff_2 " << coeff_2.getVal() << " " << coeff_2.getError() << std::endl;
    params << "a_3 " << a_3.getVal() << " " << a_3.getError() << std::endl;
    params << "b_3 " << b_3.getVal() << " " << b_3.getError() << std::endl;
    params << "csi_3 " << csi_3.getVal() << " " << csi_3.getError() << std::endl;
    params << "sigma_3 " << sigma_3.getVal() << " " << sigma_3.getError() << std::endl;
    params << "ratio_3 " << ratio_3.getVal() << " " << ratio_3.getError() << std::endl;
    params << "frac_3 " << frac_3.getVal() << " " << frac_3.getError() << std::endl;
    params << "coeff_3 " << coeff_3.getVal() << " " << coeff_3.getError() << std::endl;
    params << "a_5a " << a_5a.getVal() << " " << a_5a.getError() << std::endl;
    params << "b_5a " << b_5a.getVal() << " " << b_5a.getError() << std::endl;
    params << "csi_5a " << csi_5a.getVal() << " " << csi_5a.getError() << std::endl;
    params << "sigma_5a " << sigma_5a.getVal() << " " << sigma_5a.getError() << std::endl;
    params << "ratio_5a " << ratio_5a.getVal() << " " << ratio_5a.getError() << std::endl;
    params << "frac_5a " << frac_5a.getVal() << " " << frac_5a.getError() << std::endl;
    params << "coeff_5a " << coeff_5a.getVal() << " " << coeff_5a.getError() << std::endl;
    params.close();

    return 0;
}
