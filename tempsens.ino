
//#include <SimpleServo.h>
//#include <DigiCDC.h>



#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AMG88xx.h>
#include <Adafruit_MLX90614.h>
Adafruit_AMG88xx amg;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



void setup() {
//	Serial.begin(115200);

	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
//	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
//		Serial.println(F("SSD1306 allocation failed"));
		for(;;);
	}

	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
		display.println(amg.begin() ? "Good 8833" : "Bad 8833");
	display.setCursor(64, 0);
		display.println(mlx.begin() ? "Good 906" : "Bad 906");
	display.display();
	delay(500);
}




float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
float low, high, avg;

void calcbounds(float * arry){
	float px = arry[0];
	low = px;
	high = px;
	avg = px;
	for(int i = 1; i < AMG88xx_PIXEL_ARRAY_SIZE; i++){
		px = arry[i];
		if(px < low) low = px;
		else if(px > high) high = px;
		avg += px;
	}
	avg /= AMG88xx_PIXEL_ARRAY_SIZE;
}

static const uint8_t pixorder[64] = {
34, 1, 57, 22, 15, 49, 18, 24, 46, 23, 45, 14, 28, 47, 61, 62, 31, 50, 38, 58, 56, 6, 48, 0, 27, 33, 32, 43, 55, 5, 12, 29, 10, 3, 59, 42, 40, 44, 16, 35, 11, 17, 39, 37, 30, 20, 60, 25, 26, 2, 9, 21, 41, 4, 7, 36, 19, 52, 51, 54, 13, 53, 8
};


int shufoff = 0;

void drawfire(float *arry){
	int dxoff = 64;
	int dyoff = 0;
	float delt = (high-low) / 64.0;
	high = high-delt*2;
	low = low+delt*5;
	delt = (high-low) / 64.0;
	for(int y = 0; y < 8; y++){
	float * pxoff = &arry[8*y];
//	int pixstarty = dyoff + ((7-y)*6);
	int pixstarty = dyoff + ((7-y)*8);
	for(int x = 0; x < 8; x++){
		float px = pxoff[x];
		int color = (px - low) / delt;
		if(color > 64) color = 64;
		else if(color < 0) color = 0;
//		int pixstartx = dxoff + ((7-x)*6);
		int pixstartx = dxoff + ((7-x)*8);

		int i;
		for(i = 0; i < color; i++){
			int pixloc = pixorder[(shufoff+i)%64];
			int pxx = pixloc%8;
			int pxy = pixloc/8;
			display.drawPixel(pixstartx + pxx, pixstarty + pxy, WHITE);
		}
		shufoff += 659;
		shufoff %=64;
//		display.drawPixel(pixstartx, pixstarty, color > 16);
//		display.fillCircle(pixstartx+3, pixstarty+3, color/10, WHITE);
//		if( color > 16) drawPixel(pixstartx, pixstarty);

	}}

//	display.drawBitmap(dxoff, dyoff, bitmap, 48, 48, 0);

}

// the loop function runs over and over again forever
void loop() {
	float amgc = amg.readThermistor();
	float mlxamb = mlx.readAmbientTempC();
	float mlxobj = mlx.readObjectTempC();
	amg.readPixels(pixels);
	calcbounds(pixels);
	display.clearDisplay();
	drawfire(pixels);


	display.setTextSize(1);
	display.setTextColor(WHITE);
/*
	display.setCursor(0, 0);
	display.print("8833 *c");
	display.setCursor(0, 8);
	display.print(amgc);

	display.setCursor(60, 0);
	display.print("906 *c");
	display.setCursor(60, 8);
	display.print(mlxamb);
	display.print(" ");
//	display.setCursor(64, 8);
	display.print(mlxobj);
*/

	display.setCursor(0, 16);
	display.print("Low ");
	display.println(low);
	display.print("Avg ");
	display.println(avg);
	display.print("Hi  ");
	display.println(high);


	display.display();
//	delay(500);
/*
  SerialUSB.write(SerialUSB.read());
  SerialUSB.delay(1000);

  delay(1000);                       // wait for a second

  SerialUSB.delay(300);
*/

}
