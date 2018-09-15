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

    // Share a variable with a pre-existing one
    void AddShared(std::string name1, std::string name2);

    // Print list of items
    void Print();

    // Turn off overwrite warnings
    void SetWarnings(bool warn);

    // Resetter 
    void Reset();

protected:
    // Adding function
    void AddItem(std::string name, RooAbsReal * item);
    void AddItem(std::string name, RooAbsReal * item, bool warn);

    // Checking functions
    bool CheckForExistence(std::string name);

    // Data members
    const std::string m_name;
    std::map<std::string, RooAbsReal*> m_map;
    bool m_warn;

};

#endif // MANAGER_BASE_HPP
