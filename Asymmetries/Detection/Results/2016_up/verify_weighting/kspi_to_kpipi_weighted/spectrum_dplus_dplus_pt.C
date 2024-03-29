void spectrum_dplus_dplus_pt()
{
//=========Macro generated from canvas: c1_n2/c1_n2
//=========  (Tue Nov 13 23:55:28 2018) by ROOT version6.08/06
   TCanvas *c1_n2 = new TCanvas("c1_n2", "c1_n2",0,0,700,500);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   c1_n2->SetHighLightColor(2);
   c1_n2->Range(-3456.79,-0.09724255,21234.57,0.5105234);
   c1_n2->SetFillColor(0);
   c1_n2->SetBorderMode(0);
   c1_n2->SetBorderSize(2);
   c1_n2->SetTickx(1);
   c1_n2->SetTicky(1);
   c1_n2->SetLeftMargin(0.14);
   c1_n2->SetRightMargin(0.05);
   c1_n2->SetTopMargin(0.05);
   c1_n2->SetBottomMargin(0.16);
   c1_n2->SetFrameLineWidth(2);
   c1_n2->SetFrameBorderMode(0);
   c1_n2->SetFrameLineWidth(2);
   c1_n2->SetFrameBorderMode(0);
   Double_t xAxis13[14] = {0, 500, 1000, 1500, 2000, 2500, 3000, 4000, 5000, 7500, 10000, 13000, 16000, 20000}; 
   
   TH1F *ks_to_kpipi_dplus_spectra_from_pt__13 = new TH1F("ks_to_kpipi_dplus_spectra_from_pt__13","ks_to_kpipi_dplus_spectra_from_pt",13, xAxis13);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(2,-3.084615e-05);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(3,0.001171218);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(4,0.04091984);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(5,0.09631035);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(6,0.1100374);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(7,-0.3524711);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(8,0.3216686);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(9,0.3429537);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(10,0.2327195);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(11,0.1399631);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinContent(12,0.06675839);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(2,1.461816e-06);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(3,4.691167e-05);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(4,0.0001495639);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(5,0.0001982235);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(6,0.0001840742);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(7,0.01846969);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(8,0.0006247518);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(9,0.0006994789);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(10,0.001054791);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(11,0.001606093);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetBinError(12,0.00240658);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetMinimum(0);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetMaximum(0.4801351);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetEntries(1.125819e+07);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetStats(0);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetFillColor(4);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetFillStyle(3004);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetLineColor(4);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetLineWidth(2);
   ks_to_kpipi_dplus_spectra_from_pt__13->SetMarkerStyle(22);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetXaxis()->SetTitle("#it{p_{T}} [MeV/#it{c}]");
   ks_to_kpipi_dplus_spectra_from_pt__13->GetXaxis()->SetNdivisions(505);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetXaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetXaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetXaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetXaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetXaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetXaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetYaxis()->SetTitle("Candidates (arb. units)");
   ks_to_kpipi_dplus_spectra_from_pt__13->GetYaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetYaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetYaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetYaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetYaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetYaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetZaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetZaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetZaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetZaxis()->SetTitleOffset(1.2);
   ks_to_kpipi_dplus_spectra_from_pt__13->GetZaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_from_pt__13->Draw("HIST");
   Double_t xAxis14[14] = {0, 500, 1000, 1500, 2000, 2500, 3000, 4000, 5000, 7500, 10000, 13000, 16000, 20000}; 
   
   TH1F *ks_to_kpipi_dplus_spectra_to_pt__14 = new TH1F("ks_to_kpipi_dplus_spectra_to_pt__14","ks_to_kpipi_dplus_spectra_to_pt",13, xAxis14);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(0,0.9247566);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(3,6.290254e-06);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(4,0.001177771);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(5,0.01301253);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(6,0.0514801);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(7,0.1406358);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(8,0.240898);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(9,0.2368163);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(10,0.1638222);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(11,0.09955698);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinContent(12,0.05259395);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(0,0.118106);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(3,1.036697e-06);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(4,1.636161e-05);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(5,8.21072e-05);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(6,0.0002311632);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(7,0.0003008118);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(8,0.0005014273);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(9,0.0003944661);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(10,0.000411591);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(11,0.0003040912);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetBinError(12,0.0002565186);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetEntries(2.952046e+07);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetStats(0);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetFillColor(2);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetFillStyle(3005);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetLineColor(2);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetLineWidth(2);
   ks_to_kpipi_dplus_spectra_to_pt__14->SetMarkerStyle(23);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetXaxis()->SetNdivisions(505);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetXaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetXaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetXaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetXaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetXaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetXaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetYaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetYaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetYaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetYaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetYaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetYaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetZaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetZaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetZaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetZaxis()->SetTitleOffset(1.2);
   ks_to_kpipi_dplus_spectra_to_pt__14->GetZaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_to_pt__14->Draw("HIST SAME");
   
   TLegend *leg = new TLegend(0.555,0.73,0.89,0.92,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetTextFont(132);
   leg->SetTextSize(0.05);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(2);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("ks_to_kpipi_dplus_spectra_from_pt","[#it{D^{+}#rightarrowK_{s}^{0}#pi^{+}}] D^{+}","f");
   entry->SetFillColor(4);
   entry->SetFillStyle(3004);
   entry->SetLineColor(4);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(132);
   entry=leg->AddEntry("ks_to_kpipi_dplus_spectra_to_pt","[#it{D^{#pm}#rightarrowK^{#mp}#pi^{#pm}#pi^{#pm}}] D^{+}","f");
   entry->SetFillColor(2);
   entry->SetFillStyle(3005);
   entry->SetLineColor(2);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(132);
   leg->Draw();
   c1_n2->Modified();
   c1_n2->cd();
   c1_n2->SetSelected(c1_n2);
}
