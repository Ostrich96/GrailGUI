#pragma once
#include <stdint.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

class Stock {
 private:
  float open, high, low, close;
  uint64_t volume;

 public:
  Stock(){};
  friend std::ostream& operator<<(std::ostream& o, const Stock& s);
  static bool approxEqual(float a, float b, float eps);
  static bool reportError(const Stock& s1, const Stock& s2, float eps);
};
/*
class DeltaEncodeStock {
private:
  uint16_t open, high, low, close, volume; // all 16 bits

public:
  DeltaEncodeStock(const Stock& s)
  friend ostream& operator <<(ostream& o, const Stock& s) {}
  void write();
};

int testStockDelta(const char filename[]) {
  ifstream f(filename);
  Stock s;
  float lastDayClosePrice; // note, special case for  first day,
lastDayClosePrice = open while (f >> s) { DeltaEncodeStock d(s,
lastDayClosePrice); // delta encode it Stock s2 =
d.computeOriginal(lastDayClosePrice); s2.reportError();
  }
}

/*

Example
    .42518   1.42518  --> delta 60000
    .42518   .42519   --> delta 00001
    .42518   .43726   --> delta 01208
    .42518  1.6252    --> delta 65535 (overflow)              1.6252  stored
below Trading suspended --> delta 65534                         Nan 123.45
132.45 = 9.00 --> delta 900



     nan   nan   nan   nan  -->  65535  0 0 0
volume
    13000102  --> 130 *10^5 --> 5  130   130 + (5 << 12)     0101 0000 1000 0010
    57002041  --> 570 *10^5 --> 5  570                       0101 0010 0011 1010
   123000000  --> 123 *10^6 --> 6  123                       0110 0000 0111 1001

1230000000 - 57000000 = 43000000                             0101

aapl.txt --> 436k
aapl.delta = 80k
aapl.delta.lzma --> 65kb    70kb

lzma -e apple.delta


Server: List<Stock> quoteApple;
server sends updates to client


Client: List<Stock> quoteApple;
quoteApple.updateSince([129.23, 129.45, 129.47...]) // send as delta

*/
class DeltaEncodeStock {
 private:
  uint16_t open, high, low, close, volumn;

 public:
  DeltaEncodeStock(const Stock& s){};
  friend std::ostream& operator<<(std::ostream& o, const Stock& s);
  void write();
};

int testStockDelta(const char filename[]) { std::ifstream f(filename);
  Stock s;
  float lastDayClosePrice;
  while (f >> s){
    DeltaEncodeStock d(s, lastDayClosePrice)
  }
}