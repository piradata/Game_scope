#include "defines.h"

#define BTTN1_PIN        47
#define BTTN2_PIN        45
#define BTTN3_PIN        43
#define BTTN4_PIN        41

void io_setup() {
  pinMode(BTTN1_PIN, INPUT);
  pinMode(BTTN2_PIN, INPUT);
  pinMode(BTTN3_PIN, INPUT);
  pinMode(BTTN4_PIN, INPUT);
}

bttn_states io_read_bttns() {
  bttn_states state;
  
  kernel_disable_interrupts();
  state.bttn1 = digitalRead(BTTN1_PIN);
  state.bttn2 = digitalRead(BTTN2_PIN);
  state.bttn3 = digitalRead(BTTN3_PIN);
  state.bttn4 = digitalRead(BTTN4_PIN);
 
  kernel_enable_interrupts();

  return state;
}

