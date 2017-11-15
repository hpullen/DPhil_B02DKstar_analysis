rule all:
    input:
        expand("/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/{mode}_selected.root", year = ["2011", "2012", "2015", "2016"], mag = ["up", "down"], mode = ["Kpipipi", "piKpipi"]),
        expand("/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/pipipipi_selected.root", year = ["2015", "2016"], mag = ["up", "down"])

####################################################
# Merge ganga job outputs into single, smaller tuple
####################################################
rule merge_Kpi_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/twoBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/twoBody/{year_input}_{mag_input}/"
        "Kpi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} Kpi data twoBody {wildcards.mag_input} "
        "Tuple_Kpi/DecayTree"

rule merge_piK_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/twoBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/twoBody/{year_input}_{mag_input}/"
        "piK.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} piK data twoBody {wildcards.mag_input} "
        "Tuple_piK/DecayTree"

rule merge_KK_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/twoBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/twoBody/{year_input}_{mag_input}/"
        "KK.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} KK data twoBody {wildcards.mag_input} "
        "Tuple_KK/DecayTree"

rule merge_pipi_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/twoBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/twoBody/{year_input}_{mag_input}/"
        "pipi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} pipi data twoBody {wildcards.mag_input} "
        "Tuple_pipi/DecayTree"

rule merge_Kpipipi_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/fourBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year_input}_{mag_input}/"
        "Kpipipi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} Kpipipi data fourBody {wildcards.mag_input} "
        "Tuple_Kpipipi/DecayTree"

rule merge_piKpipi_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/fourBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year_input}_{mag_input}/"
        "piKpipi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} piKpipi data fourBody {wildcards.mag_input} "
        "Tuple_piKpipi/DecayTree"

rule merge_pipipipi_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/fourBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year_input}_{mag_input}/"
        "pipipipi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} pipipipi data fourBody {wildcards.mag_input} "
        "Tuple_pipipipi/DecayTree"

################################################################
# Merge Ganga outputs into single nTuple for Monte Carlo samples
################################################################
wildcard_constraints:
    MC_year="201[56]",
    year="201[1256]",
    helicity="[01][01][01]"

rule merge_MC_signal_twoBody:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/MC/twoBody/{mode}/{MC_year}_"
        "{mag}/"
    output:
        "/data/lhcb/users/pullen/B02DKstar/MC/twoBody/{mode}/{MC_year}_{mag}/"
        "{mode}.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.MC_year} {wildcards.mode} MC twoBody/{wildcards.mode} "
        "{wildcards.mag} Tuple_{wildcards.mode}/DecayTree"

rule merge_MC_Kpipipi:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/MC/fourBody/Kpipipi/{year}_"
        "{mag}/"
    output:
        "/data/lhcb/users/pullen/B02DKstar/MC/fourBody/Kpipipi/{year}_{mag}/"
        "Kpipipi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year} Kpipipi MC fourBody/Kpipipi {wildcards.mag} "
        "Tuple_K3pi/DecayTree"

rule merge_MC_pipipipi:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/MC/fourBody/pipipipi/{year}_"
        "{mag}/"
    output:
        "/data/lhcb/users/pullen/B02DKstar/MC/fourBody/pipipipi/{year}_{mag}/"
        "pipipipi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year} pipipipi MC fourBody/pipipipi {wildcards.mag} "
        "Tuple_4pi/DecayTree"

rule merge_MC_backgrounds:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/MC/backgrounds/{type}/"
        "{year}_{mag}/"
    output:
        "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/{type}/{year}_{mag}/"
        "Kpi.root"
    shell: 
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year} Kpi MC backgrounds/{wildcards.type} {wildcards.mag} "
        "Tuple_Kpi/DecayTree"

rule merge_MC_part_reco:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/MC/backgrounds/{type}/"
        "{particle}/{helicity}/{year}_{mag}/"
    output:
        "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/{type}/{particle}/"
        "{helicity}/{year}_{mag}/Kpi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year} Kpi MC backgrounds/{wildcards.type}/{wildcards.particle}/"
        "{wildcards.helicity} {wildcards.mag} Tuple_Kpi/DecayTree"


###########################################################
# BDT training using background data and signal Monte Carlo
###########################################################
# rule train_BDT_twoBody:
#     input:
#         signal_15_down = "/data/lhcb/users/pullen/B02DKstar/MC/twoBody/{mode}/"
#                          "2015_down/{mode}.root",
#         signal_15_up = "/data/lhcb/users/pullen/B02DKstar/MC/twoBody/{mode}/"
#                        "2015_up/{mode}.root",
#         signal_16_down = "/data/lhcb/users/pullen/B02DKstar/MC/twoBody/{mode}/"
#                          "2016_down/{mode}.root",
#         signal_16_up = "/data/lhcb/users/pullen/B02DKstar/MC/twoBody/{mode}/"
#                        "2016_up/{mode}.root",
#         background_15_down = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/"
#                              "2015_down/{mode}.root",
#         background_15_up = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/"
#                              "2015_up/{mode}.root",
#         background_16_down = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/"
#                              "2016_down/{mode}.root",
#         background_16_up = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/"
#                              "2016_up/{mode}.root"
#     output: 
#         "/home/pullen/analysis/B02DKstar/BDT/Code/weights/"
#         "TMVAClassification_BDTG_{mode}_run2.weights.xml"
#     shell: 
#         "cd /home/pullen/analysis/B02DKstar/BDT/Code; ./TrainMVA "
#         "{wildcards.mode}_run2 -s {input.signal_15_down} {input.signal_15_up} "
#         "{input.signal_16_down} {input.signal_16_up} -b "
#         "{input.background_15_down} {input.background_15_up} "
#         "{input.background_16_down} {input.background_16_up} | tee logs/"
#         "{wildcards.mode}.log"


#################################
# BDT application to data nTuples
#################################
rule apply_BDT_Kpipipi:
    input:
        "/home/pullen/analysis/B02DKstar/BDT/Code/weights/TMVAClassification_BDTG_Kpipipi_run2.weights.xml", tuple = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/Kpipipi.root"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
        "Kpipipi_withBDTG.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/BDT/Code/; ./ApplyMVA Kpipipi_run2 "
        "{input.tuple} {output} DecayTree"

rule apply_BDT_piKpipi:
    input:
        "/home/pullen/analysis/B02DKstar/BDT/Code/weights/TMVAClassification_BDTG_Kpipipi_run2.weights.xml", tuple = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/piKpipi.root"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
        "piKpipi_withBDTG.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/BDT/Code/; ./ApplyMVA Kpipipi_run2 "
        "{input.tuple} {output} DecayTree"

rule apply_BDT_pipipipi:
    input:
        "/home/pullen/analysis/B02DKstar/BDT/Code/weights/TMVAClassification_BDTG_pipipipi_run2.weights.xml", tuple = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/pipipipi.root"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
        "pipipipi_withBDTG.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/BDT/Code/; ./ApplyMVA pipipipi_run2 "
        "{input.tuple} {output} DecayTree"


###########################################################
# Calculation of variables and application of loose BDT cut
###########################################################
rule calculate_vars_fourBody:
    input:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
        "{mode}_withBDTG.root"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
        "{mode}_withVars_withCuts.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Make_tuples/; ./MakeVarsTuple "
        "data {wildcards.year} {wildcards.mag} fourBody {wildcards.mode}"
    

############################################################
# Get fully selected nTuple (apart from BDT cut) for fitting
############################################################
rule get_selected_tuple_fourBody:
    input:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
        "{mode}_withVars_withCuts.root"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
        "{mode}_selected.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Make_tuples/; ./MakeSelectedTuple "
        "{wildcards.year} {wildcards.mag} {wildcards.mode}"

