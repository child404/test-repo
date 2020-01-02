#pragma once

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Definition of overloaded operators

template<typename T>
ostream& operator << (ostream& os, const vector<T>& v) {
  int i = 0;
  for (const auto& item : v) {
    if (i != 0) {
      cout << ' ';
    }
    os << item;
    ++i;
  }
  return os;
}
