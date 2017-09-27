#include <Adafruit_NeoPixel.h>
// Add an if i = 0 and this is not the first time, check k- the i reset value and go up
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(450, 3, NEO_GRB + NEO_KHZ800);  //44 Needs to be changed to 330 when running reg. code. Pin (3) matters too
void setup() {
  pixels.begin();
  pixels.show();
  pixels.setBrightness(10);
  Serial.begin(115200);
  randomSeed(analogRead(0)); // Set the seed for the pseudo-random number generator. If analog pin 0 is disconnected, it will generate random noise, suitable for our purposes.
  //delay(100);
} // void setup()

const bool serialon = false; //This boolean is used for the testing of rain only without the camera. This does not turn off Serial, just the camera input

int tickStart = 0; // Only exists for testing on circuits.io, should mostly be 0
int length = 45; // Please only change these void loop variables and the delay, no other code
int rain_space = 9; // should always be less than length (otherwise this code is inefficient for the purpose)
int column_num = 10; // The number of columns (counted by humans) in the code
int offst = 0; // This is the number of pixels offset a raindrop will be. This is based off of current_column later on and is defined here
int rand1 = 0; // Intended to be the first value that gets changed for randomness, cycles every 10 tick values.
int rand2 = 0; // Intended to be the second value that gets changed for randomness, cycles every 10 rand1 values.

int rand1prev = 0; // The previous value of rand1, used to allow seamlessness when tick resets
int rand2prev = 0; // The previous value of rand2, used to allow seamlessness when tick resets

// If anyone cares about having a start up animation, ask me and I will add a boolean here
int camera [10] = {45, 45, 45, 45, 45, 45, 45, 45, 45, 45}; // each of these values is the y value of the edge divided to split into 45 parts, x values in (divided by 10), May not be needed and could be defined alongside sereal

int ran = 0; // Holds the random number generated for each iteration
int offset [4] = {5, 3, 0, 1}; //Like random, this is an array to make sure line offset is maintained

int topRed = 71; //71 The next lines of code refer to the color values of pixels. This is the command center for changing color.
int topGreen = 116; //116
int topBlue = 255; //255

int botRed = 51; //51
int botGreen = 106; //106
int botBlue = 72; //72

int midRed = 71; //71
int midGreen = 116; //116
int midBlue = 156; //156

void loop() { // Runs repeatedly
  for (int tick = tickStart; tick > -2; tick++) {

    for (int l = 0; l < column_num; l++) { // l is a variable that is used to check for a column_num amount of Serial values
      if (serialon) {
        while (Serial.available() == 0); // Wait for Serial input
        int inChar = length - (int)Serial.read();
        camera [l] = inChar;
      }
    }

    int dropStart = floor(tick / rain_space); //I need this to find a value close to what k should be so the arduino does less work (I would like help)
    for (int current_column = 0; current_column < column_num; current_column++) { //
      offst = offset [(3 * current_column) % 4]; // Sets offst relative to the current column being changed
      for (int dropNumber = dropStart; dropNumber > -2; dropNumber++) { //For each tick value and current_column, drop number checks if a raindrop should be in a place every rainspace tiles when being ofset by the rand value
        if (tick == 89) { //The check if the tick should be reset
          tick = -3; //resets tick
          dropNumber = -3; //makes it so no code is run when tick is not the intended value, these two lines end the code
        } else {
          if (dropNumber == -1) { //Debugging for negative k values
            dropNumber++;
          }
          if ((dropNumber >= 0) && (dropNumber <= floor(length / rain_space))) {
            randomSeed((current_column % 10) + (dropNumber % 10) + (rand1prev % 10) + (rand2prev % 10)); // Re-seed the pseudo-random number generator
          } else {
            randomSeed((current_column % 10) + (dropNumber % 10) + (rand1 % 10) + (rand2 % 10)); // Re-seed the psuedo-random number generator
          }
          ran = random(0, 5);
          int pixelPlace = tick - (dropNumber * rain_space + ran) + length - offst; //chooses a position from 0 to length-1 in which to place rain
          if ((current_column == 4) && (dropNumber == 4)) {
            /*Serial.print("j k pixelPlace ran (j*length)+pixelPlace");
              Serial.print(current_column);
              Serial.print(" ");
              Serial.print(dropNumber);
              Serial.print(" ");
              Serial.print(pixelPlace);
              Serial.print(" ");
              Serial.print(ran);
              Serial.print(" ");
              Serial.println((current_column*length)+pixelPlace);*/
          }
          if (current_column % 2 == 0) { //Because this was not turned into a matrix, we must check if an increase in the pixel being called on moves down or up
            if ((pixelPlace > camera [current_column]) && (pixelPlace <= length)) {
              for (int l = camera [current_column]; l < length; l++) { //Turns off rain which is under the camera's input area
                pixels.setPixelColor((current_column * length) + l, pixels.Color(0, 0, 0));
              }
            } if ((pixelPlace >= camera [current_column]) && (pixelPlace <= camera [current_column] + 2)) {//length is an arbitrary location where the rain stops. Cough Cough Hint Hint
              pixels.setPixelColor((current_column * length) + pixelPlace - 3, pixels.Color(0, 0, 0));
              if (pixelPlace <= camera [current_column] + 1) {
                pixels.setPixelColor((current_column * length) + pixelPlace - 2, pixels.Color(topRed, topGreen, topBlue));
                if (pixelPlace <= camera [current_column]) {
                  pixels.setPixelColor((current_column * length) + pixelPlace - 1, pixels.Color(midRed, midGreen, midBlue));
                }
              }
            } else if (pixelPlace >= 0 && pixelPlace <= camera [current_column] - 1) {
              pixels.setPixelColor((current_column * length) + pixelPlace, pixels.Color(botRed, botGreen, botBlue));
              if ((pixelPlace) >= 1) {
                pixels.setPixelColor((current_column * length) + pixelPlace - 1, pixels.Color(midRed, midGreen, midBlue));
                if (pixelPlace >= 2) {
                  pixels.setPixelColor((current_column * length) + pixelPlace - 2, pixels.Color(topRed, topGreen, topBlue));
                  if (pixelPlace >= 3) {
                    pixels.setPixelColor((current_column * length) + pixelPlace - 3, pixels.Color(0, 0, 0));
                  }
                }
              }
            } else if (pixelPlace < 0) {
              dropNumber = -3;
            }
          }
          if (current_column % 2 == 1) { //could be else, kept for readability, used to see if an increase in pixel value moves down or up
            if ((pixelPlace > camera [current_column]) && (pixelPlace <= length)) {
              for (int l = camera [current_column]; l < length; l++) { //Used to make sure every light below camera is shut off
                pixels.setPixelColor(((current_column + 1)*length - 1) - l, pixels.Color(0, 0, 0));
              }
            } if (pixelPlace >= camera [current_column] && pixelPlace <= camera [current_column] + 2) { //length is an arbitrary location where the rain stops. Cough Cough Hint Hint
              pixels.setPixelColor(((current_column + 1)*length - 1) - pixelPlace + 3, pixels.Color(0, 0, 0));
              if (pixelPlace <= camera [current_column] + 1) {
                pixels.setPixelColor(((current_column + 1)*length - 1) - pixelPlace + 2, pixels.Color(topRed, topGreen, topBlue));
                if (pixelPlace <= camera [current_column]) {
                  pixels.setPixelColor(((current_column + 1)*length - 1) - pixelPlace + 1, pixels.Color(midRed, midGreen, midBlue));
                }
              }
            } else if ((pixelPlace >= 0) && (pixelPlace <= camera [current_column] - 1)) {
              pixels.setPixelColor(((current_column + 1)*length - 1) - pixelPlace , pixels.Color(botRed, botGreen, botBlue));
              if (pixelPlace >= 1) {
                pixels.setPixelColor(((current_column + 1)*length - 1) - pixelPlace + 1, pixels.Color(midRed, midGreen, midBlue));
                if (pixelPlace >= 2) {
                  pixels.setPixelColor(((current_column + 1)*length - 1) - pixelPlace + 2, pixels.Color(topRed, topGreen, topBlue));
                  if (pixelPlace >= 3) {
                    pixels.setPixelColor(((current_column + 1)*length - 1) - pixelPlace + 3, pixels.Color(0, 0, 0));
                  }
                }
              }
            } else  if (pixelPlace < 0) {
              dropNumber = -3;
            }
          }
        }
      }
    }

    if (!serialon) {
      //delay(6); // Delay for 6 milliseconds
    }

    pixels.show();
    tickStart = -1; // Only for testing

    int lightning_column = random(0, column_num); // Random column

    if ((tick + rand1 + rand2) % 32 == 13) {
      for (int l = 0; l < length; l++) {
        pixels.setPixelColor((lightning_column * length) + l, pixels.Color(255, 255, 100)); //Turns on lightning and sets the color
      }
    }

    pixels.show();

    if (tick == -3) {
      rand1prev = rand1;
      rand2prev = rand2;
      rand1++;
      if (rand1 == 10) {
        rand2++;
        rand1 = 0;
        if (rand2 == 10) {
          rand2 = 0;
        }
      }
    }
  }
}
