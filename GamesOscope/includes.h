#include "Arduino.h"

//_DAC_H
#define DAC_MAX_CHAN_0 4095
#define DAC_MAX_CHAN_1 4095
void dac_setup();

//_KERNEL_H
void kernel_setup();
int kernel_add_task(void (*f)(void), int period_us, int priority); // Supports up to 9 tasks, 0 is highest priority, 0..15 priority levels
void kernel_remove_task(int index);
void kernel_remove_all_tasks();
void kernel_enable_interrupts();
void kernel_disable_interrupts();

//_LCD_H
#define LCD_SIZE_W 600
#define LCD_SIZE_H 480
void lcd_draw_pixel(int x, int y);
void lcd_draw_line(int x0, int y0, int x1, int y1);
void lcd_draw_rect(int x0, int y0, int x1, int y1);
void lcd_fill_rect(int x0, int y0, int x1, int y1);
void lcd_draw_circle(int x, int y, int r);
void lcd_fill_circle(int x, int y, int r);
void lcd_draw_number(int x, int y, int w, int h, int spacing, int n);

//_IO_H
typedef struct {
  bool bttn1, bttn2, bttn3, bttn4;
} bttn_states;
void io_setup();
bttn_states io_read_bttns();

//_OTHER_H
void print_s(char* s);
void print_n(int i);
long random1(long howbig);
long random2(long howsmall, long howbig);
void vector_rotate(double* x, double* y, double rads);
void vector_scale(double* x, double* y, double multiplier);



//_GAME_MAIN_H
#define STARTING_GAME -1
#define GAME_COUNT 2
void game_start();


//_GAME_SNAKE_H
void game_snake_start();

//_GAME_PONG_H
void game_pong_start();


