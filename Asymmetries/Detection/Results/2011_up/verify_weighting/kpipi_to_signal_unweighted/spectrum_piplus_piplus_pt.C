void spectrum_piplus_piplus_pt()
{
//=========Macro generated from canvas: c1_n2/c1_n2
//=========  (Tue Dec  4 16:10:24 2018) by ROOT version6.08/06
   TCanvas *c1_n2 = new TCanvas("c1_n2", "c1_n2",0,722,700,500);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   c1_n2->SetHighLightColor(2);
   c1_n2->Range(-3456.79,-0.08337225,21234.57,0.4377044);
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
   Double_t xAxis21[14] = {0, 500, 1000, 1500, 2000, 2500, 3000, 4000, 5000, 7500, 10000, 13000, 16000, 20000}; 
   
   TH1F *kpipi_to_signal_pi_spectra_from_pt__21 = new TH1F("kpipi_to_signal_pi_spectra_from_pt__21","kpipi_to_signal_pi_spectra_from_pt",13, xAxis21);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinContent(1,0.08620082);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinContent(2,0.2940361);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinContent(3,0.2539166);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinContent(4,0.1964436);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinContent(5,0.1025372);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinContent(6,0.0467643);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinContent(7,0.01634187);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinContent(8,0.003528745);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinContent(9,0.0002307409);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinError(1,0.0001461335);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinError(2,0.0002698948);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinError(3,0.000250807);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinError(4,0.0002206039);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinError(5,0.0001593804);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinError(6,0.0001076345);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinError(7,4.499146e-05);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinError(8,2.090688e-05);
   kpipi_to_signal_pi_spectra_from_pt__21->SetBinError(9,3.381204e-06);
   kpipi_to_signal_pi_spectra_from_pt__21->SetMinimum(0);
   kpipi_to_signal_pi_spectra_from_pt__21->SetMaximum(0.4116505);
   kpipi_to_signal_pi_spectra_from_pt__21->SetEntries(4120510);
   kpipi_to_signal_pi_spectra_from_pt__21->SetStats(0);
   kpipi_to_signal_pi_spectra_from_pt__21->SetFillColor(4);
   kpipi_to_signal_pi_spectra_from_pt__21->SetFillStyle(3004);
   kpipi_to_signal_pi_spectra_from_pt__21->SetLineColor(4);
   kpipi_to_signal_pi_spectra_from_pt__21->SetLineWidth(2);
   kpipi_to_signal_pi_spectra_from_pt__21->SetMarkerStyle(22);
   kpipi_to_signal_pi_spectra_from_pt__21->GetXaxis()->SetTitle("#it{p_{T}} [MeV/#it{c}]");
   kpipi_to_signal_pi_spectra_from_pt__21->GetXaxis()->SetNdivisions(505);
   kpipi_to_signal_pi_spectra_from_pt__21->GetXaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_from_pt__21->GetXaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_from_pt__21->GetXaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_from_pt__21->GetXaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_from_pt__21->GetXaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_from_pt__21->GetXaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_from_pt__21->GetYaxis()->SetTitle("Candidates (arb. units)");
   kpipi_to_signal_pi_spectra_from_pt__21->GetYaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_from_pt__21->GetYaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_from_pt__21->GetYaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_from_pt__21->GetYaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_from_pt__21->GetYaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_from_pt__21->GetYaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_from_pt__21->GetZaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_from_pt__21->GetZaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_from_pt__21->GetZaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_from_pt__21->GetZaxis()->SetTitleOffset(1.2);
   kpipi_to_signal_pi_spectra_from_pt__21->GetZaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_from_pt__21->Draw("HIST");
   Double_t xAxis22[14] = {0, 500, 1000, 1500, 2000, 2500, 3000, 4000, 5000, 7500, 10000, 13000, 16000, 20000}; 
   
   TH1F *kpipi_to_signal_pi_spectra_to_pt__22 = new TH1F("kpipi_to_signal_pi_spectra_to_pt__22","kpipi_to_signal_pi_spectra_to_pt",13, xAxis22);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(1,0.05588489);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(2,0.2358263);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(3,0.1397322);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(4,0.1561187);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(5,0.1309446);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(6,0.06515627);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(7,0.1136082);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(8,0.08271606);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(9,0.01402179);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(10,0.003569119);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(11,5.881235e-05);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(12,-0.0003498511);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinContent(13,0.002712868);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(1,0.03448231);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(2,0.06972357);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(3,0.05900807);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(4,0.06188753);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(5,0.0579862);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(6,0.04884808);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(7,0.03517914);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(8,0.03210717);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(9,0.009018036);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(10,0.004519105);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(11,5.881235e-05);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(12,0.0003498511);
   kpipi_to_signal_pi_spectra_to_pt__22->SetBinError(13,0.002712868);
   kpipi_to_signal_pi_spectra_to_pt__22->SetEntries(330);
   kpipi_to_signal_pi_spectra_to_pt__22->SetStats(0);
   kpipi_to_signal_pi_spectra_to_pt__22->SetFillColor(2);
   kpipi_to_signal_pi_spectra_to_pt__22->SetFillStyle(3005);
   kpipi_to_signal_pi_spectra_to_pt__22->SetLineColor(2);
   kpipi_to_signal_pi_spectra_to_pt__22->SetLineWidth(2);
   kpipi_to_signal_pi_spectra_to_pt__22->SetMarkerStyle(23);
   kpipi_to_signal_pi_spectra_to_pt__22->GetXaxis()->SetNdivisions(505);
   kpipi_to_signal_pi_spectra_to_pt__22->GetXaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_to_pt__22->GetXaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_to_pt__22->GetXaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_to_pt__22->GetXaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_to_pt__22->GetXaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_to_pt__22->GetXaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_to_pt__22->GetYaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_to_pt__22->GetYaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_to_pt__22->GetYaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_to_pt__22->GetYaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_to_pt__22->GetYaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_to_pt__22->GetYaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_to_pt__22->GetZaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_to_pt__22->GetZaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_to_pt__22->GetZaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_to_pt__22->GetZaxis()->SetTitleOffset(1.2);
   kpipi_to_signal_pi_spectra_to_pt__22->GetZaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_to_pt__22->Draw("HIST SAME");
   
   TLegend *leg = new TLegend(0.555,0.73,0.89,0.92,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetTextFont(132);
   leg->SetTextSize(0.05);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(2);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("kpipi_to_signal_pi_spectra_from_pt","[#it{D^{#pm}#rightarrowK^{#mp}#pi^{#pm}#pi^{#pm}}] #pi^{#pm}","f");
   entry->SetFillColor(4);
   entry->SetFillStyle(3004);
   entry->SetLineColor(4);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(132);
   entry=leg->AddEntry("kpipi_to_signal_pi_spectra_to_pt","[#it{Signal}] #pi^{#pm}","f");
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
