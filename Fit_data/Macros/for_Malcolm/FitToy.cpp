#include <iostream>

#include "TRandom.h"

#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooAbsPdf.h"
#include "RooCategory.h"
#include "RooFormulaVar.h"
#include "RooRandom.h"
#include "RooAddPdf.h"
#include "RooDataHist.h"
#include "RooExponential.h"
#include "RooCBShape.h"
#include "RooArgList.h"
#include "RooFitResult.h"

// =========================================================================
// Macro to generate and fit a simple toy with and without a piK signal peak
// =========================================================================
int main(int argc, char * argv[]) {

    // =====
    // Setup
    // =====
    // Settings (change these to try different type of toy)
    bool free_Kpi_yield = true;
    bool free_expo_yield = false;
    if (!free_Kpi_yield && !free_expo_yield) {
        std::cout << "Must free at least one yield (Kpi signal or piK background"
            << std::endl;
        return -1;
    }
    
    // Make fit variable
    RooRealVar Bd_M("Bd_M", "", 5000, 5800);

    // Make fit categories: Kpi and piK
    RooCategory cat("category", "");
    cat.defineType("Kpi");
    cat.defineType("piK");


    // ============
    // Make toy PDF
    // ============
    // Signal shape fixed parameters (from my fit to Monte Carlo)
    RooRealVar alpha_L("alpha_L", "", 1.90);
    RooRealVar alpha_R("alpha_R", "", -3.00);
    RooRealVar n_L("n_L", "", 2.48);
    RooRealVar n_R("n_R", "", 0.76);
    RooRealVar ratio("ratio", "", 0.545);
    RooRealVar frac("frac", "", 0.275);

    // Signal mean and width (from my fit to data)
    RooRealVar mean("mean", "", 5278.2);
    RooRealVar sigma_L("sigma_L", "", 20.67);
    RooFormulaVar sigma_R("sigma_R", "@0 * @1", 
            RooArgList(sigma_L, ratio));

    // Make double crystal ball shape for signal
    RooCBShape * CB_L = new RooCBShape("CB_L", "", Bd_M, 
            mean, sigma_L, alpha_L, n_L);
    RooCBShape * CB_R = new RooCBShape("CB_R", "", Bd_M, 
            mean, sigma_R, alpha_R, n_R);
    RooAddPdf * signal_shape = new RooAddPdf("signal_shape", "",
            RooArgList(*CB_L, *CB_R), RooArgList(frac));

    // Make exponential shape for background
    RooRealVar slope("slope", "", -0.000519); // From my fit to data
    RooExponential * expo = new RooExponential("expo", "", Bd_M, slope);

    // Yields (from previous analysis)
    RooRealVar toy_n_signal_Kpi("toy_n_signal_Kpi", "", 774.9);
    RooRealVar toy_n_expo_piK("toy_n_expo_piK", "", 922.6);
    RooRealVar toy_R_piK_vs_Kpi("toy_R_piK_vs_Kpi", "", 0.06);
    RooFormulaVar toy_n_signal_piK("toy_n_signal_piK", "@0 * @1",
            RooArgList(toy_n_signal_Kpi, toy_R_piK_vs_Kpi));

    // Make total fit PDF
    RooAddPdf * toy_Kpi = new RooAddPdf("toy_Kpi", "", 
            RooArgList(*signal_shape), RooArgList(toy_n_signal_Kpi));
    RooAddPdf * toy_piK = new RooAddPdf("toy_piK", "", 
            RooArgList(*signal_shape, *expo), 
            RooArgList(toy_n_signal_piK, toy_n_expo_piK));
    RooSimultaneous * toy_pdf = new RooSimultaneous("toy_pdf", "", cat);
    toy_pdf->addPdf(*toy_Kpi, "Kpi");
    toy_pdf->addPdf(*toy_piK, "piK");


    // =============
    // Draw toy PDFs
    // =============
    RooPlot * frame = Bd_M.frame();
    toy_Kpi->plotOn(frame);
    TCanvas * canvas = new TCanvas("canvas", "", 500, 400);
    frame->Draw();
    canvas->SaveAs("Plots/Kpi_toy.pdf");
    frame->remove("toy_Kpi");
    toy_piK->plotOn(frame);
    canvas->SaveAs("Plots/piK_toy.pdf");
    delete canvas;


    // ====================
    // Generate toy dataset
    // ====================
    // Set random number seed
    TRandom * rand = new TRandom;
    rand->SetSeed();
    RooRandom::setRandomGenerator(rand);

    // Generate binned dataset
    RooDataHist * data = toy_pdf->generateBinned(RooArgList(Bd_M, cat),
            toy_pdf->expectedEvents(cat));


    // ============
    // Make fit PDF
    // ============
    // Make yields
    RooRealVar n_signal_Kpi;
    RooRealVar n_expo_piK;
    RooRealVar R_piK_vs_Kpi("R_piK_vs_Kpi", "", 0.06, 0, 10);
    if (free_Kpi_yield) {
        n_signal_Kpi = RooRealVar("n_signal_Kpi", "", 100, 0, 8000);
    } else {
        n_signal_Kpi = RooRealVar("n_signal_Kpi", "", toy_n_signal_Kpi.getVal());
    }
    if (free_expo_yield) {
        n_expo_piK = RooRealVar("n_expo_piK", "", 1000, 0, 8000);
    } else {
        n_expo_piK = RooRealVar("n_expo_piK", "", toy_n_expo_piK.getVal());
    }
    RooFormulaVar n_signal_piK("n_signal_piK", "@0 * @1",
            RooArgList(n_signal_Kpi, R_piK_vs_Kpi));

    // Make PDFs
    RooAddPdf * fit_Kpi = new RooAddPdf("fit_Kpi", "",
            RooArgList(*signal_shape), RooArgList(n_signal_Kpi));
    RooAddPdf * fit_piK = new RooAddPdf("fit_piK", "",
            RooArgList(*signal_shape, *expo),
            RooArgList(n_signal_piK, n_expo_piK));
    RooSimultaneous * fit_pdf = new RooSimultaneous("fit_pdf", "", cat);
    fit_pdf->addPdf(*fit_Kpi, "Kpi");
    fit_pdf->addPdf(*fit_piK, "piK");


    // ===========
    // Perform fit
    // ===========
    RooFitResult * result = fit_pdf->fitTo(*data, RooFit::Save(),
            RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));

    result->Print("v");
    double NLL = result->minNll();


    // =========================================
    // Make fit PDF without piK signal component
    // =========================================
    // Make yields
    RooRealVar null_n_signal_Kpi;
    RooRealVar null_n_expo_piK;
    if (free_Kpi_yield) {
        null_n_signal_Kpi = RooRealVar("null_n_signal_Kpi", "", 100, 0, 8000);
    } else {
        null_n_signal_Kpi = RooRealVar("null_n_signal_Kpi", "",
                toy_n_signal_Kpi.getVal());
    }
    if (free_expo_yield) {
        null_n_expo_piK = RooRealVar("null_n_expo_piK", "", 1000, 0, 8000);
    } else {
        null_n_expo_piK = RooRealVar("null_n_expo_piK", "",
                toy_n_expo_piK.getVal());
    }

    // Make PDFs: piK has exponential background only
    RooAddPdf * null_fit_Kpi = new RooAddPdf("null_fit_Kpi", "",
            RooArgList(*signal_shape), RooArgList(null_n_signal_Kpi));
    RooAddPdf * null_fit_piK = new RooAddPdf("null_fit_piK", "",
            RooArgList(*expo), RooArgList(null_n_expo_piK));
    RooSimultaneous * null_fit_pdf = new RooSimultaneous("null_fit_pdf", "", cat);
    null_fit_pdf->addPdf(*null_fit_Kpi, "Kpi");
    null_fit_pdf->addPdf(*null_fit_piK, "piK");


    // ================================
    // Perform fit with null hypothesis
    // ================================
    RooFitResult * result_null = null_fit_pdf->fitTo(*data, RooFit::Save(),
            RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    result_null->Print("v");
    double NLL_null = result_null->minNll();


    // ======================
    // Calculate significance
    // ======================
    double significance = sqrt(2 * (NLL_null - NLL));
    std::cout << "Significance: " << significance << std::endl;

}
