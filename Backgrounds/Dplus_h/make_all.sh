#!/usr/bin/env bash
for MODE in Kpi piK KK pipi; do
    root -b -q "make_plots.C(\"$MODE\")"
    root -b -q "make_plots.C(\"$MODE\", true)"
    cp -rf Plots/$MODE/ /home/pullen/analysis/B02DKstar/ANA_resources/Plots/Backgrounds/other/Dplus_h/
done
