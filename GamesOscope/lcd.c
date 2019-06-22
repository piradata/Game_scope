#include "defines.h"

void lcd_draw_pixel(int x, int y) {
  dac_write(x * DAC_MAX_CHAN_0 / LCD_SIZE_W, y * DAC_MAX_CHAN_1 / LCD_SIZE_H);
}

void lcd_fill_rect(int x0, int y0, int x1, int y1) {
  if (x0 > x1) {
    swap(x0, x1);
  }
  if (y0 > y1) {
    swap(y0, y1);
  }

  for (int x = x0; x <= x1; x++) {
    for (int y = y0; y <= y1; y++) {
      lcd_draw_pixel(x, y);
    }
  }
}

void lcd_draw_line(int x0, int y0, int x1, int y1) {
  int dy = y1 - y0;
  int dx = x1 - x0;
  int stepx, stepy;

  if (dy < 0) {
    dy = -dy;
    stepy = -1;
  }
  else {
    stepy = 1;
  }

  if (dx < 0) {
    dx = -dx;
    stepx = -1;
  } else {
    stepx = 1;
  }

  dy *= 2;
  dx *= 2;

  lcd_draw_pixel(x0, y0); // Draw the first pixel

  if (dx > dy) {
    int fraction = dy - (dx >> 1);
    while (x0 != x1) {
      if (fraction >= 0) {
        y0 += stepy;
        fraction -= dx;
      }
      x0 += stepx;
      fraction += dy;
      lcd_draw_pixel(x0, y0);
    }
  }
  else {
    int fraction = dx - (dy >> 1);
    while (y0 != y1) {
      if (fraction >= 0) {
        x0 += stepx;
        fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      lcd_draw_pixel(x0, y0);
    }
  }
}

void lcd_fill_circle(int x0, int y0, int r) {
  int r2 = r * r;
  int area = r2 << 2;
  int rr = r << 1;

  for (int i = 0; i < area; i++) {
    int tx = (i % rr) - r;
    int ty = (i / rr) - r;

    if (tx * tx + ty * ty <= r2) {
      lcd_draw_pixel(x0 + tx, y0 + ty);
    }
  }
}

void lcd_draw_circle_octant(int x0, int y0, int r, int xm1, int ym1, int xm2, int ym2) {
  int x = r;
  int y = 0;
  int err = 0;

  while (x >= y) {
    lcd_draw_pixel(x0 + x * xm1 + y * ym1, y0 + x * xm2 + y * ym2);
    
    if (err <= 0) {
      y += 1;
      err += 2 * y + 1;
    }
    if (err > 0) {
      x -= 1;
      err -= 2 * x + 1;
    }
  }
}

void lcd_draw_circle(int x, int y, int r) {
  lcd_draw_circle_octant(x, y, r,  1,  0,  0,  1); // 1, ccw
  lcd_draw_circle_octant(x, y, r,  0,  1,  1,  0); // 2, cw
  lcd_draw_circle_octant(x, y, r,  0, -1,  1,  0); // 3, ccw
  lcd_draw_circle_octant(x, y, r, -1,  0,  0,  1); // 4, cw
  lcd_draw_circle_octant(x, y, r, -1,  0,  0, -1); // 5, ccw
  lcd_draw_circle_octant(x, y, r,  0, -1, -1,  0); // 6, cw
  lcd_draw_circle_octant(x, y, r,  0,  1, -1,  0); // 7, ccw
  lcd_draw_circle_octant(x, y, r,  1,  0,  0, -1); // 8, cw
}

void lcd_draw_rect(int x0, int y0, int x1, int y1) {
  lcd_draw_line(x0, y0, x1, y0);
  lcd_draw_line(x1, y0, x1, y1);
  lcd_draw_line(x1, y1, x0, y1);
  lcd_draw_line(x0, y1, x0, y0);
}

int digit_lines[10][16] = {
  {6, 0, 1, 1, 3, 3, 5, 5, 4, 4, 2, 2, 0},
  {2, 1, 3, 3, 5},
  {5, 0, 1, 1, 3, 3, 2, 2, 4, 4, 5},
  {5, 0, 1, 1, 3, 3, 5, 5, 4, 3, 2},
  {4, 0, 2, 2, 3, 3, 1, 3, 5},
  {5, 1, 0, 0, 2, 2, 3, 3, 5, 5, 4},
  {6, 1, 0, 0, 2, 2, 4, 4, 5, 5, 3, 3, 2},
  {3, 0, 1, 1, 3, 3, 5},
  {7, 0, 1, 1, 3, 3, 5, 5, 4, 4, 2, 2, 0, 2, 3},
  {6, 3, 1, 1, 0, 0, 2, 2, 3, 3, 5, 5, 4},
};

double digit_x_pos_for_point(int p) {
  switch (p) {
    case 0: case 2: case 4: return 0;
    case 1: case 3: case 5: return 1;
    default: return 0;
  }
}

double digit_y_pos_for_point(int p) {
  switch (p) {
    case 4: case 5: return 0;
    case 2: case 3: return 0.5;
    case 0: case 1: return 1;
    default: return 0;
  }
}

void lcd_draw_digit(int x, int y, int w, int h, int d) {
  d = clamp(d, 0, 9);

  int line_count = digit_lines[d][0];
  for (int i = 1; i < line_count * 2 + 1; i += 2) {
    int p1 = digit_lines[d][i];
    int p2 = digit_lines[d][i + 1];

    lcd_draw_line(
      x + digit_x_pos_for_point(p1) * w, y + digit_y_pos_for_point(p1) * h,
      x + digit_x_pos_for_point(p2) * w, y + digit_y_pos_for_point(p2) * h
    );
  }
}

void lcd_draw_number(int x, int y, int w, int h, int spacing, int n) {  
  if (n) {
    for (int g = floor(log10(n)); g >= 0; g--) {
      int d = n / pow(10, g);
      lcd_draw_digit(x, y, w, h, d);
      n -= d * pow(10, g);
      x += w + spacing;
    }
  }
  else {
    lcd_draw_digit(x, y, w, h, 0);
  }
}
