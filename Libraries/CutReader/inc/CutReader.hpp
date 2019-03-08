#ifndef CUT_READER_HPP
#define CUT_READER_HPP

#include <string>
#include <vector>

#include "TCut.h"

// ====================================
// Class for reading in cuts for a mode
// ====================================
class CutReader {

public:
    CutReader();
    CutReader(std::string mode);
    CutReader(std::string mode, std::string dir);
    ~CutReader();

    TCut GetCut();
    TCut GetCut(std::string search_term);
    TCut GetCut(std::vector<std::string> search_terms);
    TCut GetCutExcept(std::string search_term);
    TCut GetCutExcept(std::vector<std::string> search_term);

    void SetMode();
    void SetMode(std::string mode);
    void Print();

private:
    std::string m_mode;
    std::string m_dir;

    std::vector<TCut> ReadCommonCut();
    std::vector<TCut> ReadModeCut(std::string mode);

    std::vector<TCut> m_common_cut;
    std::vector<TCut> m_mode_cut;

};

#endif // CUT_READER_HPP
