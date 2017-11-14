rule all:
    input: expand("/data/lhcb/users/pullen/B02DKstar/data/twoBody/{year}_{mag}/"
                  "{mode}.root", year=["2011", "2012", "2015", "2016"], mag=["up", 
                  "down"], mode=["Kpi", "piK", "KK", "pipi"]), 
           expand("/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
                  "{mode}.root", year=["2011", "2012", "2015", "2016"], mag=["up", 
                  "down"], mode=["Kpipipi", "piKpipi"]), 
           expand("/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year}_{mag}/"
                  "pipipipi.root", year=["2015", "2016"], mag=["up", "down"])


####################################################
# Merge ganga job outputs into single, smaller tuple
####################################################
rule merge_Kpi_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/twoBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/twoBody/{year_input}_{mag_input}/"
        "Kpi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} Kpi data twoBody {wildcards.mag_input} "
        "Tuple_Kpi/DecayTree"

rule merge_piK_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/twoBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/twoBody/{year_input}_{mag_input}/"
        "piK.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} piK data twoBody {wildcards.mag_input} "
        "Tuple_piK/DecayTree"

rule merge_KK_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/twoBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/twoBody/{year_input}_{mag_input}/"
        "KK.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} KK data twoBody {wildcards.mag_input} "
        "Tuple_KK/DecayTree"

rule merge_pipi_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/twoBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/twoBody/{year_input}_{mag_input}/"
        "pipi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} pipi data twoBody {wildcards.mag_input} "
        "Tuple_pipi/DecayTree"

rule merge_Kpipipi_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/fourBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year_input}_{mag_input}/"
        "Kpipipi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} Kpipipi data fourBody {wildcards.mag_input} "
        "Tuple_Kpipipi/DecayTree"

rule merge_piKpipi_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/fourBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year_input}_{mag_input}/"
        "piKpipi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} piKpipi data fourBody {wildcards.mag_input} "
        "Tuple_piKpipi/DecayTree"

rule merge_pipipipi_data:
    input:
        "/data/lhcb/users/pullen/gangadir/job_output/data/fourBody/{year_input}_"
        "{mag_input}/",
        "/home/pullen/analysis/B02DKstar/Merge_ganga_tuples/MakeTuple"
    output:
        "/data/lhcb/users/pullen/B02DKstar/data/fourBody/{year_input}_{mag_input}/"
        "pipipipi.root"
    shell:
        "cd /home/pullen/analysis/B02DKstar/Merge_ganga_tuples/; ./makeTuple.sh "
        "{wildcards.year_input} pipipipi data fourBody {wildcards.mag_input} "
        "Tuple_pipipipi/DecayTree"
