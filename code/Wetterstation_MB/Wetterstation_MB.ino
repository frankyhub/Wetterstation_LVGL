/************************************************************************************************* 
                                      PROGRAMMINFO
************************************************************************************************** 
Funktion: ESP32 TFT2.8" Wetterstatio
**************************************************************************************************
Version: 20.09.2024
**************************************************************************************************
Board:  ESP DEV Module ESP32-2432S028R
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
**************************************************************************************************
C++ Arduino IDE V2.3.2
**************************************************************************************************
**************************************************************************************************
Librarys
- lvgl" library version 9.X by kisvegabor - https://lvgl.io/
- TFT_eSPI" library by Bodmer - https://github.com/Bodmer/TFT_eSPI
- WiFi.h V0.16.1
- WebServer.h
 **************************************************************************************************/
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "weather_images.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "R2-D2";
const char* password = "xxx";


String latitude = "47.7900";
String longitude = "11.8333";
String location = "Miesbach";
String timezone = "Europe/Berlin";


String current_date;
String last_weather_update;
String temperature;
String humidity;
int is_day;
int weather_code = 0;
String weather_description;

#define TEMP_CELSIUS 1

#if TEMP_CELSIUS
  String temperature_unit = "";
  const char degree_symbol[] = "\u00B0C";
#else
  String temperature_unit = "&temperature_unit=fahrenheit";
  const char degree_symbol[] = "\u00B0F";
#endif

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

static lv_obj_t * weather_image;
static lv_obj_t * text_label_date;
static lv_obj_t * text_label_temperature;
static lv_obj_t * text_label_humidity;
static lv_obj_t * text_label_weather_description;
static lv_obj_t * text_label_time_location;

static void timer_cb(lv_timer_t * timer){
  LV_UNUSED(timer);
  get_weather_data();
  get_weather_description(weather_code);
  lv_label_set_text(text_label_date, current_date.c_str());
  lv_label_set_text(text_label_temperature, String("      " + temperature + degree_symbol).c_str());
  lv_label_set_text(text_label_humidity, String("   " + humidity + "%").c_str());
  lv_label_set_text(text_label_weather_description, weather_description.c_str());
  lv_label_set_text(text_label_time_location, String("Letztes Update: " + last_weather_update + "  |  " + location).c_str());
}

void lv_create_main_gui(void) {
  LV_IMAGE_DECLARE(image_weather_sun);
  LV_IMAGE_DECLARE(image_weather_cloud);
  LV_IMAGE_DECLARE(image_weather_rain);
  LV_IMAGE_DECLARE(image_weather_thunder);
  LV_IMAGE_DECLARE(image_weather_snow);
  LV_IMAGE_DECLARE(image_weather_night);
  LV_IMAGE_DECLARE(image_weather_temperature);
  LV_IMAGE_DECLARE(image_weather_humidity);

  get_weather_data();

  weather_image = lv_image_create(lv_screen_active());
  lv_obj_align(weather_image, LV_ALIGN_CENTER, -80, -20);
  
  get_weather_description(weather_code);

  text_label_date = lv_label_create(lv_screen_active());
  lv_label_set_text(text_label_date, current_date.c_str());
  lv_obj_align(text_label_date, LV_ALIGN_CENTER, 70, -70);
  lv_obj_set_style_text_font((lv_obj_t*) text_label_date, &lv_font_montserrat_26, 0);
  lv_obj_set_style_text_color((lv_obj_t*) text_label_date, lv_palette_main(LV_PALETTE_TEAL), 0);

  lv_obj_t * weather_image_temperature = lv_image_create(lv_screen_active());
  lv_image_set_src(weather_image_temperature, &image_weather_temperature);
  lv_obj_align(weather_image_temperature, LV_ALIGN_CENTER, 30, -25);
  text_label_temperature = lv_label_create(lv_screen_active());
  lv_label_set_text(text_label_temperature, String("      " + temperature + degree_symbol).c_str());
  lv_obj_align(text_label_temperature, LV_ALIGN_CENTER, 70, -25);
  lv_obj_set_style_text_font((lv_obj_t*) text_label_temperature, &lv_font_montserrat_22, 0);

  lv_obj_t * weather_image_humidity = lv_image_create(lv_screen_active());
  lv_image_set_src(weather_image_humidity, &image_weather_humidity);
  lv_obj_align(weather_image_humidity, LV_ALIGN_CENTER, 30, 20);
  text_label_humidity = lv_label_create(lv_screen_active());
  lv_label_set_text(text_label_humidity, String("   " + humidity + "%").c_str());
  lv_obj_align(text_label_humidity, LV_ALIGN_CENTER, 70, 20);
  lv_obj_set_style_text_font((lv_obj_t*) text_label_humidity, &lv_font_montserrat_22, 0);

  text_label_weather_description = lv_label_create(lv_screen_active());
  lv_label_set_text(text_label_weather_description, weather_description.c_str());
  lv_obj_align(text_label_weather_description, LV_ALIGN_BOTTOM_MID, 0, -40);
  lv_obj_set_style_text_font((lv_obj_t*) text_label_weather_description, &lv_font_montserrat_18, 0);

  text_label_time_location = lv_label_create(lv_screen_active());
  lv_label_set_text(text_label_time_location, String("letztes Update: " + last_weather_update + "  |  " + location).c_str());
  lv_obj_align(text_label_time_location, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_style_text_font((lv_obj_t*) text_label_time_location, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color((lv_obj_t*) text_label_time_location, lv_palette_main(LV_PALETTE_GREY), 0);

  lv_timer_t * timer = lv_timer_create(timer_cb, 600000, NULL);
  lv_timer_ready(timer);
}

/*
  WWetter Interpretation 
  0	Clear sky
  1, 2, 3	Mainly clear, partly cloudy, and overcast
  45, 48	Fog and depositing rime fog
  51, 53, 55	Drizzle: Light, moderate, and dense intensity
  56, 57	Freezing Drizzle: Light and dense intensity
  61, 63, 65	Rain: Slight, moderate and heavy intensity
  66, 67	Freezing Rain: Light and heavy intensity
  71, 73, 75	Snow fall: Slight, moderate, and heavy intensity
  77	Snow grains
  80, 81, 82	Rain showers: Slight, moderate, and violent
  85, 86	Snow showers slight and heavy
  95 *	Thunderstorm: Slight or moderate
  96, 99 *	Thunderstorm with slight and heavy hail
*/
void get_weather_description(int code) {
  switch (code) {
    case 0:
      if(is_day==1) { lv_image_set_src(weather_image, &image_weather_sun); }
      else { lv_image_set_src(weather_image, &image_weather_night); }
      weather_description = "klarer Himmel";
      break;
    case 1: 
      if(is_day==1) { lv_image_set_src(weather_image, &image_weather_sun); }
      else { lv_image_set_src(weather_image, &image_weather_night); }
      weather_description = "vorwiegend klar";
      break;
    case 2: 
      lv_image_set_src(weather_image, &image_weather_cloud);
      weather_description = "teilweise bewölkt";
      break;
    case 3:
      lv_image_set_src(weather_image, &image_weather_cloud);
      weather_description = "bedeckt";
      break;
    case 45:
      lv_image_set_src(weather_image, &image_weather_cloud);
      weather_description = "Nebel";
      break;
    case 48:
      lv_image_set_src(weather_image, &image_weather_cloud);
      weather_description = "Raureifnebel";
      break;
    case 51:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "leichter Nieselregen";
      break;
    case 53:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "Nieselregen";
      break;
    case 55:
      lv_image_set_src(weather_image, &image_weather_rain); 
      weather_description = "starker Nieselregen";
      break;
    case 56:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "gefrierender Nieselregen";
      break;
    case 57:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "gefrierender Nieselregen";
      break;
    case 61:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "leichter Regen";
      break;
    case 63:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "mittlerer Regen";
      break;
    case 65:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "starker Regen";
      break;
    case 66:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "gefrierender Regen";
      break;
    case 67:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "gefrierender, starker Regen";
      break;
    case 71:
      lv_image_set_src(weather_image, &image_weather_snow);
      weather_description = "leichter Schneefall";
      break;
    case 73:
      lv_image_set_src(weather_image, &image_weather_snow);
      weather_description = "Schnefall";
      break;
    case 75:
      lv_image_set_src(weather_image, &image_weather_snow);
      weather_description = "starker Schneefall";
      break;
    case 77:
      lv_image_set_src(weather_image, &image_weather_snow);
      weather_description = "Hagel";
      break;
    case 80:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "leichte Regenschauer";
      break;
    case 81:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "Regenschauer";
      break;
    case 82:
      lv_image_set_src(weather_image, &image_weather_rain);
      weather_description = "heftige Regenschauer";
      break;
    case 85:
      lv_image_set_src(weather_image, &image_weather_snow);
      weather_description = "leichter Schneefall";
      break;
    case 86:
      lv_image_set_src(weather_image, &image_weather_snow);
      weather_description = "starker Schneefall";
      break;
    case 95:
      lv_image_set_src(weather_image, &image_weather_thunder);
      weather_description = "Gewitter";
      break;
    case 96:
      lv_image_set_src(weather_image, &image_weather_thunder);
      weather_description = "Gewitter, leichter Hagel";
      break;
    case 99:
      lv_image_set_src(weather_image, &image_weather_thunder);
      weather_description = "Gewitter und Hagel";
      break;
    default: 
      weather_description = "unbekannter Wettercode";
      break;
  }
}

void get_weather_data() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String("http://api.open-meteo.com/v1/forecast?latitude=" + latitude + "&longitude=" + longitude + "&current=temperature_2m,relative_humidity_2m,is_day,precipitation,rain,weather_code" + temperature_unit + "&timezone=" + timezone + "&forecast_days=1");
    http.begin(url);
    int httpCode = http.GET(); // Make the GET request

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        //Serial.println("Request information:");
        //Serial.println(payload);
        // Parse the JSON to extract the time
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          const char* datetime = doc["current"]["time"];
          temperature = String(doc["current"]["temperature_2m"]);
          humidity = String(doc["current"]["relative_humidity_2m"]);
          is_day = String(doc["current"]["is_day"]).toInt();
          weather_code = String(doc["current"]["weather_code"]).toInt();
          /*Serial.println(temperature);
          Serial.println(humidity);
          Serial.println(is_day);
          Serial.println(weather_code);
          Serial.println(String(timezone));*/
          // Split the datetime into date and time
          String datetime_str = String(datetime);
          int splitIndex = datetime_str.indexOf('T');
          current_date = datetime_str.substring(0, splitIndex);
          last_weather_update = datetime_str.substring(splitIndex + 1, splitIndex + 9); // Extract time portion
        } else {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
        }
      }
    } else {
  Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end(); // Close connection
  } else {
    Serial.println("Not connected to Wi-Fi");
  }
}

void setup() {
  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.begin(115200);
  Serial.println(LVGL_Arduino);

  //Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to Wi-Fi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  // LVGL
  lv_init();
  // Register print function for debugging
  lv_log_register_print_cb(log_print);

  // display object
  lv_display_t * disp;
  disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);
  lv_create_main_gui();
}

void loop() {
  lv_task_handler();  
  lv_tick_inc(5);     
  delay(5);           
}
