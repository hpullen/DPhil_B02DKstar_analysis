1. DaVinci scripts

2. Initial nTuple production/reduction (trigger cuts and K*0 helicity cuts):
   Merge_ganga_tuples/ 
   ./makeTuple.sh <YEAR> <MODE> <data/MC> <twoBody/fourBody/backgroundType> <MAG> <TREENAME>
   
3. Make training tuples for BDT:
   Make_tuples
   ./MakeTrainingTuple <data/MC> <YEAR> <MAG> <twoBody/fourBody> <D0mode>

4. Train BDT:
   ./TrainMVA <MODE> -s <SIGNAL_FILES> -b <BACKGROUND_FILES>
   Apply BDT: (note: also calculates some useful parameters)
   ./ApplyMVA <MODE> <INPUT_FILE> <OUTPUT_FILE>

5. Apply full selection
   Make_tuples (selections described in Selections/common.param and mode.param)
   For data:
   ./MakeSelectedTuple <YEAR> <MAG> <MODE>
   For MC:
   ./MakeSelectedMCTuple <BGTYPE> <YEAR> <MAG>

6. Get MC shapes
   Fit_monte_carlo/Code
   ./FitData <MODE>
   ./FitLowMass
   ./FitRho
   ./FitDKpipi (mention origin?)
   Plots and Results

7. Calculate efficiencies

8. Calculate background constraints

9. Perform fit to data (note special option e.g. split, share_GLW)
   (Note: some external inputs saved in Parameters)

10. Run toy studies to get fitter bias and plot biases, get large biases

11. Systematics studies (write a script to run all?)
