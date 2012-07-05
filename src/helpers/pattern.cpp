#include "pattern.h"
#include <list>
#include <algorithm>
#include "strings.h"
#include "lists.h"

using namespace std;

// foward declarationd due to mutual recursion between matchPattern and matchSegmentPattern
map<string, string> matchSegmentPattern(list<string>, list<string>, map<string, string>);

map<string, string> bindPattern(const string &pattern, const string &input, const map<string, string> &bindings)
{
  map<string, string> newBindings = bindings;
  map<string, string>::const_iterator kv = bindings.find(pattern);
  newBindings = bindings;
  newBindings[pattern] = input;
  if (kv != bindings.end() && kv->second != input) {
    newBindings["FAIL"] = "FAILED";
  }
  return newBindings;
}

map<string, string> bindPattern(const string &pattern, const list<string> &input, const map<string, string> &bindings) {
  return bindPattern(pattern, join(input), bindings);
}

bool isTokenVariable(const string &token) {
  return startsWith(token, "?");
}

bool isSegmentVariable(const string &token) {
  return startsWith(token, "?*");
}

bool isLiteral(const string &token) {
  return !isTokenVariable(token);
}

bool isFailedMatch(const map<string, string> &bindings) {
  return bindings.find("FAIL") != bindings.end();
}


map<string, string> matchPattern(list<string> pattern, list<string> input, map<string, string> bindings) {
  list<string>::iterator pIter = pattern.begin();
  list<string>::iterator iIter = input.begin();
  while (pIter != pattern.end()) {
    if (iIter == input.end()) {
      bindings["FAIL"] = "FAILED";
      return bindings;
    }
    else if (isSegmentVariable(*pIter)) {
      return matchSegmentPattern(slice(pIter, pattern.end()), slice(iIter, input.end()), bindings);
    }
    else if (isTokenVariable(*pIter)) {
      bindings = bindPattern(*pIter, *iIter, bindings);
    }
    else if (isLiteral(*pIter)) {
      if (*pIter != *iIter) {
	bindings["FAIL"] = "FAILED";
	return bindings;
      }
    }
    pIter++;
    iIter++;
  }
  return bindings;
}

map<string, string> matchPattern(string pattern, string input)
{
  map<string, string> bindings;
  return matchPattern(split(pattern), split(input), bindings);
}

map<string, string> matchSegmentPattern(list<string> pattern, list<string> input, map<string, string> bindings)
{
  map<string, string> newBindings;
  
  list<string>::iterator iter = pattern.begin();
  string pat = *iter;
  iter++;
  string toMatch =  (iter != pattern.end()) ? *iter : "";
  
  list<string> newPattern = slice(iter, pattern.end());
  list<string> newInput;
  list<string> bindingTokens;
  
  list<string>::iterator iIter = input.begin();
  while (iIter != input.end()) {
    iIter = find(iIter, input.end(), toMatch);
    newInput = slice(iIter, input.end());
    bindingTokens = slice(input.begin(), iIter);
    newBindings = matchPattern(newPattern, newInput,
			       bindPattern(pat, bindingTokens, bindings));
    if (newBindings.find("FAIL") == newBindings.end()) {
      return newBindings;
    }
    if (iIter != input.end()) { iIter++; }
  }
  newBindings["FAIL"] = "FAILED";
  return newBindings;
}
