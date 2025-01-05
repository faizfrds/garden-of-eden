#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#define ONE_WIRE_BUS D5
#define DHTPIN D3
#define DHTTYPE DHT11
#define pump D6

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);  //lcd i2c
DHT dht(DHTPIN, DHTTYPE);            //temperature

const int airValue = 620;
const int waterValue = 310;
int soilMoistureValue = 0;
int soilmoist = 0;
int humidity, temp;

void setup(void) {
  Wire.begin(D2, D1);  // SDA=D2, SCL=D1 for NodeMCU
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print(" Garden of Eden");
  lcd.setCursor(0, 1);
  lcd.print(" Faiz Firdaus");
  delay(1000);
  Serial.begin(9600);  // initializing seial communication at 9600 bits/s
  sensors.begin();
  dht.begin();
  delay(1500);
  lcd.clear();
  lcd.print("Mst=   %, T=   c");
  lcd.setCursor(0, 1);
  lcd.print("Hum=   %, P= OFF");
}

void loop(void) {
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  Serial.print("temp: ");
  Serial.println(temp);
  lcd.setCursor(12, 0);
  lcd.print(temp);
  soilMoistureValue = analogRead(A0);
  soilmoist = map(soilMoistureValue, airValue, waterValue, 0, 100);

  //soil moisture checker
  if (soilmoist >= 100) {
    soilmoist = 100;
  } else if (soilmoist <= 0) {
    soilmoist = 0;
  } else if (soilmoist >= 80) {  //range for optimal germination is 21%-80% moisture
    warning(false, "moisture");
  } else if (soilmoist <= 20) {  //range for optimal germination is 21%-80% moisture
    warning(true, "moisture");
  }

  if (temp > 30) {
    warning(false, "temperature");
  } else if (temp < 20) {
    warning(true, "temperature");
  }

  Serial.print("soil moisture: ");
  Serial.print(soilmoist);
  Serial.println("%");
  lcd.setCursor(4, 0);
  lcd.print(soilmoist);
  lcd.print(" ");
  humidity = dht.readHumidity();

  if (isnan(humidity)) {
    Serial.println("DHT 11 error");
    return;
  } else {
    Serial.print("humidity: ");
    Serial.println(humidity);
    Serial.println();
    lcd.setCursor(4, 1);
    lcd.print(humidity);
  }
  delay(1500);
}

void warning(const bool inc, const char* msg) {
  lcd.clear();
  String command = inc == true ? "Inc " : "Dec ";
  String warn = command + String(msg);
  lcd.print(warn);
  lcd.setCursor(0, 1);
  lcd.print("now!");
  delay(1500);
  lcd.clear();
  lcd.print("Mst=   %, T=   c");
  lcd.setCursor(0, 1);
  lcd.print("Hum=   %, P= OFF");
}
