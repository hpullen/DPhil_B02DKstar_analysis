#!/usr/bin/env bash
INPUT_STRING="-i"
INPUT_STRING="$INPUT_STRING Kpi ../Selection/Results/selection_efficiency_lambda_K.txt"
./AverageEfficiency ../Values/selection_efficiency_lambda_K.param $INPUT_STRING 
