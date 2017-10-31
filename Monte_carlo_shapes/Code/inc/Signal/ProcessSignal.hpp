#ifndef PROCESS_SIGNAL_H
#define PROCESS_SIGNAL_H

#include <vector>

#include "Signal/SignalData.hpp"

// =======================================================
// Class for applying selection/fitting to all backgrounds
// =======================================================

class ProcessSignal {
public:
    ProcessSignal();
    ~ProcessSignal();

    void processAll();
    void processSingle(std::string name, std::string filepath, 
            std::string treename, BgType bgType, FitShape shape);

private:
    std::map<std::string, std::map<std::string, double>> m_params;
    std::map<std::string, int> m_events;

    void addBackgrounds();
    void addBackground(std::string name, std::vector<std::string> filepaths,
            std::string treename, BgType bgType, FitShape shape);
    
    void makeNeventsFile();
    void makeParamsFiles();

};
#endif // PROCESS_SIGNAL_H
