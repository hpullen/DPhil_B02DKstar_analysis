for VAR in $(cat ../../../BDT/VariableLists/Kpi_run2.txt); do
    root -b -q "compare_data_MC.C(\"$VAR\")"
done
