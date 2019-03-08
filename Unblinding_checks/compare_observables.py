from numpy.random import multivariate_normal
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import sys


def generate_yields(name, old_val, old_err, new_val, new_err, corr):
    mean = [old_val, new_val]
    cov = [[old_err**2, corr*old_err*new_err], [corr*old_err*new_err, new_err**2]]
    old, new = multivariate_normal(mean, cov, 10000).T
    plt.plot(old, new, 'x')
    plt.xlabel('Old yield')
    plt.ylabel('New yield')
    plt.axis('equal')
    plt.savefig('Plots/Yields/{}_yields.png'.format(name))
    plt.clf()
    return old, new


def make_plots(name, old, new, directory, sigma_diff_val):
    diff = new - old
    sigma_diff = (new - old)/np.sqrt(np.var(new))
    n_bins = 50

    # Plot histograms of each value
    plt.hist(old, n_bins, alpha=0.5, facecolor='blue', label='Old analysis')
    plt.hist(new, n_bins, alpha=0.5, facecolor='red', label = 'New analysis')
    print '\n{}: \nOld: {} +/- {}\nNew: {} +/- {}'.format(name, np.mean(old), np.sqrt(np.var(old)),
            np.mean(new), np.sqrt(np.var(new)))
    plt.legend()
    plt.xlabel(name)
    plt.ylabel('Number of toys')
    plt.savefig('Plots/{}/{}.png'.format(directory, name))
    plt.clf()
    
    # Plot values against each other
    plt.plot(old, new, 'x')
    plt.xlabel('Old {}'.format(name))
    plt.ylabel('New {}'.format(name))
    plt.savefig('Plots/{}/{}_old_vs_new.png'.format(directory, name))
    plt.clf()

    # Plot difference 
    plt.hist(diff, n_bins)
    plt.xlabel('new {} - old {}'.format(name, name))
    plt.ylabel('Number of toys')
    plt.savefig('Plots/{}/{}_diff.png'.format(directory, name))
    plt.clf()
    
    # Plot sigma difference
    plt.hist(sigma_diff, n_bins)
    plt.xlabel('(new {} - old {})/new uncertainty'.format(name, name))
    plt.ylabel('Number of toys')
    plt.axvline(sigma_diff_val, color='r', linestyle='dashed', linewidth=1)
    plt.axvline(-sigma_diff_val, color='r', linestyle='dashed', linewidth=1)
    plt.savefig('Plots/{}/{}_sigma_diff.png'.format(directory, name))
    plt.clf()

def get_probability(old, new, sigma_diff):
    diff = (new - old)/np.sqrt(np.var(new))
    gt_diff = [d for d in diff if np.absolute(d) > sigma_diff]
    n_total = len(diff) * 1.0
    n_gt = len(gt_diff) * 1.0
    print "Probability of deviation > {}: {}".format(sigma_diff, n_gt/n_total)

def ratio_study(name, old_top, old_bottom, new_top, new_bottom, sigma_diff, directory):
    old_ratio = old_top/old_bottom
    new_ratio = new_top/new_bottom
    make_plots(name, old_ratio, new_ratio, directory, sigma_diff)
    get_probability(old_ratio, new_ratio, sigma_diff)

def asym_study(name, old, old_bar, new, new_bar, sigma_diff, directory):
    old_asym = (old_bar - old)/(old_bar + old)
    new_asym = (new_bar - new)/(new_bar + new)
    make_plots(name, old_asym, new_asym, directory, sigma_diff)
    get_probability(old_asym, new_asym, sigma_diff)


def get_diff(old, new, sigma):
    return np.absolute((new - old)/sigma)

def get_Nbar_from_asym(N, asym):
    return N * (1 + asym)/(1 - asym)

def get_Nbar_from_ratio(N_sup, N_fav, N_fav_bar, ratio):
    return (N_fav + N_fav_bar)*ratio - N_sup

# Input args
dirname = ""
Kpi_corr = 0.74
KK_corr = 0.83
pipi_corr = 0.85
if len(sys.argv) > 1:
    if sys.argv[1] == "--no_corr":
        Kpi_corr, KK_corr, pipi_corr = (0, 0, 0)
        dirname = "No_corr"
    elif sys.argv[1] == "--high_corr":
        Kpi_corr, KK_corr, pipi_corr = (1, 1, 1)
        dirname = "High_corr"
    elif sys.argv[1] == "--mid_corr":
        Kpi_corr, KK_corr, pipi_corr = (0.5, 0.5, 0.5)
        dirname = "Mid_corr"
    else:
        print 'Unrecognised option: {}'.format(sys.argv[1])

# R_minus
R_minus_central = 0.056
R_minus_new = 0.1189
R_minus_err = 0.0344
R_minus_diff = get_diff(R_minus_central, R_minus_new, R_minus_err)
N_Kpi_bar_old = 370
N_Kpi_bar_new = 321
Kpi_bar_old, Kpi_bar_new = generate_yields('Kpi_B0bar', N_Kpi_bar_old, 22, N_Kpi_bar_new, 16, Kpi_corr)
piK_bar_old, piK_bar_new = generate_yields('piK_B0bar', N_Kpi_bar_old * R_minus_central, 12, N_Kpi_bar_new * R_minus_central, 11, Kpi_corr)
ratio_study('R_minus', piK_bar_old, Kpi_bar_old, piK_bar_new, Kpi_bar_new, R_minus_diff, dirname)

# R_plus
R_plus_central = 0.057
R_plus_new = 0.0455
R_plus_err = 0.0319
R_plus_diff = get_diff(R_plus_central, R_plus_new, R_plus_err)
N_Kpi_old = 405
N_Kpi_new = 306
Kpi_old, Kpi_new = generate_yields('Kpi_B0', N_Kpi_old, 23, N_Kpi_new, 15, Kpi_corr)
piK_old, piK_new = generate_yields('piK_B0', N_Kpi_old * R_plus_central, 12, N_Kpi_new * R_plus_central, 10, Kpi_corr)
ratio_study('R_plus', piK_old, Kpi_old, piK_new, Kpi_new, R_plus_diff, dirname)

# A_KK
A_KK_central = -0.198
A_KK_new = -0.1915
A_KK_err = 0.157
A_KK_diff = get_diff(A_KK_central, A_KK_new, A_KK_err)
N_KK_old = 53
N_KK_new = 38
KK_old, KK_new = generate_yields('KK_B0', N_KK_old, 10, N_KK_new, 8, KK_corr)
N_KK_bar_old = get_Nbar_from_asym(N_KK_old, A_KK_central)
N_KK_bar_new = get_Nbar_from_asym(N_KK_new, A_KK_central)
KK_bar_old, KK_bar_new = generate_yields('KK_B0bar', N_KK_bar_old, 9, N_KK_bar_new, 6, KK_corr)
asym_study('A_KK', KK_old, KK_bar_old, KK_new, KK_bar_new, A_KK_diff, dirname)

# R_KK
R_KK_central = 1.054
R_KK_new = 0.927
R_KK_err = 0.154
R_KK_diff = get_diff(R_KK_central, R_KK_new, R_KK_err)
N_KK_bar_old_alt = get_Nbar_from_ratio(N_KK_old, N_Kpi_old, N_Kpi_bar_old, R_KK_central)
N_KK_bar_new_alt = get_Nbar_from_ratio(N_KK_new, N_Kpi_new, N_Kpi_bar_new, R_KK_central)
KK_bar_old_alt, KK_bar_new_alt = generate_yields('KK_B0bar_alt', N_KK_bar_old_alt, 9, N_KK_bar_new_alt, 6, KK_corr)
ratio_study('R_KK', KK_old + KK_bar_old_alt, Kpi_old + Kpi_bar_old,
        KK_new + KK_bar_new_alt, Kpi_new + Kpi_bar_new, R_KK_diff, dirname)

# A_pipi
A_pipi_central = -0.092
A_pipi_new = -0.06068
A_pipi_err = 0.228
A_pipi_diff = get_diff(A_pipi_central, A_pipi_new, A_pipi_err)
N_pipi_old = 21
N_pipi_new = 15
pipi_old, pipi_new = generate_yields('pipi_B0', N_pipi_old, 7, N_pipi_new, 5, pipi_corr)
N_pipi_bar_old = get_Nbar_from_asym(N_pipi_old, A_pipi_central)
N_pipi_bar_new = get_Nbar_from_asym(N_pipi_new, A_pipi_central)
pipi_bar_old, pipi_bar_new = generate_yields('pipi_B0bar', N_pipi_bar_old, 6, N_pipi_bar_new, 4, pipi_corr)
asym_study('A_pipi', pipi_old, pipi_bar_old, pipi_new, pipi_bar_new, A_pipi_diff, dirname)

# R_pipi
R_pipi_central = 1.214
R_pipi_new = 1.275
R_pipi_err = 0.237
R_pipi_diff = get_diff(R_pipi_central, R_pipi_new, R_pipi_err)
N_pipi_bar_old_alt = get_Nbar_from_ratio(N_pipi_old, N_Kpi_old, N_Kpi_bar_old, R_pipi_central)
N_pipi_bar_new_alt = get_Nbar_from_ratio(N_pipi_new, N_Kpi_new, N_Kpi_bar_new, R_pipi_central)
pipi_bar_old_alt, pipi_bar_new_alt = generate_yields('pipi_B0bar_alt', N_pipi_bar_old_alt, 6, N_pipi_bar_new_alt, 4, pipi_corr)
ratio_study('R_pipi', pipi_old + pipi_bar_old_alt, Kpi_old + Kpi_bar_old,
        pipi_new + pipi_bar_new_alt, Kpi_new + Kpi_bar_new, R_pipi_diff, dirname)

# R_ds_KK
R_ds_KK_central = 0.103
R_ds_KK_new = 0.0886
R_ds_KK_err = 0.0159
R_ds_KK_diff = get_diff(R_ds_KK_central, R_ds_KK_new, R_ds_KK_err)
N_Bs_KK_old = (N_KK_old + N_KK_bar_old)/R_ds_KK_central
N_Bs_KK_new = (N_KK_new + N_KK_bar_new)/R_ds_KK_central
Bs_KK_old, Bs_KK_new = generate_yields('Bs_KK', N_Bs_KK_old, 13, N_Bs_KK_new, 15, KK_corr)
ratio_study('R_ds_KK', KK_old + KK_bar_old, Bs_KK_old, KK_new + KK_bar_new, Bs_KK_new, R_ds_KK_diff, dirname)

# R_ds_pipi
R_ds_pipi_central = 0.147
R_ds_pipi_new = 0.1838
R_ds_pipi_err = 0.0539
R_ds_pipi_diff = get_diff(R_ds_pipi_central, R_ds_pipi_new, R_ds_pipi_err)
N_Bs_pipi_old = (N_pipi_old + N_pipi_bar_old)/R_ds_pipi_central
N_Bs_pipi_new = (N_pipi_new + N_pipi_bar_new)/R_ds_pipi_central
Bs_pipi_old, Bs_pipi_new = generate_yields('Bs_pipi', N_Bs_pipi_old, 7, N_Bs_pipi_new, 7, pipi_corr)
ratio_study('R_ds_pipi', pipi_old + pipi_bar_old, Bs_pipi_old, pipi_new + pipi_bar_new, Bs_pipi_new, R_ds_pipi_diff, dirname)
