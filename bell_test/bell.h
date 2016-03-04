#ifndef bell_h
#define bell_h

#include <stdint.h>

class Bell
{
  public:
    static void initialize(const int highFrequencyPin, const int lowFrequencyPinA, const int lowFrequencyPinB);
    static void start();
    static void stop();
  private:
    static void saveRegistries();
    static void restoreRegistries();
};

#endif
