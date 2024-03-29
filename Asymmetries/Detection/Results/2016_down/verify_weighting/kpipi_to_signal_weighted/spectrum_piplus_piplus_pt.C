void spectrum_piplus_piplus_pt()
{
//=========Macro generated from canvas: c1_n2/c1_n2
//=========  (Tue Nov 13 23:12:27 2018) by ROOT version6.08/06
   TCanvas *c1_n2 = new TCanvas("c1_n2", "c1_n2",0,0,700,500);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   c1_n2->SetHighLightColor(2);
   c1_n2->Range(-3456.79,-0.07289647,21234.57,0.3827065);
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
   Double_t xAxis5[14] = {0, 500, 1000, 1500, 2000, 2500, 3000, 4000, 5000, 7500, 10000, 13000, 16000, 20000}; 
   
   TH1F *kpipi_to_signal_pi_spectra_from_pt__5 = new TH1F("kpipi_to_signal_pi_spectra_from_pt__5","kpipi_to_signal_pi_spectra_from_pt",13, xAxis5);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(1,0.06408823);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(2,0.2570902);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(3,0.1735909);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(4,0.1787579);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(5,0.1201626);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(6,0.08700818);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(7,0.05972359);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(8,0.04068938);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(9,0.01516692);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(10,0.003651073);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinContent(11,7.097335e-05);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(1,0.0001462023);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(2,0.0003586259);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(3,0.0002865326);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(4,0.0003196452);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(5,0.0002770266);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(6,0.0002694171);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(7,0.0001666072);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(8,0.0001603353);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(9,7.539311e-05);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(10,4.119347e-05);
   kpipi_to_signal_pi_spectra_from_pt__5->SetBinError(11,3.02014e-06);
   kpipi_to_signal_pi_spectra_from_pt__5->SetMinimum(0);
   kpipi_to_signal_pi_spectra_from_pt__5->SetMaximum(0.3599263);
   kpipi_to_signal_pi_spectra_from_pt__5->SetEntries(4.346674e+07);
   kpipi_to_signal_pi_spectra_from_pt__5->SetStats(0);
   kpipi_to_signal_pi_spectra_from_pt__5->SetFillColor(4);
   kpipi_to_signal_pi_spectra_from_pt__5->SetFillStyle(3004);
   kpipi_to_signal_pi_spectra_from_pt__5->SetLineColor(4);
   kpipi_to_signal_pi_spectra_from_pt__5->SetLineWidth(2);
   kpipi_to_signal_pi_spectra_from_pt__5->SetMarkerStyle(22);
   kpipi_to_signal_pi_spectra_from_pt__5->GetXaxis()->SetTitle("#it{p_{T}} [MeV/#it{c}]");
   kpipi_to_signal_pi_spectra_from_pt__5->GetXaxis()->SetNdivisions(505);
   kpipi_to_signal_pi_spectra_from_pt__5->GetXaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_from_pt__5->GetXaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_from_pt__5->GetXaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_from_pt__5->GetXaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_from_pt__5->GetXaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_from_pt__5->GetXaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_from_pt__5->GetYaxis()->SetTitle("Candidates (arb. units)");
   kpipi_to_signal_pi_spectra_from_pt__5->GetYaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_from_pt__5->GetYaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_from_pt__5->GetYaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_from_pt__5->GetYaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_from_pt__5->GetYaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_from_pt__5->GetYaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_from_pt__5->GetZaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_from_pt__5->GetZaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_from_pt__5->GetZaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_from_pt__5->GetZaxis()->SetTitleOffset(1.2);
   kpipi_to_signal_pi_spectra_from_pt__5->GetZaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_from_pt__5->Draw("HIST");
   Double_t xAxis6[14] = {0, 500, 1000, 1500, 2000, 2500, 3000, 4000, 5000, 7500, 10000, 13000, 16000, 20000}; 
   
   TH1F *kpipi_to_signal_pi_spectra_to_pt__6 = new TH1F("kpipi_to_signal_pi_spectra_to_pt__6","kpipi_to_signal_pi_spectra_to_pt",13, xAxis6);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(1,0.05850794);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(2,0.1995841);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(3,0.1538905);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(4,0.1833457);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(5,0.1266739);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(6,0.102939);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(7,0.07475165);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(8,0.06002269);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(9,0.03057682);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(10,0.005591307);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(11,0.002777381);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(12,0.001031993);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinContent(13,0.0003070564);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(1,0.01469941);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(2,0.0274324);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(3,0.02396222);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(4,0.02693632);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(5,0.02196596);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(6,0.02012051);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(7,0.01199649);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(8,0.01049433);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(9,0.004948231);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(10,0.002015061);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(11,0.001272543);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(12,0.0008177226);
   kpipi_to_signal_pi_spectra_to_pt__6->SetBinError(13,0.0003070564);
   kpipi_to_signal_pi_spectra_to_pt__6->SetEntries(1421);
   kpipi_to_signal_pi_spectra_to_pt__6->SetStats(0);
   kpipi_to_signal_pi_spectra_to_pt__6->SetFillColor(2);
   kpipi_to_signal_pi_spectra_to_pt__6->SetFillStyle(3005);
   kpipi_to_signal_pi_spectra_to_pt__6->SetLineColor(2);
   kpipi_to_signal_pi_spectra_to_pt__6->SetLineWidth(2);
   kpipi_to_signal_pi_spectra_to_pt__6->SetMarkerStyle(23);
   kpipi_to_signal_pi_spectra_to_pt__6->GetXaxis()->SetNdivisions(505);
   kpipi_to_signal_pi_spectra_to_pt__6->GetXaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_to_pt__6->GetXaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_to_pt__6->GetXaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_to_pt__6->GetXaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_to_pt__6->GetXaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_to_pt__6->GetXaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_to_pt__6->GetYaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_to_pt__6->GetYaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_pi_spectra_to_pt__6->GetYaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_to_pt__6->GetYaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_to_pt__6->GetYaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_pi_spectra_to_pt__6->GetYaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_to_pt__6->GetZaxis()->SetLabelFont(132);
   kpipi_to_signal_pi_spectra_to_pt__6->GetZaxis()->SetLabelSize(0.06);
   kpipi_to_signal_pi_spectra_to_pt__6->GetZaxis()->SetTitleSize(0.072);
   kpipi_to_signal_pi_spectra_to_pt__6->GetZaxis()->SetTitleOffset(1.2);
   kpipi_to_signal_pi_spectra_to_pt__6->GetZaxis()->SetTitleFont(132);
   kpipi_to_signal_pi_spectra_to_pt__6->Draw("HIST SAME");
   
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
