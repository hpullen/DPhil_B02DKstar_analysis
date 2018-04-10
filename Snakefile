# nTuple making
include: "snake_modules/Selection/Merge_ganga_tuples.snake"
include: "snake_modules/Selection/Merged_tuples_list.snake"


# include: "snake_modules/Merge_data.snake"
# include: "snake_modules/Merge_data_full_helicity.snake"
# include: "snake_modules/Merge_MC_full_helicity.snake"
# include: "snake_modules/Merge_DKpipi.snake"
#include: "snake_modules/Train_BDT.snake"
# include: "snake_modules/Apply_BDT.snake"
# include: "snake_modules/Calculate_efficiencies.snake"
# include: "snake_modules/Fit_monte_carlo.snake"

# nTuple making
# include: "snake_modules/Make_selection.snake"

# # Background studies
# include: "snake_modules/Charmless.snake"
# include: "snake_modules/Double_misID.snake"

# # Monte carlo fits
# include: "snake_modules/Fit_setup_plots.snake"

# # ANA tables and plots
# include: "snake_modules/ANA_plots.snake"


rule all:
    input:
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/.merged_tuples_up_to_date"
