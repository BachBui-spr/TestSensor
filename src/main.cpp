#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// Cấu hình các chân
#define DHTPIN 14              // Chân DATA của DHT11
#define DHTTYPE DHT11          // Loại cảm biến
#define RAIN_SENSOR_PIN 32     // Cảm biến mưa (AO)
#define SOIL_SENSOR_PIN 35     // Cảm biến độ ẩm đất (AO)
#define RELAY_PIN 26           // Relay điều khiển
#define RAIN_THRESHOLD 2000    // Ngưỡng mưa (giá trị ADC)

// Khởi tạo cảm biến và LCD
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Địa chỉ LCD: 0x27 hoặc 0x3F

void setup() {
  Serial.begin(115200);
  delay(1000);  // Chờ ổn định

  Wire.begin(21, 22);       // Khởi động I2C với SDA = 21, SCL = 22
  lcd.begin(16, 2);         // Khởi tạo LCD với đúng kích thước
  lcd.backlight();          // Bật đèn nền

  dht.begin();              // Khởi động DHT
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Relay mặc định tắt
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  int soilValue = analogRead(SOIL_SENSOR_PIN);
  float rainPercent = map(rainValue, 3500, 500, 0, 100);
  rainPercent = constrain(rainPercent, 0, 100);

  float soilPercent = map(soilValue, 3000, 1000, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  Serial.println("----- Data -----");

  lcd.clear();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println(" DHT11 error");
    lcd.setCursor(0, 0);
    lcd.print(" DHT11 error!");
  } else {
    Serial.printf(" Temperature: %.1f C\n", temperature);
    Serial.printf(" Humidity: %.0f %%\n", humidity);

    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temperature, 1);
    lcd.print((char)223); //  độ °
    lcd.print("C  ");
    lcd.print("H:");
    lcd.print(humidity, 0);
    lcd.print("%");
  }

  Serial.printf(" Rain sensor: %.0f %%\n", rainValue);
  Serial.printf(" Soil moisture: %.0f %%\n", soilPercent);

  lcd.setCursor(0, 1);
  lcd.print("HE: ");
  lcd.print(soilPercent, 0);
  lcd.print("%  ");
  if (rainValue < RAIN_THRESHOLD) {
    lcd.print("R-ON ");
    Serial.println(" Rain : On Relay");
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    lcd.print("UR-OFF");
    Serial.println(" UnRain : Off Relay");
    digitalWrite(RELAY_PIN, LOW);
  }

  Serial.println("----------------------------");
  delay(2000);
}
 