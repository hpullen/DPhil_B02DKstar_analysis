for mag in "up" "down"; do
    for particle in "gamma" "pi"; do
        for helicity in "100" "010" "001"; do
            ./../../../MakeSelectedTuple /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/lowMass/$particle/$helicity 2016 $mag
            ./../../../MakeSelectedTuple /data/lhcb/users/pullen/B02DKstar/MC/backgrounds/Bs_lowMass/$particle/$helicity 2016 $mag
        done
    done
done
