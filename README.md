# B0 -> D0 K\*0 analysis using Run 1 + Run 2 data


This repository contains code for performing the analysis of B0 -> D0 K\*0 decays, with D0 -> two body modes (Kπ, πK, KK, ππ) and four body modes (Kπππ, πKππ, ππππ).


## 1. Location of data files

The ROOT files for the analysis are stored on the Oxford data disc:
```
/data/lhcb/users/pullen/B02DKstar/
```

## 3. DaVinci processing

### 3.1 Running DaVinci jobs on data

DaVinci selection code for data can be found at:

[DaVinci_scripts/data/](https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/data)

DaVinci options are configured for two body modes in the files:

[twoBody/2011_Bd_DKstar.py](https://github.com/hpullen/B02DKstar/blob/master/DaVinci_scripts/data/twoBody/2011_Bd_DKstar.py)
[twoBody/2012_Bd_DKstar.py](https://github.com/hpullen/B02DKstar/blob/master/DaVinci_scripts/data/twoBody/2012_Bd_DKstar.py)
[twoBody/2015_Bd_DKstar.py](https://github.com/hpullen/B02DKstar/blob/master/DaVinci_scripts/data/twoBody/2015_Bd_DKstar.py)
[twoBody/2016_Bd_DKstar.py](https://github.com/hpullen/B02DKstar/blob/master/DaVinci_scripts/data/twoBody/2016_Bd_DKstar.py)

The Run 1 and Run 2 files have different trigger options due to changes in trigger names. Files for separate years are needed in order to get the correct CondDB tags for that year.

Ganga options for each year and magnetic polarity can be found in the same directory, for example:

[job_2011_down.py] (https://github.com/hpullen/B02DKstar/blob/master/DaVinci_scripts/data/twoBody/job_2011_down.py)

is the options file for 2011 data, magnet down. These files contain BKQueries pointing to the required year and polarity.

DaVinci and Ganga options for the four body modes are found in the corresponding files in the fourBody directory. In this case, the Run 1 files use a different stripping line to the Run 2 files, due to the desired line not being present in the latest Run 1 stripping.

### 3.2 Running DaVinci jobs on Monte Carlo

DaVinci scripts for running on Monte Carlo are found under:

[DaVinci_scripts/MC/] (https://github.com/hpullen/B02DKstar/tree/master/DaVinci_scripts/MC)

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
