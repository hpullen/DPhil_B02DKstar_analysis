#ifndef STARTING_PARAM_READER_H
#define STARTING_PARAM_READER_H

#include <map>
#include <string>

#include "StartingParam.hpp"

// =============================================
// Class to read starting parameters from a file
// =============================================

class StartingParamReader {
public:
    StartingParamReader();
    StartingParamReader(std::string filename);
    ~StartingParamReader();

    void readParams(std::string filename);
    StartingParam getParam(std::string name);

private:
    std::map<std::string, StartingParam> m_params;
    bool m_params_read;
};

#endif // STARTING_PARAM_READER_H
