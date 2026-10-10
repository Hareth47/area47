/*
 * ============================================================
 *  Triple Attack Detection System - FINAL
 *  ESP8266 + LCD + Web Dashboard (Modern UI)
 * ============================================================
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

// ============================================================
// ⚙️ الإعدادات
// ============================================================
const char* WIFI_SSID     = "RAM";
const char* WIFI_PASSWORD = "779443847r@00f";

LiquidCrystal_I2C lcd(0x27, 16, 2);
ESP8266WebServer server(80);

// ============================================================
// 📊 الحالات
// ============================================================
enum AttackState { SECURE, UDP_FLOOD, SYN_FLOOD, PORT_SCAN };
AttackState currentState = SECURE;
unsigned long lastAlertTime = 0;
const unsigned long RESET_TIMEOUT = 5000;

// ============================================================
// 📈 الإحصائيات
// ============================================================
struct Stats {
  unsigned long udpCount    = 0;
  unsigned long synCount    = 0;
  unsigned long scanCount   = 0;
  unsigned long totalAlerts = 0;
  unsigned long alertsPerMin = 0;
  unsigned long minuteStart = 0;
  unsigned long sessionStart = 0;
  int lastRate = 0;
  char lastSrc[16] = "---";
};

Stats stats;

// ============================================================
// 💾 EEPROM
// ============================================================
const int EEPROM_SIZE = 64;
const int EEPROM_MAGIC_ADDR = 0;
const int EEPROM_TOTAL_ADDR = 4;
const byte EEPROM_MAGIC_VALUE = 0xAB;

// ============================================================
// 💾 EEPROM Functions
// ============================================================
void saveToEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
  EEPROM.put(EEPROM_TOTAL_ADDR, stats.totalAlerts);
  EEPROM.commit();
}

void loadFromEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC_VALUE) {
    EEPROM.get(EEPROM_TOTAL_ADDR, stats.totalAlerts);
    Serial.println("[EEPROM] Loaded: " + String(stats.totalAlerts));
  }
}

// ============================================================
// 🖥️ LCD
// ============================================================
void updateLCD() {
  lcd.clear();
  if (currentState == SECURE) {
    lcd.setCursor(0, 0);
    lcd.print("STATUS: SECURE");
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(stats.totalAlerts);
    lcd.print(" M:");
    lcd.print(stats.alertsPerMin);
  } else {
    lcd.setCursor(0, 0);
    switch (currentState) {
      case UDP_FLOOD: lcd.print("UDP FLOOD "); break;
      case SYN_FLOOD: lcd.print("SYN FLOOD "); break;
      case PORT_SCAN: lcd.print("PORT SCAN "); break;
      default:        lcd.print("ATTACK    "); break;
    }
    lcd.print(stats.lastRate);
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(stats.totalAlerts);
    lcd.print(" M:");
    lcd.print(stats.alertsPerMin);
  }
}

// ============================================================
// 🌐 POST /alert
// ============================================================
void handleAlert() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"no body\"}");
    return;
  }

  String body = server.arg("plain");
  StaticJsonDocument<512> doc;
  if (deserializeJson(doc, body)) {
    server.send(400, "application/json", "{\"error\":\"bad json\"}");
    return;
  }

  const char* type = doc["type"] | "UNKNOWN";
  int rate         = doc["rate"] | 0;
  const char* src  = doc["src_ip"] | "---";

  if (strcmp(type, "UDP_FLOOD") == 0) {
    currentState = UDP_FLOOD;
    stats.udpCount++;
  } else if (strcmp(type, "SYN_FLOOD") == 0) {
    currentState = SYN_FLOOD;
    stats.synCount++;
  } else if (strcmp(type, "PORT_SCAN") == 0) {
    currentState = PORT_SCAN;
    stats.scanCount++;
  } else if (strcmp(type, "SECURE") == 0) {
    currentState = SECURE;
  }

  stats.totalAlerts++;
  stats.lastRate = rate;
  strncpy(stats.lastSrc, src, 15);
  stats.lastSrc[15] = '\0';

  unsigned long now = millis();
  if (now - stats.minuteStart > 60000) {
    stats.minuteStart = now;
    stats.alertsPerMin = 0;
  }
  stats.alertsPerMin++;

  if (stats.totalAlerts % 5 == 0) saveToEEPROM();

  lastAlertTime = now;
  updateLCD();

  String response = "{";
  response += "\"status\":\"ok\",";
  response += "\"total\":" + String(stats.totalAlerts) + ",";
  response += "\"udp\":" + String(stats.udpCount) + ",";
  response += "\"syn\":" + String(stats.synCount) + ",";
  response += "\"scan\":" + String(stats.scanCount) + ",";
  response += "\"per_min\":" + String(stats.alertsPerMin);
  response += "}";
  server.send(200, "application/json", response);

  Serial.print("[ALERT] ");
  Serial.print(type);
  Serial.print(" rate=");
  Serial.print(rate);
  Serial.print(" src=");
  Serial.println(src);
}

// ============================================================
// 🌐 صفحة HTML - Modern Dashboard
// ============================================================
void handleRoot() {
  unsigned long uptime = (millis() - stats.sessionStart) / 1000;
  unsigned long hours = uptime / 3600;
  unsigned long minutes = (uptime % 3600) / 60;
  unsigned long seconds = uptime % 60;

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Triple Attack Detection System</title>
<style>
*{margin:0;padding:0;box-sizing:border-box;}
body{
  font-family:'Segoe UI',Tahoma,Arial,sans-serif;
  background:linear-gradient(135deg,#0f0c29,#302b63,#24243e);
  color:#fff;min-height:100vh;padding:20px;
}
.container{max-width:1200px;margin:0 auto;}
.header{
  text-align:center;padding:30px 20px;margin-bottom:20px;
  background:linear-gradient(135deg,rgba(0,212,255,0.1),rgba(0,255,136,0.1));
  border-radius:20px;
  border:2px solid rgba(0,212,255,0.3);
  box-shadow:0 10px 40px rgba(0,212,255,0.2);
}
.header h1{
  font-size:2.5em;margin-bottom:10px;
  background:linear-gradient(90deg,#00d4ff,#00ff88);
  -webkit-background-clip:text;-webkit-text-fill-color:transparent;
  background-clip:text;
}
.header p{color:#aaa;font-size:1.1em;}
.status-banner{
  padding:30px;border-radius:20px;text-align:center;
  margin-bottom:20px;transition:all 0.5s ease;
  box-shadow:0 10px 40px rgba(0,0,0,0.3);
}
.status-secure{
  background:linear-gradient(135deg,#00ff88,#00d4ff);
  color:#0f0c29;
}
.status-attack{
  background:linear-gradient(135deg,#ff4444,#ff6b6b);
  animation:pulse 1s infinite;
}
@keyframes pulse{
  0%,100%{transform:scale(1);box-shadow:0 10px 40px rgba(255,68,68,0.5);}
  50%{transform:scale(1.02);box-shadow:0 10px 60px rgba(255,68,68,0.8);}
}
.status-banner h2{font-size:2.5em;margin-bottom:10px;}
.status-banner .rate{font-size:1.3em;opacity:0.9;}
.grid{
  display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));
  gap:20px;margin-bottom:20px;
}
.card{
  background:rgba(255,255,255,0.05);
  backdrop-filter:blur(10px);
  border-radius:15px;padding:25px;
  border:1px solid rgba(255,255,255,0.1);
  transition:all 0.3s ease;
  text-align:center;
}
.card:hover{
  transform:translateY(-5px);
  border-color:rgba(0,212,255,0.5);
  box-shadow:0 15px 40px rgba(0,212,255,0.2);
}
.card .icon{font-size:2.5em;margin-bottom:10px;}
.card .value{
  font-size:2.5em;font-weight:bold;
  background:linear-gradient(90deg,#00d4ff,#00ff88);
  -webkit-background-clip:text;-webkit-text-fill-color:transparent;
  background-clip:text;
}
.card .label{color:#aaa;font-size:1em;margin-top:5px;}
.stat-card-udp .value{
  background:linear-gradient(90deg,#ff4444,#ff6b6b);
  -webkit-background-clip:text;-webkit-text-fill-color:transparent;
  background-clip:text;
}
.stat-card-syn .value{
  background:linear-gradient(90deg,#ffa500,#ffcc00);
  -webkit-background-clip:text;-webkit-text-fill-color:transparent;
  background-clip:text;
}
.stat-card-scan .value{
  background:linear-gradient(90deg,#00d4ff,#0088ff);
  -webkit-background-clip:text;-webkit-text-fill-color:transparent;
  background-clip:text;
}
.section{
  background:rgba(255,255,255,0.05);
  backdrop-filter:blur(10px);
  border-radius:15px;padding:25px;margin-bottom:20px;
  border:1px solid rgba(255,255,255,0.1);
}
.section h3{
  font-size:1.5em;margin-bottom:20px;
  color:#00d4ff;border-bottom:2px solid rgba(0,212,255,0.3);
  padding-bottom:10px;
}
.info-row{
  display:flex;justify-content:space-between;
  padding:12px 0;border-bottom:1px solid rgba(255,255,255,0.05);
}
.info-row:last-child{border-bottom:none;}
.info-label{color:#aaa;}
.info-value{font-weight:bold;color:#00d4ff;}
.actions{
  display:flex;gap:15px;justify-content:center;
  margin-top:20px;flex-wrap:wrap;
}
.btn{
  padding:15px 30px;border-radius:10px;
  border:none;cursor:pointer;
  font-size:1em;font-weight:bold;
  text-decoration:none;display:inline-block;
  transition:all 0.3s ease;
}
.btn-danger{
  background:linear-gradient(135deg,#ff4444,#ff6b6b);
  color:#fff;
}
.btn-danger:hover{
  transform:translateY(-3px);
  box-shadow:0 10px 30px rgba(255,68,68,0.4);
}
.btn-primary{
  background:linear-gradient(135deg,#00d4ff,#00ff88);
  color:#0f0c29;
}
.btn-primary:hover{
  transform:translateY(-3px);
  box-shadow:0 10px 30px rgba(0,212,255,0.4);
}
.footer{
  text-align:center;color:#666;
  padding:20px;font-size:0.9em;
}
.live-dot{
  display:inline-block;width:10px;height:10px;
  border-radius:50%;background:#00ff88;
  margin-right:8px;
  animation:blink 1.5s infinite;
}
@keyframes blink{
  0%,100%{opacity:1;}
  50%{opacity:0.3;}
}
</style>
</head>
<body>
<div class="container">

  <div class="header">
    <h1>🛡️ Triple Attack Detection System</h1>
    <p>نظام كشف وتصنيف هجمات الشبكات <span class="live-dot"></span></p>
  </div>
)rawliteral";

  // ============ Status Banner ============
  String cls = (currentState == SECURE) ? "status-secure" : "status-attack";
  String statusIcon, statusText;
  switch (currentState) {
    case SECURE:    statusIcon = "✅"; statusText = "الشبكة آمنة"; break;
    case UDP_FLOOD: statusIcon = "🚨"; statusText = "هجوم UDP Flood"; break;
    case SYN_FLOOD: statusIcon = "🚨"; statusText = "هجوم SYN Flood"; break;
    case PORT_SCAN: statusIcon = "🚨"; statusText = "فحص منافذ"; break;
  }
  
  html += "<div class='status-banner " + cls + "'>";
  html += "<h2>" + statusIcon + " " + statusText + "</h2>";
  if (currentState != SECURE) {
    html += "<div class='rate'>معدل الهجوم: " + String(stats.lastRate) + " حزمة/ثانية</div>";
    html += "<div class='rate'>المصدر: " + String(stats.lastSrc) + "</div>";
  } else {
    html += "<div class='rate'>النظام يراقب الشبكة</div>";
  }
  html += "</div>";

  // ============ Statistics Cards ============
  html += "<div class='grid'>";
  
  html += "<div class='card'>";
  html += "<div class='icon'>📊</div>";
  html += "<div class='value'>" + String(stats.totalAlerts) + "</div>";
  html += "<div class='label'>إجمالي التنبيهات</div>";
  html += "</div>";
  
  html += "<div class='card stat-card-udp'>";
  html += "<div class='icon'>🔴</div>";
  html += "<div class='value'>" + String(stats.udpCount) + "</div>";
  html += "<div class='label'>UDP Flood</div>";
  html += "</div>";
  
  html += "<div class='card stat-card-syn'>";
  html += "<div class='icon'>🟠</div>";
  html += "<div class='value'>" + String(stats.synCount) + "</div>";
  html += "<div class='label'>SYN Flood</div>";
  html += "</div>";
  
  html += "<div class='card stat-card-scan'>";
  html += "<div class='icon'>🔵</div>";
  html += "<div class='value'>" + String(stats.scanCount) + "</div>";
  html += "<div class='label'>Port Scan</div>";
  html += "</div>";
  
  html += "</div>";

  // ============ Info Section ============
  html += "<div class='section'>";
  html += "<h3>📈 معلومات إضافية</h3>";
  html += "<div class='info-row'><span class='info-label'>في آخر دقيقة</span>";
  html += "<span class='info-value'>" + String(stats.alertsPerMin) + " هجوم</span></div>";
  html += "<div class='info-row'><span class='info-label'>مدة التشغيل</span>";
  html += "<span class='info-value'>" + String(hours) + "س " + String(minutes) + "د " + String(seconds) + "ث</span></div>";
  html += "<div class='info-row'><span class='info-label'>آخر معدل</span>";
  html += "<span class='info-value'>" + String(stats.lastRate) + " حزمة/ث</span></div>";
  html += "<div class='info-row'><span class='info-label'>آخر مصدر</span>";
  html += "<span class='info-value'>" + String(stats.lastSrc) + "</span></div>";
  html += "</div>";

  // ============ Actions ============
  html += "<div class='section'>";
  html += "<h3>⚙️ الإجراءات</h3>";
  html += "<div class='actions'>";
  html += "<a href='/reset' class='btn btn-danger'>🔄 إعادة تعيين</a>";
  html += "<a href='/' class='btn btn-primary'>🔃 تحديث</a>";
  html += "</div>";
  html += "</div>";

  html += "<div class='footer'>Triple Attack Detection System v2.0 — IoT Project</div>";

  html += "</div>";
  html += "<script>setTimeout(()=>location.reload(),5000);</script>";
  html += "</body></html>";

  server.send(200, "text/html; charset=utf-8", html);
}

// ============================================================
// 🌐 /stats JSON
// ============================================================
void handleStats() {
  String json = "{\"state\":\"";
  switch (currentState) {
    case SECURE:    json += "SECURE"; break;
    case UDP_FLOOD: json += "UDP_FLOOD"; break;
    case SYN_FLOOD: json += "SYN_FLOOD"; break;
    case PORT_SCAN: json += "PORT_SCAN"; break;
  }
  json += "\",";
  json += "\"total\":" + String(stats.totalAlerts) + ",";
  json += "\"udp\":" + String(stats.udpCount) + ",";
  json += "\"syn\":" + String(stats.synCount) + ",";
  json += "\"scan\":" + String(stats.scanCount) + ",";
  json += "\"per_min\":" + String(stats.alertsPerMin) + ",";
  json += "\"last_rate\":" + String(stats.lastRate) + ",";
  json += "\"last_src\":\"" + String(stats.lastSrc) + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

// ============================================================
// 🌐 /reset
// ============================================================
void handleReset() {
  stats.totalAlerts = 0;
  stats.udpCount = 0;
  stats.synCount = 0;
  stats.scanCount = 0;
  stats.alertsPerMin = 0;
  stats.lastRate = 0;
  strcpy(stats.lastSrc, "---");
  saveToEEPROM();
  server.send(200, "text/html", "<meta http-equiv='refresh' content='1;url=/'>");
  updateLCD();
}

// ============================================================
// 🔧 setup
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\n=== Triple Attack Detection System ===");

  stats.sessionStart = millis();
  stats.minuteStart = millis();

  loadFromEEPROM();

  Wire.begin(4, 5);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi OK");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(3000);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/stats", HTTP_GET, handleStats);
  server.on("/alert", HTTP_POST, handleAlert);
  server.on("/reset", HTTP_GET, handleReset);
  server.begin();

  Serial.println("HTTP server started");
  updateLCD();
}

// ============================================================
// 🔁 loop
// ============================================================
void loop() {
  server.handleClient();

  if (WiFi.status() != WL_CONNECTED) {
    static unsigned long lastReconnect = 0;
    if (millis() - lastReconnect > 10000) {
      lastReconnect = millis();
      WiFi.reconnect();
    }
  }

  if (currentState != SECURE && (millis() - lastAlertTime > RESET_TIMEOUT)) {
    currentState = SECURE;
    updateLCD();
    Serial.println("Reset to SECURE");
  }
}