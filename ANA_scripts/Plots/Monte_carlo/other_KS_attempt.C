
                    // tree_data->Draw(">>elist_data_" + var, "abs(Bd_ConsD_M - 5279.81) < 25");
                    // tree_MC->Draw(">>elist_MC_" + var);
                    // // Kolmogorov Smirnov test: make samples
                    // // Name of variable
                    // bool is_log = false;
                    // bool is_acos = false;
                    // std::string var_str = std::string(var);
                    // if (var_str.find("acos") == 0) {
                        // is_acos = true;
                        // var_str = var_str.substr(5, std::string::npos);
                    // } else if (var_str.find("log10") == 0) {
                        // is_log = true;
                        // var_str = var_str.substr(6, std::string::npos);
                    // }
                    // std::cout << "Variable string: " << var_str << std::endl;

                    // // Data sample
                    // Double_t * sample1;
                    // TEventList * elist_data = (TEventList*)gDirectory->Get("elist_data_" + var);
                    // // int count1 = elist_data->GetN();
                    // // double val_data;
                    // // tree_data->SetBranchAddress(var, &val_data);
                    // // for (int i = 0; i < count1; i++) {
                        // // tree_data->GetEntry(elist_data->GetEntry(i));
                        // // sample1[i] = val_data;
                    // // }

                    // // // MC sample
                    // // Double_t * sample2;
                    // // TEventList * elist_MC = (TEventList*)gDirectory->Get("elist_MC_" + var);
                    // // int count2 = elist_MC->GetN();
                    // // double val_MC;
                    // // tree_MC->SetBranchAddress(var, &val_MC);
                    // // for (int i = 0; i < count2; i++) {
                        // // tree_MC->GetEntry(elist_MC->GetEntry(i));
                        // // sample2[i] = val_MC;
                    // // }

                    // ROOT::Math::GoFTest * gof_test = new ROOT::Math::GoFTest(count1, sample1, count2, sample2);
                    // double ks = gof_test->KolmogorovSmirnov2SamplesTest("t");
