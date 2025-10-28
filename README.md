Contents of this documentation
Automated process to order various drinks from a cobot.

The RobotOrderingSystem is a process automation for detecting empty glasses and initiating automated drink preparation. It connects multiple components, including 3D-printed load-cell scales and cases, two ESP32 microcontroller, a robot arm, and a process engine (CPEE) to enable seamless interaction between hardware and software in an event-driven workflow.

When an empty glass is placed on one of the scales, the ESP32 measures its weight and publishes the data via MQTT. The CPEE process engine is subscribed to the respective MQTT topic and reacts to these messages by triggering the robot arm, which picks up the detected glass and places it in a designated location where another program can continue the workflow (e.g., filling or mixing the drink).

To integrate user feedback, the system also includes a simple voting interface with two physical buttons: a green button for “good” and a red button for “bad.” This allows users to rate each prepared drink, providing valuable input for continuous improvement of recipes and system performance.

The RobotOrderingSystem was created by Aron Lausch as part of the practical course “Sustainable Process Automation” at the Technical University of Munich (TUM). The goal of this project is to document the development process, showcase the integration of IoT and process automation technologies and provide guidance for replicating and extending the system.

# Demo Video
The following video shows the full process: [here](https://youtube.com/shorts/Zt_JufU5usw?feature=share)


The ordering and vote system looks like the following.


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

# Architecture
## Hardware components
### Scale
### 3D printed parts 
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

### weightpublish service
The weightpublish service publishes the weight data of the scales via MQTT to the respective topics. Each scale has its own topic. The data is not send periodically, but rather when it changes above and below a certain treshold (70g).  

### votepublish service
The votepublish service publishes the button data of the scales via MQTT to one topic. Each button publishes its id, when the respective button is pressed. The total amount of votes is stored and updated in the cpe.

### Dashboard 
The dashboard simply displays the ratings of each cocktail (left to right).
<img width="2880" height="1636" alt="Dashboard_Cocktail_Ratings" src="https://github.com/user-attachments/assets/b25d1438-ac18-4798-ab72-8592bc7d4842" />


# Process
There are two main CPE processes: one for the ordering system and one for the vote system. 

## Ordering System Process
The ordering system process looks like the following. It can be found [here.](https://cpee.org/flow/edit.html?monitor=https://cpee.org/flow/engine/1031/)

<img width="720" height="540" alt="OrderingCPE" src="https://github.com/user-attachments/assets/efbd442e-aad5-4aad-b490-e44a95ced652" />

## Step 1
Before starting the process the robot needs to be in the starting position and the gripper needs to be activated the first time. Also, the queue needs to be initialized. 

## Step 2
This process step starts with an XOR which checks if "1","2","3" or "4" is the first value in the queue. If one of the conditions is true, the respective subprocess is started. After processing, the order is dequeued from the queue. The process step 2 is parallel to process step 3.

## Step 3
Now a parallel process is started with four processes, which all wait for (weight) data to arrive. If data is send to one of the subscribed topics, then that process is started, which means that the number of the scale is put into the queue. 

## Example subprocess 1
You can find the subprocess 1 [here.](https://cpee.org/flow/edit.html?monitor=https://cpee.org/flow/engine/421/)
For each order place (scale) there is a subprocess, which is called in the main process. In that subprocess the offset value (for subprocess 1 its 0, for subprocess 2 its 1 etc.) is set via the "drink_ordered" endpoint. 
The robot programm is then called via the "accept_order" endpoint, which executes the actual robot program.
Afterwards a placeholder robot programm is called via the cocktail_1 endpoint.
Future projects could implement the actual making of the cocktail and alter the existing cocktail robot program. 
The extension of the project will be explained in the later section.


