#include "RooPlot.h"
#include "TLegend.h"
#include "RooRealVar.h"
#include "TCanvas.h"

#include "ParameterManager.hpp"
#include "ShapeManager.hpp"
#include "ParameterReader.hpp"

void MakeShape(ShapeManager * m_shapes, ParameterManager * m_pars) {

    // DKpipi background
    std::vector<std::string> DKpipi_horns = {"1a", "3", "5a"};
    for (auto horns : DKpipi_horns) {
        m_shapes->AddHorns("DKpipi_" + horns + "_shape", "DKpipi_a_" + horns, 
                "DKpipi_b_" + horns, "DKpipi_csi_" + horns, "shift", 
                "DKpipi_sigma_" + horns, "DKpipi_ratio_" + horns,
                "DKpipi_frac_" + horns);
    }
    m_shapes->AddHorns("DKpipi_2_shape", "DKpipi_a_2", "DKpipi_b_2", 
            "DKpipi_csi_2", "shift", "DKpipi_sigma_2", "DKpipi_ratio_2",
            "DKpipi_frac_2");
    m_shapes->CombineShapes("DKpipi", {
            {"DKpipi_1a_shape", "DKpipi_coeff_1a"},
            {"DKpipi_2_shape", "DKpipi_coeff_2"},
            {"DKpipi_3_shape", "DKpipi_coeff_3"},
            {"DKpipi_5a_shape", "DKpipi_coeff_5a"}});
}


int main(int argc, char * argv[]) {

    // Make managers
    ParameterManager * m_pars = new ParameterManager();
    m_pars->AddRealVar("Bd_M", 5000, 5800);
    m_pars->AddRealVar("shift", 0);
    m_pars->SetWarnings(false);
    ShapeManager * m_shapes = new ShapeManager("shapes", (RooRealVar*)m_pars->Get("Bd_M"), m_pars);

    // Read parameters from file
    ParameterReader * pr = new ParameterReader("/home/pullen/analysis/B02DKstar/"
            "Fit_monte_carlo/Results/");
    pr->ReadParameters("DKpipi", "DKpipi.param");

    // Make canvas and plot
    TCanvas * canv = new TCanvas("canv", "", 500, 400);
    RooPlot * plot = ((RooRealVar*)m_pars->Get("Bd_M"))->frame(RooFit::Title("DKpipi shape"));

    // Plot initial shape
    std::vector<std::string> DKpipi_comps = {"1a", "2", "3", "5a"};
    std::vector<std::string> DKpipi_pars = {"a", "b", "csi", "sigma", "ratio", 
        "frac", "coeff"};
    for (auto comp : DKpipi_comps) {
        for (auto par : DKpipi_pars) {
            m_pars->AddRealVar("DKpipi_" + par + "_" + comp, 
                    pr->GetValue("DKpipi", par + "_" + comp));
        }
    }
    MakeShape(m_shapes, m_pars);
    m_shapes->Get("DKpipi")->plotOn(plot, RooFit::DrawOption("C"), RooFit::LineColor(1),
            RooFit::Name("original"));

    // Repeat several times
    int color = 1;
    for (int i = 0; i < 5; i++) {
        color++;

        // Make parameters
        for (auto comp : DKpipi_comps) {
            for (auto par : DKpipi_pars) {
                m_pars->AddRandomisedVar("DKpipi_" + par + "_" + comp,
                        pr->GetValue("DKpipi", par + "_" + comp),
                        pr->GetError("DKpipi", par + "_" + comp));
            }
        }

        // Remake shape
        m_shapes->Reset();
        MakeShape(m_shapes, m_pars);
        m_shapes->Get("DKpipi")->plotOn(plot, RooFit::DrawOption("C"), RooFit::LineColor(color));
    }


    // Save plot
    canv->cd();
    plot->Draw();
    TLegend * leg = new TLegend(0.6, 0.6, 0.85, 0.85);
    leg->AddEntry(plot->findObject("original"), "Original shape");
    leg->Draw();
    canv->SaveAs("Plots/DKpipi_variation.pdf");
    delete canv;

}
