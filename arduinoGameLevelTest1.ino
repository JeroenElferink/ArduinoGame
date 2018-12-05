// demo for displaying bitmaps on an 320 x 240 tft lcd screen with Arduino

#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "ArduinoNunchuk/ArduinoNunchuk.h" // library for nunchuk


// define SPI pins
#define TFT_DC 9    // DC pin
#define TFT_CS 10   // Chip Select pin

// define some common colors via hex codes
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY    0xD6BA
#define BROWN   0xB222

// define background color (black)
#define BACKGROUND BLACK

// define screensize
#define SCREEN_WIDTH 240 //the screenwidth is 240 pixels
#define SCREEN_HEIGHT 320 // the screenheight is 320 pixels


const int16_t width = 45;     // set width of image, image cannot be wider than the screenwidth (240)
const int16_t height = 66;   // set height of image, image cannot be wider than the screenheight (320)

// start SPI communication with the display shield
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// constructor for a ArduinoNunchuk object
ArduinoNunchuk myNunchuck;

// this is the name of the image which is included in the graphics.c file
extern uint8_t bombArray[];
extern uint8_t frameArray[];
extern uint8_t blockArray[];
extern uint8_t spookBodyArray[];
extern uint8_t ogenBovenArray[];
extern uint8_t ogenLinksArray[];
extern uint8_t ogenOnderArray[];
extern uint8_t ogenRechtsArray[];
extern uint8_t oogwitArray[];


// this is an array in which the colornames are stored
uint16_t colorArray[] = {BLACK, BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE, GREY};

uint16_t coordinates[73][2] = {
	{22,73}, {22,94}, {22,115}, {22,136}, {22,157}, {22,178}, {22,199},
	{43,73}, {43,115}, {43,157}, {43,199},
	{64,31}, {64,52}, {64,73}, {64,94}, {64,115}, {64,136}, {64,157}, {64,178}, {64,199},
	{85,31}, {85,73}, {85,115}, {85,157}, {85,199},
	{106,31}, {106,52}, {106,73}, {106,94}, {106,115}, {106,136}, {106,157}, {106,178}, {106,199},
	{127,31}, {127,73}, {127,115}, {127,157}, {127,199},
	{148,31}, {148,52}, {148,73}, {148,94}, {148,115}, {148,136}, {148,157}, {148,178}, {148,199},
	{169,31}, {169,73}, {169,115}, {169,157}, {169,199},
	{190,31}, {190,52}, {190,73}, {190,94}, {190,115}, {190,136}, {190,157}, {190,178}, {190,199},
	{211,31}, {211,73}, {211,115}, {211,157},
	{232,31}, {232,52}, {232,73}, {232,94}, {232,115}, {232,136}, {232,157}
};


int x; // x coordinate
int y; // y coordinate
int xold; // x coordinate of the previous position
int yold; // y coordinate of the previous position
int analogXOld; // old x value of the nunchuck
int analogYOld; // old y value of the nunchuck

int startX1 = 22;
int startY1 = 31;
int startX2;
int startY2;

int curX1 = startX1;
int curY1 = startY1;



int16_t main (void){
	init();
	myNunchuck.init();
	tft.begin();
	tft.setRotation(1);
	
	Serial.begin(9600);
	
	redrawScreen();
	frame(0, 0);
	drawSpookPlayer(startX1, startY1);
	
	while(1){
				
		walkWithNunchuk();
						
		/*
		drawRandomLevel();
		*/
		
				
		
		/*
		for(int i=43; i<=199; i+=4){
			drawSpookPlayer(22, i);
			delay(100);
			undrawSpookPlayer(22, i);
		}
		*/	
		
		//delay(2000);
		
	}
	
}

void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
	int16_t i, j, byteWidth = (w + 7) / 8;
	uint8_t byte;
	for(j=0; j<h; j++) {
		for(i=0; i<w; i++) {
			if(i & 7) byte <<= 1;
			else byte = pgm_read_byte(bitmap + j * byteWidth + i / 8);
			if(byte & 0x80) tft.drawPixel(x+i, y+j, color);
		}
	}
}

// function to overdraw the previously drawn bitmap with the backgroundcolor
void undrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t backgroundColor) {
	int16_t i, j, byteWidth = (w + 7) / 8;
	uint8_t byte;
	for(j=0; j<h; j++) {
		for(i=0; i<w; i++) {
			if(i & 7) byte <<= 1;
			else byte = pgm_read_byte(bitmap + j * byteWidth + i / 8);
			if(byte & 0x80) tft.drawPixel(x+i, y+j, backgroundColor);
		}
	}
}



// function to redraw the background over everything that's currently on the screen (sort of a refresh/reset function)
void redrawScreen(){
	tft.fillScreen(BACKGROUND);   // black background
	//tft.drawRect(0,0,319,240,WHITE);  // draw a white frame
}

/*
//function that lets poppetjes walk down from the left side of the screen
void playerFront(int16_t x, int16_t y){
	drawBitmap(x, y, playerFrontArray, 13, 20, RED);
	
}

void playerLeft(int16_t x, int16_t y){
	drawBitmap(x, y, playerLeftArray, 45, 66, RED);
	
}

void playerRight(int16_t x, int16_t y){
	drawBitmap(x, y, playerRightArray, 45, 66, BLUE);
	
}

void playerBack(int16_t x, int16_t y){
	drawBitmap(x, y, playerBackArray, 13, 20, BLUE);
	
}
*/

void bomb(int16_t x, int16_t y){
	drawBitmap(x, y, bombArray, 38, 31, BLACK);
	
}

void frame(int16_t x, int16_t y){
	drawBitmap(x, y, frameArray, 273, 240, GREY);
	
}

void block(int16_t x, int16_t y){
	drawBitmap(x, y, blockArray, 19, 19, BROWN);
	
}

void drawRandomLevel(){
	uint16_t alreadyPlaced[73];
	for(int i = 0; i <= 50; i++){
		uint16_t random = rand() % (73 + 1);
		if(alreadyExistsInArray(random, alreadyPlaced)) {
			i--;
			} else {
			alreadyPlaced[random] = random;
			uint16_t x1 = coordinates[random][0];
			uint16_t y1 = coordinates[random][1];
			if(x1 >= 22 && y1 >= 31){
				block(x1, y1);
				
			}
		}
	}
	memset(alreadyPlaced,'0',sizeof(alreadyPlaced));
	
}


bool alreadyExistsInArray(int val, int *arr){
	int i;
	for (i=0; i <= 73; i++) {
		if (arr[i] == val)
		return true;
	}
	return false;
}




// new functions for walking and stuff
void drawSpookPlayer(int16_t x, int16_t y){
	drawBitmap(x, y, spookBodyArray, 19, 19, RED);
	drawBitmap(x, y, oogwitArray, 19, 19, WHITE);
	drawBitmap(x, y, ogenOnderArray, 19, 19, BLUE);
}

void undrawSpookPlayer(int16_t x, int16_t y){
	undrawBitmap(x, y, spookBodyArray, 19, 19, BACKGROUND);
	undrawBitmap(x, y, oogwitArray, 19, 19, BACKGROUND);
	undrawBitmap(x, y, ogenOnderArray, 19, 19, BACKGROUND);
}

void moveRight(){
	int newX1 = curX1 + 21;
	//int newY1 = curY1 + 21;
	undrawSpookPlayer(curX1, curY1);
	drawSpookPlayer(newX1, curY1);
	curX1 = newX1;
	//curY1 = newY1;
}

void moveLeft(){
	int newX1 = curX1 - 21;
	undrawSpookPlayer(curX1, curY1);
	drawSpookPlayer(newX1, curY1);
	curX1 = newX1;
}

void moveUp(){
	int newY1 = curY1 - 21;
	undrawSpookPlayer(curX1, curY1);
	drawSpookPlayer(curX1, newY1);
	curY1 = newY1;
}

void moveDown(){
	int newY1 = curY1 + 21;
	undrawSpookPlayer(curX1, curY1);
	drawSpookPlayer(curX1, newY1);
	curY1 = newY1;
}


void walkWithNunchuk(){
	myNunchuck.update(); // update the nunchuck data
	
	if (analogXOld != myNunchuck.analogX || analogYOld != myNunchuck.analogY) { // if either the x or y coordinate has changed, we have to redraw the spookje
		analogXOld = myNunchuck.analogX; // update X value
		analogYOld = myNunchuck.analogY; // update Y value
		if (myNunchuck.analogX>200){
			Serial.println(myNunchuck.analogX);
			moveRight();
			}else if (myNunchuck.analogX<40){
			Serial.println(myNunchuck.analogX);
			moveLeft();
			}else if (myNunchuck.analogY>200){
			Serial.println(myNunchuck.analogY);
			moveUp();
			}else if (myNunchuck.analogY<40){
			Serial.println(myNunchuck.analogY);
			moveDown();
		}
	}
	
}


void testNunchuk(){
	myNunchuck.update();

	Serial.print(myNunchuck.analogX, DEC);
	Serial.print(' ');
	Serial.print(myNunchuck.analogY, DEC);
	Serial.print(' ');
	Serial.print(myNunchuck.accelX, DEC);
	Serial.print(' ');
	Serial.print(myNunchuck.accelY, DEC);
	Serial.print(' ');
	Serial.print(myNunchuck.accelZ, DEC);
	Serial.print(' ');
	Serial.print(myNunchuck.zButton, DEC);
	Serial.print(' ');
	Serial.println(myNunchuck.cButton, DEC);
}