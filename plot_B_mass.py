import ROOT

canvas = ROOT.TCanvas("canvas", "", 900, 600)
for mode in ["Kpi", "piK"]:

    # Open data
    tree = ROOT.TChain("DecayTree")
    for year in ["2015", "2016"]:
        for mag in ["up", "down"]:
            path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/{}_{}/{}_selected.root".format(year, mag, mode)
            tree.Add(path)

    # Plot histograms
    for flav in ["B0", "B0bar"]:
        hist_name = "hist_{}_{}".format(flav, mode)
        hist = ROOT.TH1F(hist_name, "", 50, 5200, 5600)
        cut = ROOT.TCut("KstarK_ID {} 0".format(">" if flav == "B0" else "<"))
        if mode == "piK":
            cut += "Bd_ConsD_M > 5279 + 50"
        tree.Draw("Bd_ConsD_M >> {}".format(hist_name), cut)
        hist.SetLineWidth(1)
        hist.SetMarkerStyle(8)
        hist.SetLineColor(ROOT.kBlack)
        hist.SetMarkerColor(ROOT.kBlack)
        canvas.Clear()
        hist.Draw("E")
        canvas.SaveAs("B_mass_{}_{}.pdf".format(flav, mode))
