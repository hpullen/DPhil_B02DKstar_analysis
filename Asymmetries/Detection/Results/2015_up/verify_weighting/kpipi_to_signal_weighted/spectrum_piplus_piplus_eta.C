void spectrum_piplus_piplus_eta()
{
//=========Macro generated from canvas: c1_n2/c1_n2
//=========  (Tue Nov 13 18:49:36 2018) by ROOT version6.08/06
   TCanvas *c1_n2 = new TCanvas("c1_n2", "c1_n2",10,32,700,500);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   c1_n2->SetHighLightColor(2);
   c1_n2->Range(1.346914,-0.06498145,5.297531,0.3411526);
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
   Double_t xAxis7[9] = {1.9, 2.4, 2.65, 3, 3.3, 3.5, 3.8, 4.5, 5.1}; 
   
   TH1F *kpipi_to_signal_pi_spectra_from_eta__7 = new TH1F("kpipi_to_signal_pi_spectra_from_eta__7","kpipi_to_signal_pi_spectra_from_eta",8, xAxis7);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinContent(0,0.0008022727);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinContent(1,0.1793556);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinContent(2,0.1861603);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinContent(3,0.2291757);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinContent(4,0.1552669);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinContent(5,0.1873547);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinContent(6,0.05152838);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinContent(7,0.0108855);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinContent(8,0.0002730385);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinContent(9,4.250119e-07);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinError(0,8.906415e-06);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinError(1,0.0004501524);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinError(2,0.0004755177);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinError(3,0.0004454341);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinError(4,0.000385007);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinError(5,0.0006181921);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinError(6,0.0002259112);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinError(7,7.541102e-05);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinError(8,4.8768e-06);
   kpipi_to_signal_pi_spectra_from_eta__7->SetBinError(9,1.395621e-08);
   kpipi_to_signal_pi_spectra_from_eta__7->SetMinimum(0);
   kpipi_to_signal_pi_spectra_from_eta__7->SetMaximum(0.3208459);
   kpipi_to_signal_pi_spectra_from_eta__7->SetEntries(3.196529e+07);
   kpipi_to_signal_pi_spectra_from_eta__7->SetStats(0);
   kpipi_to_signal_pi_spectra_from_eta__7->SetFillColor(4);
   kpipi_to_signal_pi_spectra_from_eta__7->SetFillStyle(3004);
   kpipi_to_signal_pi_spectra_from_eta__7->SetLineColor(4);
   kpipi_to_signal_pi_spectra_from_eta__7->SetLineWidth(2);
   kpipi_to_signal_pi_spectra_from_eta__7->SetMarkerStyle(22);
   kpipi_to_signal_pi_spectra_from_eta__7->GetXaxis()->SetTitle("#it{#eta}");
   kpipi_to_signal_pi_spectra_from_eta__7->GetXaxis()->SetNdivisions(505);
   kpipi_to_signal_pi_spectra_from_eta__7->GetXaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_from_eta__7->GetXaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_from_eta__7->GetXaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_from_eta__7->GetXaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_from_eta__7->GetXaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_from_eta__7->GetXaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_from_eta__7->GetYaxis()->SetTitle("Candidates (arb. units)");
   kpipi_to_signal_pi_spectra_from_eta__7->GetYaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_from_eta__7->GetYaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_from_eta__7->GetYaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_from_eta__7->GetYaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_from_eta__7->GetYaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_from_eta__7->GetYaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_from_eta__7->GetZaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_from_eta__7->GetZaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_from_eta__7->GetZaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_from_eta__7->GetZaxis()->SetTitleOffset(1.2);
   kpipi_to_signal_pi_spectra_from_eta__7->GetZaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_from_eta__7->Draw("HIST");
   Double_t xAxis8[9] = {1.9, 2.4, 2.65, 3, 3.3, 3.5, 3.8, 4.5, 5.1}; 
   
   TH1F *kpipi_to_signal_pi_spectra_to_eta__8 = new TH1F("kpipi_to_signal_pi_spectra_to_eta__8","kpipi_to_signal_pi_spectra_to_eta",8, xAxis8);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinContent(1,0.2168176);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinContent(2,0.1237216);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinContent(3,0.1902128);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinContent(4,0.1580968);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinContent(5,0.1920216);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinContent(6,0.09434631);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinContent(7,0.0132916);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinContent(8,0.01149162);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinError(1,0.05610289);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinError(2,0.06379815);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinError(3,0.06633017);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinError(4,0.07085071);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinError(5,0.09651045);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinError(6,0.05266497);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinError(7,0.01529389);
   kpipi_to_signal_pi_spectra_to_eta__8->SetBinError(8,0.01238536);
   kpipi_to_signal_pi_spectra_to_eta__8->SetEntries(207);
   kpipi_to_signal_pi_spectra_to_eta__8->SetStats(0);
   kpipi_to_signal_pi_spectra_to_eta__8->SetFillColor(2);
   kpipi_to_signal_pi_spectra_to_eta__8->SetFillStyle(3005);
   kpipi_to_signal_pi_spectra_to_eta__8->SetLineColor(2);
   kpipi_to_signal_pi_spectra_to_eta__8->SetLineWidth(2);
   kpipi_to_signal_pi_spectra_to_eta__8->SetMarkerStyle(23);
   kpipi_to_signal_pi_spectra_to_eta__8->GetXaxis()->SetNdivisions(505);
   kpipi_to_signal_pi_spectra_to_eta__8->GetXaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_to_eta__8->GetXaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_to_eta__8->GetXaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_to_eta__8->GetXaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_to_eta__8->GetXaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_to_eta__8->GetXaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_to_eta__8->GetYaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_to_eta__8->GetYaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_to_eta__8->GetYaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_to_eta__8->GetYaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_to_eta__8->GetYaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_to_eta__8->GetYaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_to_eta__8->GetZaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_to_eta__8->GetZaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_to_eta__8->GetZaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_to_eta__8->GetZaxis()->SetTitleOffset(1.2);
   kpipi_to_signal_pi_spectra_to_eta__8->GetZaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_to_eta__8->Draw("HIST SAME");
   
   TLegend *leg = new TLegend(0.555,0.73,0.89,0.92,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetTextFont(132);
   leg->SetTextSize(0.05);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(2);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("kpipi_to_signal_pi_spectra_from_eta","[#it{D^{#pm}#rightarrowK^{#mp}#pi^{#pm}#pi^{#pm}}] #pi^{#pm}","f");
   entry->SetFillColor(4);
   entry->SetFillStyle(3004);
   entry->SetLineColor(4);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(132);
   entry=leg->AddEntry("kpipi_to_signal_pi_spectra_to_eta","[#it{Signal}] #pi^{#pm}","f");
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
