# Get final PID efficiency with uncertainty
for FLAV in "" "_B0" "_B0bar"; do
    for RUN in "run1" "run2"; do
        OUTFILE="../Values/PID_efficiency${FLAV}_${RUN}_final.param"
        INFILE="../Values/PID_efficiency${FLAV}_${RUN}.param"
        ALTFILE="../Values/PID_efficiency${FLAV}_${RUN}_alt.param"
        if [[ -f $OUTFILE ]]; then rm $OUTFILE; fi
        for MODE in "Kpi" "KK" "pipi" "Kpipipi" "pipipipi"; do
            VAL=$(awk "/^$MODE\ /{print \$2}" $INFILE)
            ALT_VAL=$(awk "/^$MODE\ /{print \$2}" $ALTFILE)
            ERR=$(bc -l <<< "$VAL - $ALT_VAL" | sed 's/-/0/')
            echo "$MODE $VAL $ERR" >> $OUTFILE
        done
    done
done
