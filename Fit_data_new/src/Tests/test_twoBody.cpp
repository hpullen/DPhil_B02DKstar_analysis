// ========================================
// Script for testing ShapeMaker code
// ========================================
#include <iostream>

#include "TwoBodyPdfMaker.hpp"
#include "Plotter.hpp"

int main(int argc, char * argv[]) {

    // Make mass variable
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);

    // Make category
    RooCategory * cat = new RooCategory("modes", "");
    cat->defineType("Kpi");
    cat->defineType("piK");
    cat->defineType("KK");
    cat->defineType("pipi");

    // Make a TwoBodyPdfMaker
    TwoBodyPdfMaker * tb = new TwoBodyPdfMaker("test", Bd_M, cat);
    tb->Shape();
    tb->SaveHistograms("Histograms/test.root");
    delete tb;

}
