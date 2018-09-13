# Print asymmetry values to tables and files
source ../s_no.sh

# Production asymmetry
ANA_DIR="/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Fit_setup"
PROD_DIR="/home/pullen/analysis/B02DKstar/Asymmetries/Production/Results"
s_no $(awk '/B0/{print $2, $3}' "${PROD_DIR}/production_asymmetry_run1.param") > ${ANA_DIR}/Asymmetry/prod_asym_B0.tex
s_no $(awk '/Bs/{print $2, $3}' "${PROD_DIR}/production_asymmetry_run1.param") > ${ANA_DIR}/Asymmetry/prod_asym_Bs.tex

# Detection asymmetry
DET_DIR="/home/pullen/analysis/B02DKstar/Asymmetries/Detection/Results"
s_no $(awk '/A_Kpi/{print $2, $3}' "${DET_DIR}/A_Kpi_run1.param") > ${ANA_DIR}/Asymmetry/det_asym_run1.tex
s_no $(awk '/A_Kpi/{print $2, $3}' "${DET_DIR}/A_Kpi_run2.param") > ${ANA_DIR}/Asymmetry/det_asym_run2.tex

