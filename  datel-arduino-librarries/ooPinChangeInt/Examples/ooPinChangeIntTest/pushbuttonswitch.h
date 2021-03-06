#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <pins_arduino.h>
  #include <WProgram.h>
#endif
#include <ByteBuffer.h>
#include "uint8ToString.h"

extern ByteBuffer printBuffer;
char numBuffer[4] = { 0, 0, 0, 0 };
#define PRINTIT

// To use the library, define a class that subclasses CallBackInterface.
// And also, include a method (C++ talk for "subroutine") called "cbmethod()" in the class.
// Use this class as a template to create your own; it's not hard.  You don't
// even have to understand what you're doing at first.
// How do you subclass?  Like this:

class pushbuttonswitch : public CallBackInterface
{
 public:
   uint8_t count;
   uint8_t pin;
   uint8_t mode;
   static boolean start;
   char *name;

   pushbuttonswitch (uint8_t _pin, char *_name, uint8_t _mode): pin(_pin), name(_name), mode(_mode) {
     init();
   };

   void cbmethod() {
     #ifdef PRINTIT
     showMode();
     printBuffer.putString(name); printBuffer.putString(":"); 
     printBuffer.putString(" p"); uint8ToString(numBuffer, pin); printBuffer.putString(numBuffer);
     printBuffer.putString(" P"); uint8ToString(numBuffer, port); printBuffer.putString(numBuffer);
     #endif
     if (pushbuttonswitch::start) {
       #ifdef PRINTIT
       printBuffer.putString(" ct++");
       #endif
       count++;
     }
     #ifdef PRINTIT
     else printBuffer.putString(" nocount");
     printBuffer.putString("\n");
     #endif
   };
   
   uint8_t getCount() {
     //printBuffer.putString(" count: ");
     //uint8ToString(numBuffer, count); printBuffer.putString(numBuffer); printBuffer.putString("\n");
     return count;
   }
   
   char *getName() {
     return name;
   }
   
   void reset() {
     count=0;
   }
   
   void showMode() {
    switch (mode) {
    case FALLING:
       printBuffer.putString("-F-");
    break;
    case RISING:
      printBuffer.putString("+R+");
    break;
    case CHANGE:
      printBuffer.putString("*C*");
    break;
    }
  }

 protected:
  uint8_t port;
  void init () {
    port=digitalPinToPort(pin);
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
    PCintPort::attachInterrupt(pin, this, mode);
  };
};
boolean pushbuttonswitch::start=false;

// ***********************************************************************************
// *** CLASS startswitch *************************************************************
// ***********************************************************************************
class startswitch : public pushbuttonswitch, public CallBackInterface
{
 public:
  startswitch (uint8_t _spin, char *_sname, uint8_t _smode) :
    pushbuttonswitch(_spin, _sname, _smode),
    detachables(NULL), initial(true) { }

  void addDetachableSwitch(pushbuttonswitch *aswitch) {
    if (detachables==NULL) {
      detachables=new detachableSwitch(aswitch); detachables->next=NULL; return;
    }
    tmp=detachables;
    while (tmp->next!=NULL) {
      tmp=tmp->next;
    }
    tmp->next=new detachableSwitch(aswitch); tmp=tmp->next; tmp->next=NULL;
  }
  
  void cbmethod() {
    uint8_t apin;
    showMode();
    tmp=detachables;
    if (pushbuttonswitch::start) {
      printBuffer.putString("f2: STOP! Counting off.\n");
      printBuffer.putString("Interrupt OFF on ");
      while (tmp != NULL) {
        apin=tmp->aswitch->pin;
        printBuffer.putString(tmp->aswitch->name);
        printBuffer.putString("("); uint8ToString(numBuffer, apin), printBuffer.putString(numBuffer);
        printBuffer.putString(") ");
        PCintPort::detachInterrupt(apin);
        tmp=tmp->next;
      }
      pushbuttonswitch::start=false;
    } else {
      printBuffer.putString("START! p");   uint8ToString(numBuffer, pin); printBuffer.putString(numBuffer);
      printBuffer.putString("-P"); uint8ToString(numBuffer, port); printBuffer.putString(numBuffer);
      printBuffer.putString("\n");
      if (! initial) {
        printBuffer.putString("re-Attach interrupts; ");
        while (tmp != NULL) {
          PCintPort::attachInterrupt(tmp->aswitch->pin, tmp->aswitch, tmp->aswitch->mode);
          uint8ToString(numBuffer, tmp->aswitch->pin); printBuffer.putString(" "); printBuffer.putString(numBuffer);
          tmp=tmp->next;
        }
        printBuffer.putString("\n");
      } else {
        initial=false;
      }
      pushbuttonswitch::start=true;
    }
  }
  

 private:
   boolean initial;
   class detachableSwitch {
     public:
       detachableSwitch(pushbuttonswitch *_aswitch) : aswitch(_aswitch) {
         next=NULL;
       }
       detachableSwitch *next;
       pushbuttonswitch *aswitch;
   };
   detachableSwitch *detachables;
   detachableSwitch *tmp;
};

