#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <SPI.h>

// ── Pin Definitions ─────────────────────────────────────────
#define TFT_CS    1   // ← change to your pin
#define TFT_RST   2   // ← change to your pin
#define TFT_DC    3   // ← change to your pin
#define TFT_SCLK  4   // ← change to your pin
#define TFT_MOSI  5   // ← change to your pin

// ── Credentials ─────────────────────────────────────────────
char*       SSID          = "YOUR_WIFI_SSID";
const char* PASSWORD      = "YOUR_WIFI_PASSWORD";
const char* CLIENT_ID     = "YOUR_SPOTIFY_CLIENT_ID";
const char* CLIENT_SECRET = "YOUR_SPOTIFY_CLIENT_SECRET";

// ── Objects ──────────────────────────────────────────────────
Spotify        sp(CLIENT_ID, CLIENT_SECRET);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// ── State ────────────────────────────────────────────────────
String lastArtist    = "";
String lastTrackname = "";

// ────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  // TFT init
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);           // landscape; remove for portrait
  tft.fillScreen(ST77XX_BLACK);
  Serial.println("TFT Initialized!");

  // WiFi
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.printf("\nConnected!\n");

  // Show IP on screen
  tft.setCursor(0, 0);
  tft.write(WiFi.localIP().toString().c_str());

  // Spotify auth
  sp.begin();
  while (!sp.is_auth()) {
    sp.handle_client();
  }
  Serial.println("Connected to Spotify!");
  tft.fillScreen(ST77XX_BLACK); // clear IP after auth
}

// ────────────────────────────────────────────────────────────
void loop() {
  String currentArtist    = sp.current_artist_names();
  String currentTrackname = sp.current_track_name();

  // Update artist if changed and valid
  if (lastArtist != currentArtist &&
      currentArtist != "Something went wrong" &&
      !currentArtist.isEmpty()) {
    tft.fillScreen(ST77XX_BLACK);
    lastArtist = currentArtist;
    Serial.println("Artist: " + lastArtist);
    tft.setCursor(10, 10);
    tft.write(lastArtist.c_str());
  }

  // Update track name if changed and valid
  if (lastTrackname != currentTrackname &&
      currentTrackname != "Something went wrong" &&
      currentTrackname != "null") {
    lastTrackname = currentTrackname;
    Serial.println("Track: " + lastTrackname);
    tft.setCursor(10, 40);
    tft.write(lastTrackname.c_str());
  }

  delay(2000); // respect Spotify's rate limit
}