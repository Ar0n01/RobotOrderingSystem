The RobotOrderingSystem is a process automation for detecting empty glasses and initiating automated drink preparation. It connects multiple components, including 3D-printed load-cell scales and cases, two ESP32 microcontroller, a robot arm, and a process engine (CPEE) to enable seamless interaction between hardware and software in an event-driven workflow.

When an empty glass is placed on one of the scales, the ESP32 measures its weight and publishes the data via MQTT. The CPEE process engine is subscribed to the respective MQTT topic and reacts to these messages by triggering the robot arm, which picks up the detected glass and places it in a designated location where another program can continue the workflow (e.g., filling or mixing the drink).

To integrate user feedback, the system also includes a simple voting interface with two physical buttons: a green button for “good” and a red button for “bad.” This allows users to rate each prepared drink, providing valuable input for continuous improvement of recipes and system performance.

The RobotOrderingSystem was created by Aron Lausch as part of the practical course “Sustainable Process Automation” at the Technical University of Munich (TUM). The goal of this project is to document the development process, showcase the integration of IoT and process automation technologies and provide guidance for replicating and extending the system.

# Contents of this documentation
+ [Demo Video](#demo-video)
+ [Architecture and Components](#architecture)
  + [Overview](#overview)
  + [Hardware](#hardware)
  + [Software](#software)
+ [Process](#process)
  + [Step 1](#step1)
  + [Step 2](#step2)
  + [Step 3](#step3)
+ [First setup](#firstsetup)
+ [Credits](#credits)
+ [Contact](#contact)

# Demo Video
The following video shows the full process: [here](https://youtube.com/shorts/Zt_JufU5usw?feature=share)


The ordering and vote system looks like the following.
<img width="4032" height="3024" alt="OrderingSystemOverview" src="https://github.com/user-attachments/assets/dbc34cfa-2013-4fdc-99ae-fb34ae6958ed" />

# Architecture

## Overview
<img width="720" height="405" alt="Architecture Overview Entwicklungspraktikum" src="https://github.com/user-attachments/assets/041ff811-5008-4d0c-ac70-01a5a351af11" />



## Hardware components
### ESP32
<img width="384" height="319" alt="image" src="https://github.com/user-attachments/assets/62f8b965-19be-489e-82a8-e9de8d9c4530" />

### Load cell
<img width="401" height="247" alt="image" src="https://github.com/user-attachments/assets/f2050803-94d8-42f2-b9af-dc2f94b3b9b3" />

### 3D printed scale
<img width="538" height="616" alt="casing" src="https://github.com/user-attachments/assets/acaecca2-5b93-4ea7-aa52-632666626a33" />

### 3D printed cases
<img width="720" height="405" alt="Architecture Overview Entwicklungspraktikum - Scales" src="https://github.com/user-attachments/assets/55b6aa09-0681-4281-86b2-f7c20ab4bc80" />

+ **Scale-case:** Scales are placed on top of the support inside the case. Underneath is room for the ESP-32, cables and electric supply.
  
<img width="720" height="405" alt="Architecture Overview Entwicklungspraktikum" src="https://github.com/user-attachments/assets/fda7844c-9e85-47a5-a454-cce1ae86825e" />

+ **Button-case:** Buttons can be put inside the holes. There are two different cases, one for the outer part and one for the middle parts. Therefore it's modular and can be extended as you like.
+ **ESP32-case:** Case used to cover the ESP32 and power supply on the side of the ordering system. 

### Robot
The cobot arm in the university lab is already set up. It has three pre-programmed moves that you can start using specific endpoints.

## Software
### CPE
The CPEE is a process engine that coordinates the process and steers all components of the ordering system via HTTP. Read more about the CPEE [here](https://cpee.org/). This specific CPEE process is available [here](https://cpee.org/flow/edit.html?monitor=https://cpee.org/flow/engine/135/).

### Weightpublish service
The weightpublish service publishes the weight data of the scales via MQTT to the respective topics. Each scale has its own topic. The data is not send periodically, but rather when it changes above and below a certain treshold (70g).  

### Votepublish service
The votepublish service publishes the button data of the scales via MQTT to one topic. Each button publishes its id, when the respective button is pressed. The total amount of votes is stored and updated in the cpe.

### Dashboard 
The dashboard simply displays the ratings of each cocktail (left to right).
<img width="2880" height="1636" alt="Dashboard_Cocktail_Ratings" src="https://github.com/user-attachments/assets/b25d1438-ac18-4798-ab72-8592bc7d4842" />

# Process
There are two main CPE processes: one for the ordering system and one for the vote system. Each one will be explained in more detail in the following.

## Ordering system process
The ordering system process looks like the following. It can be found [here.](https://cpee.org/flow/edit.html?monitor=https://cpee.org/flow/engine/1031/)

<img width="720" height="540" alt="OrderingCPE" src="https://github.com/user-attachments/assets/efbd442e-aad5-4aad-b490-e44a95ced652" />

## Step 1
Before starting the process the robot needs to be in the starting position and the gripper needs to be activated the first time. Also, the queue needs to be initialized. 

## Step 2
This process step starts with an XOR which checks if "1","2","3" or "4" is the first value in the queue. If one of the conditions is true, the respective subprocess is started. After processing, the order is dequeued from the queue. The process step 2 is parallel to process step 3.

## Step 3
Now a parallel process is started with four processes, which all wait for (weight) data to arrive. If data is send to one of the subscribed topics, the respective values get updated. If the new value is above 70g, then the order is put into the queue.

## Example subprocess 1
You can find the subprocess 1 [here.](https://cpee.org/flow/edit.html?monitor=https://cpee.org/flow/engine/421/)
For each order place (scale) there is a subprocess, which is called in the main process. In that subprocess the offset value (for subprocess 1 its 0, for subprocess 2 its 1 etc.) is set via the "drink_ordered" endpoint. 
The robot programm is then called via the "accept_order" endpoint, which executes the actual robot program.
Afterwards a placeholder robot programm is called via the cocktail_1 endpoint.
Future projects could implement the actual making of the cocktail and alter the existing cocktail robot program. 
The extension of the project will be explained in the later section.

## Vote process
You can find the vote process [here.](https://cpee.org/flow/edit.html?monitor=https://cpee.org/flow/engine/421/)
<img width="2880" height="1556" alt="ButtonProcessCPE" src="https://github.com/user-attachments/assets/a67a3112-fcad-4215-a603-c197fc081eb7" />
This process subscribes to the MQTT topic of the buttons. Therefore, whenever a button is pushed, the ID of the button is send here. 
In the event of new data, the array is altered (+1 added) at the index of the sent value - 1.

## First setup
The software is split up into three main components:
+ (1) publishweight service
+ (2) publishvote service
+ (3) dashboard

The publishweight service and publishvote service run on the respective ESPs-32. The first ESP-32 needs to be connected to the scales and the second one to the buttons. In the following I'll briefly show how to connect the ESP-32 with the hardware.

### Wiring of scales with ESP-32
![Uploadi<svg width="1280" height="720" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" overflow="hidden"><defs><linearGradient x1="200.5" y1="178.5" x2="200.5" y2="589.5" gradientUnits="userSpaceOnUse" spreadMethod="reflect" id="fill0"><stop offset="0" stop-color="#D2D2D2"/><stop offset="0.5" stop-color="#C8C8C8"/><stop offset="1" stop-color="#C0C0C0"/></linearGradient><linearGradient x1="640.5" y1="178.5" x2="640.5" y2="349.5" gradientUnits="userSpaceOnUse" spreadMethod="reflect" id="fill1"><stop offset="0" stop-color="#D2D2D2"/><stop offset="0.5" stop-color="#C8C8C8"/><stop offset="1" stop-color="#C0C0C0"/></linearGradient><linearGradient x1="640.5" y1="418.5" x2="640.5" y2="589.5" gradientUnits="userSpaceOnUse" spreadMethod="reflect" id="fill2"><stop offset="0" stop-color="#D2D2D2"/><stop offset="0.5" stop-color="#C8C8C8"/><stop offset="1" stop-color="#C0C0C0"/></linearGradient><linearGradient x1="1080.5" y1="178.5" x2="1080.5" y2="349.5" gradientUnits="userSpaceOnUse" spreadMethod="reflect" id="fill3"><stop offset="0" stop-color="#D2D2D2"/><stop offset="0.5" stop-color="#C8C8C8"/><stop offset="1" stop-color="#C0C0C0"/></linearGradient></defs><g><rect x="0" y="0" width="1280" height="720" fill="#AFABAB"/><path d="M96.3597 199 58 199" stroke="#FF0000" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd"/><path d="M0 0 0.249974 68.0406" stroke="#FF0000" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(-1 0 0 1 59.25 131)"/><path d="M58 132 453.5 132" stroke="#FF0000" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd"/><path d="M0 0 107.789 0.000104987" stroke="#CC6600" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(-1 0 0 1 411.789 575)"/><rect x="95.5001" y="178.5" width="210" height="411" stroke="#000000" stroke-width="0.666667" stroke-miterlimit="8" fill="url(#fill0)"/><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="24" transform="translate(167.911 391)">ESP 32</text><rect x="535.5" y="178.5" width="210" height="171" stroke="#000000" stroke-width="0.666667" stroke-miterlimit="8" fill="url(#fill1)"/><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="24" transform="translate(608 271)">HX711</text><rect x="535.5" y="418.5" width="210" height="171" stroke="#000000" stroke-width="0.666667" stroke-miterlimit="8" fill="url(#fill2)"/><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="24" transform="translate(613.75 511)">OLED</text><rect x="975.5" y="178.5" width="210" height="171" stroke="#000000" stroke-width="0.666667" stroke-miterlimit="8" fill="url(#fill3)"/><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="24" transform="translate(1035.62 271)">Load Cell</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(544.758 491)">GND</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(544.758 454)">VDD</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(544.758 565)">SDA</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(544.758 528)">SCK</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(544.758 250)">DT</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(544.758 212)">GND</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(544.758 324)">VCC</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(544.758 287)">SCK</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(719.398 250)">E</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(730.898 250)">-</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(716.232 212)">E +</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(714.732 324)">A +</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(717.898 287)">A</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(730.898 287)">-</text><path d="M0 0 0.000104987 316.731" stroke="#FF0000" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(1 0 0 -1 453 447.731)"/><path d="M0 0 83.3162 0.000104987" stroke="#FF0000" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(-1 0 0 1 536.316 448)"/><path d="M0 0 83.0662 0.000104987" stroke="#FF0000" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(-1 0 0 1 536.066 319)"/><path d="M0 0 124.316 1.04315" stroke="#CC6600" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(-1 0 0 1 535.316 485)"/><path d="M0 0 1.0001 367.536" stroke="#CC6600" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(-1 0 0 1 412 207)"/><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(265.109 580)">GND</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(105.468 204)">5V</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(227.609 522)">G 22</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(265.109 522)">(I2C)</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(231.275 465)">G 21 (I2C)</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(263.775 316)">G 16</text><text font-family="Calibri,Calibri_MSFontService,sans-serif" font-weight="400" font-size="16" transform="translate(264.609 288)">G 4</text><path d="M535.316 207 411 207" stroke="#CC6600" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd"/><path d="M0 0 45.5517 0 45.5517 38.1934 230.105 38.1934" stroke="#4472C4" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(1 0 0 -1 305 282.193)"/><path d="M0 0 66.8018 0 66.8018 29.6075 231.605 29.6075" stroke="#00B050" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(1 0 0 -1 304 311.607)"/><path d="M305 460 373.054 460 373.054 560.554 535.105 560.554" stroke="#4472C4" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd"/><path d="M305 517 345.052 517 345.052 522.647 535.105 522.647" stroke="#00B050" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd"/><path d="M0 0 229.105 0.000104987" stroke="#FF0000" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(-1 0 0 1 974.105 207)"/><path d="M0 0 230.105 0.000104987" stroke="#000000" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(-1 0 0 1 975.105 244)"/><path d="M0 0 230.105 0.416798" stroke="#FFFFFF" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd" transform="matrix(-1 0 0 1 975.105 281)"/><path d="M975.605 319.834 745 319" stroke="#00B050" stroke-width="2" stroke-miterlimit="8" fill="none" fill-rule="evenodd"/></g></svg>ng Wiring Diagram.svg…]()


### Wiring of buttons with ESP-32
The wiring of the buttons is pretty straight forward. First you connect one row with the gnd pin. For one button you always connect one cable with a normal pin (12,13, etc) and then one cable with the gnd pin (row). 
<img width="3024" height="4032" alt="Wiring_buttons (1)" src="https://github.com/user-attachments/assets/b3cbbbf3-7570-4940-afda-0ae841ecc044" />

### Scale setup
The code for the ESP-32 (scales) is in the `publishweight-service` subdirectory. The file is named "publishweight". 

The simplest way to get the code compiling and running is to use PlatformIO which can be installed as a VS Code Extension.

When PlatformIO is installed just open the command palette (`Ctrl + Shift + P` when using the standard VS Code keyboard shortcuts) and run the "Upload" or "Upload and Monitor" PlatformIO task.
This automatically compiles the code and uploads it to the connected ESP-32.

In the `publishweight-service` folder, there are two code files, `calibratescale.cpp` and `publishweight.cpp`.
To decide which of the files to compile and upload to the ESP change the `build_src_filter` property in the `platformio.ini` file within the root folder.

First, each load cell has to be calibrated, for this, the calibration value has to be found empirically. This can be done by selecting the `calibratescale.cpp` in the `platformio.ini` file and then using the "Upload and Monitor" task to run the calibration program. When following the program either note the calibration factor down or save it to EEPROM.

Before the weight cell can be deployed using the `publishweight.cpp` file, a couple of things have to be adapted in it.
If you manually noted down the factor, set `useEEPROM` to `false` and update the `calibrationValue` variable accordingly. If you saved it to EEPROM just set the `useEEPROM` to `true`.

To connect to the WiFi, set the WiFi's `ssid` and `password` to the corresponding variables.
Lastly, to allow communication with the MQTT Broker you have to set the port (`port` variable) and IP Address (`server` variable) of the Mosquitto MQTT Broker. 

### Button setup
The code for the ESP-32 (buttons) is in the `publisvotes-service` subdirectory. The file is named "publishvotes". 
This service reads up to 8 buttons on an ESP32 and publishes the pressed button ID via MQTT to a topic. Connect your buttons to the GPIO pins you want (one side to the GPIO, the other to GND). I'll provide an example as a picture below.

+ (1) In the code, configure the button_pins to match your wiring order, e.g. const int button_pins[8] = {13, 32, 33, 25, 26, 27, 14, 12}
The order in this array defines which button sends which ID (1–8). 

+ (2) Enter your Wi-Fi and MQTT broker details in the top section.

+ (3) Upload the code to your ESP32.

+ (4) When you press a button, its ID (1–8) is sent to the MQTT topic. In this example to al/votes

# Credits
Scale calibration, scale wiring and scale stl acquired from [here.](https://github.com/DominikVoigt/esp32-weightsensor)
