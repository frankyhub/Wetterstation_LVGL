# Wetterstation_LVGL
ESP32, TFT 2.8", Light and Versatile Graphics Library


<a name="oben"></a>

<div align="center">

  |[:skull:ISSUE](https://github.com/frankyhub/Wetterstation_LVGL/issues?q=is%3Aissue)|[:speech_balloon: Forum /Discussion](https://github.com/frankyhub/Wetterstation_LVGL/discussions)|[:grey_question:WiKi](https://github.com/frankyhub/Wetterstation_LVGL/wiki)|
|--|--|--|
| | | |
|![GitHub issues](https://img.shields.io/github/issues/frankyhub/Wetterstation_LVGL)![GitHub closed issues](https://img.shields.io/github/issues-closed/frankyhub/Wetterstation_LVGL)|![GitHub Discussions](https://img.shields.io/github/discussions/frankyhub/Wetterstation_LVGL)|![GitHub release (with filter)](https://img.shields.io/github/v/release/frankyhub/Wetterstation_LVGL)|
|![GitHub closed pull requests](https://img.shields.io/github/issues-pr-closed/finaldie/skull.svg)[](https://github.com/frankyhub/Wetterstation_LVGL/pulls)|[<img src="https://img.shields.io/github/license/finaldie/skull.svg">](https://github.com/frankyhub/Wetterstation_LVGL/blob/main/LICENSE.md)| ![GitHub User's stars](https://img.shields.io/github/stars/frankyhub)|
</div>


## ESP32 - TFT 2.8 Wetterdaten

![Bild](pic/Wetter01.png)

## Verdrahtung

| TFT LCD | ESP32 | 
| -------- | -------- | 
| T_IRQ	|   GPIO 36| 
| T_OUT	|   GPIO 39| 
| T_DIN| 	  GPIO 32| 
| T_CS	 |  GPIO 33| 
| T_CLK	 |  GPIO 25| 
| SDO(MISO)| 	GPIO 12| 
| LED	|   GPIO 21| 
| SCK	|   GPIO 14| 
| SDI(MOSI)| 	GPIO 13| 
| D/C| 	  GPIO 2| 
| RESET| 	EN/RESET| 
| CS	|   GPIO 15| 
| GND	|   GND| 
| VCC	|   5V (or 3.3V)*| 
| -------- | -------- | 
| *| | 
| VCC = 5V | J1=OPEN| 
| VCC = 3.3V | J1=CLOSE| 
| -------- | -------- | 

Danke an https://randomnerdtutorials.com/

---

<div style="position:absolute; left:2cm; ">   
<ol class="breadcrumb" style="border-top: 2px solid black;border-bottom:2px solid black; height: 45px; width: 900px;"> <p align="center"><a href="#oben">nach oben</a></p></ol>
</div>  

---
