#ifndef NMEA0183UTILS_H
#define NMEA0183UTILS_H

// Function declarations
std::string getChecksum(std::string sentence);

bool compareChecksum(std::string sentence);


#endif