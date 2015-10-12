#include "ring.h"

using namespace telephone;

Ring::Ring(long duration, long delay) {
  this->duration = duration;
  this->delay = delay;
  this->started = false;
  this->ringing = false;
  this->ringHandler = NULL;
}

void Ring::setHandler(RingHandler handler) {
  this->ringHandler = handler;
}

void Ring::start(long millis) {
  this->started = true;
  this->ringing = true;
  this->fromMillis = millis;
  if (this->ringHandler != NULL) {
    this->ringHandler(HIGH);
  }
}

void Ring::stop() {
  this->started = false;
  this->ringing = false;  
  if (this->ringHandler != NULL) {
    this->ringHandler(LOW);
  }
}

void Ring::refresh(long millis) {
  if (!this->started) {
    return;
  }
  
  if (this->ringing) {
    if (millis - this->fromMillis > duration) {
      this->ringing = false;
      this->fromMillis = millis;
      if (this->ringHandler != NULL) {
        this->ringHandler(LOW);
      }
    }
  } else { // !ringing
    if (millis - this->fromMillis > delay) {
      this->ringing = true;
      this->fromMillis = millis;
      if (this->ringHandler != NULL) {
        this->ringHandler(HIGH);
      }
    }
  }  
}
