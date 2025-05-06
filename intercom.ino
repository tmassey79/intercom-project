
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

const int encoderPinA = 2;
const int encoderPinB = 3;
const int selectButtonPin = 4;

volatile int encoderPosition = 0;
int lastReportedPosition = 0;
bool A_set = false;
bool B_set = false;
unsigned long lastInteraction = 0;
bool inMenu = true;
bool inSubMenu = false;
int currentMenu = 0;
int currentSubMenu = 0;

String menus[] = {"Volume", "Bluetooth", "Intercom"};
int numMenus = sizeof(menus) / sizeof(menus[0]);

String volumeOptions[] = {"Set Volume", "Mute"};
String bluetoothOptions[] = {"Pair", "Unpair", "Status"};
String intercomOptions[] = {"Start", "Stop", "Settings"};

int numSubMenus[] = {2, 3, 3};

void setup() {
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), doEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), doEncoderB, CHANGE);

  lcd.init();
  lcd.backlight();

  rtc.begin();
  lastInteraction = millis();
  displayMenu();
}

void loop() {
  if (digitalRead(selectButtonPin) == LOW) {
    delay(200);
    if (!inSubMenu) {
      inSubMenu = true;
      currentSubMenu = 0;
    } else {
      // Handle submenu selection if needed
    }
    displayMenu();
    lastInteraction = millis();
  }

  if (encoderPosition != lastReportedPosition) {
    if (!inSubMenu) {
      currentMenu = (currentMenu + (encoderPosition - lastReportedPosition) + numMenus) % numMenus;
    } else {
      currentSubMenu = (currentSubMenu + (encoderPosition - lastReportedPosition) + numSubMenus[currentMenu]) % numSubMenus[currentMenu];
    }
    lastReportedPosition = encoderPosition;
    displayMenu();
    lastInteraction = millis();
  }

  if (millis() - lastInteraction > 10000) {
    if (inMenu || inSubMenu) {
      lcd.clear();
      inMenu = false;
      inSubMenu = false;
    }
    displayDateTime();
  }
}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menus[currentMenu]);

  if (inSubMenu) {
    lcd.setCursor(0, 1);
    switch (currentMenu) {
      case 0:
        lcd.print(volumeOptions[currentSubMenu]);
        break;
      case 1:
        lcd.print(bluetoothOptions[currentSubMenu]);
        break;
      case 2:
        lcd.print(intercomOptions[currentSubMenu]);
        break;
    }
  }
}

void displayDateTime() {
  DateTime now = rtc.now();
  lcd.setCursor(0, 0);
  lcd.print(now.toString("DD-MM-YYYY"));
  lcd.setCursor(0, 1);
  lcd.print(now.toString("hh:mm:ss"));
}

void doEncoderA() {
  A_set = digitalRead(encoderPinA) == HIGH;
  updateEncoder();
}

void doEncoderB() {
  B_set = digitalRead(encoderPinB) == HIGH;
  updateEncoder();
}

void updateEncoder() {
  if (A_set == B_set) {
    encoderPosition++;
  } else {
    encoderPosition--;
  }
}