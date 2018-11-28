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
    double sq_weight_sum = 0;
    for (unsigned int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        sum += eff * weight;
        weight_sum += weight;
        sq_weight_sum += weight * weight;
    }
    double mean = sum / weight_sum;

    // Calculate variance
    double diff_sum = 0;
    double n = tree->GetEntries();
    for (unsigned int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        diff_sum += weight * (eff - mean) * (eff - mean);
    }
    double var = sq_weight_sum / (weight_sum * weight_sum) 
        * weight_sum / (weight_sum * weight_sum - sq_weight_sum)
        * diff_sum;

    // Return mean
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Uncertainty: " << sqrt(var) << std::endl;

}
