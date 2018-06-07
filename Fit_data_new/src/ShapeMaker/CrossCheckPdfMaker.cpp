#include "CrossCheckPdfMaker.hpp"

// ===========
// Constructor
// ===========
CrossCheckPdfMaker::CrossCheckPdfMaker(RooRealVar * x, RooCategory * cat, 
        bool blind) :
    DataPdfMaker(x, cat, blind) {}


// ==================================
// Make components then adjust yields
// ==================================
void CrossCheckPdfMaker::MakeComponents() {

    // Default function
    DataPdfMaker::MakeComponents();

    // Make adjustments
    m_pars->SetWarnings(false);
    std::vector<std::string> runs = {"_run1", "_run2"};
    for (auto run : runs) {

        // Favoured mode backgrounds
        std::map<std::string, double> fracs = {{"low", 1.5}, {"DKpipi", 0.15}};
        for (std::string type : {"low", "DKpipi"}) {
            for (std::string mode : {"Kpi", "Kpipipi"}) {
                double start = m_pars->GetValue("N_signal_" + mode + run) 
                    * fracs[type];
                m_pars->AddRealVar("N_" + type + "_" + mode + run, start,
                        start/2, start * 2);
            }
        }
        fracs.emplace("Bs_low", 1.1);
        for (std::string mode : {"piK", "piKpipi"}) {
            double start = m_pars->GetValue("N_Bs_" + mode + run) * fracs["Bs_low"];
            m_pars->AddRealVar("N_Bs_low_" + mode + run, start, start/2, start * 2);
        }

        // Low mass/signal/DKpipi
        for (std::string type : {"signal", "low", "DKpipi"}) {

            // Get starting values based on favourd mode
            std::map<std::string, double> starts;
            starts["KK"] = m_pars->GetValue("N_" + type + "_Kpi" + run) / 10;
            starts["pipi"] = m_pars->GetValue("N_" + type + "_Kpi" + run) / 30;
            starts["piK"] = m_pars->GetValue("N_" + type + "_Kpi" + run) * 0.06;
            starts["piKpipi"] = m_pars->GetValue("N_" + type + "_Kpipipi" + run) * 0.06;
            starts["pipipipi"] = m_pars->GetValue("N_" + type + "_Kpipipi" + 
                    run) / 10;

            for (std::string mode : {"KK", "pipi", "piK", "piKpipi", "pipipipi"}) {
                if (mode == "pipipipi" && run == "_run1") continue;
                if (type == "signal" && m_blind) {
                    // Blind
                    m_pars->AddRealVar("N_" + type + "_" + mode + run  + "_blind",
                            starts[mode], starts[mode] / 2, starts[mode] * 2);
                    m_pars->AddUnblindVar("N_" + type + "_" + mode + run,
                            "N_" + type + "_" + mode + run + "_blind",
                            "blind_signal_" + mode + run, starts[mode]/4);
                } else {
                    m_pars->AddRealVar("N_" + type + "_" + mode + run, 
                            starts[mode], starts[mode] / 2, starts[mode] * 2);

                }
            }
        }

        // Bs
        std::map<std::string, double> starts;
        starts["KK"] = m_pars->GetValue("N_Bs_piK" + run) / 10;
        starts["pipi"] = m_pars->GetValue("N_Bs_piK" + run) / 30;
        starts["pipipipi"] = m_pars->GetValue("N_Bs_piKpipi" + run) / 10;
        for (std::string mode : {"KK", "pipi", "pipipipi"}) {
            if (mode == "pipipipi" && run == "_run1") continue;
            m_pars->AddRealVar("N_Bs_" + mode + run, 
                    starts[mode], starts[mode] / 2, starts[mode] * 2);
        }
    }

    // Remake rho yields using Bs yields
    for (std::string mode : {"KK", "pipi", "pipipipi"}) {
        std::string fav = (mode == "pipipipi") ? "Kpipipi" : "Kpi";
        std::string sup = (mode == "pipipipi") ? "piKpipi" : "piK";
        m_pars->AddRealVar("R_rho_" + mode, 1);
        for (auto run : {"_run1", "_run2"}) {
            m_pars->AddFormulaVar("N_rho_" + mode + run, "@0 * @1 * @2 / @3",
                    ParameterList("N_rho_" + fav + run, "R_rho_" + mode,
                        "N_Bs_" + mode + run, "N_Bs_" + sup + run));
            // m_pars->AddFormulaVar("N_rho_" + mode, "@0 * @1 / @2",
                    // ParameterList("N_rho_" + fav, "R_rho_" + mode, "R_corr_" + mode));
            for (std::string sign : {"_plus", "_minus"}) {
                m_pars->AddFormulaVar("N_rho_" + mode + run + sign, "@0/2",
                        ParameterList("N_rho_" + mode + run));
            }
        }
    }

    // Remake KK, pipi, pipipipi Bs low yields (completely constrained)
    for (std::string mode : {"KK", "pipi", "pipipipi"}) {

        // Make ratio and asymmetry (fixed)
        // m_pars->AddRealVar("R_Bs_low_" + mode, 1, 0.5, 1.5);
        m_pars->AddRealVar("R_Bs_low_" + mode, 1);
        m_pars->AddRealVar("A_Bs_low_" + mode, 0);

        // Calculate raw yields
        std::string sup = (mode == "pipipipi") ? "piKpipi" : "piK";
        for (auto run : runs) {
            m_pars->AddFormulaVar("N_Bs_low_" + mode + run, "@0 * @1 / @2",
                    ParameterList("N_Bs_low_" + sup + run, "R_Bs_low_" + mode,
                        "R_corr_" + mode + run));
            m_pars->AddFormulaVar("N_Bs_low_" + mode + run + "_minus", 
                    "@0 * @1 * (1 - @2) / (2 * @3)", 
                    ParameterList("N_Bs_low_" + sup + run, "R_Bs_low_" + mode, 
                        "A_Bs_low_" + mode, "R_corr_" + mode + run));
            m_pars->AddFormulaVar("N_Bs_low_" + mode + run + "_plus", 
                    "@0 * @1 * (1 + @2) / (2 * @3 * @4)", 
                    ParameterList("N_Bs_low_" + sup + run, "R_Bs_low_" + mode, 
                        "A_Bs_low_" + mode, "R_corr_" + mode + run,
                        "a_corr_" + mode + "_s" + run));
        }
    }

    m_pars->SetWarnings(true);
}

