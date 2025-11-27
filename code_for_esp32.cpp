#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

#define MENU_BUTTON 4  // GPIO4 connected to button (with internal pull-up)

const char* ssid = "OREO";
const char* password = "OreoCake";

LiquidCrystal_I2C lcd(0x27, 16, 2);  // change to 0x3F if needed

// Menu items
String menuItems[] = {"Weather", "Tasks", "Next Class"};
int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);
int currentIndex = 0;

// For Weather suggestion scrolling
String suggestionText = "";
unsigned long scrollTimer = 0;
int scrollIndex = 0;

// Button debounce
bool lastButtonState = HIGH;
bool lastStableState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Retry control
bool retryPending = false;
unsigned long lastRetryTime = 0;
const unsigned long retryInterval = 15000;  // 15s wait before retry

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  pinMode(MENU_BUTTON, INPUT_PULLUP); // Use internal pull-up

  // Initial display
  lcd.setCursor(0, 0);
  lcd.print("SSID: ");
  lcd.print(ssid);
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(2500);
  lcd.clear();

  displayScreen(currentIndex);
}

void loop() {
  // Button logic with proper debounce
  int reading = digitalRead(MENU_BUTTON);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != lastStableState) {
      lastStableState = reading;

      // If button just pressed (falling edge)
      if (lastStableState == LOW) {
        currentIndex = (currentIndex + 1) % menuLength;
        suggestionText = "";
        scrollIndex = 0;
        retryPending = false;
        displayScreen(currentIndex);
      }
    }
  }
  lastButtonState = reading;

  // Scroll suggestion if needed
  if (menuItems[currentIndex] == "Weather" && suggestionText.length() > 16) {
    if (millis() - scrollTimer > 400) {
      lcd.setCursor(0, 1);
      lcd.print(suggestionText.substring(scrollIndex, scrollIndex + 16));
      scrollIndex++;
      if (scrollIndex > suggestionText.length() - 16) scrollIndex = 0;
      scrollTimer = millis();
    }
  }

  // Retry weather if failed
  if (menuItems[currentIndex] == "Weather" && retryPending) {
    if (millis() - lastRetryTime > retryInterval) {
      fetchWeatherAndDisplay();
    }
  }

  delay(10); // loop delay
}

// Show content per menu item
void displayScreen(int index) {
  lcd.clear();

  if (menuItems[index] == "Weather") {
    fetchWeatherAndDisplay();
  } else if (menuItems[index] == "Tasks") {
    lcd.setCursor(0, 0);
    lcd.print("1: Math HW");
    lcd.setCursor(0, 1);
    lcd.print("2: Revise Ch.3");
  } else if (menuItems[index] == "Next Class") {
    lcd.setCursor(0, 0);
    lcd.print("CS Class @10:30");
    lcd.setCursor(0, 1);
    lcd.print("Room: B2 / Lab 1");
  }
}

// Make HTTP call and show weather info
void fetchWeatherAndDisplay() {
  HTTPClient http;
  http.begin("https://braj.onrender.com/weather/onboard");  // your API endpoint

  int httpCode = http.GET();
  if (httpCode == 200) {
    retryPending = false;
    String payload = http.getString();
    Serial.println(payload);

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      lcd.setCursor(0, 0);
      lcd.print("JSON Parse Err");
      retryPending = true;
      lastRetryTime = millis();
      return;
    }

    // Extract fields
    float temperature = doc["temperature"];
    int rainProb = doc["rain_probability_next_6h"];
    const char* umbrella = doc["umbrella_recommended"];
    suggestionText = doc["suggestion"].as<String>();

    // Format and print top line
    String line1 = "T:" + String(temperature, 1) + " R:" + String(rainProb) + " U:" + 
                   (String(umbrella) == "yes" ? "Y" : "N");

    lcd.setCursor(0, 0);
    lcd.print(line1.substring(0, 16)); // Clip to 16 chars

    // Bottom line: suggestion
    if (suggestionText.length() <= 16) {
      lcd.setCursor(0, 1);
      lcd.print(suggestionText);
    } else {
      scrollIndex = 0;
      scrollTimer = millis(); // Ready to scroll
    }

  } else {
    lcd.setCursor(0, 0);
    lcd.print("Weather fetch");
    lcd.setCursor(0, 1);
    lcd.print("failed! Retry...");
    retryPending = true;
    lastRetryTime = millis();
  }

  http.end();
}
