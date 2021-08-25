# B<sup>0</sup> -> D<sup>0</sup> K<sup>\*0</sup> analysis using Run 1 + Run 2 data


This repository contains code for performing the analysis of B<sup>0</sup> -> D<sup>0</sup> K<sup>\*0</sup> decays, with D<sup>0</sup> -> two body modes (Kπ, πK, KK, ππ) and four body modes (Kπππ, πKππ, ππππ).


## 1. Location of data files

The ROOT files for the analysis are stored on the Oxford data disc:
```
/data/lhcb/users/pullen/B02DKstar/
```

## 2. DaVinci processing

### 2.1 Running DaVinci jobs on data

DaVinci selection code for data can be found at:

[DaVinci_scripts/data/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/data)

DaVinci options are configured for two body modes in the files:

[twoBody/2011_Bd_DKstar.py](https://github.com/hpullen/B02DKstar/blob/master/DaVinci_scripts/data/twoBody/2011_Bd_DKstar.py)
[twoBody/2012_Bd_DKstar.py](https://github.com/hpullen/B02DKstar/blob/master/DaVinci_scripts/data/twoBody/2012_Bd_DKstar.py)
[twoBody/2015_Bd_DKstar.py](https://github.com/hpullen/B02DKstar/blob/master/DaVinci_scripts/data/twoBody/2015_Bd_DKstar.py)
[twoBody/2016_Bd_DKstar.py](https://github.com/hpullen/B02DKstar/blob/master/DaVinci_scripts/data/twoBody/2016_Bd_DKstar.py)

The Run 1 and Run 2 files have different trigger options due to changes in trigger names. Files for separate years are needed in order to get the correct CondDB tags for that year.

Ganga options for each year and magnetic polarity can be found in the same directory, for example:

[job_2011_down.py](https://github.com/hpullen/B02DKstar/blob/master/DaVinci_scripts/data/twoBody/job_2011_down.py)

is the options file for 2011 data, magnet down. These files contain BKQueries pointing to the required year and polarity.

DaVinci and Ganga options for the four body modes are found in the corresponding files in the fourBody directory. In this case, the Run 1 files use a different stripping line to the Run 2 files, due to the desired line not being present in the latest Run 1 stripping.

### 2.2 Running DaVinci jobs on Monte Carlo

DaVinci scripts for running on Monte Carlo are found under:

[DaVinci_scripts/MC/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC)

Scripts and Ganga options for running over two- and four-body signal modes are found in:

[/twoBody/Kpi/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC/twoBody/Kpi)
[/twoBody/KK/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC/twoBody/KK)
[/twoBody/pipi/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC/twoBody/pipi)
[/fourBody/Kpipipi/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC/fourBody/Kpipipi)
[/fourBody/pipipipi/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC/fourBody/pipipipi)

The Monte Carlo used includes 2012 Sim08a and 2016 Sim09b samples. The two-body modes also include 2015 Sim09b.

Additionally, DaVinci scripts for various background modes are found under:

[/backgrounds/Bs/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC/backgrounds/Bs): B0_s -> (D0 -> Kπ) K\*0
[/backgrounds/Bs_lowMass/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC/backgrounds/Bs_lowMass): B0_s -> D\*0 K\*0
[/backgrounds/Lambda_b/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC/backgrounds/Lambda_b): Λ\_b
[/backgrounds/lowMass/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC/backgrounds/lowMass): B0 -> D\*0 K\*0
[/backgrounds/rho/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC/backgrounds/rho): B0 -> (D0 -> Kπ) ρ0

### 2.3 Format of output nTuples

For each subjob, Ganga downloads a .root file. The two-body files each contain four nTuples:
```
Tuple_Kpi
Tuple_piK
Tuple_KK
Tuple_pipi
```
These correspond to each of the four D0 decay modes: the ADS modes, Kπ (favoured) and πK (suppressed), and the GLW modes, KK and ππ.


The four-body files for Run 2 each contain three nTuples:
```
Tuple_Kpipipi
Tuple_piKpipi
Tuple_pipipipi
```
The Run 1 version of these files only contain the first two nTuples, since there is no stripping line to select D0 -> ππππ in Run 1.

### 2.4 Merging Ganga subjobs

After a Ganga is complete, it is manually moved to:
```
/data/lhcb/users/pullen/gangadir/job_output/
```
and given a descriptive name. The ROOT files for each subjob within the job can then be merged using a bash script:

[/Merge_ganga_tuples/merge.sh](https://github.com/hpullen/B02DKstar/blob/master/Merge_ganga_tuples/merge.sh)

The result is saved into a single file for each job called Total.root.

The ROOT files can also be merged using a compiled C++ script:
[/Merge_ganga_tuples/src/makeTuple.sh](https://github.com/hpullen/B02DKstar/blob/master/Merge_ganga_tuples/src/makeTuple.sh) 

This applies some basic cuts to reduce the nTuple size (especially important for data): trigger cuts, K\*0 mass requirement and helicity cut. All irrelevant branches are turned off if the script is run for a data sample.

### 3. BDT training and application

### 3.1 Sample preparation

Training samples have cuts applied to D0 mass, K\*0 mass, and K\*0 helicity angle. 
Signal samples come from Monte Carlo, and have the additional requirement of background category 0.
Background samples come from data, and have the additional requirement of B0 mass > 5800 MeV.
These are made using a bash script.

### 3.2 Training using TMVA

Variables to use in the BDT are listed in a text file.
A separate BDT is trained for each of Kpi, KK, pipi, Kpipipi, pipipipi. The outputs are saved to weights.
Output ROOT files containing plots and histograms for the BDT are saved to ../Outputs.

### 3.3 Applying BDT weights to nTuples

Code to apply the weights to a data or MC sample can be found here. 
This outputs a file called (mode)\_withBDTG.root for each mode.
We end up with four files per original Total.root file for two-body, and three for four-body.


## 4. Producing final nTuples

Variables calculated: helicity angle, double mis-ID swapped mass (for ADS modes), D0
flight distance significance. An nTuple with a K\*0 mass and helicity angle cut, as well as a BDTG > 0.2 sanity cut to reduce size, is saved.
The full BDT cut, D0 FDS cut, double mis-ID cut, and PID cuts will be applied at the fitting stage.
This is done for both signal and Monte Carlo.
This saves ROOT files called (mode)\_withVars\_withCuts.root.

## 5. Fitting to Monte Carlo

This uses the final nTuples produced in the previous section.
There is a script for fitting to a signal peak with a double Crystal Ball.
Will add a script for Rho0 double Crystal Ball shape, and low mass shapes.
Plots are saved to Plots.
The parameters are saved to text files in Results.

## 6. Fitting to data
Fits to selected data are performed in the Fit_data directory with:
```
./FitData --split (--splitObs)
```
where the --splitObs option tells the fit to measure the physics observables separately for Run 1 and Run 2.

Toy studies are performed using:
```
./FitterBias <id> --split (--splitObs)
```
where the id variable assigns a number to the file that the results are saved to. This script runs 10 toys each time and saves them to a single file.

Systematics studies are performed using:
```
./Systematics <id> <source> (--splitObs)
```

where "source" refers to the source of uncertainty to be studied.
