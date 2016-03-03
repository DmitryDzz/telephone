#ifndef bell_h
#define bell_h

#include <stdint.h>

class Bell
{
  public:
    static void initialize(const int highFrequencyPin, const int lowFrequencyPinA, const int lowFrequencyPinB);
    static void startSound();
    static void stopSound();
  private:
    static void saveRegistries();
    static void restoreRegistries();
};

#endif
