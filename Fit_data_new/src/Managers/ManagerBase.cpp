#include "RooAbsReal.h"

#include "ManagerBase.hpp"


// ===========
// Constructor
// ===========
ManagerBase::ManagerBase(std::string name) : m_name(name) {
}


// ===========
// Get an item
// ===========
RooAbsReal * ManagerBase::Get(std::string name) {

    // Check item exists
    if (!CheckForExistence(name)) {
        std::cout << "Error: " << name << " not found in " << m_name << std::endl;
        return NULL;
    }

    // Return the item
    return m_map[name];

}


// ============================
// Get an item with [] operator
// ============================
RooAbsReal * ManagerBase::operator[](std::string name) {
    return Get(name);
}


// ===================
// Get name of manager
// ===================
std::string ManagerBase::GetName() {
    return m_name;
}


// =============
// Reset the map
// =============
void ManagerBase::Reset() {
    m_map.clear();
}


// ==========================================================
// Add an item to the map (duplication warning on by default)
// ==========================================================
void ManagerBase::AddItem(std::string name, RooAbsReal * item) {
    AddItem(name, item, true);
}


// ======================================================
// Add an item to the map with custom duplication warning
// ======================================================
void ManagerBase::AddItem(std::string name, RooAbsReal * item, bool warn) {

    // Check for duplication if warning setting turned on
    if (warn) {
        if (CheckForExistence(name)) {
            std::cout << "Warning: item " << name << " already exists in " << m_name
                << ". Overwriting." << std::endl;
        }
    }

    // Add item to map
    m_map.emplace(name, item);

}


// ===========================
// Check if item exists in map
// ===========================
bool ManagerBase::CheckForExistence(std::string name) {
    return (m_map.count(name));
}
