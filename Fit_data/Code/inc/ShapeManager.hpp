#ifndef SHAPE_MANAGER_H
#define SHAPE_MANAGER_H

#include <map>
#include <string>

class RooAbsPdf;
class ParameterManager;

// ============================================
// Class for storing and managing RooFit shapes
// ============================================
class ShapeManager {

public:
    ShapeManager(std::string name);
    ~ShapeManager();

    RooAbsPdf * getShape(std::string name);
    RooAbsPdf * operator[](std::string name);

    void addGaussian(std::string name, ParameterManager * params, std::string mean,
            std::string width);
    void addHorns(std::string name, ParameterManager * params, std::string x, 
            std::string a, std::string b, std::string csi, std::string shift, 
            std::string sigma, std::string ratio, std::string f);
    void addHill(std::string name, ParameterManager * params, std::string x, 
            std::string a, std::string b, std::string csi, std::string shift, 
            std::string sigma, std::string ratio, std::string f);
    void addLittleHorns(std::string name, ParameterManager * params, std::string x, 
            std::string a, std::string b, std::string csi, std::string shift, 
            std::string sigma, std::string ratio, std::string f, std::string shiftg);
    void addCrystalBall(std::string name, ParameterManager * params, std::string x,
            std::string mean, std::string sigma, std::string alpha, std::string n);
    void addExponential(std::string name, ParameterManager * params, std::string x,
            std::string slope);

    void addAddPdf(std::string name, ParameterManager * yields, 
            ShapeManager * shapes, 
            std::map<std::string, std::string> shapes_yields);

    void reset();

private:
    std::string m_name;
    std::map<std::string, RooAbsPdf *> m_shapes;

    void checkForDuplicates(std::string);
};


#endif // SHAPE_MANAGER_H
