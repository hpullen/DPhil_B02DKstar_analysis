#include <map>
#include <string>

std::map<std::string, std::string> paper_names = {
    {"A_signal_KK", "$\\mathcal{A}_{\\CP}^{KK}$"},
    {"A_signal_pipi", "$\\mathcal{A}_{\\CP}^{\\pi\\pi}$"},
    {"R_signal_KK", "$\\mathcal{R}_{\\CP}^{KK}$"},
    {"R_signal_pipi", "$\\mathcal{R}_{\\CP}^{\\pi\\pi}$"},
    {"A_signal_pipipipi", "$\\mathcal{A}_{\\CP}^{4\\pi}$"},
    {"R_signal_pipipipi", "$\\mathcal{R}_{\\CP}^{4\\pi}$"},
    {"R_signal_piK_plus", "$\\mathcal{R}_+^{\\pi K}$"},
    {"R_signal_piK_minus", "$\\mathcal{R}_-^{\\pi K}$"},
    {"R_signal_piKpipi_plus", "$\\mathcal{R}_+^{\\pi K\\pi\\pi}$"},
    {"R_signal_piKpipi_minus", "$\\mathcal{R}_-^{\\pi K\\pi\\pi}$"},
    {"A_signal_Kpi", "$\\mathcal{A}_{\\rm ADS}^{K\\pi}$"},
    {"A_signal_Kpipipi", "$\\mathcal{A}_{\\rm ADS}^{K\\pi\\pi\\pi}$"},
    {"A_Bs_piK", "$\\mathcal{A}_{s,\\rm ADS}^{\\pi K}$"},
    {"A_Bs_piKpipi", "$\\mathcal{A}_{s,\\rm ADS}^{\\pi K\\pi\\pi}$"},
    {"A_Bs_KK", "$\\mathcal{A}_{s,\\CP}^{KK}$"},
    {"A_Bs_pipi", "$\\mathcal{A}_{s,\\CP}^{\\pi\\pi}$"},
    {"R_Bs_KK", "$\\mathcal{R}_{s,\\CP}^{KK}$"},
    {"R_Bs_pipi", "$\\mathcal{R}_{s,\\CP}^{\\pi\\pi}$"},
    {"A_Bs_pipipipi", "$\\mathcal{A}_{s,\\CP}^{4\\pi}$"},
    {"R_Bs_pipipipi_run2", "$\\mathcal{R}_{s,\\CP}^{4\\pi}$"},

    {"A_signal_KK_run1", "$\\mathcal{A}_{\\CP,1}^{KK}$"},
    {"A_signal_pipi_run1", "$\\mathcal{A}_{\\CP,1}^{\\pi\\pi}$"},
    {"R_signal_KK_run1", "$\\mathcal{R}_{\\CP,1}^{KK}$"},
    {"R_signal_pipi_run1", "$\\mathcal{R}_{\\CP,1}^{\\pi\\pi}$"},
    {"A_signal_pipipipi_run1", "$\\mathcal{A}_{\\CP,1}^{4\\pi}$"},
    {"R_signal_pipipipi_run1", "$\\mathcal{R}_{\\CP,1}^{4\\pi}$"},
    {"R_signal_piK_run1_plus", "$\\mathcal{R}_{+,1}^{\\pi K}$"},
    {"R_signal_piK_run1_minus", "$\\mathcal{R}_{-,1}^{\\pi K}$"},
    {"R_signal_piKpipi_run1_plus", "$\\mathcal{R}_{+,1}^{\\pi K\\pi\\pi}$"},
    {"R_signal_piKpipi_run1_minus", "$\\mathcal{R}_{-,1}^{\\pi K\\pi\\pi}$"},
    {"A_signal_Kpi_run1", "$\\mathcal{A}_{\\rm ADS,1}^{K\\pi}$"},
    {"A_signal_Kpipipi_run1", "$\\mathcal{A}_{\\rm ADS,1}^{K\\pi\\pi\\pi}$"},
    {"A_Bs_piK_run1", "$\\mathcal{A}_{s,\\rm ADS,1}^{\\pi K}$"},
    {"A_Bs_piKpipi_run1", "$\\mathcal{A}_{s,\\rm ADS,1}^{\\pi K\\pi\\pi}$"},
    {"A_Bs_KK_run1", "$\\mathcal{A}_{s,\\CP,1}^{KK}$"},
    {"A_Bs_pipi_run1", "$\\mathcal{A}_{s,\\CP,1}^{\\pi\\pi}$"},
    {"R_Bs_KK_run1", "$\\mathcal{R}_{s,\\CP,1}^{KK}$"},
    {"R_Bs_pipi_run1", "$\\mathcal{R}_{s,\\CP,1}^{\\pi\\pi}$"},
    {"A_Bs_pipipipi_run1", "$\\mathcal{A}_{s,\\CP,1}^{4\\pi}$"},

    {"A_signal_KK_run2", "$\\mathcal{A}_{\\CP,2}^{KK}$"},
    {"A_signal_pipi_run2", "$\\mathcal{A}_{\\CP,2}^{\\pi\\pi}$"},
    {"R_signal_KK_run2", "$\\mathcal{R}_{\\CP,2}^{KK}$"},
    {"R_signal_pipi_run2", "$\\mathcal{R}_{\\CP,2}^{\\pi\\pi}$"},
    {"A_signal_pipipipi_run2", "$\\mathcal{A}_{\\CP,2}^{4\\pi}$"},
    {"R_signal_pipipipi_run2", "$\\mathcal{R}_{\\CP,2}^{4\\pi}$"},
    {"R_signal_piK_run2_plus", "$\\mathcal{R}_{+,2}^{\\pi K}$"},
    {"R_signal_piK_run2_minus", "$\\mathcal{R}_{-,2}^{\\pi K}$"},
    {"R_signal_piKpipi_run2_plus", "$\\mathcal{R}_{+,2}^{\\pi K\\pi\\pi}$"},
    {"R_signal_piKpipi_run2_minus", "$\\mathcal{R}_{-,2}^{\\pi K\\pi\\pi}$"},
    {"A_signal_Kpi_run2", "$\\mathcal{A}_{\\rm ADS,2}^{K\\pi}$"},
    {"A_signal_Kpipipi_run2", "$\\mathcal{A}_{\\rm ADS,2}^{K\\pi\\pi\\pi}$"},
    {"A_Bs_piK_run2", "$\\mathcal{A}_{s,\\rm ADS,2}^{\\pi K}$"},
    {"A_Bs_piKpipi_run2", "$\\mathcal{A}_{s,\\rm ADS,2}^{\\pi K\\pi\\pi}$"},
    {"A_Bs_KK_run2", "$\\mathcal{A}_{s,\\CP,2}^{KK}$"},
    {"A_Bs_pipi_run2", "$\\mathcal{A}_{s,\\CP,2}^{\\pi\\pi}$"},
    {"R_Bs_KK_run2", "$\\mathcal{R}_{s,\\CP,2}^{KK}$"},
    {"R_Bs_pipi_run2", "$\\mathcal{R}_{s,\\CP,2}^{\\pi\\pi}$"},
    {"A_Bs_pipipipi_run2", "$\\mathcal{A}_{s,\\CP,2}^{4\\pi}$"}
};