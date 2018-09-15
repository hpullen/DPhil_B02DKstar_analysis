#include <fstream>

#include "RooAbsReal.h"

#include "ManagerBase.hpp"


// ===========
// Constructor
// ===========
ManagerBase::ManagerBase(std::string name) : m_name(name), m_warn(true) {
}


// ===========
// Get an item
// ===========
RooAbsReal * ManagerBase::Get(std::string name) {

    // Check item exists
    if (!CheckForExistence(name)) {
        std::cout << "Error: " << name << " not found in " << m_name << std::endl;
        exit (EXIT_FAILURE);
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


// ==============================
// Print list of all stored items
// ==============================
void ManagerBase::Print() {
    std::cout << m_name << " contents: " << std::endl;
    for (auto item : m_map) {
        std::cout << item.first << std::endl;
    }
}


// ======================================================
// Make new item pointing to same object as existing item
// ======================================================
void ManagerBase::AddShared(std::string name1, std::string name2) {
    AddItem(name1, Get(name2));
}


// Turn off overwrite warnings
void ManagerBase::SetWarnings(bool warn) {
    m_warn = warn;
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
    AddItem(name, item, m_warn);
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
    m_map[name] = item;

}


// ===========================
// Check if item exists in map
// ===========================
bool ManagerBase::CheckForExistence(std::string name) {
    return (m_map.count(name));
}
