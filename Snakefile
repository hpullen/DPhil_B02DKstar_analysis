include: "snake_modules/Merge_data.snake"
include: "snake_modules/Merge_MC.snake"
include: "snake_modules/Train_BDT.snake"
include: "snake_modules/Apply_BDT.snake"

rule all:
    input:
        expand("/home/pullen/analysis/B02DKstar/BDT/Code/weights/TMVAClassification_BDTG_{mode}_run2.weights.xml", mode = ["Kpi", "KK", "pipi", "Kpipipi", "pipipipi"])




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

