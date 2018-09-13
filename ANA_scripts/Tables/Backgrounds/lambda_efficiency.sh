# Get efficiency of lambda b 
source ../s_no.sh
EFF_DIR="/home/pullen/analysis/B02DKstar/Efficiencies/Values/"
ACC_FILE="${EFF_DIR}/acceptance_efficiency_lambda.param"
ACC_EFF=$(awk '/Kpi/{print $2}' $ACC_FILE)
ACC_ERR=$(awk '/Kpi/{print $3}' $ACC_FILE)
SEL_FILE="${EFF_DIR}/selection_efficiency_lambda_K.param"
SEL_EFF=$(awk '/Kpi/{print $2}' $SEL_FILE)
SEL_ERR=$(awk '/Kpi/{print $3}' $SEL_FILE)
PID_FILE="${EFF_DIR}/PID_efficiency_Lambda_K.param"
PID_EFF=$(awk '/Kpi/{print $2}' $PID_FILE)
PID_ERR=$(awk '/Kpi/{print $3}' $PID_FILE)
TOT=$(bc -l <<< "$ACC_EFF * $SEL_EFF * $PID_EFF")
TOT_ERR=$(bc -l <<< "$TOT * sqrt(($ACC_ERR/$ACC_EFF)^2 + ($SEL_ERR/$SEL_EFF)^2 + ($PID_ERR/$PID_EFF)^2)")
s_no $TOT $TOT_ERR > "/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Backgrounds/lambda_efficiency.tex"
