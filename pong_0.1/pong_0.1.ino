/*
        Tek Block letter XY display with spin effect
        Created 31 Mar 2013 by Alan Wolke
        uses PORTs and R-2R ladder - note Y limited to 0-63, 6-bit
        R2R time constant 2.7k*10n or 3.9k * 6.8n
        works well with scope set for 0.5V/div XY mode
*/


#define FIGURE_DELAY  100  // trace delay in uS. Adjust as desired.
#define NUM_POINTS  13  // number of XY points in figure
#define TAMANHO_BARRA 16
#define COMPRIMENTO_BARRA 8
#define TAMANHO_X 254
#define TAMANHO_Y 254

//x coords on port d
unsigned int x_points[NUM_POINTS];

//y coords on port b - limited to 0 - 63 with map function later
unsigned int y_points[NUM_POINTS];

// some initial starting values...
//byte x_min = 500;
//byte x_max = 0;
//int y_min = 500;
//int y_max = 0;
int V1 = 0;
int V2 = 0;


void setup()
{
  Serial.begin(9600);
  DDRD = B11111111; //initialize PORTs for writing
  DDRB = B00111111;
  DDRC = B00110000;

  byte t;

  //  for (t = 0; t < NUM_POINTS; t++) //get min, max and mid values of X
  //  {
  //    x_min = min(x_min, x_points[t]);
  //    x_max = max(x_max, x_points[t]);
  //    y_min = min(y_min, y_points[t]);
  //    y_max = max(y_max, y_points[t]);
  //  }
}

void loop()
{
  byte t;
  byte sx;
  float p;
  byte flip;
  if (digitalRead(A0) or digitalRead(A1) or digitalRead(A2) or digitalRead(A3)) {
    if (V1 < TAMANHO_Y - TAMANHO_BARRA)V1 += digitalRead(A3);
    if (V1 > 0) V1 -= digitalRead(A2);
    if (V2 < TAMANHO_Y - TAMANHO_BARRA)V2 += digitalRead(A0);
    if (V2 > 0) V2 -= digitalRead(A1);
    delay(50); //tirar esta mierda daqui
    Serial.print("V1 = ");
    Serial.print(V1);
    Serial.print(" // V2 = ");
    Serial.println(V2);
    //    Serial.println(((PORTC << 2) & B11000000) | (PORTB & B00111111));
  }

  x_points[0] = 0;
  y_points[0] = 0;
  x_points[1] = 0;
  y_points[1] = V1;
  x_points[2] = COMPRIMENTO_BARRA;
  y_points[2] = V1;
  x_points[3] = COMPRIMENTO_BARRA;
  y_points[3] = V1 + TAMANHO_BARRA;
  x_points[4] = 0;
  y_points[4] = V1 + TAMANHO_BARRA;
  x_points[5] = 0;
  y_points[5] = TAMANHO_Y;
  x_points[6] = TAMANHO_X;
  y_points[6] = TAMANHO_Y;
  x_points[7] = TAMANHO_X;
  y_points[7] = V2 + TAMANHO_BARRA;
  x_points[8] = TAMANHO_X - COMPRIMENTO_BARRA;
  y_points[8] = V2 + TAMANHO_BARRA;
  x_points[9] = TAMANHO_X - COMPRIMENTO_BARRA;
  y_points[9] = V2;
  x_points[10] = TAMANHO_X;
  y_points[10] = V2;
  x_points[11] = TAMANHO_X;
  y_points[11] = 0;
  x_points[12] = 0;
  y_points[12] = 0;
  
  for (t = 0; t < NUM_POINTS; t++)  // run through the X points
  {
    PORTD = x_points[t];
    PORTB = (((y_points[t]) & B00111111) | (PORTB & B11000000));
    //PORTB = (((y_points[t]>>2) & B00111111) | (PORTB & B11000000));//n usa essta caraia
    PORTC = (((y_points[t]) >> 2) & (B00110000) | (PORTC & B11001111));
    delayMicroseconds(FIGURE_DELAY);    // wait FIGURE_DELAY microseconds
  }
}
