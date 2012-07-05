#include "lists.h"

using namespace std;
list<string> slice(const list<string>::iterator &first, const list<string>::iterator &last) {
  list<string> sliced;
  for (list<string>::iterator i=first; i != last; i++) {
    sliced.push_back(*i);
  }
  return sliced;
}
