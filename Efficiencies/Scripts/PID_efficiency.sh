# Function to calculate PID efficiency for one config
calc_efficiencies() {

    # Input args
    FLAV=$1
    BINNING=$2
    EXTRA_OPT=""
    if [[ $# == 3 ]]; then
        EXTRA_OPT=$3
    fi

    # Get particle name
    if [[ $FLAV == "combined" ]]; then
        PARTICLE=""
    else
        PARTICLE="_${FLAV}"
    fi

    # Get binning name
    if [[ $BINNING = "default" ]]; then
        BNAME=""
    else 
        BNAME="_alt"
    fi

    # Get modes to loop over
    MODES_SHARE=""
    if [[ $EXTRA_OPT == "" ]]; then
        MODES="Kpi KK pipi Kpipipi piKpipi"
        if [[ $FLAV == "combined" ]]; then
            MODES="Kpi KK pipi"
            MODES_SHARE="Kpipipi pipipipi"
        fi
    elif [[ $EXTRA_OPT == "--doubleSwap" ]]; then
        MODES="Kpi Kpipipi"
        if [[ $FLAV == "combined" ]]; then
            MODES="Kpi"
            MODES_SHARE="Kpipipi"
        fi
    elif [[ $EXTRA_OPT == "--rho" ]]
        MODES="Kpi"
    else
        echo "Unrecognised option: $EXTRA_OPT"
        return
    fi

    # Create input string
    INPUT_STR="-i"
    DIR="../PID/Results/"

    # Loop through modes with a single file
    for MODE in $MODES; do
        INPUT_STR="${INPUT_STRING} $MODE ${DIR}/${MODE}${PARTICLE}${BNAME}.param"
    done

    # Loop through modes with files to combine
    if [[ $FLAV == "combined" 
    for MODE in $MODES_SHARE; do
        INPUT_STR






}
