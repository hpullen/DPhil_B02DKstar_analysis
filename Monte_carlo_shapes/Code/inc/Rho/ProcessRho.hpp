#ifndef PROCESS_RHO_H
#define PROCESS_RHO_H

#include <vector>
#include <map>

// =======================================================
// Class for applying selection/fitting to all backgrounds
// =======================================================

class ProcessRho {
public:
    ProcessRho();
    ~ProcessRho();

    void processAll();

private:
    std::map<std::string, std::map<std::string, double>> m_params;
    std::map<std::string, int> m_events;

    void addBackgrounds();
    
    void makeNeventsFile();
    void makeParamsFiles();

};
#endif // PROCESS_RHO_H
