# nTuple making
include: "snake_modules/Selection/Merge_ganga_tuples.snake"
include: "snake_modules/Selection/Merged_tuples_list.snake"
include: "snake_modules/Selection/Preselection_plots.snake"

# BDT
include: "snake_modules/BDT/Make_training_tuples.snake"
include: "snake_modules/BDT/Training_tuple_list.snake"

# Background studies
# include: "snake_modules/Charmless.snake"
include: "snake_modules/Backgrounds/Double_misID.snake"

# Monte carlo fits
# include: "snake_modules/Fit_setup/Fit_setup_plots.snake"
include: "snake_modules/Fit_setup/MC_fits.snake"
include: "snake_modules/Fit_setup/MC_param_tables.snake"

# ANA tables and plots
include: "snake_modules/ANA/ANA_plots.snake"
include: "snake_modules/ANA/ANA_tables.snake"


rule all:
    input:
        "/home/pullen/analysis/B02DKstar/ANA_resources/.tables_up_to_date",
        "/home/pullen/analysis/B02DKstar/ANA_resources/.plots_up_to_date"

rule clean:
    shell:
        "rm ANA_resources/.plots_up_to_date;"
        "rm ANA_resources/.tables_up_to_date"
