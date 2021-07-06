#pragma once
#include <stdint.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
class Stock {
 public:
  float open, high, low, close;
  uint64_t volume;
  bool first_day : 1;
  uint8_t decimal = 4;

  Stock(){};
  uint64_t getVolume() { return volume; }
  float getClose() { return close; }
  friend std::ostream& operator<<(std::ostream& o, const Stock& s) {
    o << s.open << " " << s.high << " " << s.low << " " << s.close << " "
      << s.volume;
    return o;
  };
  friend std::istream& operator>>(std::istream& i, Stock& t) {
    char buf[1024];
    t.first_day = false;
    i.getline(buf, sizeof(buf));
    uint32_t mm, dd, yyyy;
    double decOpen, decHi, decLow, decClose;
    uint32_t openInterest;
    char sep;  // store the bogus - and , separators
    sscanf(buf, "%u-%u-%u,%f,%f,%f,%f,%lu,%u", &yyyy, &mm, &dd, &t.open,
           &t.high, &t.low, &t.close, &t.volume, &openInterest);
    return i;
  }
  static bool approxEqual(float a, float b, float eps) {
    return abs(b - a) < eps;
  };
  static bool reportError(const Stock& s1, const Stock& s2, float eps1,
                          float eps2) {
    bool ok = true;
    if (!(ok &= approxEqual(s1.open, s2.open, eps1))) {
      std::cerr << "error:" << s1.open << " " << s2.open;
    };
    if (!(ok &= approxEqual(s1.high, s2.high, eps1))) {
      std::cerr << "error:" << s1.high << " " << s2.high;
    };
    if (!(ok &= approxEqual(s1.low, s2.low, eps1))) {
      std::cerr << "error:" << s1.low << " " << s2.low;
    };
    if (!(ok &= approxEqual(s1.close, s2.close, eps1))) {
      std::cerr << "error:" << s1.close << " " << s2.close;
    };
    if (!(ok &= approxEqual(s1.volume, s2.volume, eps2))) {
      std::cerr << "error:" << s1.volume << " " << s2.volume;
    };
  };
};

class DeltaEncodeStock {
 private:
  uint16_t high, low, close, volume;
  int16_t open;

 public:
  DeltaEncodeStock(const Stock& s, Stock* prev_day = nullptr) {
    if (s.first_day) {
      open = s.open * pow(10, s.decimal);
      volume = s.volume / pow(10, 5);
    } else {
      open = (s.open - prev_day->getClose()) * pow(10, s.decimal);
      volume = (s.volume - prev_day->getVolume()) / pow(10, 5);
    }
    high = (s.high - s.open) * pow(10, s.decimal);
    low = (s.open - s.low) * pow(10, s.decimal);
    close = (s.close - s.low) * pow(10, s.decimal);
  };
  friend std::ostream& operator<<(std::ostream& o, const DeltaEncodeStock& d) {
    o << d.open << " " << d.high << " " << d.low << " " << d.close;
    return o;
    void write() {
      
    }
  };
};

int testStockDelta(const char filename[]) {
  std::ifstream f(filename);
  Stock s;
  std::vector<Stock> stocks;
  while (f >> s) {
    if (stocks.size() > 0) {
      DeltaEncodeStock d(s, &stocks[stocks.size() - 1]);
    } else {
      DeltaEncodeStock d(s);
    };
    stocks.push_back(d);
  }
}