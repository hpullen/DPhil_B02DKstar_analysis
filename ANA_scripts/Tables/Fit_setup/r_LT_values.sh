# Get efficiencies ratios between Bs and B0 and print to files
source ../s_no.sh

# Print efficiency ratio for a run
print_one() {
    RUN=$1
    OUTFILE="../../../ANA_resources/Tables/Fit_setup/Values/r_LT_${RUN}.tex"
    INFILE_B0="../../../Efficiencies/Values/total_efficiency_${RUN}.param"
    INFILE_Bs="../../../Efficiencies/Values/total_efficiency_Bs_${RUN}.param"
    EFF_B0=$(awk '/^Kpi /{print $2}' $INFILE_B0)
    ERR_B0=$(awk '/^Kpi /{print $3}' $INFILE_B0)
    EFF_Bs=$(awk '/^Kpi /{print $2}' $INFILE_Bs)
    ERR_Bs=$(awk '/^Kpi /{print $3}' $INFILE_Bs)
    RATIO=$(bc -l <<< "$EFF_Bs/$EFF_B0")
    RATIO_ERR=$(bc -l <<< "$RATIO * sqrt(($ERR_Bs/$EFF_Bs)^2 + ($ERR_B0/$EFF_B0)^2)")
    n_no $RATIO $RATIO_ERR > $OUTFILE
}

# Print efficiency ratio for each run
print_one run1
print_one run2


# Also print 
n_no $(awk '{print $2, $3}' "../../../Parameters/hadronization_fraction.param") > "../../../ANA_resources/Tables/Fit_setup/Values/fs_fd.tex"
