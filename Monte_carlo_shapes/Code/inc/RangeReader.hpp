#ifndef RANGE_READER_H
#define RANGE_READER_H

#include <string>

// ==========================================
// Class for reading in a mass range from a file
// ==========================================


class RangeReader {
public:
    RangeReader();
    RangeReader(std::string filename);
    ~RangeReader();
    
    void readRange(std::string filename);

    double getLow();
    double getHigh();

private:
    bool m_range_read;
    double m_low;
    double m_high;
    const double m_default_low = 5000;
    const double m_default_high = 5800;
};

#endif // RANGE_READER_H




