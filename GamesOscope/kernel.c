#include "defines.h"

#define NUM_TIMERS 9

typedef struct {
  Tc* tc;
  uint32_t channel;
  IRQn_Type irq;
} timer_type;

timer_type timers[NUM_TIMERS] = { // XXX
  {TC0, 0, TC0_IRQn},
  {TC0, 1, TC1_IRQn},
  {TC0, 2, TC2_IRQn},
  {TC1, 0, TC3_IRQn},
  {TC1, 1, TC4_IRQn},
  {TC1, 2, TC5_IRQn},
  {TC2, 0, TC6_IRQn},
  {TC2, 1, TC7_IRQn},
  {TC2, 2, TC8_IRQn},
};

void (*callbacks[NUM_TIMERS])() = {}; // XXX

void timer_start(int index, int period_us, uint32_t priority) {  
  timer_type t = timers[index];
  
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)t.irq);

  TC_Configure(t.tc, t.channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1);
  TC_SetRC(t.tc, t.channel, CPU_FREQ_HZ / 2 / 1000000 * period_us);
  
  t.tc->TC_CHANNEL[t.channel].TC_IER = TC_IER_CPCS;
  t.tc->TC_CHANNEL[t.channel].TC_IDR =~ TC_IER_CPCS;

  NVIC_ClearPendingIRQ(t.irq);
  NVIC_SetPriority(t.irq, priority);
  NVIC_EnableIRQ(t.irq);
  
  TC_Start(t.tc, t.channel);
}

void timer_stop(int index) {
  NVIC_DisableIRQ(timers[index].irq);
  TC_Stop(timers[index].tc, timers[index].channel);
}

void kernel_enable_interrupts() {
  cpu_irq_enable();
}

void kernel_disable_interrupts() {
  cpu_irq_disable();
}

int kernel_add_task(void (*f)(void), int period_us, int priority) {
  for (int i = 0; i < NUM_TIMERS; i++){
    if (!callbacks[i]) {
      callbacks[i] = f;
      timer_start(i, period_us, priority);
      return i;
    }
  }
  return -1;
}

void kernel_remove_task(int index) {
  callbacks[index] = 0;
  timer_stop(index);
}

void kernel_remove_all_tasks() {
  for (int i = 0; i < NUM_TIMERS; i++) {
    kernel_remove_task(i);
  }
}

void timer_callback(int t) {
  TC_GetStatus(timers[t].tc, timers[t].channel);
  callbacks[t]();
}

void TC0_Handler(void) { timer_callback(0); }
void TC1_Handler(void) { timer_callback(1); }
void TC2_Handler(void) { timer_callback(2); }
void TC3_Handler(void) { timer_callback(3); }
void TC4_Handler(void) { timer_callback(4); }
void TC5_Handler(void) { timer_callback(5); }
void TC6_Handler(void) { timer_callback(6); }
void TC7_Handler(void) { timer_callback(7); }
void TC8_Handler(void) { timer_callback(8); }

