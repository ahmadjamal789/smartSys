/*
  Smart Home Monitoring System
  Board : Arduino Uno

  Behaviour
  - Startup: shows "SMART HOME" / "MONITOR SYSTEM"
  - LCD line 1: temperature + humidity (DHT11)
  - LCD line 2: status
        "GAS DETECTED!"   -> MQ sensor above threshold, buzzer ON
        "MOTION DETECTED" -> PIR triggered
        "All Clear"       -> nothing happening
    (if gas AND motion happen together, line 2 alternates between them)
  - Green LED is ON all the time, and turns OFF only while motion is detected
  - Red LED is ON only while motion is detected
  - Buzzer sounds ONLY for gas/smoke

  PIN MAP (change to match your wiring)
    DHT11 data ........ D2
    PIR output ........ D3
    Green LED ......... D5  (use a 220 ohm resistor)
    Red LED ........... D6  (use a 220 ohm resistor)
    Buzzer ............ D8
    MQ sensor (AO) .... A0
    LCD I2C ........... SDA = A4, SCL = A5
*/

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------- Pins ----------
#define DHT_PIN     2
#define PIR_PIN     9
#define BLUE_LED    5
#define GREEN_LED   8
#define RED_LED     7
#define BUZZER_PIN  6
#define MQ_PIN      A0

// ---------- Settings ----------
#define DHT_TYPE        DHT11
#define LCD_ADDRESS     0x27      // try 0x3F if the screen stays blank
#define GAS_THRESHOLD   100       // 0-1023, raise it if you get false alarms
#define BUZZER_FREQ     2000      // Hz

const unsigned long DHT_INTERVAL     = 1500;  // DHT11 needs >= 1s between reads
const unsigned long DISPLAY_INTERVAL = 250;
const unsigned long ALTERNATE_TIME   = 1000;  // used when gas + motion together

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHT_PIN, DHT_TYPE);

float temperature;
float humidity;

unsigned long lastDhtRead     = 0;
unsigned long lastDisplayDraw = 0;

// Print text on a row and pad with spaces so old characters get erased
void printLine(uint8_t row, const String &text) {
  String line = text;
  while (line.length() < 20) line += ' ';
  lcd.setCursor(0, row);
  lcd.print(line.substring(0, 20));
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BLUE_LED, HIGH);  // green is on by default
  digitalWrite(GREEN_LED, LOW);

  dht.begin();
  lcd.init();
  lcd.backlight();

  // Opening screen
  printLine(0, "   SMART HOME");
  printLine(1, " MONITOR SYSTEM");
  delay(3000);

  lcd.clear();
  printLine(0, "Warming up...");
  printLine(1, "Please wait");
  delay(2000);  // For real hardware, the MQ and PIR sensors need longer (30s+)
  lcd.clear();
}

void loop() {
  unsigned long now = millis();

  // ---------- Read sensors ----------
  bool motion      = digitalRead(PIR_PIN) == HIGH;
  bool gasDetected = analogRead(MQ_PIN) > GAS_THRESHOLD;

  if (now - lastDhtRead >= DHT_INTERVAL) {
    lastDhtRead = now;
    temperature = dht.readTemperature();
    humidity    = dht.readHumidity();
  }
  // ---------- Outputs ----------
  // Red on + green off when motion; otherwise green on, red off
  digitalWrite(GREEN_LED, motion ? HIGH : LOW);
  digitalWrite(BLUE_LED, motion ? LOW : HIGH);

  // Buzzer only for gas/smoke
  if (gasDetected) {
    tone(BUZZER_PIN, BUZZER_FREQ);
    digitalWrite(RED_LED, HIGH);
  } else {
    noTone(BUZZER_PIN);
    digitalWrite(RED_LED, LOW);
  }

  // ---------- LCD ----------
  if (now - lastDisplayDraw >= DISPLAY_INTERVAL) {
    lastDisplayDraw = now;

    // Line 1: temperature and humidity
    if (isnan(temperature) || isnan(humidity)) {
      printLine(0, "DHT read error");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.print(temperature, 0);
      lcd.print((char)223);  // degree symbol
      lcd.print("C Humid:");
      lcd.print(humidity, 0);
      lcd.print("%    ");
    }

    // Line 2: status
    // if (gasDetected && motion) {
    //  // bool showGas = ((now / ALTERNATE_TIME) % 2) == 0;
    //  //  printLine(1, showGas ? "GAS DETECTED!" : "MOTION DETECTED");
    // }

        // Row 1: motion (blank when no motion)
    printLine(1, motion ? "MOTION DETECTED" : "");

    // Row 2: gas (blank when no gas)
    printLine(2, gasDetected ? "GAS DETECTED!" : "");

    // Row 3: only when nothing is happening
    printLine(3, (!motion && !gasDetected) ? "All Clear" : "");
  };
}


