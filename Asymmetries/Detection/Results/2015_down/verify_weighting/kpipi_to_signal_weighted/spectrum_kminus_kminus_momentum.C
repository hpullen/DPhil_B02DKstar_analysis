void spectrum_kminus_kminus_momentum()
{
//=========Macro generated from canvas: c1_n2/c1_n2
//=========  (Tue Nov 13 22:08:20 2018) by ROOT version6.08/06
   TCanvas *c1_n2 = new TCanvas("c1_n2", "c1_n2",0,0,700,500);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   c1_n2->SetHighLightColor(2);
   c1_n2->Range(-19.25926,-0.03773157,132.5926,0.1980908);
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
   Double_t xAxis1[18] = {2, 6, 14, 18, 22, 26, 30, 34, 38, 40, 44, 48, 52, 56, 60, 75, 100, 125}; 
   
   TH1F *kpipi_to_signal_k_spectra_from_momentum__1 = new TH1F("kpipi_to_signal_k_spectra_from_momentum__1","kpipi_to_signal_k_spectra_from_momentum",17, xAxis1);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(1,0.04417845);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(2,0.1274114);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(3,0.07900683);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(4,0.09699735);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(5,0.08282986);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(6,0.05102356);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(7,0.1066008);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(8,0.09957062);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(9,0.07695355);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(10,0.06086751);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(11,0.04960281);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(12,0.04095421);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(13,0.02862799);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(14,0.01939763);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(15,0.02156096);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(16,0.009741694);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(17,0.004674833);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinContent(18,0.009767805);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(1,0.0001769444);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(2,0.0002414043);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(3,0.0002088307);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(4,0.0002442731);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(5,0.0002436393);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(6,0.0001552963);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(7,0.000293955);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(8,0.000303133);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(9,0.0003922071);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(10,0.0002364572);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(11,0.0002118099);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(12,0.0001988467);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(13,0.0001839576);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(14,0.0001382393);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(15,9.737326e-05);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(16,5.571215e-05);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(17,6.305759e-05);
   kpipi_to_signal_k_spectra_from_momentum__1->SetBinError(18,0.0005727248);
   kpipi_to_signal_k_spectra_from_momentum__1->SetMinimum(0);
   kpipi_to_signal_k_spectra_from_momentum__1->SetMaximum(0.1862997);
   kpipi_to_signal_k_spectra_from_momentum__1->SetEntries(4.332414e+07);
   kpipi_to_signal_k_spectra_from_momentum__1->SetStats(0);
   kpipi_to_signal_k_spectra_from_momentum__1->SetFillColor(4);
   kpipi_to_signal_k_spectra_from_momentum__1->SetFillStyle(3004);
   kpipi_to_signal_k_spectra_from_momentum__1->SetLineColor(4);
   kpipi_to_signal_k_spectra_from_momentum__1->SetLineWidth(2);
   kpipi_to_signal_k_spectra_from_momentum__1->SetMarkerStyle(22);
   kpipi_to_signal_k_spectra_from_momentum__1->GetXaxis()->SetTitle("#it{momentum} [GeV/#it{c}]");
   kpipi_to_signal_k_spectra_from_momentum__1->GetXaxis()->SetNdivisions(505);
   kpipi_to_signal_k_spectra_from_momentum__1->GetXaxis()->SetLabelFont(132);
   kpipi_to_signal_k_spectra_from_momentum__1->GetXaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_k_spectra_from_momentum__1->GetXaxis()->SetLabelSize(0.06);
   kpipi_to_signal_k_spectra_from_momentum__1->GetXaxis()->SetTitleSize(0.072);
   kpipi_to_signal_k_spectra_from_momentum__1->GetXaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_k_spectra_from_momentum__1->GetXaxis()->SetTitleFont(132);
   kpipi_to_signal_k_spectra_from_momentum__1->GetYaxis()->SetTitle("Candidates (arb. units)");
   kpipi_to_signal_k_spectra_from_momentum__1->GetYaxis()->SetLabelFont(132);
   kpipi_to_signal_k_spectra_from_momentum__1->GetYaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_k_spectra_from_momentum__1->GetYaxis()->SetLabelSize(0.06);
   kpipi_to_signal_k_spectra_from_momentum__1->GetYaxis()->SetTitleSize(0.072);
   kpipi_to_signal_k_spectra_from_momentum__1->GetYaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_k_spectra_from_momentum__1->GetYaxis()->SetTitleFont(132);
   kpipi_to_signal_k_spectra_from_momentum__1->GetZaxis()->SetLabelFont(132);
   kpipi_to_signal_k_spectra_from_momentum__1->GetZaxis()->SetLabelSize(0.06);
   kpipi_to_signal_k_spectra_from_momentum__1->GetZaxis()->SetTitleSize(0.072);
   kpipi_to_signal_k_spectra_from_momentum__1->GetZaxis()->SetTitleOffset(1.2);
   kpipi_to_signal_k_spectra_from_momentum__1->GetZaxis()->SetTitleFont(132);
   kpipi_to_signal_k_spectra_from_momentum__1->Draw("HIST");
   Double_t xAxis2[18] = {2, 6, 14, 18, 22, 26, 30, 34, 38, 40, 44, 48, 52, 56, 60, 75, 100, 125}; 
   
   TH1F *kpipi_to_signal_k_spectra_to_momentum__2 = new TH1F("kpipi_to_signal_k_spectra_to_momentum__2","kpipi_to_signal_k_spectra_to_momentum",17, xAxis2);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(1,0.06422584);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(2,0.08720633);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(3,0.06814131);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(4,0.09685894);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(5,0.076462);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(6,0.03497919);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(7,0.1278028);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(8,0.07496947);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(9,0.1330712);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(10,0.0197819);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(11,0.09315979);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(12,0.01897556);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(13,0.03173742);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(14,0.01413031);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(15,0.02754853);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(16,0.01816752);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(17,0.01278194);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinContent(18,0.1986146);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(1,0.0291149);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(2,0.02625817);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(3,0.0282182);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(4,0.03620844);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(5,0.03253307);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(6,0.02352819);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(7,0.04191107);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(8,0.03286805);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(9,0.05914894);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(10,0.01470861);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(11,0.03538127);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(12,0.01938945);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(13,0.02141845);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(14,0.01342904);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(15,0.01006271);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(16,0.006328063);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(17,0.005197957);
   kpipi_to_signal_k_spectra_to_momentum__2->SetBinError(18,0.1091367);
   kpipi_to_signal_k_spectra_to_momentum__2->SetEntries(339);
   kpipi_to_signal_k_spectra_to_momentum__2->SetStats(0);
   kpipi_to_signal_k_spectra_to_momentum__2->SetFillColor(2);
   kpipi_to_signal_k_spectra_to_momentum__2->SetFillStyle(3005);
   kpipi_to_signal_k_spectra_to_momentum__2->SetLineColor(2);
   kpipi_to_signal_k_spectra_to_momentum__2->SetLineWidth(2);
   kpipi_to_signal_k_spectra_to_momentum__2->SetMarkerStyle(23);
   kpipi_to_signal_k_spectra_to_momentum__2->GetXaxis()->SetNdivisions(505);
   kpipi_to_signal_k_spectra_to_momentum__2->GetXaxis()->SetLabelFont(132);
   kpipi_to_signal_k_spectra_to_momentum__2->GetXaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_k_spectra_to_momentum__2->GetXaxis()->SetLabelSize(0.06);
   kpipi_to_signal_k_spectra_to_momentum__2->GetXaxis()->SetTitleSize(0.072);
   kpipi_to_signal_k_spectra_to_momentum__2->GetXaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_k_spectra_to_momentum__2->GetXaxis()->SetTitleFont(132);
   kpipi_to_signal_k_spectra_to_momentum__2->GetYaxis()->SetLabelFont(132);
   kpipi_to_signal_k_spectra_to_momentum__2->GetYaxis()->SetLabelOffset(0.01);
   kpipi_to_signal_k_spectra_to_momentum__2->GetYaxis()->SetLabelSize(0.06);
   kpipi_to_signal_k_spectra_to_momentum__2->GetYaxis()->SetTitleSize(0.072);
   kpipi_to_signal_k_spectra_to_momentum__2->GetYaxis()->SetTitleOffset(0.95);
   kpipi_to_signal_k_spectra_to_momentum__2->GetYaxis()->SetTitleFont(132);
   kpipi_to_signal_k_spectra_to_momentum__2->GetZaxis()->SetLabelFont(132);
   kpipi_to_signal_k_spectra_to_momentum__2->GetZaxis()->SetLabelSize(0.06);
   kpipi_to_signal_k_spectra_to_momentum__2->GetZaxis()->SetTitleSize(0.072);
   kpipi_to_signal_k_spectra_to_momentum__2->GetZaxis()->SetTitleOffset(1.2);
   kpipi_to_signal_k_spectra_to_momentum__2->GetZaxis()->SetTitleFont(132);
   kpipi_to_signal_k_spectra_to_momentum__2->Draw("HIST SAME");
   
   TLegend *leg = new TLegend(0.555,0.73,0.89,0.92,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetTextFont(132);
   leg->SetTextSize(0.05);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(2);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("kpipi_to_signal_k_spectra_from_momentum","[#it{D^{#pm}#rightarrowK^{#mp}#pi^{#pm}#pi^{#pm}}] K^{#mp}","f");
   entry->SetFillColor(4);
   entry->SetFillStyle(3004);
   entry->SetLineColor(4);
   entry->SetLineStyle(1);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(132);
   entry=leg->AddEntry("kpipi_to_signal_k_spectra_to_momentum","[#it{Signal}] K^{#mp}","f");
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
