#include "defines.h"

#define PAD_W (LCD_SIZE_W / 32)
#define PAD_H (LCD_SIZE_H / 4)
#define PAD_BORDER_W (LCD_SIZE_W / 16)
#define BALL_SIZE (LCD_SIZE_W / 30)

#define SCORE_EDGE_PADDING (LCD_SIZE_W / 25)
#define DIGIT_PADDING (LCD_SIZE_W / 40)
#define DIGIT_SIZE (LCD_SIZE_W / 25)

#define PAD_START_SPEED (1.5)
#define BALL_START_SPEED 0.9
#define SPEED_FACTOR_INC (0.1)

typedef struct {
  double left_pad_pos_y;
  double right_pad_pos_y;
  double ball_pos_x;
  double ball_pos_y;
  int score_left;
  int score_right;
} shared_values_t;

shared_values_t shared_values;
#define DEFINE_SHARED_VALUES shared_values_t values;
#define SET_SHARED_VALUES { kernel_disable_interrupts(); shared_values = values; kernel_enable_interrupts(); }
#define GET_SHARED_VALUES { kernel_disable_interrupts(); values = shared_values; kernel_enable_interrupts(); }

double speed_factor = 2;
double ball_vel_x = 0;
double ball_vel_y = 0;

void pong_reset() {  
  DEFINE_SHARED_VALUES
  GET_SHARED_VALUES
  
  // Reset scores
  values.score_left = 0;
  values.score_right = 0;
  speed_factor = 1;
  
  // Reset ball position
  values.ball_pos_x = LCD_SIZE_W / 2 - BALL_SIZE / 2;
  values.ball_pos_y = LCD_SIZE_H / 2 - BALL_SIZE / 2;

  // Reset pad positions
  values.left_pad_pos_y = LCD_SIZE_H / 2 - PAD_H / 2;
  values.right_pad_pos_y = LCD_SIZE_H / 2 - PAD_H / 2;

  SET_SHARED_VALUES
}

void pong_start() {
  DEFINE_SHARED_VALUES
  GET_SHARED_VALUES //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Reset ball position
  values.ball_pos_x = LCD_SIZE_W / 2 - BALL_SIZE / 2;
  values.ball_pos_y = LCD_SIZE_H / 2 - BALL_SIZE / 2;

  // Set ball velocity vector randomly
  ball_vel_x = BALL_START_SPEED * ((int)random1(2) * 2 - 1);
  ball_vel_y = 0;

  vector_rotate(&ball_vel_x, &ball_vel_y, (double)random2(-PI * 100 / 5, PI * 100 / 5) / 100);

  SET_SHARED_VALUES
}

void pong_draw_ball() {
  DEFINE_SHARED_VALUES
  GET_SHARED_VALUES
  
  // Draw ball
  lcd_fill_rect(values.ball_pos_x, values.ball_pos_y, values.ball_pos_x + BALL_SIZE, values.ball_pos_y + BALL_SIZE);
}

void pong_draw_pads() {
  DEFINE_SHARED_VALUES
  GET_SHARED_VALUES
  
  // Draw pads
  lcd_draw_rect(PAD_BORDER_W, values.left_pad_pos_y, PAD_BORDER_W + PAD_W, values.left_pad_pos_y + PAD_H);
  lcd_draw_rect(LCD_SIZE_W - PAD_BORDER_W - PAD_W, values.right_pad_pos_y, LCD_SIZE_W - PAD_BORDER_W, values.right_pad_pos_y + PAD_H);
}

void pong_draw_static() {
  DEFINE_SHARED_VALUES
  GET_SHARED_VALUES
  
  // Draw border
  lcd_draw_rect(0, 0, LCD_SIZE_W - 1, LCD_SIZE_H - 1);

  // Draw score
  int left_score_digit_count = values.score_left ? ceil(log10(values.score_left + 1)) : 1;
  lcd_draw_number(LCD_SIZE_W / 2 - SCORE_EDGE_PADDING - DIGIT_SIZE * left_score_digit_count - DIGIT_PADDING * (left_score_digit_count - 1), LCD_SIZE_H - DIGIT_SIZE * 2 - SCORE_EDGE_PADDING, DIGIT_SIZE, DIGIT_SIZE * 2, DIGIT_PADDING, values.score_left);
  lcd_draw_number(LCD_SIZE_W / 2 + SCORE_EDGE_PADDING, LCD_SIZE_H - DIGIT_SIZE * 2 - SCORE_EDGE_PADDING, DIGIT_SIZE, DIGIT_SIZE * 2, DIGIT_PADDING, values.score_right);
}

void pong_process() {
  // Get shared values
  DEFINE_SHARED_VALUES
  GET_SHARED_VALUES
  
  // Read button inputs
  bttn_states bttns = io_read_bttns();
  
  double pad_speed = PAD_START_SPEED;
  
  // Left player buttons
  if (bttns.bttn1) {
    values.left_pad_pos_y += pad_speed;
  }
  if (bttns.bttn2) {
    values.left_pad_pos_y -= pad_speed;
  }

  // Right player buttons
  if (bttns.bttn3) {
    values.right_pad_pos_y += pad_speed;
  }
  if (bttns.bttn4) {
    values.right_pad_pos_y -= pad_speed;
  }

  // Pads cannot be out of bounds
  values.left_pad_pos_y = clamp(values.left_pad_pos_y, 0, LCD_SIZE_H - PAD_H);
  values.right_pad_pos_y = clamp(values.right_pad_pos_y, 0, LCD_SIZE_H - PAD_H);
  
  // Move ball
  values.ball_pos_x += ball_vel_x * speed_factor;
  values.ball_pos_y += ball_vel_y * speed_factor;

  // Ball bounce top/bottom
  if (values.ball_pos_y < 0 || values.ball_pos_y + BALL_SIZE > LCD_SIZE_H) {
    ball_vel_y *= -1;
  }

  // Ball bounce pad left
  if (values.ball_pos_x < PAD_BORDER_W + PAD_W) {
    double ball_offset_from_pad_bottom = values.ball_pos_y + BALL_SIZE - values.left_pad_pos_y;
    
    if (ball_offset_from_pad_bottom > 0 && ball_offset_from_pad_bottom < PAD_H + BALL_SIZE) {
      ball_vel_x *= -1;
      ball_vel_y = ball_vel_x * (ball_offset_from_pad_bottom * (1 - (-1)) / (PAD_H + BALL_SIZE - 0) - 1) * 3 / 4;
      
      values.ball_pos_x = max(values.ball_pos_x, PAD_BORDER_W + PAD_W);
      
      speed_factor += SPEED_FACTOR_INC;
    }
  }

  // Ball bounce pad right
  if (values.ball_pos_x + BALL_SIZE > LCD_SIZE_W - PAD_BORDER_W - PAD_W) {
    double ball_offset_from_pad_bottom = values.ball_pos_y + BALL_SIZE - values.right_pad_pos_y;
    
    if (ball_offset_from_pad_bottom > 0 && ball_offset_from_pad_bottom < PAD_H + BALL_SIZE) {
      ball_vel_x *= -1;
      ball_vel_y = -ball_vel_x * (ball_offset_from_pad_bottom * (1 - (-1)) / (PAD_H + BALL_SIZE - 0) - 1) * 3 / 4;
      
      values.ball_pos_x = min(values.ball_pos_x, LCD_SIZE_W - PAD_BORDER_W - PAD_W - BALL_SIZE);
      
      speed_factor += SPEED_FACTOR_INC;
    }
  }

  SET_SHARED_VALUES

  // Ball out left
  if (values.ball_pos_x < 0) {
    values.score_right++;
    SET_SHARED_VALUES
    pong_start();
  }

  // Ball out right
  if (values.ball_pos_x + BALL_SIZE > LCD_SIZE_W) {
    values.score_left++;
    SET_SHARED_VALUES
    pong_start();
  }

  GET_SHARED_VALUES

  // Ball cannot be out of bounds
  values.ball_pos_x = clamp(values.ball_pos_x, 0, LCD_SIZE_W - BALL_SIZE);
  values.ball_pos_y = clamp(values.ball_pos_y, 0, LCD_SIZE_H - BALL_SIZE);
  
  // Set shared values
  SET_SHARED_VALUES
}

void game_pong_start() {
  pong_reset();
  pong_start();

  kernel_add_task(pong_draw_ball, 5000, 0);
  kernel_add_task(pong_process, 5000, 1);
  kernel_add_task(pong_draw_pads, 8000, 2);
  kernel_add_task(pong_draw_static, 15000, 3);
}



