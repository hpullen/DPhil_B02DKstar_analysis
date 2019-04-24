void plot_likelihood() {

    // Fill arrays
    Double_t val[100], nll[100], sigma[100];
    int n=0; val[n] = 272; nll[n] = 85390.6;
    n++; val[n] = 277; nll[n] = 85389.8;
    n++; val[n] = 282; nll[n] = 85389.1;
    n++; val[n] = 287; nll[n] = 85388.6;
    // n++; val[n] = 292; nll[n] = 85399.7;
    n++; val[n] = 297; nll[n] = 85387.9;
    n++; val[n] = 302; nll[n] = 85387.7;
    n++; val[n] = 307; nll[n] = 85387.7;
    // n++; val[n] = 312; nll[n] = 85379.5;
    // n++; val[n] = 317; nll[n] = 85391.8;

    // Calculate sigma
    double min_nll = 85387.7;
    for (int i = 0; i <= n; i++) {
        sigma[i] = nll[i] - min_nll;
    }

    // Plot
    std::string par = "N_signal_Kpi_run1_plus";
    TGraph * gr = new TGraph(n+1, val, sigma);
    gr->SetLineColor(2);
    gr->SetLineWidth(2);
    gr->SetMarkerColor(2);
    gr->SetMarkerStyle(21);
    gr->SetTitle("");
    gr->GetXaxis()->SetTitle(par.c_str());
    gr->GetYaxis()->SetTitle("#DeltaNLL");
    TCanvas * canvas = new TCanvas("canv", "", 900, 900);
    gr->Draw("ACP");
    TLine line(265, 0.5, 310, 0.5);
    line.SetLineStyle(2);
    line.SetLineWidth(1);
    line.SetLineColor(kBlue);
    line.Draw();
    canvas->SaveAs(("../profile_likelihood/" + par + ".pdf").c_str());
}
