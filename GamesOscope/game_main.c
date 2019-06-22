#include "defines.h"

#define DIGIT_PADDING (LCD_SIZE_W / 25)
#define DIGIT_SIZE (LCD_SIZE_W / 15)

typedef struct {
  void (*start_f)();
} game_info_t;

game_info_t games[] = {
  { game_snake_start },
  { game_pong_start },
};

bool last_start_bttn_state;

  int game_choice;

void game_chooser_draw() {
  
  lcd_draw_rect(0, 0, LCD_SIZE_W - 1, LCD_SIZE_H - 1);
  lcd_draw_number(LCD_SIZE_W / 2 - DIGIT_SIZE / 2, LCD_SIZE_H / 2 - DIGIT_SIZE, DIGIT_SIZE, DIGIT_SIZE * 2, DIGIT_PADDING, game_choice + 1);
}

void game_chooser_process() {
  
  bttn_states bttns = io_read_bttns();
  
  if (bttns.bttn1) { game_choice=0; }
  else if (bttns.bttn2) { game_choice=1; }
  if (bttns.bttn4) { game_enter(game_choice);}
}

void game_enter(int game) {
  kernel_remove_all_tasks();

  if (game == -1) {
    game_choice = 0;
    kernel_add_task(game_chooser_draw, 10000, 0);
    kernel_add_task(game_chooser_process,300000, 1);
  }
  else {
    games[game].start_f();
  }
}

void game_start() {  
  last_start_bttn_state = false;
  game_enter(STARTING_GAME);
}

