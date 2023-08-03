#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "nmea0183utils.h"

std::string getChecksum(std::string sentence)
{
    // Find position of $ and * delims
    size_t dollarDelim = sentence.find('$');
    size_t starDelim = sentence.find('*');

    // Store content separately
    std::string content = sentence.substr(dollarDelim + 1, starDelim - (dollarDelim + 1));

    // Calculate the checksum
    int intChecksum = 0;
    for (size_t i = 0; i < content.size(); i++)
    {
        intChecksum = intChecksum ^ int(content[i]);
    }

    // Format to string and padded with 1 leading zero if necessary
    std::stringstream strStreamChecksum;
    strStreamChecksum << std::hex << std::setw(2) << std::setfill('0') << intChecksum;
    std::string strChecksum = strStreamChecksum.str();

    return strChecksum;
}

bool compareStrings(std::string str1, std::string str2)
{
    transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
    transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
    return (str1 == str2);
}

bool compareChecksum(std::string sentence)
{
    bool evalComp;
    size_t starDelim = sentence.find('*');
    std::string recChecksum = sentence.substr(starDelim + 1, 2);

    if (compareStrings(getChecksum(sentence), recChecksum))
    {
        evalComp = true; // std::cout << "Checksum is the same\n";
    }
    else
    {
        evalComp = false; // std::cout << "Checksum no same\n";
    }

    return evalComp;
}