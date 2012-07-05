#ifndef STRINGS_H
#define STRINGS_H

#include <string>
#include <list>

std::list<std::string> split(const std::string &s, char sep=' ');
std::string join(const std::list<std::string> &s, char sep=' ');
bool startsWith(const std::string &s, const std::string &w);
std::string intToString(int number);
#endif
