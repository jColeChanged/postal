#ifndef PATTERN_H
#define PATTERN_H
#include <map>
#include <string>

// This is a wrapper method around a matchPattern which accepts tokenized
// input and a map/dictionary of bindings. Tokens which begin with ? match
// a single word. Tokens which begin with ?* match up until a match is
// found for the next token. If two tokens with the same name are supplied
// then it will return a match only if it finds a pattern in which both of
// the tokens would capture the same strings. When matching fails the
// bindings returned will include "FAIL" -> "FAILED"

// pattern and input will be tokenized when submitted such that "?foo bar
// baz" becomes ["?foo" "bar" "baz"]. If matched against "boo bar baz" a
// bindings map mapping "?foo"->"boo" would be returned.
std::map<std::string, std::string> matchPattern(std::string pattern,
						std::string input);

bool isFailedMatch(const std::map<std::string, std::string> &bindings);
#endif
