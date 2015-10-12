#ifndef ring_h
#define ring_h

#include <Arduino.h>

namespace telephone
{
  typedef void (*RingHandler)(boolean);
  
  class Ring
  {
    public:
      Ring(long duration, long delay);
      void setHandler(RingHandler handler);
      void start(long millis);
      void stop();
      void refresh(long millis);
    private:
      long duration;
      long delay;
      RingHandler ringHandler;
      boolean started;
      boolean ringing;
      long fromMillis;
  };
}

#endif
