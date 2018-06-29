# nTuple making
# include: "snake_modules/Selection/Merge_ganga_tuples.snake"
# include: "snake_modules/Selection/Merged_tuples_list.snake"
include: "snake_modules/Selection/Preselection_plots.snake"
include: "snake_modules/Selection/Selected_tuples.snake"
include: "snake_modules/Selection/Weight_tuples.snake"

# BDT
include: "snake_modules/BDT/Make_training_tuples.snake"
include: "snake_modules/BDT/Training_tuple_list.snake"
include: "snake_modules/BDT/Apply_BDT.snake"
include: "snake_modules/BDT/Tuples_with_BDT_list.snake"

# Background studies
# include: "snake_modules/Charmless.snake"
include: "snake_modules/Backgrounds/Double_misID.snake"

# Monte carlo fits
# include: "snake_modules/Fit_setup/Fit_setup_plots.snake"
include: "snake_modules/Fit_setup/MC_fits.snake"
include: "snake_modules/Fit_setup/MC_param_tables.snake"

# Correction calculation
include: "snake_modules/Corrections/Asymmetry.snake"
include: "snake_modules/Corrections/Selection_efficiency.snake"

# Data fit
include: "snake_modules/Data_fit/Data_fit.snake"

# ANA tables and plots
include: "snake_modules/ANA/ANA_plots.snake"
include: "snake_modules/ANA/ANA_tables.snake"


rule all:
    input:
        "/home/pullen/analysis/B02DKstar/ANA_resources/.plots_up_to_date",
        "/home/pullen/analysis/B02DKstar/ANA_resources/.tables_up_to_date"

rule clean:
    shell:
        # "rm /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/.merged_tuples_up_to_date;"
        # "rm /home/pullen/analysis/B02DKstar/BDT/.BDT_tuples_up_to_date;"
        "rm ANA_resources/.plots_up_to_date;"
        "rm ANA_resources/.tables_up_to_date"
