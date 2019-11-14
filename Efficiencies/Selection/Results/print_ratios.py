from uncertainties import ufloat
from uncertainties.umath import *

# Selection efficiencies
effs = {}
effs["KK_2012_down"] = ufloat(0.010575, 0.000282)
effs["KK_2012_up"] = ufloat(0.010405, 0.000278)
effs["KK_2015_down"] = ufloat(0.011170, 0.000156)
effs["KK_2015_up"] = ufloat(0.010959, 0.000149)
effs["KK_2016_down"] = ufloat(0.011641, 0.000158)
effs["KK_2016_up"] = ufloat(0.011173, 0.000156)
effs["Kpi_2011_down"] = ufloat(0.011583, 0.000218)
effs["Kpi_2011_up"] = ufloat(0.011754, 0.000221)
effs["Kpi_2012_down"] = ufloat(0.011396, 0.000210)
effs["Kpi_2012_up"] = ufloat(0.011368, 0.000206)
effs["Kpi_2015_down"] = ufloat(0.011974, 0.000160)
effs["Kpi_2015_up"] = ufloat(0.012142, 0.000159)
effs["Kpi_2016_down"] = ufloat(0.012990, 0.000162)
effs["Kpi_2016_up"] = ufloat(0.013112, 0.000162)
effs["Kpipipi_2012_down"] = ufloat(0.003328, 0.000060)
effs["Kpipipi_2012_up"] = ufloat(0.003262, 0.000058)
effs["Kpipipi_2016_down"] = ufloat(0.004973, 0.000070)
effs["Kpipipi_2016_up"] = ufloat(0.004908, 0.000070)
effs["pipi_2012_down"] = ufloat(0.011087, 0.000284)
effs["pipi_2012_up"] = ufloat(0.011195, 0.000289)
effs["pipi_2015_down"] = ufloat(0.011954, 0.000152)
effs["pipi_2015_up"] = ufloat(0.012431, 0.000163)
effs["pipi_2016_down"] = ufloat(0.012777, 0.000162)
effs["pipi_2016_up"] = ufloat(0.012841, 0.000163)
effs["pipipipi_2016_down"] = ufloat(0.005236, 0.000073)
effs["pipipipi_2016_up"] = ufloat(0.005059, 0.000072)

# Calculate ratios
ratios = {}
for m in ("down", "up"):  

    # Two-body
    for y in ("12", "15", "16"):
        for mode in ("KK", "pipi"):
            ratios["KK_20{}_{}".format(y, m)] = effs["{}_20{}_{}".format(mode, y, m)] / effs["Kpi_20{}_{}".format(y, m)]

    # Four-body
    ratios["4pi_2016_{}".format(m)] = effs["pipipipi_2016_{}".format(m)] / effs["Kpipipi_2016_{}".format(m)]


# Print ratios
for r in ratios:
    print "Ratio {}: {}".format(r, ratios[r])


