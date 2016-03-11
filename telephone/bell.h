#ifndef bell_h
#define bell_h

#include <stdint.h>

class Bell
{
  public:
    /**
     * Method initialize() must be called before start() and stop() methods.
     * highFrequencyActivityPin – pin to enable/disable high frequency generator.
     * lowFrequencyPinA – low frequency signal for DC-AC converter (positive half wave).
     * lowFrequencyPinB – low frequency signal for DC-AC converter (negative half wave).
     */
    static void initialize(const int highFrequencyActivityPin,
                           const int lowFrequencyPinA,
                           const int lowFrequencyPinB);
   /**
    * start() starts bell ringing sequence: 4 seconds ringing, 4 senonds silence...
    */
    static void start();
    /**
     * stop() stops ringing, turnes off high frequence generator, DC-DC boost converter
     * and DC-AC converter.
     */
    static void stop();
  private:
    static void saveRegistries();
    static void restoreRegistries();
};

#endif
