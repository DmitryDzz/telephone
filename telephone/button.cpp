#include "button.h"

using namespace telephone;

Button::Button(int releaseValue, int pressValue)
{
  this->middleValue = (pressValue - releaseValue) / 2;
  this->buttonHandler = NULL;
}

void Button::setHandler(ButtonHandler handler)
{
  this->buttonHandler = handler;
}

void Button::refresh(int value, long millis)
{
  if (!this->waiting)
  {
    this->currentState = getButtonState(value);
  }
  
  if (this->currentState != this->previousState)
  {
    unsigned long currentTimeMillis = millis;
    if (!this->waiting)
    {
      this->waiting = true;
      this->nextTimeMillis = currentTimeMillis + DELAY_MILLIS;
      return;
    }
    
    if (currentTimeMillis >= this->nextTimeMillis) this->waiting = false;
    else return;
      
    this->currentState = getButtonState(value);
    if (this->currentState != this->previousState)
    {
      if (this->buttonHandler != NULL)
      {
        this->buttonHandler(this->currentState);
      }
      this->previousState = this->currentState;
    }
  }
}

ButtonState Button::getButtonState(int value)
{
  if (this->middleValue > 0) {
    return value <= this->middleValue ? RELEASED : PRESSED;
  } else {
    return value <= -this->middleValue ? PRESSED : RELEASED;
  }
}

