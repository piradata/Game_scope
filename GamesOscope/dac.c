#include "defines.h"

void dac_setup() { 
  pmc_enable_periph_clk(ID_DACC);       // Enable dac clock
  dacc_reset(DACC);                     // Reset dac
  dacc_set_transfer_mode(DACC, 1);      // Full word transfer mode
  dacc_enable_flexible_selection(DACC); // Tag selection mode enabled
  dacc_set_power_save(DACC, 0, 1);      // Sleep mode off, fast wakeup on
  dacc_enable_channel(DACC, 0);         // Enable output channel 0
  dacc_enable_channel(DACC, 1);         // Enable output channel 1
  bs1(DACC->DACC_IER, 0);               // Enable txrdy interrupt
}

void dac_write(uint16_t chan0, uint16_t chan1) {
  while (!(dacc_get_interrupt_status(DACC) & DACC_ISR_TXRDY));
  DACC->DACC_CDR = (0b0001 << 28) | ((chan1 & 0x0FFF) << 16) | (0b0000 << 12) | ((chan0 & 0x0FFF) << 0);
}

