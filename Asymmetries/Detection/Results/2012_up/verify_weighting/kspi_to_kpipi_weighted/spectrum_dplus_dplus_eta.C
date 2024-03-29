void spectrum_dplus_dplus_eta()
{
//=========Macro generated from canvas: c1_n2/c1_n2
//=========  (Tue Nov 13 18:07:13 2018) by ROOT version6.08/06
   TCanvas *c1_n2 = new TCanvas("c1_n2", "c1_n2",10,32,700,500);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   c1_n2->SetHighLightColor(2);
   c1_n2->Range(1.346914,-0.08196097,5.297531,0.4302951);
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
   Double_t xAxis15[9] = {1.9, 2.4, 2.65, 3, 3.3, 3.5, 3.8, 4.5, 5.1}; 
   
   TH1F *ks_to_kpipi_dplus_spectra_from_eta__15 = new TH1F("ks_to_kpipi_dplus_spectra_from_eta__15","ks_to_kpipi_dplus_spectra_from_eta",8, xAxis15);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinContent(1,0.02729872);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinContent(2,0.1603709);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinContent(3,0.2579047);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinContent(4,0.2890588);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinContent(5,0.1845922);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinContent(6,0.06579366);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinContent(7,0.01467829);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinContent(8,0.0003027493);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinError(1,0.0004854531);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinError(2,0.001454195);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinError(3,0.001666893);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinError(4,0.002114442);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinError(5,0.001363496);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinError(6,0.0004447613);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinError(7,7.468188e-05);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetBinError(8,6.378743e-06);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetMinimum(0);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetMaximum(0.4046823);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetEntries(1780576);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetStats(0);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetFillColor(4);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetFillStyle(3004);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetLineColor(4);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetLineWidth(2);
   ks_to_kpipi_dplus_spectra_from_eta__15->SetMarkerStyle(22);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetXaxis()->SetTitle("#it{#eta}");
   ks_to_kpipi_dplus_spectra_from_eta__15->GetXaxis()->SetNdivisions(505);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetXaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetXaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetXaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetXaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetXaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetXaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetYaxis()->SetTitle("Candidates (arb. units)");
   ks_to_kpipi_dplus_spectra_from_eta__15->GetYaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetYaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetYaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetYaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetYaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetYaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetZaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetZaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetZaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetZaxis()->SetTitleOffset(1.2);
   ks_to_kpipi_dplus_spectra_from_eta__15->GetZaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_from_eta__15->Draw("HIST");
   Double_t xAxis16[9] = {1.9, 2.4, 2.65, 3, 3.3, 3.5, 3.8, 4.5, 5.1}; 
   
   TH1F *ks_to_kpipi_dplus_spectra_to_eta__16 = new TH1F("ks_to_kpipi_dplus_spectra_to_eta__16","ks_to_kpipi_dplus_spectra_to_eta",8, xAxis16);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinContent(1,0.02561703);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinContent(2,0.1589187);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinContent(3,0.2556508);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinContent(4,0.2870098);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinContent(5,0.1871384);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinContent(6,0.07851743);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinContent(7,0.00712756);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinContent(8,2.025199e-05);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinError(1,0.0001375119);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinError(2,0.0005252768);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinError(3,0.0005592322);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinError(4,0.0006045588);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinError(5,0.0005534114);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinError(6,0.0002642783);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinError(7,4.033707e-05);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetBinError(8,4.081468e-07);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetEntries(1.657298e+07);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetStats(0);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetFillColor(2);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetFillStyle(3005);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetLineColor(2);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetLineWidth(2);
   ks_to_kpipi_dplus_spectra_to_eta__16->SetMarkerStyle(23);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetXaxis()->SetNdivisions(505);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetXaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetXaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetXaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetXaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetXaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetXaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetYaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetYaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetYaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetYaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetYaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetYaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetZaxis()->SetLabelFont(132);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetZaxis()->SetLabelSize(0.06);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetZaxis()->SetTitleSize(0.072);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetZaxis()->SetTitleOffset(1.2);
   ks_to_kpipi_dplus_spectra_to_eta__16->GetZaxis()->SetTitleFont(132);
   ks_to_kpipi_dplus_spectra_to_eta__16->Draw("HIST SAME");
   
   TLegend *leg = new TLegend(0.555,0.73,0.89,0.92,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetTextFont(132);
   leg->SetTextSize(0.05);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(2);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("ks_to_kpipi_dplus_spectra_from_eta","[#it{D^{+}#rightarrowK_{s}^{0}#pi^{+}}] D^{+}","f");
   entry->SetFillColor(4);
   entry->SetFillStyle(3004);
   entry->SetLineColor(4);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(132);
   entry=leg->AddEntry("ks_to_kpipi_dplus_spectra_to_eta","[#it{D^{#pm}#rightarrowK^{#mp}#pi^{#pm}#pi^{#pm}}] D^{+}","f");
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
