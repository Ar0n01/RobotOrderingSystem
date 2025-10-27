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
+ [Demo Video](#Demo-Video)
+ [Architecture and Components](#)
  + [Overview](#)
  + [Hardware](#)
  + [Software](#)


