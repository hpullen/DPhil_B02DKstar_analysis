#include <sstream>

#include "TFile.h"

#include "ToyBDTPdfMaker.hpp"


// ===========
// Constructor
// ===========
ToyBDTPdfMaker::ToyBDTPdfMaker(RooRealVar * x, RooCategory * cat, double cut_Kpi, 
        double cut_KK, double cut_pipi, double cut_Kpipipi, double cut_pipipipi) :
    ToyPdfMaker(x, cat, "Results/twoAndFourBody_data_split_looseBDT.root"),
    m_cuts(MakeCutMap(cut_Kpi, cut_KK, cut_pipi, cut_Kpipipi, cut_pipipipi)) {}


// ==========
// Destructor
// ==========
ToyBDTPdfMaker::~ToyBDTPdfMaker() {}


// =======================================================
// Rescale yields using signal and background efficiencies
// =======================================================
void ToyBDTPdfMaker::RescaleYields() {

    // Get efficiencies
    std::map<std::string, std::map<std::string, double>> effs = GetEfficiencies();

    // Loop through modes and make rescaled yields
    for (auto mode : m_modes) {

        // Get name of BDT mode
        std::string mode_short = mode;
        if (mode.find("_plus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_plus"));
        } else if (mode.find("_minus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_minus"));
        }
        std::string mode_bdt = mode_short;
        if (mode_short == "piK") mode_bdt = "Kpi";
        else if (mode_short == "piKpipi") mode_bdt = "Kpipipi";

        // Get signal and background efficiency for mode
        m_pars->AddRealVar("eff_rescale_sig_" + mode, effs[mode_bdt]["signal"]);
        m_pars->AddRealVar("eff_rescale_bg_" + mode, effs[mode_bdt]["background"]);
        if (effs[mode_bdt]["signal"] < 0.9999999) {
            std::cout << "ToyBDTPdfMaker: " << mode << 
                " signal yields will be rescaled by a factor " << 
                effs[mode_bdt]["signal"] << std::endl;
            std::cout << "ToyBDTPdfMaker: " << mode << 
                " background yield will be rescaled by a factor " << 
                effs[mode_bdt]["background"] << std::endl;
        }

        // Make rescaled yields
        // Background
        m_pars->AddFormulaVar("rescaled_n_expo_" + mode, "@0 * @1",
                ParameterList("n_expo_" + mode, "eff_rescale_bg_" + mode));

        // Signal and physics backgrounds
        m_pars->AddFormulaVar("rescaled_n_signal_" + mode, "@0 * @1",
                ParameterList("n_signal_" + mode, "eff_rescale_sig_" + mode));
        m_pars->AddFormulaVar("rescaled_n_low_" + mode, "@0 * @1",
                ParameterList("n_low_" + mode, "eff_rescale_sig_" + mode));
        m_pars->AddFormulaVar("rescaled_n_rho_" + mode, "@0 * @1",
                ParameterList("n_rho_" + mode, "eff_rescale_sig_" + mode));

        // Suppressed modes only
        if (!(mode_short == "Kpi" || mode_short == "Kpipipi")) {
            m_pars->AddFormulaVar("rescaled_n_Bs_" + mode, "@0 * @1",
                    ParameterList("n_Bs_" + mode, "eff_rescale_sig_" + mode));
            m_pars->AddFormulaVar("rescaled_n_Bs_low_" + mode, "@0 * @1",
                    ParameterList("n_Bs_low_" + mode, "eff_rescale_sig_" + mode));
        }

    } // End mode loop
}


// =============================================
// Make shapes for each mode with rescaled yield
// =============================================
void ToyBDTPdfMaker::MakeModeShapes() {

    // Loop through modes 
    for (std::string mode : m_modes) {

        // Map of shapes and yields
        std::map<std::string, std::string> shapes;

        // Get mode name without plus or minus
        std::string mode_short = mode;
        if (mode.find("_plus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_plus"));
        } else if (mode.find("_minus") != std::string::npos) {
            mode_short = mode.substr(0, mode.find("_minus"));
        }

        // Determine shape type
        bool is_four_body = (mode_short == "Kpipipi" || 
                mode_short == "piKpipi" || mode_short == "pipipipi");
        bool is_favoured = (mode_short == "Kpi" || mode_short == "Kpipipi");
        std::string type = is_four_body ? "4body_" : "";

        // Shapes common to all modes
        shapes.emplace(type + "signal", "rescaled_n_signal_" + mode);
        shapes.emplace(type + "rho", "rescaled_n_rho_" + mode);
        shapes.emplace("expo_" + mode_short, "rescaled_n_expo_" + mode);
        shapes.emplace("low_" + mode, "rescaled_n_low_" + mode);

        // Shapes for suppressed modes only
        if (!is_favoured) {
            shapes.emplace(type + "Bs", "rescaled_n_Bs_" + mode);
            shapes.emplace(type + "Bs_low", "rescaled_n_Bs_low_" + mode);
        }

        // DKpipi shape
        // if (is_favoured || mode_short == "piK") {
            // shapes.emplace(type + "DKpipi", "n_DKpipi_" + mode);
        // }

        // Make the shape
        m_shapes->CombineShapes(mode, shapes);
    }
}



// ===================================================
// Override shape making function to include rescaling
// ===================================================
void ToyBDTPdfMaker::MakeShape() {

    // Reset managers
    m_pars->Reset();
    m_shapes->Reset();

    // Set up parameters
    SetConstantParameters();
    SetFloatingParameters();
    SetDependentParameters();
    RescaleYields();

    // Set up shapes
    MakeComponentShapes();
    MakeModeShapes();
    MakeSimultaneousShape();

    // Change flag
    m_shapeMade = true;
}


// ==================
// Set up map of cuts
// ==================
std::map<std::string, double> ToyBDTPdfMaker::MakeCutMap(double cut_Kpi,
        double cut_KK, double cut_pipi, double cut_Kpipipi, double cut_pipipipi) {
    std::map<std::string, double> cuts;
    cuts["Kpi"] = cut_Kpi;
    cuts["KK"] = cut_KK;
    cuts["pipi"] = cut_pipi;
    cuts["Kpipipi"] = cut_Kpipipi;
    cuts["pipipipi"] = cut_pipipipi;
    return cuts;
}


// ==========================================================
// Get map of efficiencies for signal/background in each mode
// ==========================================================
std::map<std::string, std::map<std::string, double>> ToyBDTPdfMaker::GetEfficiencies() {

    // Map to store efficiency factors
    std::map<std::string, std::map<std::string, double>> effs;

    // Luminosity for each year
    double lumi_2015 = 0.281;
    double lumi_2016 = 1.605;
    double lumi_total = lumi_2015 + lumi_2016;

    // Loop through modes and calculate signal and background efficiencies
    std::string eff_dir = "/home/pullen/analysis/B02DKstar/Optimize_BDT_cut/"
        "Efficiencies/";
    for (std::string mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        bool is_fourBody = (mode == "Kpipipi" || mode == "pipipipi");
        std::map<std::string, std::map<std::string, double>> effs_mode;

        // Loop through signal and background
        for (std::string type : {"signal", "background"}) {

            // Loop through year/polarity
            for (std::string mag : {"up", "down"}) {
                for (std::string year : {"2015", "2016"}) {

                    // No signal MC available for 2015 4body
                    if (type == "signal" && is_fourBody && year == "2015") continue;

                    // Add efficiency to map
                    effs_mode[type][year + "_" + mag] = GetEfficiency(eff_dir + 
                            year + "_" + mag + "/" + type + "_" + mode + ".root", 
                            m_cuts[mode]);

                } // End year loop
            } // End mag loop

            // Average over efficiencies and add to map
            double average_eff = 0;
            double eff_2016 = (effs_mode[type]["2016_down"] + 
                        effs_mode[type]["2016_up"])/2;
            if (type == "signal" && is_fourBody) {
                average_eff = eff_2016;
            } else {
                double eff_2015 = (effs_mode[type]["2015_down"] + 
                        effs_mode[type]["2015_up"])/2;
                average_eff = (lumi_2015 * eff_2015 + lumi_2016 * eff_2016)/lumi_total;
            }
            effs[mode][type] = average_eff;

        } // End signal/background loop
    } // End mode loop

    return effs;
}


// ==========================================
// Read a single efficiency value from a file
// ==========================================
double ToyBDTPdfMaker::GetEfficiency(std::string filename, double cut) {

    // Name of variable
    std::stringstream ss;
    ss << "efficiency_" << std::setprecision(1) << cut;

    // Open file and get efficiency variable
    TFile * file = TFile::Open(filename.c_str(), "READ");
    RooRealVar * eff = (RooRealVar*)file->Get(ss.str().c_str());
    double eff_val = eff->getVal();
    file->Close();
    return eff_val;
}
