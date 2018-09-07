// ==============================================================
// Macro to extract mean efficiency and uncertainty from a sample
// ==============================================================
void get_efficiency(TString sample) {

    // Open sample file
    TFile * file = TFile::Open(sample, "READ");
    TTree * tree = (TTree*)file->Get("CalibTool_PIDCalibTree");

    // Set branch addresses
    float eff;
    float weight;
    tree->SetBranchAddress("Event_PIDCalibEff", &eff);
    tree->SetBranchAddress("Event_PIDCalibEffWeight", &weight);

    // Get weighted mean
    double sum = 0;
    double weight_sum = 0;
    for (unsigned int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        sum += eff * weight;
        weight_sum += weight;
    }
    double mean = sum / weight_sum;

    // Calculate variance
    double square_sum = 0;
    double n = tree->GetEntries();
    for (unsigned int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        square_sum += weight * weight * (eff - mean) * (eff - mean);
    }
    double var = n * square_sum / ((n - 1) * weight_sum * weight_sum);

    // Return mean
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Uncertainty: " << sqrt(var) << std::endl;

}
