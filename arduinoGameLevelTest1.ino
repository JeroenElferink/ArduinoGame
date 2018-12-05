// demo for displaying bitmaps on an 320 x 240 tft lcd screen with Arduino

//#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

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

const int16_t width = 45;     // set width of image, image cannot be wider than the screenwidth (240)
const int16_t height = 66;   // set height of image, image cannot be wider than the screenheight (320)

// start SPI communication with the display shield
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// this is the name of the image which is included in the graphics.c file
extern uint8_t playerFrontArray[];
extern uint8_t playerLeftArray[];
extern uint8_t playerRightArray[];
extern uint8_t bombArray[];
extern uint8_t playerBackArray[];
extern uint8_t frameArray[];
extern uint8_t blockArray[];


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


int16_t main (void){
	init();
	tft.begin();
	tft.setRotation(1);
	
	while(1){
		redrawScreen();
		frame(0, 0);
		
		//playerFront(60, 100);
		//playerLeft(110, 100);
		//playerRight(160, 100);
		//playerBack(210, 100);
		//bomb(260, 100);
		//walkingPoppetjes();
		
		drawRandomLevel();
		
		
		delay(10000);
		
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

//void walkRight(int16_t x, int16_t, y) {
//	drawBitmap(x, y, frameArray, 272, 240, BLACK);
//}

// function to draw the image in random places and colors, takes the amount of images to be drawn as only parameter
// void randomPoppetje(int amountOfPoppetjes){
// 	int x, y, c;
// 	uint16_t randomColor;
// 	for (int i=0; i<amountOfPoppetjes; i++){
// 		x = random(320);  // get a random x coordinate
// 		y = random(240);  // get a random y coordinate
// 		c = random(1, 9);    // get a random number, this is to get a random color from the colorArray
// 		randomColor = colorArray[c];
// 		drawBitmap(x, y, poppetje, width, height, randomColor);
// 		delay(0);
// 	}
// }

//function that lets two poppetjes walk in opposite direction from the right and left side of the screen
void walkingPoppetjes(){
	for(int i=10; i<250; i=i+20){
		drawBitmap(i, 30, playerRightArray, width, height, RED);
		drawBitmap(250-i, 170, playerLeftArray, width, height, BLUE);
		delay(400);
		undrawBitmap(i, 30, playerRightArray, width, height, BACKGROUND);
		undrawBitmap(250-i, 170, playerLeftArray, width, height, BACKGROUND);
	}
}

// function that lets poppetjes walk down from the left side of the screen
// void walkingDownPoppetjes(){
// 	drawBitmap(10, 40, poppetje, width, height, WHITE);
// 	delay(1000);
// 	drawBitmap(60, 70, poppetje, width, height, WHITE);
// 	delay(1000);
// 	drawBitmap(110, 100, poppetje, width, height, WHITE);
// 	delay(1000);
// 	drawBitmap(160, 120, poppetje, width, height, WHITE);
// 	delay(1000);
// 	drawBitmap(210, 150, poppetje, width, height, WHITE);
// 	delay(1000);
//
// 	undrawBitmap(10, 40, poppetje, width, height, BACKGROUND);
// 	delay(1000);
// 	undrawBitmap(60, 70, poppetje, width, height, BACKGROUND);
// 	delay(1000);
// 	undrawBitmap(110, 100, poppetje, width, height, BACKGROUND);
// 	delay(1000);
// 	undrawBitmap(160, 120, poppetje, width, height, BACKGROUND);
// 	delay(1000);
// 	undrawBitmap(210, 150, poppetje, width, height, BACKGROUND);
// 	delay(1000);
// }


