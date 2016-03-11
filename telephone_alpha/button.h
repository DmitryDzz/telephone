#ifndef button_h
#define button_h

#include <Arduino.h>

namespace telephone
{
  enum ButtonState
  {
    RELEASED = 0,
    PRESSED = 1
  };

  typedef void (*ButtonHandler)(ButtonState);
  
  class Button
  {
    public:
      Button(int releaseValue, int pressValue);
      void setHandler(ButtonHandler handler);
      void refresh(int value, long millis);
    private:
      int middleValue;

      boolean waiting = false;
      unsigned long nextTimeMillis = 0;
      const unsigned long DELAY_MILLIS = 10;
      
      ButtonState currentState = RELEASED;
      ButtonState previousState = RELEASED;
      
      ButtonHandler buttonHandler;
      
      ButtonState getButtonState(int value);
  };
}

#endif
