#include "defines.h"

#define LEFT  1
#define UP    2
#define DOWN  3
#define RIGHT 4

#define GRID_W 20
#define GRID_H 16
#define GRID_SIZE (LCD_SIZE_W / GRID_W)
#define SNAKE_SIZE (GRID_SIZE / 2)
#define POINT_SIZE (SNAKE_SIZE - 2)

#define SCORE_EDGE_PADDING (LCD_SIZE_W / 25)
#define DIGIT_PADDING (LCD_SIZE_W / 40)
#define DIGIT_SIZE (LCD_SIZE_W / 25)

typedef struct {
  int x;
  int y;
} node;

typedef struct {
  node snake[GRID_W * GRID_H + 1];
  node fruit;
  int len;
} shared_values_t;

int dir;

shared_values_t shared_values;

#define DEFINE_SHARED_VALUES shared_values_t values;
#define SET_SHARED_VALUES { kernel_disable_interrupts(); shared_values = values; kernel_enable_interrupts(); }
#define GET_SHARED_VALUES { kernel_disable_interrupts(); values = shared_values; kernel_enable_interrupts(); }

node new_fruit_position() {
  DEFINE_SHARED_VALUES
  GET_SHARED_VALUES
  
  bool em_cima;
  node fruit;

  do {
    fruit.x = random2(0, GRID_W - 1);
    fruit.y = random2(0, GRID_H - 1);

    em_cima = false;
    for (int i = 0; i < values.len; i++) {
      if (fruit.x == values.snake[i].x && fruit.y == values.snake[i].y) {
        em_cima = true;
        break;
      }
    }
  } while (em_cima);

  return fruit;
}

void reset() {
  DEFINE_SHARED_VALUES
    
  dir = 0;
  values.len = 1;
  values.snake[0].y = GRID_H / 2;
  values.snake[0].x = GRID_W / 2;

  SET_SHARED_VALUES
  values.fruit = new_fruit_position(); // Reset point position
  SET_SHARED_VALUES
}

void snake_draw() {
  DEFINE_SHARED_VALUES
  GET_SHARED_VALUES

  //draw bounds
  lcd_draw_rect(0, 0, LCD_SIZE_W - 1, LCD_SIZE_H - 1);

  //draw snake head
  lcd_fill_circle(values.snake[0].x * GRID_SIZE + GRID_SIZE / 2, values.snake[0].y * GRID_SIZE + GRID_SIZE / 2, SNAKE_SIZE);

//draw snake body
  for (int j = 1; j < values.len; j++) {
    lcd_draw_circle(values.snake[j].x * GRID_SIZE + GRID_SIZE / 2, values.snake[j].y * GRID_SIZE + GRID_SIZE / 2, SNAKE_SIZE);
  }
  

  //draw point
  lcd_draw_circle(values.fruit.x * GRID_SIZE + GRID_SIZE / 2, values.fruit.y * GRID_SIZE + GRID_SIZE / 2, POINT_SIZE);
}

void snake_process() {

  // Read button inputs
  bttn_states bttns = io_read_bttns();

  // up
  if (bttns.bttn1 && dir != DOWN) {
    dir = UP;
  }
  //down
  if (bttns.bttn2 && dir != UP) {
    dir = DOWN;
  }
  //left
  if (bttns.bttn3 && dir != RIGHT) {
    dir = LEFT;
  }
  //down
  if (bttns.bttn4 && dir != LEFT) {
    dir = RIGHT;
  }

  DEFINE_SHARED_VALUES
  GET_SHARED_VALUES

  for (int i = values.len - 1; i >= 0; i--) {
    values.snake[i + 1].x = values.snake[i].x;
    values.snake[i + 1].y = values.snake[i].y;
  }

  if (dir == UP) {
    values.snake[0].y++;
  }
  if (dir == LEFT) {
    values.snake[0].x--;
  }
  if (dir == RIGHT) {
    values.snake[0].x++;
  }
  if (dir == DOWN) {
    values.snake[0].y--;
  }


  if ( (values.snake[0].x == values.fruit.x)  &&   (values.snake[0].y == values.fruit.y )) {
    values.fruit = new_fruit_position();
    values.len++;
  }

  bool collision = false;
  for (int i = 1; i < values.len; i++) {
    if (values.snake[0].x == values.snake[i].x && values.snake[0].y == values.snake[i].y) {
      collision = true;
      break;
    }
  }

  SET_SHARED_VALUES
  
  // If goes out of limits, the game restarts
  if (values.snake[0].y < 0 || values.snake[0].y > GRID_H - 1 || values.snake[0].x < 0 || values.snake[0].x > GRID_W - 1 || collision) {
    reset();
  }
}

void game_snake_start() {
  reset();
  kernel_add_task(snake_draw, 30000, 0);
  kernel_add_task(snake_process, 250000, 1);
  
  
}

