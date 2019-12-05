#ifndef PLOTTER_H
#define PLOTTER_H

#include <string>

class Plotter {

public: 
    Plotter();
    Plotter(std::string mode, std::string type);
    ~Plotter();

    void plotFit();
    void plotFit(std::string comp1, std::string comp2);
    void plotFit(std::string comp1, std::string comp2, std::string comp3);
    void plotFit(std::vector<std::string> components);
    void plotLowMassFits();

private:
    std::string m_mode;
    std::string m_type;
    std::string m_path;

    void plotSingleLowMassComponent(std::string comp);

};

#endif // PLOTTER_H
