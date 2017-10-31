#ifndef PLOTTER_H
#define PLOTTER_H

#include <string>

class Plotter {

public: 
    Plotter(std::string mode, std::string type);
    ~Plotter();

    void plotFit();
    void plotFit(std::string comp1, std::string comp2);

private:
    std::string m_mode;
    std::string m_type;
    std::string m_path;

};

#endif // PLOTTER_H
