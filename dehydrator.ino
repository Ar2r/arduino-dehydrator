#include <dht.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DHTTYPE DHT22
#define DHTPIN 3
#define HEATPIN 2
DHT dht(DHTPIN, DHTTYPE);

long tempSetpoint = 32; // Максимальная температура
long tempDiff = 2; // на сколько разрешено снижать температуру до повторного включения обогревателя
int power = 0; //OFF
int errorCountSensor = 0;

void setup() {
	Serial.begin(9600);
	Serial.println("Dehydrator automator started");
	
	// initialize digital pin 13 as an output.
	pinMode(HEATPIN, OUTPUT); // Используем пин для включения нагревателя
	
	lcd.begin();
	lcd.setCursor(0, 1);
	lcd.print("Level:");
	lcd.print(tempSetpoint);
	lcd.setCursor(8, 1);
	lcd.print("+-");
	lcd.setCursor(10, 1);
	lcd.print(tempDiff);
}

// the loop function runs over and over again forever
void loop() {

	float h = dht.readHumidity();
	float t = dht.readTemperature();
	
	// check if returns are valid, if they are NaN (not a number) then something went wrong!
	if (isnan(t) || isnan(h)) {
		Serial.println("Failed to read from DHT");
		errorCountSensor++;
		} else {
		
		lcd.setCursor(0, 0); // 1 строка
		lcd.print("T:");
		lcd.setCursor(2, 0);
		lcd.print(t);
		
		lcd.setCursor(8, 0);
		lcd.print("E:");
		lcd.setCursor(10, 0);
		lcd.print(errorCountSensor);
		
		if(power==1){
			// включено. Проверяем
			if(t>tempSetpoint){
				power = 0;
			}
			}else{
			// Выключен подогрев. Нужно включить?
			if(t<tempSetpoint-tempDiff){
				power=1;
			}
		}
	}
	
	if(power==1){
		print_heat_status(1);
		digitalWrite(HEATPIN, HIGH);
		}else{
		print_heat_status(0);
		digitalWrite(HEATPIN, LOW);
	}
	delay(3000);
	
}

void print_heat_status(int flag){
	lcd.setCursor(13, 1);
	
	if(flag==1){
		lcd.print("ON ");
		}else{
		lcd.print("OFF");
	}
}
