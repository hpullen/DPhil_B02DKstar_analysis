#ifndef MANAGER_BASE_HPP
#define MANAGER_BASE_HPP

#include <string>
#include <map>

class RooAbsReal;

// ==========================================================
// Base class for storing and retrieving RooAbsReals in a map
// ==========================================================
class ManagerBase {

public:
    ManagerBase(std::string name);
    virtual ~ManagerBase() {};

    // Getter functions
    RooAbsReal * Get(std::string name);
    RooAbsReal * operator[](std::string name);
    std::string GetName();

    // Print list of items
    void Print();

    // Resetter 
    void Reset();

protected:
    // Adding function
    void AddItem(std::string name, RooAbsReal * item);
    void AddItem(std::string name, RooAbsReal * item, bool warn);

    // Checking functions
    bool CheckForExistence(std::string name);

    // Data members
    std::string m_name;
    std::map<std::string, RooAbsReal*> m_map;

};

#endif // MANAGER_BASE_HPP
