void spectrum_trigger_pi_trigger_pi_pt()
{
//=========Macro generated from canvas: c1_n2/c1_n2
//=========  (Tue Nov 13 23:55:04 2018) by ROOT version6.08/06
   TCanvas *c1_n2 = new TCanvas("c1_n2", "c1_n2",0,0,700,500);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   c1_n2->SetHighLightColor(2);
   c1_n2->Range(-3456.79,-0.1031962,21234.57,0.5417799);
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
   Double_t xAxis9[14] = {0, 500, 1000, 1500, 2000, 2500, 3000, 4000, 5000, 7500, 10000, 13000, 16000, 20000}; 
   
   TH1F *ks_to_kpipi_pi_spectra_from_pt__9 = new TH1F("ks_to_kpipi_pi_spectra_from_pt__9","ks_to_kpipi_pi_spectra_from_pt",13, xAxis9);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinContent(3,0.007334489);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinContent(4,0.3571801);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinContent(5,0.3639508);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinContent(6,0.2248809);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinContent(7,-0.01460925);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinContent(8,0.04318254);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinContent(9,0.008556712);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinContent(10,0.006909337);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinContent(11,0.00254115);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinContent(12,7.322754e-05);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinError(3,0.0006914351);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinError(4,0.00316701);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinError(5,0.002582389);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinError(6,0.001639927);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinError(7,0.00477343);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinError(8,0.0003684735);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinError(9,3.602036e-05);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinError(10,4.488667e-05);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinError(11,3.544125e-05);
   ks_to_kpipi_pi_spectra_from_pt__9->SetBinError(12,8.237241e-06);
   ks_to_kpipi_pi_spectra_from_pt__9->SetMinimum(0);
   ks_to_kpipi_pi_spectra_from_pt__9->SetMaximum(0.5095311);
   ks_to_kpipi_pi_spectra_from_pt__9->SetEntries(1.125819e+07);
   ks_to_kpipi_pi_spectra_from_pt__9->SetStats(0);
   ks_to_kpipi_pi_spectra_from_pt__9->SetFillColor(4);
   ks_to_kpipi_pi_spectra_from_pt__9->SetFillStyle(3004);
   ks_to_kpipi_pi_spectra_from_pt__9->SetLineColor(4);
   ks_to_kpipi_pi_spectra_from_pt__9->SetLineWidth(2);
   ks_to_kpipi_pi_spectra_from_pt__9->SetMarkerStyle(22);
   ks_to_kpipi_pi_spectra_from_pt__9->GetXaxis()->SetTitle("#it{p_{T}} [MeV/#it{c}]");
   ks_to_kpipi_pi_spectra_from_pt__9->GetXaxis()->SetNdivisions(505);
   ks_to_kpipi_pi_spectra_from_pt__9->GetXaxis()->SetLabelFont(132);
   ks_to_kpipi_pi_spectra_from_pt__9->GetXaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_pi_spectra_from_pt__9->GetXaxis()->SetLabelSize(0.06);
   ks_to_kpipi_pi_spectra_from_pt__9->GetXaxis()->SetTitleSize(0.072);
   ks_to_kpipi_pi_spectra_from_pt__9->GetXaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_pi_spectra_from_pt__9->GetXaxis()->SetTitleFont(132);
   ks_to_kpipi_pi_spectra_from_pt__9->GetYaxis()->SetTitle("Candidates (arb. units)");
   ks_to_kpipi_pi_spectra_from_pt__9->GetYaxis()->SetLabelFont(132);
   ks_to_kpipi_pi_spectra_from_pt__9->GetYaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_pi_spectra_from_pt__9->GetYaxis()->SetLabelSize(0.06);
   ks_to_kpipi_pi_spectra_from_pt__9->GetYaxis()->SetTitleSize(0.072);
   ks_to_kpipi_pi_spectra_from_pt__9->GetYaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_pi_spectra_from_pt__9->GetYaxis()->SetTitleFont(132);
   ks_to_kpipi_pi_spectra_from_pt__9->GetZaxis()->SetLabelFont(132);
   ks_to_kpipi_pi_spectra_from_pt__9->GetZaxis()->SetLabelSize(0.06);
   ks_to_kpipi_pi_spectra_from_pt__9->GetZaxis()->SetTitleSize(0.072);
   ks_to_kpipi_pi_spectra_from_pt__9->GetZaxis()->SetTitleOffset(1.2);
   ks_to_kpipi_pi_spectra_from_pt__9->GetZaxis()->SetTitleFont(132);
   ks_to_kpipi_pi_spectra_from_pt__9->Draw("HIST");
   Double_t xAxis10[14] = {0, 500, 1000, 1500, 2000, 2500, 3000, 4000, 5000, 7500, 10000, 13000, 16000, 20000}; 
   
   TH1F *ks_to_kpipi_pi_spectra_to_pt__10 = new TH1F("ks_to_kpipi_pi_spectra_to_pt__10","ks_to_kpipi_pi_spectra_to_pt",13, xAxis10);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinContent(3,0.004540154);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinContent(4,0.3189897);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinContent(5,0.3203399);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinContent(6,0.1987105);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinContent(7,0.1006145);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinContent(8,0.04289478);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinContent(9,0.01225983);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinContent(10,0.00157268);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinContent(11,7.792423e-05);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinContent(12,3.945698e-08);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinError(3,0.0001005398);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinError(4,0.0007490106);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinError(5,0.0006701149);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinError(6,0.0004593757);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinError(7,0.0002206974);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinError(8,0.0001400225);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinError(9,4.707393e-05);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinError(10,1.802956e-05);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinError(11,3.908754e-06);
   ks_to_kpipi_pi_spectra_to_pt__10->SetBinError(12,1.311975e-08);
   ks_to_kpipi_pi_spectra_to_pt__10->SetEntries(2.952046e+07);
   ks_to_kpipi_pi_spectra_to_pt__10->SetStats(0);
   ks_to_kpipi_pi_spectra_to_pt__10->SetFillColor(2);
   ks_to_kpipi_pi_spectra_to_pt__10->SetFillStyle(3005);
   ks_to_kpipi_pi_spectra_to_pt__10->SetLineColor(2);
   ks_to_kpipi_pi_spectra_to_pt__10->SetLineWidth(2);
   ks_to_kpipi_pi_spectra_to_pt__10->SetMarkerStyle(23);
   ks_to_kpipi_pi_spectra_to_pt__10->GetXaxis()->SetNdivisions(505);
   ks_to_kpipi_pi_spectra_to_pt__10->GetXaxis()->SetLabelFont(132);
   ks_to_kpipi_pi_spectra_to_pt__10->GetXaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_pi_spectra_to_pt__10->GetXaxis()->SetLabelSize(0.06);
   ks_to_kpipi_pi_spectra_to_pt__10->GetXaxis()->SetTitleSize(0.072);
   ks_to_kpipi_pi_spectra_to_pt__10->GetXaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_pi_spectra_to_pt__10->GetXaxis()->SetTitleFont(132);
   ks_to_kpipi_pi_spectra_to_pt__10->GetYaxis()->SetLabelFont(132);
   ks_to_kpipi_pi_spectra_to_pt__10->GetYaxis()->SetLabelOffset(0.01);
   ks_to_kpipi_pi_spectra_to_pt__10->GetYaxis()->SetLabelSize(0.06);
   ks_to_kpipi_pi_spectra_to_pt__10->GetYaxis()->SetTitleSize(0.072);
   ks_to_kpipi_pi_spectra_to_pt__10->GetYaxis()->SetTitleOffset(0.95);
   ks_to_kpipi_pi_spectra_to_pt__10->GetYaxis()->SetTitleFont(132);
   ks_to_kpipi_pi_spectra_to_pt__10->GetZaxis()->SetLabelFont(132);
   ks_to_kpipi_pi_spectra_to_pt__10->GetZaxis()->SetLabelSize(0.06);
   ks_to_kpipi_pi_spectra_to_pt__10->GetZaxis()->SetTitleSize(0.072);
   ks_to_kpipi_pi_spectra_to_pt__10->GetZaxis()->SetTitleOffset(1.2);
   ks_to_kpipi_pi_spectra_to_pt__10->GetZaxis()->SetTitleFont(132);
   ks_to_kpipi_pi_spectra_to_pt__10->Draw("HIST SAME");
   
   TLegend *leg = new TLegend(0.555,0.73,0.89,0.92,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetTextFont(132);
   leg->SetTextSize(0.05);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(2);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("ks_to_kpipi_pi_spectra_from_pt","[#it{D^{+}#rightarrowK_{s}^{0}#pi^{+}}] #pi^{+}","f");
   entry->SetFillColor(4);
   entry->SetFillStyle(3004);
   entry->SetLineColor(4);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(132);
   entry=leg->AddEntry("ks_to_kpipi_pi_spectra_to_pt","[#it{D^{#pm}#rightarrowK^{#mp}#pi^{#pm}#pi^{#pm}}] #pi^{#pm}","f");
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
