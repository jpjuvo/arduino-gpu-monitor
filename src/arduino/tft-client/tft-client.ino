#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"
#include "math.h"

MCUFRIEND_kbv tft;

// *** Define Name of Color
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define GREY    0x2108
// Meter colour schemes
#define RED2RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5

// GPU variables
int gpu_util_p = 50;
int mem_util_p = 50;
String gpu_name = "";
int current_temp = 0;
int max_temp = 100;
int used_mem = 0;
int total_mem = 100;


// Coordinates
int center_x = 170;
int titles_x_coord = 180;
int values_x_coord = 185;

String str; 
String key;
String val; 
String field_str;

void setup() {
  tft.reset();
  Serial.begin(9600);
  Serial.setTimeout(5000) ;
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1); // Set Rotation at 0 degress (default)
  tft.invertDisplay(true);

  tft.fillScreen(BLACK); 
  
  // Titles
  tft.setTextSize (2);
  tft.setTextColor (WHITE,BLACK);
  
  tft.setCursor (titles_x_coord, 5);
  tft.print ("GPU");

  tft.setCursor (titles_x_coord, 57);
  tft.print ("Temperature C");

  
  tft.setCursor (titles_x_coord, 107);
  tft.print ("Used memory MB");

  tft.setCursor (titles_x_coord, 157);
  tft.print ("Total memory MB");
  
  //Design Interface (lines) x,y,w,h
  // Left horizontal
  tft.fillRect(0, 160, center_x, 4, BLUE);
  // center vertical
  tft.fillRect(center_x, 0, 4, 360, BLUE);
  // Right horizontal lines - 50 spacing (- 4 for lines)
  tft.fillRect(center_x, 50, 320, 4, BLUE);
  tft.fillRect(center_x, 98, 320, 4, BLUE);
  tft.fillRect(center_x, 148, 320, 4, BLUE);
  tft.fillRect(center_x, 198, 320, 4, BLUE);

  update();
  update_display_values();
}

void update_display_values(){
  tft.setTextSize (2);
  tft.setTextColor ( RED , BLACK);
  
  // Name
  tft.setCursor(values_x_coord, 28);
  tft.print(gpu_name);
  
  tft.setCursor(values_x_coord, 78);
  tft.print(current_temp);

  tft.setCursor(values_x_coord, 128);
  tft.print(used_mem);

  tft.setCursor(values_x_coord, 178);
  tft.print(total_mem);
}

void update() {
  Serial.println("update");
  int counter = 0;
  while(counter < 100) {
    delay(100);
    while(Serial.available() > 0 ){
      str = Serial.readString();
      // Split into fields
      int index = 0;
      while(getValue(str, ',', index).length() > 1) {
        field_str = getValue(str, ',', index);
        key = getValue(field_str, ':', 0);
        val = getValue(field_str, ':', 1);
        
        if(key.equals("name")) {
          gpu_name = val;
        }
        if(key.equals("gpu_util_p")) {
          gpu_util_p = val.toInt();
        }
        if(key.equals("mem_util_p")) {
          mem_util_p = val.toInt();
        }
        if(key.equals("curr_temp")){
          current_temp = val.toInt();
        }
        if(key.equals("used_mem")) {
          used_mem = val.toInt();
        }
        if(key.equals("total_mem")) {
          total_mem = val.toInt();
        }
        
        index++;
      }
      return;
    }
    counter++;
  }
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() {
  int xpos = 0, ypos = 5, gap = 4, radius = 40;
  // Draw a large meter
  xpos = 5, ypos = 5, gap = 65, radius = 65;
  ringMeter(gpu_util_p, 1, 99, xpos, ypos, radius, "GPU %", GREEN2RED); 

  // Draw a large meter
  xpos = 5, ypos = 170, gap = 65, radius = 65;
  ringMeter(mem_util_p, 1, 99, xpos, ypos, radius, "Mem %", GREEN2RED);

  delay(10000);

  update();
  update_display_values();
}

int ringMeter(int value, int vmin, int vmax, int x, int y, int r, char *units, byte scheme)
{
  
  x += r; y += r;   
  int w = r / 3;    
  int angle = 150;  
  int v = map(value, vmin, vmax, -angle, angle); 
  byte seg = 3; 
  byte inc = 6; 
  
  int colour = BLACK;
  
  for (int i = -angle + inc / 2; i < angle - inc / 2; i += inc) {
    
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (r - w) + x;
    uint16_t y0 = sy * (r - w) + y;
    uint16_t x1 = sx * r + x;
    uint16_t y1 = sy * r + y;
    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * 0.0174532925);
    float sy2 = sin((i + seg - 90) * 0.0174532925);
    int x2 = sx2 * (r - w) + x;
    int y2 = sy2 * (r - w) + y;
    int x3 = sx2 * r + x;
    int y3 = sy2 * r + y;
    if (i < v) { // Fill in coloured segments with 2 triangles
      switch (scheme) {
        case 0: colour = RED; break; // Fixed colour
        case 1: colour = GREEN; break; // Fixed colour
        case 2: colour = BLUE; break; // Fixed colour
        case 3: colour = rainbow(map(i, -angle, angle, 0, 127)); break; // Full spectrum blue to red
        case 4: colour = rainbow(map(i, -angle, angle, 70, 127)); break; // Green to red (high temperature etc)
        case 5: colour = rainbow(map(i, -angle, angle, 127, 63)); break; // Red to green (low battery etc)
        default: colour = BLUE; break; // Fixed colour
      }
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);
      //text_colour = colour; // Save the last colour drawn
    }
    else // Fill in blank segments
    {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, BLACK);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, BLACK);
    }
  }
  // Convert value to a string
  char buf[10];
  byte len = 0;  if (value > 999) len = 5;
  dtostrf(value, 1, len, buf);
    //buf[len] = 1; 
  //buf[len] = 2; // Add blanking space and terminator, helps to centre text too!
  // Set the text colour to default
  tft.fillRect(x - 25, y - 25, 50, 50, BLACK);
  tft.setTextColor(GREEN, BLACK);
  tft.setCursor(x - 20, y - 20); tft.setTextSize(4);
  tft.print(buf);
  tft.setTextColor(GREEN, BLACK);
  tft.setCursor(x - 35, y + 65); tft.setTextSize(2);
  tft.print(units); // Units display
  // Calculate and return right hand side x coordinate
  return x + r;
}

unsigned int rainbow(byte value)
{
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = blue through to 127 = red
  byte red = 0; // Red is the top 5 bits of a 16 bit colour value
  byte green = 0;// Green is the middle 6 bits
  byte blue = 0; // Blue is the bottom 5 bits
  byte quadrant = value / 32;
  if (quadrant == 0) {
    blue = 31;
    green = 2 * (value % 32);
    red = 0;
  }
  if (quadrant == 1) {
    blue = 31 - (value % 32);
    green = 63;
    red = 0;
  }
  if (quadrant == 2) {
    blue = 0;
    green = 63;
    red = value % 32;
  }
  if (quadrant == 3) {
    blue = 0;
    green = 63 - 2 * (value % 32);
    red = 31;
  }
  return (red << 11) + (green << 5) + blue;
}
// #########################################################################
// Return a value in range -1 to +1 for a given phase angle in degrees
// #########################################################################
float sineWave(int phase) {
  return sin(phase * 0.0174532925);
}
