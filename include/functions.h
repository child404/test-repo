#pragma once

// Definition of functions such as Sqr

template<typename T>
T Sqr (const T& x);

// Implementation

template<typename T>
T Sqr (const T& x) {
  return x * x;
}
