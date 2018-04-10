#include <iostream>
#include <fstream>
#include <map>

#include "CutReader.hpp"


// ===========
// Constructor
// ===========
CutReader::CutReader() :
    m_mode(""),
    m_common_cut(ReadCommonCut()) {}
CutReader::CutReader(std::string mode) : 
    m_mode(mode),
    m_common_cut(ReadCommonCut()),
    m_mode_cut(ReadModeCut(mode)) {}


// ==========
// Destructor
// ==========
CutReader::~CutReader() {}


// =============
// Get full TCut
// =============
TCut CutReader::GetCut() {
    TCut full_cut = "";
    for (auto cut : m_common_cut) {
        full_cut += cut;
    }
    for (auto cut : m_mode_cut) {
        full_cut += cut;
    }
    return full_cut;
}


// =============================
// Get cut for a single variable
// =============================
TCut CutReader::GetCut(std::string search_term) {
    std::vector<std::string> search_terms;
    search_terms.push_back(search_term);
    return GetCut(search_terms);
}


// ==============================
// Get cut for multiple variables
// ==============================
TCut CutReader::GetCut(std::vector<std::string> search_terms) {
    std::map<std::string, TCut> cuts;
    for (std::vector<TCut> cut_vec : {m_common_cut, m_mode_cut}) {
        for (auto cut : cut_vec) {
            std::string cut_str = std::string(cut);
            for (auto term : search_terms) {
                if (cut_str.find(term) != std::string::npos) {
                    cuts[term] = cut;
                }
            }
        }
    }
    for (auto term : search_terms) {
        if (cuts.find(term) == cuts.end()) {
            std::cout << "Warning: no cut found for " << term << std::endl;
        }
    }
    TCut full_cut = "";
    for (auto cut : cuts) {
        full_cut += cut.second;
    }
    return full_cut;
}


// ==============================
// Get cut excluding one variable
// ==============================
TCut CutReader::GetCutExcept(std::string search_term) {
    std::vector<std::string> search_terms;
    search_terms.push_back(search_term);
    return GetCutExcept(search_terms);
}


// ===================================
// Get cut excluding certain variables
// ===================================
TCut CutReader::GetCutExcept(std::vector<std::string> search_terms) {
    TCut full_cut = "";
    for (std::vector<TCut> cut_vec : {m_common_cut, m_mode_cut}) {
        for (auto cut : cut_vec) {
            std::string cut_str = std::string(cut);
            for (auto term : search_terms) {
                if (cut_str.find(term) == std::string::npos) {
                    full_cut += cut;
                }
            }
        }
    }
    return full_cut;
}


// ==================
// Set mode to common
// ==================
void CutReader::SetMode() {
    m_mode = "";
    m_mode_cut.clear();
}


// ===========
// Change mode
// ===========
void CutReader::SetMode(std::string mode) {
    m_mode = mode;
    m_mode_cut = ReadModeCut(mode);
}


// =============
// Print the cut
// =============
void CutReader::Print() {
    std::cout << "CutReader: CUTS FOR MODE " << m_mode << std::endl;
    std::cout << "CutReader:    Common cuts:" << std::endl;
    for (auto cut : m_common_cut) {
        std::cout << "CutReader:        " << cut << std::endl;
    }
    std::cout << "CutReader:    Mode-specific cuts:" << std::endl;
    for (auto cut : m_mode_cut) {
        std::cout << "CutReader:        " << cut << std::endl;
    }
}


// ==========================
// Read common cuts from file
// ==========================
std::vector<TCut> CutReader::ReadCommonCut() {
    std::vector<TCut> cuts;
    std::ifstream file("/home/pullen/analysis/B02DKstar/Selection/common.cut");
    std::string line;
    while (std::getline(file, line)) {
        cuts.push_back(line.c_str());
    }
    return cuts;
}


// =============================
// Read cuts for a specific mode
// =============================
std::vector<TCut> CutReader::ReadModeCut(std::string mode) {
    std::vector<TCut> cuts;
    std::string cut_mode = mode;
    if (mode == "piK") cut_mode = "Kpi";
    else if (mode == "piKpipi") cut_mode = "piKpipi";
    std::ifstream file("/home/pullen/analysis/B02DKstar/Selection/" + cut_mode + ".cut");
    std::string line;
    while (std::getline(file, line)) {
        cuts.push_back(line.c_str());
    }
    return cuts;
}


