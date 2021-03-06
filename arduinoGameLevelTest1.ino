// demo for displaying bitmaps on an 320 x 240 tft lcd screen with Arduino

#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "ArduinoNunchuk/ArduinoNunchuk.h" // library for nunchuk
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <EEPROM.h>


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
#define DARKGREY 0x1111
#define BROWN   0xB222
#define ORANGE  0xFD6A

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
extern uint8_t bombBorderArray[];
extern uint8_t bombFillArray[];
extern uint8_t bombFuseArray[];
extern uint8_t fireInsideArray[];
extern uint8_t fireMidArray[];
extern uint8_t fireOutsideArray[];
extern uint8_t heartFullArray[];
extern uint8_t heartEmptyArray[];

// this is an array in which the colornames are stored
uint16_t colorArray[] = {BLACK, BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE, GREY};

// this is an array where the placed blocks will be in
uint16_t placed[51];


// this is an array where are the possible locations are for destructible blocks, there are 73 possible locations with each an X and Y value
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

// this is an indestructable block array
uint16_t undesBlockPositions[20][2] = {
	{43,52}, {85,52}, {127,52}, {169,52}, {211,52},
	{43,94}, {85,94}, {127,94}, {169,94}, {211,94},
	{43,136}, {85,136}, {127,136}, {169,136}, {211,136},
	{43,178}, {85,178}, {127,178}, {169,178}, {211,178},
};

// this is an empty array to keep track of the fire locations
uint16_t firePositions[5][2];


int xold; // x coordinate of the previous position
int yold; // y coordinate of the previous position
int analogXOld; // old x value of the nunchuck
int analogYOld; // old y value of the nunchuck

int startX1 = 22;
int startY1 = 31;

int score1 = 0;
int score2 = 0;
int highscore = EEPROM.read(0); 

int startX2;
int startY2;

int curX1 = startX1;
int curY1 = startY1;

int curX2 = startX2;
int curY2 = startY2;

int newX1 = 0;
int newY1 = 0;

int newX2 = 0;
int newY2 = 0;

int bombX1 = 0;
int bombY1 = 0;

int bombX2 = 0;
int bombY2 = 0;

int fireX1 = 0;
int fireY1 = 0;

int fireX2 = 0;
int fireY2 = 0;

int fireX3 = 0;
int fireY3 = 0;

int fireX4 = 0;
int fireY4 = 0;

int fireX5 = 0;
int fireY5 = 0;

int change = 0;
int changeBomb = 0;
int BombPlaced = 0;
int firePlaced = 0;

int fireCheck = 0;

int hearts1 = 3;
int hearts2 = 3;

int numberOfBlocks = 50; // Number of blocks getting printed

int interruptCounterBomb = 0;
int interruptCounterFire = 0;

int16_t main (void){
	init();
	myNunchuck.init();
	tft.begin();
	tft.setRotation(1);
	
	
	cli();
	TCCR1A = 0;
	TCCR1B = 0;
	TIMSK1 = 1;
	
	TCCR1B |= (1<<CS10);
	TIMSK1 |= (1<<TOIE1);
	
	TCNT1 = 0;
	sei();
	
	Serial.begin(9600);
	
	
	while(1){
		
		mainMenu();

	}
	
}



// function to draw the hex bitmaps
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

// function which puts a bomb on the players current location
void drawBomb(){
	if(myNunchuck.zButton == 1 && changeBomb == 0 && BombPlaced == 0) {
		drawBitmap(curX1, curY1, bombBorderArray, 19, 19, GREY);
		drawBitmap(curX1, curY1, bombFillArray, 19, 19, BLACK);
		drawBitmap(curX1, curY1, bombFuseArray, 19, 19, ORANGE);
		
		bombX1 = curX1;
		bombY1 = curY1;
		
		
		
		drawSpookPlayer(curX1, curY1);
		changeBomb = 1;
		BombPlaced = 1;
		
		
	}
	
	if(myNunchuck.zButton == 0) {
		changeBomb = 0;
		
	}
	
}

// redraws the bomb if the player walks over it
void isBombPlaced() {
	if(BombPlaced == 1) {
		drawBitmap(bombX1, bombY1, bombBorderArray, 19, 19, GREY);
		drawBitmap(bombX1, bombY1, bombFillArray, 19, 19, BLACK);
		drawBitmap(bombX1, bombY1, bombFuseArray, 19, 19, ORANGE);
		
		drawSpookPlayer(curX1, curY1);
	}
}

// void isFirePlaced() {
// 	if(firePlaced == 1) {
// 		for(int i = 0; i <= 4; i++){
//
// 			if(firePositions[i][0] == curX1 && firePositions[i][1] == curY1) {
// 				drawBitmap(curX1, curY1, fireInsideArray, 19, 19, YELLOW);
// 				drawBitmap(curX1, curY1, fireMidArray, 19, 19, ORANGE);
// 				drawBitmap(curX1, curY1, fireOutsideArray, 19, 19, RED);
//
// 				//drawSpookPlayer(curX1, curY1);
// 			}
//
// 		}
//
// 	}
// }

// draws the outer blockframe
void frame(int16_t x, int16_t y){
	drawBitmap(x, y, frameArray, 273, 240, GREY);
	
}

// function to draw a block which uses x and y coordinates as input
void block(int16_t x, int16_t y){
	drawBitmap(x, y, blockArray, 19, 19, BROWN);
	
}

// draws a random level based on the random function using a random seed based on the random volt value of a pin
void drawRandomLevel(){
	uint16_t alreadyPlaced[73];
	srand(analogRead(A0));
	for(int i = 0; i <= numberOfBlocks; i++){
		uint16_t random = rand() % (73 + 1);
		if(alreadyExistsInArray(random, alreadyPlaced)) {
			i--;
			} else {
			placed[i] = random;
			alreadyPlaced[random] = random;
			uint16_t x1 = coordinates[random][0];
			uint16_t y1 = coordinates[random][1];
			if(random != 73){
				block(x1, y1);
			}
		}
	}
	memset(&alreadyPlaced, 0 ,sizeof(alreadyPlaced));
	
}

//checks if input value already exists in array
bool alreadyExistsInArray(int val, int *arr){
	int i;
	for (i=0; i <= 73; i++) {
		if (arr[i] == val)
		return true;
	}
	return false;
}


int newDirection = 0;
int prevDirection = 0;
// 1 = up, 2 = down, 3 = left, 4 = right


// new functions for walking and stuff
void drawSpookPlayer(int16_t x, int16_t y) { // add direction to be undrawn
	drawBitmap(x, y, spookBodyArray, 19, 19, RED);
	drawBitmap(x, y, oogwitArray, 19, 19, WHITE);
	if(newDirection == 1) {
		drawBitmap(x, y, ogenBovenArray, 19, 19, BLUE);
		prevDirection = 1;
		} else if(newDirection == 2 || newDirection == 0) {
		drawBitmap(x, y, ogenOnderArray, 19, 19, BLUE);
		prevDirection = 2;
		} else if(newDirection == 3) {
		drawBitmap(x, y, ogenLinksArray, 19, 19, BLUE);
		prevDirection = 3;
		} else if(newDirection == 4) {
		drawBitmap(x, y, ogenRechtsArray, 19, 19, BLUE);
		prevDirection = 4;
	}
}

// undraws the player from old location
void undrawSpookPlayer(int16_t x, int16_t y) { // add direction to be undrawn
	undrawBitmap(x, y, spookBodyArray, 19, 19, BACKGROUND);
	undrawBitmap(x, y, oogwitArray, 19, 19, BACKGROUND);
	if(prevDirection == 1) {
		undrawBitmap(x, y, ogenBovenArray, 19, 19, BACKGROUND);
		} else if(prevDirection == 2) {
		undrawBitmap(x, y, ogenOnderArray, 19, 19, BACKGROUND);
		} else if(prevDirection == 3) {
		undrawBitmap(x, y, ogenLinksArray, 19, 19, BACKGROUND);
		} else if(prevDirection == 4) {
		undrawBitmap(x, y, ogenRechtsArray, 19, 19, BACKGROUND);
	}
}

void moveRight(){
	newDirection = 4;
	newX1 = curX1 + 21;
	newY1 = curY1;
	if (borderCheckX() == 1 && wallCheck() == 1 && blockCheck() == 1){
		undrawSpookPlayer(curX1, curY1);
		drawSpookPlayer(newX1, curY1);
		curX1 = newX1;
	}
	
}

void moveLeft(){
	newDirection = 3;
	newX1 = curX1 - 21;
	newY1 = curY1;
	if (borderCheckX() == 1 && wallCheck() == 1 && blockCheck() == 1){
		undrawSpookPlayer(curX1, curY1);
		drawSpookPlayer(newX1, curY1);
		curX1 = newX1;
	}
	
}

void moveUp(){
	newDirection = 1;
	newX1 = curX1;
	newY1 = curY1 - 21;
	if (borderCheckY() == 1 && wallCheck() == 1 && blockCheck() == 1){
		undrawSpookPlayer(curX1, curY1);
		drawSpookPlayer(curX1, newY1);
		curY1 = newY1;
	}
	
}

void moveDown(){
	newDirection = 2;
	newX1 = curX1;
	newY1 = curY1 + 21;
	if (borderCheckY() == 1 && wallCheck() == 1 && blockCheck() == 1){
		undrawSpookPlayer(curX1, curY1);
		drawSpookPlayer(curX1, newY1);
		curY1 = newY1;
	}
	
}


int borderCheckY(){
	if(newY1 < 31 || newY1 > 199){
		return 0;
		} else{
		return 1;
	}
}

int borderCheckX(){
	if(newX1 < 22 || newX1 > 232){
		return 0;
		} else{
		return 1;
	}
}


int blockCheck(){
	for (int i=0; i<numberOfBlocks; i++){
		int x = placed[i];
		int y = placed[i];
		if(newX1 == coordinates[x][0] && newY1 == coordinates[y][1]){
			return 0;
		}
	}
	return 1;
}


int wallCheck(){
	for (int i=0; i<20; i++){
		int x = undesBlockPositions[i][0];
		int y = undesBlockPositions[i][1];
		if(newX1 == x && newY1 == y){
			return 0;
		}
	}
	return 1;
}

int borderCheckFireY(int Y){ //Checks if fire doesn't go through the border
	if(Y < 31 || Y > 199){
		return 0;
		} else{
		return 1;
	}
}

int borderCheckFireX(int X){ //Checks if fire doesn't go through the border
	if(X < 22 || X > 232){
		return 0;
		} else{
		return 1;
	}
}

int wallCheckFire(int X, int Y){ //Checks if fire doesn't go through indestructible blocks
	for (int i=0; i<20; i++){
		int x = undesBlockPositions[i][0];
		int y = undesBlockPositions[i][1];
		if(X == x && Y == y){
			return 0;
		}
	}
	return 1;
}




void printPlacedBlocks(){
	for (int i=0; i<51; i++){
		int number = placed[i];
		Serial.println(number);
		Serial.print(coordinates[number][0]);
		Serial.print(",") ;
		Serial.println(coordinates[number][1]);
	}
	Serial.println();
}


void walkWithNunchuk(){
	myNunchuck.update(); // update the nunchuck data
	
	if ((analogXOld != myNunchuck.analogX || analogYOld != myNunchuck.analogY) && change == 0) { // if either the x or y coordinate has changed, we have to redraw the spookje
		analogXOld = myNunchuck.analogX; // update X value
		analogYOld = myNunchuck.analogY; // update Y value
		
		if (myNunchuck.analogX>200){
			moveRight();
			change = 1;
			isBombPlaced();
			walksInFire();
			
			}else if (myNunchuck.analogX<50){
			moveLeft();
			change = 1;
			isBombPlaced();
			walksInFire();
			
			}else if (myNunchuck.analogY>200){
			moveUp();
			change = 1;
			isBombPlaced();
			walksInFire();
			
			}else if (myNunchuck.analogY<40){
			moveDown();
			change = 1;
			isBombPlaced();
			walksInFire();
			
		}
		
	}
	if (myNunchuck.analogX>100 && myNunchuck.analogX<140 && myNunchuck.analogY>110 && myNunchuck.analogY<150){
		change = 0;
	}
	
	
}

void walksInFire() { //Bug: makes the top fire in row 7 dissapear and makes score go +1
	if(firePlaced == 1) {
		for(int i = 0; i <= 4; i++) {
			if(newX1 == firePositions[i][0] && newY1 == firePositions[i][1]) {
				hearts1--;
				updateHearts();
			}
		}
	}
}


void updateScore() {
	tft.fillRect(285, 85, 22, 14, BACKGROUND);
	
	tft.setCursor(285, 85);
	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.setTextWrap(false);
	tft.print(score1);
	
	tft.fillRect(285, 153, 22, 14, BACKGROUND);
	
	tft.setCursor(285, 153);
	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.setTextWrap(false);
	tft.print(score2);
}

void updateHearts(){
	if(hearts1 == 3){
		drawBitmap(282, 15, heartFullArray, 19, 19, RED);
		drawBitmap(282, 35, heartFullArray, 19, 19, RED);
		drawBitmap(282, 55, heartFullArray, 19, 19, RED);
	}
	
	if(hearts1 == 2){
		drawBitmap(282, 15, heartFullArray, 19, 19, RED);
		drawBitmap(282, 35, heartFullArray, 19, 19, RED);
		
		undrawBitmap(282, 55, heartFullArray, 19, 19, BACKGROUND);
		drawBitmap(282, 55, heartEmptyArray, 19, 19, RED);
	}
	
	if(hearts1 == 1){
		drawBitmap(282, 15, heartFullArray, 19, 19, RED);
		
		undrawBitmap(282, 35, heartFullArray, 19, 19, BACKGROUND);
		drawBitmap(282, 35, heartEmptyArray, 19, 19, RED);
		
		undrawBitmap(282, 55, heartFullArray, 19, 19, BACKGROUND);
		drawBitmap(282, 55, heartEmptyArray, 19, 19, RED);
		
	}
	
	
	if(hearts2 == 3){
		drawBitmap(282, 175, heartFullArray, 19, 19, RED);
		drawBitmap(282, 195, heartFullArray, 19, 19, RED);
		drawBitmap(282, 215, heartFullArray, 19, 19, RED);
	}
	
	if(hearts2 == 2){
		drawBitmap(282, 175, heartFullArray, 19, 19, RED);
		drawBitmap(282, 195, heartFullArray, 19, 19, RED);
		drawBitmap(282, 215, heartEmptyArray, 19, 19, RED);
	}
	
	if(hearts2 == 1){
		drawBitmap(282, 175, heartFullArray, 19, 19, RED);
		drawBitmap(282, 195, heartEmptyArray, 19, 19, RED);
		drawBitmap(282, 215, heartEmptyArray, 19, 19, RED);
	}
	
}

void insertPlayerNumbers(){
	tft.setCursor(285, 107);
	tft.setTextColor(RED);
	tft.setTextSize(2);
	tft.setTextWrap(false);
	tft.print("P1");
	
	tft.setCursor(285, 130);
	tft.setTextColor(BLUE);
	tft.setTextSize(2);
	tft.setTextWrap(false);
	tft.print("P2");
}

void endOfGame() {
	if(0) {
		firePlaced = 0; //Make firePlaced 0 so it doesnt undraw fire over the endscreen
		
		tft.fillRect(0, 0, 320, 240, BACKGROUND);
		
		tft.setCursor(80, 80);
		tft.setTextColor(GREEN);
		tft.setTextSize(4);
		tft.setTextWrap(false);
		tft.print("YOU WIN!");
		
		tft.setCursor(77, 110);
		tft.setTextColor(WHITE);
		tft.setTextSize(1);
		tft.setTextWrap(true);
		tft.print("Press C to go back to main menu");
		
// 		if(speler.score > highscore){
// 			EEPROM.write(0, speler.score)
//			highscore = speler.score;
//
// 		}
		
		
		
		while(1) {
			
			myNunchuck.update();
			
			if(myNunchuck.cButton == 1) {
				mainMenu();
			}
		}
		
	}
	
	if(hearts1 == 0) {
		firePlaced = 0;
		
		tft.fillRect(0, 0, 320, 240, BACKGROUND);
		
		tft.setCursor(65, 80);
		tft.setTextColor(RED);
		tft.setTextSize(4);
		tft.setTextWrap(false);
		tft.print("YOU LOSE!");
		
		tft.setCursor(77, 130);
		tft.setTextColor(WHITE);
		tft.setTextSize(1);
		tft.setTextWrap(true);
		tft.print("Press C to go back to main menu");
		
//		used for testing high score
// 		EEPROM.write(0, score1);
// 		highscore = score1;

		while(1) {
			
			myNunchuck.update();
			
			if(myNunchuck.cButton == 1) {
				resetVariables();
				mainMenu();
			}
		}
	}
}

void mainMenu() {
	tft.fillRect(0, 0, 320, 240, BLACK);
	
	tft.setCursor(20, 10);
	tft.setTextColor(WHITE);
	tft.setTextSize(4);
	tft.setTextWrap(false);
	tft.print("BOMBERSPOOKS");
	
	tft.setCursor(20, 70);
	tft.setTextColor(RED);
	tft.setTextSize(1);
	tft.setTextWrap(false);
	tft.print("Player 1");
	
	drawSpookPlayer(28, 90);
	
	tft.setCursor(250, 70);
	tft.setTextColor(BLUE);
	tft.setTextSize(1);
	tft.setTextWrap(false);
	tft.print("Player 2");
	
	drawSpookPlayer(260, 90);
	
	tft.setCursor(100, 70);
	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.setTextWrap(false);
	tft.print("Press C to");
	
	tft.setCursor(100, 90);
	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.setTextWrap(false);
	tft.print("start game");
	
	tft.setCursor(80, 160);
	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.setTextWrap(false);
	tft.print("High score:");
	
	tft.setCursor(213, 160);
	tft.setTextColor(WHITE);
	tft.setTextSize(2);
	tft.setTextWrap(false);
	tft.print(highscore);
	
	while(1) {
		myNunchuck.update();
		
		if(myNunchuck.cButton == 1) {
			
			redrawScreen();
			frame(0, 0);
			
			drawRandomLevel();
			
			updateScore();
			
			drawSpookPlayer(startX1, startY1);
			
			updateHearts();
			
			insertPlayerNumbers();
			
//			printPlacedBlocks();
			
			while(1) {
				Serial.println(analogRead(A0));
				walkWithNunchuk();
				drawBomb();
				endOfGame();
			}
		}
	}
}

void resetVariables() {
	memset(&placed[50], 0, sizeof(placed));
	memset(&coordinates[73][2], 0, sizeof(coordinates));
	
	
	score1 = 0;
	score2 = 0;
	
	curX1 = startX1;
	curY1 = startY1;

	curX2 = startX2;
	curY2 = startY2;

	newX1 = 0;
	newY1 = 0;

	newX2 = 0;
	newY2 = 0;
	
	hearts1 = 3;
	hearts2 = 3;
	
	
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

ISR(TIMER1_OVF_vect) {
	interruptCounterFire++;

	if(BombPlaced == 1) {
		
		interruptCounterBomb++;
		
		
		if (interruptCounterBomb >= 500 ) { //250 interrupts ? 1 sec
			interruptCounterBomb = 0;
			interruptCounterFire = 0;
			
			
			undrawBitmap(bombX1, bombY1, bombBorderArray, 19, 19, BACKGROUND);
			undrawBitmap(bombX1, bombY1, bombFillArray, 19, 19, BACKGROUND);
			undrawBitmap(bombX1, bombY1, bombFuseArray, 19, 19, BACKGROUND);
			
			drawSpookPlayer(curX1, curY1);
			
			firePositions[0][0] = bombX1;
			firePositions[0][1] = bombY1;
			
			firePositions[1][0] = bombX1 + 21;
			firePositions[1][1] = bombY1;
			
			firePositions[2][0] = bombX1;
			firePositions[2][1] = bombY1 + 21;
			
			firePositions[3][0] = bombX1 - 21;
			firePositions[3][1] = bombY1;
			
			firePositions[4][0] = bombX1;
			firePositions[4][1] = bombY1 - 21;
			
			
			for(int x = 0; x <= 4; x++) {
				for(int i = 0; i <= 51; i++) {
					int fireCheck = placed[i];
					
					if(firePositions[x][0] == coordinates[fireCheck][0] && firePositions[x][1] == coordinates[fireCheck][1]) {
						if(borderCheckFireX(firePositions[x][0]) && borderCheckFireY(firePositions[x][1]) && wallCheckFire(firePositions[x][0], firePositions[x][1])) {
							undrawBitmap(coordinates[fireCheck][0], coordinates[fireCheck][1], blockArray, 19, 19, BACKGROUND);
							coordinates[fireCheck][0] = 0;
							coordinates[fireCheck][1] = 0;
							score1++;
							updateScore();
							
						}
					}
				}
				
				
			}
			
			
			for(int i = 0; i <=4; i++){
				
				if(borderCheckFireX(firePositions[i][0]) && borderCheckFireY(firePositions[i][1]) && wallCheckFire(firePositions[i][0], firePositions[i][1])) {
					drawBitmap(firePositions[i][0], firePositions[i][1], fireInsideArray, 19, 19, YELLOW);
					drawBitmap(firePositions[i][0], firePositions[i][1], fireMidArray, 19, 19, ORANGE);
					drawBitmap(firePositions[i][0], firePositions[i][1], fireOutsideArray, 19, 19, RED);
					
					if(firePositions[i][0] == curX1 && firePositions[i][1] == curY1) {
						hearts1 = hearts1 - 1;
						updateHearts();
					}
				}
				
			}
			firePlaced = 1;
			changeBomb = 0;
			BombPlaced = 0;
		}


	}

	if(firePlaced == 1 && interruptCounterFire >= 500){ //Undraw Fire
		interruptCounterFire = 0;
		
		for(int i = 0; i <=4; i++){
			
			if(borderCheckFireX(firePositions[i][0]) && borderCheckFireY(firePositions[i][1]) && wallCheckFire(firePositions[i][0], firePositions[i][1])) {
				undrawBitmap(firePositions[i][0], firePositions[i][1], fireInsideArray, 19, 19, BACKGROUND);
				undrawBitmap(firePositions[i][0], firePositions[i][1], fireMidArray, 19, 19, BACKGROUND);
				undrawBitmap(firePositions[i][0], firePositions[i][1], fireOutsideArray, 19, 19, BACKGROUND);
				
				drawSpookPlayer(curX1, curY1);
				
				firePositions[i][0] = NULL;
				firePositions[i][1] = NULL;
			}
			
		}
		firePlaced = 0; //Set firePlaced to 0 so the fire gets undrew only when necessary
	}
}
