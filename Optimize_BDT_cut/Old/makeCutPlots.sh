#/usr/bin/bash

# B0 ratios
./OptimizeBDTcut Kpi piK R_plus_blind
./OptimizeBDTcut Kpi piK R_minus_blind
./OptimizeBDTcut Kpi KK R_KK_vs_Kpi_blind
./OptimizeBDTcut Kpi pipi R_pipi_vs_Kpi_blind

# B0 asymmetries
./OptimizeBDTcut Kpi KK A_KK_blind
./OptimizeBDTcut Kpi pipi A_pipi_blind

# Bs ratios
./OptimizeBDTcut piK KK R_KK_vs_piK_Bs
./OptimizeBDTcut piK pipi R_pipi_vs_piK_Bs
