#include: "snake_modules/Merge_data.snake"
#include: "snake_modules/Merge_MC.snake"
#include: "snake_modules/Train_BDT.snake"
include: "snake_modules/Apply_BDT.snake"
include: "snake_modules/Make_selection.snake"
include: "snake_modules/Calculate_efficiencies.snake"
include: "snake_modules/Fit_monte_carlo.snake"

rule all:
    input:
        #expand("/data/lhcb/users/pullen/B02DKstar/data/twoBody/{year}_{mag}/{mode}_selected.root", year = ["2011", "2012", "2015", "2016"], mag = ["up", "down"], mode = ["Kpi", "piK", "KK", "pipi"]),
        #expand("/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/{mode}_selected.root", year = ["2011", "2012", "2015", "2016"], mag = ["up", "down"], mode = ["Kpipipi", "piKpipi"]),
        #expand("/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/{mode}_selected.root", year = ["2015", "2016"], mag = ["up", "down"], mode = ["pipipipi"]),
        #expand("/data/lhcb/users/pullen/B02DKstar/MC/twoBody/{mode}/{year}_{mag}/{mode}_selected.root", year = ["2015", "2016"], mag = ["up", "down"], mode = ["Kpi", "KK", "pipi"]),
        #expand("/data/lhcb/users/pullen/B02DKstar/MC/fourBody/{mode}/{year}_{mag}/{mode}_selected.root", year = ["2016"], mag = ["up", "down"], mode = ["Kpipipi"]),
        #expand("/data/lhcb/users/pullen/B02DKstar/MC/fourBody/{mode}/{year}_{mag}/{mode}_selected.root", year = ["2016"], mag = ["up", "down"], mode = ["pipipipi"])
        expand("ANA_plots/MC_fit_signal_{mode}.pdf", mode = ["Kpi", "KK", "pipi", "Kpipipi", "pipipipi", "Bs"]),
        expand("ANA_plots/MC_fit_lowMass_{bg_type}{particle}_{helicity}.pdf", bg_type = ["Bs_", ""], particle = ["gamma", "pi"], helicity = ["010", "101"])

###########################################################
# Calculation of variables and application of loose BDT cut
###########################################################
#rule calculate_vars_fourBody:
#    input:
#        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
#        "{mode}_withBDTG.root"
#    output:
#        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
#        "{mode}_withVars_withCuts.root"
#    shell:
#        "cd /home/pullen/analysis/B02DKstar/Make_tuples/; ./MakeVarsTuple "
#        "data {wildcards.year} {wildcards.mag} fourBody {wildcards.mode}"
#    
#
#############################################################
## Get fully selected nTuple (apart from BDT cut) for fitting
#############################################################
#rule get_selected_tuple_fourBody:
#    input:
#        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
#        "{mode}_withVars_withCuts.root"
#    output:
#        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
#        "{mode}_selected.root"
#    shell:
#        "cd /home/pullen/analysis/B02DKstar/Make_tuples/; ./MakeSelectedTuple "
#        "{wildcards.year} {wildcards.mag} {wildcards.mode}"

