#!/usr/bin/env bash
for MODE in Kpi piK KK pipi; do
    root -b -q "make_plots.C(\"$MODE\")"
    root -b -q "make_plots.C(\"$MODE\", true)"
done
