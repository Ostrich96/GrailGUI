#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Stock {
 private:
  float open;
  float high;
  float low;
  float close;
  uint8_t decimals;
  uint8_t precison;
  uint64_t volume;
  int16_t openDelta;  // Convert to signed int, instead of using neg bit
  uint16_t highDelta;
  uint16_t lowDelta;
  uint16_t closeDelta;
  uint16_t volumeApproximate;
  bool emptyStock : 1;
  bool firstDay : 1;

 public:
  Stock()
      : open(0),
        high(0),
        low(0),
        close(0),
        volume(0),
        precison(5),
        openDelta(0),
        highDelta(0),
        lowDelta(0),
        closeDelta(0),
        volumeApproximate(0),
        emptyStock(true),
        firstDay(false) {}

  // Remove negative bit
  friend ostream& operator<<(ostream& o, const Stock& s) {
    if (s.firstDay) {
      // set the output precison as a variable, it may be more or less than 5
      return o << setprecision(s.precison) << s.open << " " << s.highDelta
               << " " << s.lowDelta << " " << s.closeDelta << " "
               << s.volumeApproximate << '\n';
    } else {
      return o << s.openDelta << " " << s.highDelta << " " << s.lowDelta << " "
               << s.closeDelta << " " << s.volumeApproximate << '\n';
    }
  }

  // Make sure first_day is false, set true in delta()
  friend istream& operator>>(istream& i, Stock& s) {
    char symbol[32];
    char buf[1024];
    s.emptyStock = false;
    s.firstDay = false;
    i.getline(buf, sizeof(buf));
    uint32_t mm, dd, yyyy;
    // double decOpen, decHi, decLow, decClose; --
    uint32_t openInterest;
    // char sep;  // store the bogus - and , separators --
    sscanf(buf, "%u-%u-%u,%f,%f,%f,%f,%lu,%u", &yyyy, &mm, &dd, &s.open,
           &s.high, &s.low, &s.close, &s.volume, &openInterest);
    return i;
  }

  // Change default to nullptr instead of empty stock,
  void delta(Stock* prevDay = nullptr) {
    if (prevDay == nullptr) {
      firstDay = true;
    }
    // Since prev_day defaults to null, check if it exists
    double prevDayClose = (!prevDay) ? open : prevDay->getClosePrice();
    decimals = digitCount(open);
    double openDeltaDouble = (int)(open * pow(10, decimals)) -
                             (int)(prevDayClose * pow(10, decimals));
    double highDeltaDouble =
        (int)(high * pow(10, decimals)) - (int)(open * pow(10, decimals));
    double lowDeltaDouble =
        (int)(open * pow(10, decimals)) - (int)(low * pow(10, decimals));
    double closeDeltaDouble =
        (int)(close * pow(10, decimals)) - (int)(low * pow(10, decimals));
    openDelta = openDeltaDouble;
    highDelta = highDeltaDouble;
    lowDelta = lowDeltaDouble;
    closeDelta = closeDeltaDouble;
    volumeApproximate = volume / 10000;  // do something for the 10000 ? --
  }

  // determine the decimal of the open_price
  int digitCount(double open) {
    int significantBit = 5;
    int count = 0;
    int k = (int)open;
    while (k > pow(10, count)) {
      count++;
    }
    return significantBit - count;
  }

  float getClosePrice() { return close; }
  bool isEmpty() { return emptyStock; }

  // Writes stock out to ostream as bits to be read in later
  friend void binwrite(ofstream& o, const Stock& s) {
    if (s.firstDay)  // write base price if first_day, else write delta
      o.write((char*)&s.open, sizeof(float));
    else
      o.write((char*)&s.open, sizeof(int16_t));

    o.write((char*)&s.highDelta, sizeof(uint16_t));
    o.write((char*)&s.lowDelta, sizeof(uint16_t));
    o.write((char*)&s.closeDelta, sizeof(uint16_t));
    o.write((char*)&s.volumeApproximate, sizeof(uint16_t));
  }

  // detect the accuracy error between the original and decoded price and volume
  static bool approxEqual(float a, float b, float eps) {
    return abs(b - a) < eps;
  }

  // report the exact error in stock price
  static bool reportError(const Stock& s1, const Stock& s2, float eps1,
                          float eps2) {
    bool ok = true;
    if (!(ok &= approxEqual(s1.open, s2.open, eps1)))
      cerr << "error: " << s1.open << " " << s2.open;
    if (!(ok &= approxEqual(s1.high, s2.high, eps1)))
      cerr << "error: " << s1.high << " " << s2.high;
    if (!(ok &= approxEqual(s1.low, s2.low, eps1)))
      cerr << "error: " << s1.low << " " << s2.low;
    if (!(ok &= approxEqual(s1.close, s2.close, eps1)))
      cerr << "error: " << s1.close << " " << s2.close;
    if (!(ok &= approxEqual(s1.volume, s2.volume, eps2)))
      cerr << "error: " << s1.volume << " " << s2.volume;
    return ok;
  }
};

class DeltaEncodeStock {
 private:
  vector<Stock> stocks;
  ifstream f;

 public:
  DeltaEncodeStock();
  // ~DeltaEncodeStock();
  DeltaEncodeStock(string name) : f(name.c_str()), stocks() {
    char buf[1024];
    f.getline(buf, sizeof(buf));
    Stock s;

    while (f >> s) {
      if (stocks.size() > 0)
        s.delta(&stocks[stocks.size() - 1]);
      else
        s.delta();
      stocks.push_back(s);
    }
  };

  friend ostream& operator<<(ostream& o, const DeltaEncodeStock& c) {
    for (int i = 0; i < c.stocks.size(); i++) {
      o << c.stocks[i];  // Fix space at beginning of lines
    }
    return o << '\n';
  }

  // Creates output stream and writes each Stock
  friend void binwrite(const char* filename, const DeltaEncodeStock& c) {
    ofstream stream(filename, ios::binary);
    for (const auto& elem : c.stocks) {
      binwrite(stream, elem);
    }
  }

  // test 
  void testStockDelta(const char* filename) {}
};

int main() {
  const DeltaEncodeStock c("aapl.txt");
  ofstream outfile("aaplDeltaDigit.txt");
  outfile << c;
  binwrite("aaplDeltaDigit.bin", c);
}
