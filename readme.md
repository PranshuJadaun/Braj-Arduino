# **ESP32 Assistant & Notifier**

An ESP32-based IoT assistant that connects to Wi-Fi and displays **Weather**, **Tasks**, and **Next Class** information on a 16×2 I2C LCD.
It features a **single-button menu navigation**, **HTTP API weather fetching**, **auto-retry on failure**, and **scrolling long text suggestions**.

---

## ⭐ **Features**

### 🔹 1. **Multi-Screen Menu System**

Navigate between:

* **Weather**
* **Tasks**
* **Next Class**

Handled using a single button with **debounce logic**.

### 🔹 2. **Live Weather Fetching via API**

Weather data is fetched from:

```
https://braj.onrender.com/weather/onboard
```

Displayed fields:

* Temperature
* Rain Probability
* Umbrella Recommendation
* Weather Suggestion (auto-scrolling if long)

### 🔹 3. **Auto-Retry Logic**

If weather fetch fails:

* Displays an error
* Automatically retries after 15 seconds

### 🔹 4. **LCD Text Scrolling**

Long text suggestions scroll smoothly across the **bottom line** of the LCD.

### 🔹 5. **Internet-Connected**

* Connects to Wi-Fi using SSID & Password
* Shows IP address on successful connection

---

## 🛠️ **Hardware Required**

* **ESP32 Dev Module**
* **16x2 I2C LCD** (address: `0x27` or `0x3F`)
* **Tactile Button** (wired to GPIO 4 with internal pull-up)
* Jumper wires
* Breadboard

---

## 📦 **Libraries Used**

| Library               | Purpose                |
| --------------------- | ---------------------- |
| `WiFi.h`              | Connect ESP32 to Wi-Fi |
| `HTTPClient.h`        | Make HTTP GET requests |
| `LiquidCrystal_I2C.h` | Control 16×2 LCD       |
| `ArduinoJson.h`       | Parse JSON from API    |
| `Wire.h`              | I²C communication      |

---

## ⚙️ **How It Works**

### **1. Startup**

* Displays SSID
* Connects to Wi-Fi
* Shows IP address
* Loads the first menu item

### **2. Button Navigation**

* Every button press → move to next item
* Uses debounce to avoid false triggers

### **3. Weather Screen**

* Sends HTTP GET
* Parses JSON
* Displays:

  ```
  T:xx.x R:yy U:Y/N
  <Suggestion (scroll if long)>
  ```

### **4. Retry System**

If request fails:

* Shows *“Weather fetch failed! Retry…”*
* Automatically retries after 15 sec

---

## 📁 **Code**

Paste your main `.ino` code here (or link to file).

---

## 📸 Output Examples

### **Weather Screen**

```
T:29.4 R:56 U:Y
Carry an umbrella today
```

### **Tasks Screen**

```
1: Math HW
2: Revise Ch.3
```

### **Next Class**

```
CS Class @10:30
Room: B2 / Lab 1
```

---

## ▶️ **Demo Video**

📹 **[https://youtu.be/MIO73a8oofs?si=tKwuQF8kEKhd1BIf](https://youtu.be/MIO73a8oofs?si=tKwuQF8kEKhd1BIf)**

---

## 🧩 **Future Improvements**

* Add real-time clock (RTC) sync
* Add voice notifications using I2S DAC + speaker
* Add customizable tasks via mobile app
* Add home automation integrations
