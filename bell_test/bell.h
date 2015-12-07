#ifndef bell_h
#define bell_h

#include <stdint.h>

class Bell
{
  public:
    static void initialize(const int highFrequencyPin, const int lowFrequencyPin);
    static void startSound();
    static void stopSound();
  private:
    static void saveRegistries();
    static void restoreRegistries();
};

#endif
