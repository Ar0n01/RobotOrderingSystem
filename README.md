Contents of this documentation
Automated process to order various drinks from a cobot.

The RobotOrderingSystem is a process automation for detecting empty glasses and initiating automated drink preparation. It connects multiple components, including 3D-printed load-cell scales and cases, two ESP32 microcontroller, a robot arm, and a process engine (CPEE) to enable seamless interaction between hardware and software in an event-driven workflow.

When an empty glass is placed on one of the scales, the ESP32 measures its weight and publishes the data via MQTT. The CPEE process engine is subscribed to the respective MQTT topic and reacts to these messages by triggering the robot arm, which picks up the detected glass and places it in a designated location where another program can continue the workflow (e.g., filling or mixing the drink).

To integrate user feedback, the system also includes a simple voting interface with two physical buttons: a green button for “good” and a red button for “bad.” This allows users to rate each prepared drink, providing valuable input for continuous improvement of recipes and system performance.

The RobotOrderingSystem was created by Aron Lausch as part of the practical course “Sustainable Process Automation” at the Technical University of Munich (TUM). The goal of this project is to document the development process, showcase the integration of IoT and process automation technologies and provide guidance for replicating and extending the system.

# Demo Video
The following video shows the full process: XXX

The ordering systems looks like the following: XXX

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
+ [Contact](#contact)


# Hardware
## 3D printed parts 
<img width="720" height="405" alt="Architecture Overview Entwicklungspraktikum - Scales" src="https://github.com/user-attachments/assets/55b6aa09-0681-4281-86b2-f7c20ab4bc80" />

+ **Scale-case:** Scales are placed on top of the support inside the case. Underneath is room for the ESP-32, cables and electric supply.
  
<img width="720" height="405" alt="Architecture Overview Entwicklungspraktikum" src="https://github.com/user-attachments/assets/fda7844c-9e85-47a5-a454-cce1ae86825e" />

+ **Button-case:** Buttons can be put inside the holes. There are two different cases, one for the outer part and one for the middle parts. Therefore it's modular and can be extended as you like.
+ **ESP32-case:** Case used to cover the ESP32 and power supply on the side of the ordering system. 

# Robot
The cobot arm in the university lab is already set up. It has three pre-programmed moves that you can start using specific endpoints.

# Software
## CPE
The CPEE is a process engine that coordinates the process and steers all components of the ordering system via HTTP. Read more about the CPEE [here](https://cpee.org/). This specific CPEE process is available [here](https://cpee.org/flow/edit.html?monitor=https://cpee.org/flow/engine/135/).



