#ifndef RUN_ONE_TOY_MAKER_HPP
#define RUN_ONE_TOY_MAKER_HPP

#include "DataPdfMaker.hpp"

// =============================================
// Class for making a toy with yields from Run 1
// =============================================
class RunOneToyMaker : public DataPdfMaker {

public:
    RunOneToyMaker(RooRealVar * x, RooCategory * cat, std::string input_file);
    RunOneToyMaker(std::string name, RooRealVar * x, RooCategory * cat, 
            std::string input_file);
    ~RunOneToyMaker();

private:
    void SetFloatingParameters();
    std::string m_inputfile;
};

#endif // RUN_ONE_TOY_MAKER_HPP
