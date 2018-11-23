# Make PID performance histograms based on reference samples for cuts of interest with alternate binning
# Location to save to
OUTPUT_DIR=/data/lhcb/users/pullen/B02DKstar/PIDCalib/PerfHists/Alternative

# PID cuts
K_CUTS="[DLLK > 1, DLLK > 3, DLLK > 5, DLLK < -1, DLLp > 10]"
PI_CUTS="[DLLK < -1, DLLK > 1, DLLK > 5]"

# Location of script
SCRIPT=..UraniaDev_v7r0/PIDCalib/PIDPerfScripts/scripts/python/MultiTrack/MakePerfHistsRunRange.py

# Run 1
for STRIP in "20r1" "20"; do
    for MAG in MagUp MagDown; do
        python "$SCRIPT" "$STRIP" "$MAG" "Pi" "$PI_CUTS" "P" "ETA" "" --binSchemeFile="AltBinning.py" --schemeName="AltBinning"
        mv PerfHists_Pi_Strip${STRIP}_${MAG}_AltBinning_P_ETA.root $OUTPUT_DIR
        if [[ $STRIP == "20r1" ]]; then continue; fi
        python "$SCRIPT" "$STRIP" "$MAG" "K" "$K_CUTS" "P" "ETA" "" --binSchemeFile="AltBinning.py" --schemeName="AltBinning"
        mv PerfHists_K_Strip${STRIP}_${MAG}_AltBinning_P_ETA.root $OUTPUT_DIR
    done
done

# Run 2
# for STRIP in "Turbo15" "Turbo16"; do
for STRIP in "20r1"; do
    for MAG in MagUp MagDown; do
        python "$SCRIPT" "$STRIP" "$MAG" "K" "$K_CUTS" "P" "ETA" "" --binSchemeFile="AltBinning.py" --schemeName="AltBinning"
        mv PerfHists_K_${STRIP}_${MAG}_AltBinning_Brunel_P_Brunel_ETA.root $OUTPUT_DIR
        python "$SCRIPT" "$STRIP" "$MAG" "Pi" "$PI_CUTS" "Brunel_P" "Brunel_ETA" "" --binSchemeFile="AltBinning.py" --schemeName="AltBinning"
        mv PerfHists_Pi_${STRIP}_${MAG}_AltBinning_Brunel_P_Brunel_ETA.root $OUTPUT_DIR
    done
done
