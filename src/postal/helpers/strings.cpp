#include "strings.h"
#include <sstream>

using namespace std;

list<string> split(const string &s, char sep) {
  list<string> tokens;
  size_t start = 0;
  size_t end = 0;
  while (s.npos != end) {
    end = s.find(sep, start);
    if (start != end) {
      tokens.push_back(s.substr(start, end - start));
      start = end;
    }
    else {
      start++;
    }
  }
  return tokens;
}

string join(const list<string> &tokens, char sep) {
  string s = "";
  for (list<string>::const_iterator i=tokens.begin(); i!=tokens.end(); i++) {
    if (i==tokens.begin()) { 
      s = *i; 
    }
    else {
      s = s + sep + *i;
    }
  }
  return s;
}

bool startsWith(const string &s, const string &w) {
  return s.compare(0, w.length(), w) == 0;
}

bool isNumber(const string &s) {
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it)) {
    it++;
  }
  return !s.empty() && it == s.end();
}

string intToString(int number) {
   stringstream ss;
   ss << number;
   return ss.str();
}

int stringToInt(const string &s) {
  stringstream ss;
  ss << s;
  int integer;
  ss >> integer;
  return integer;
}
