<!-- PROJECT LOGO -->
<br />
<div align="center">
  <h1 align="center">Aerolyze</h1>
</div>

## Contributors
This Final Project made by Group 10:
- Aulia Anugrah Aziz - (2206059364)
- Aliyah Rizky Al-Afifah Polanda - (2206024682)
- Christopher Satya Fredella Balakosa - (2206059755)
- Mario Matthews Gunawan - (2206810452)

# I. Introduction to the problem and the solution

Gas leaks, such as LPG or carbon monoxide (CO), pose significant risks to household safety, leading to fires, explosions, or poisoning if not detected promptly. Traditional detection methods, relying on manual observation or standalone alarms, often result in delayed responses, increasing the potential for harm to lives and property. The rapid spread of hazardous gases underscores the need for a reliable, proactive monitoring system to address these challenges.

Aerolyze offers an innovative solution with a smart gas leakage detection system that ensures real-time monitoring, immediate alerts, and automated responses. Leveraging IoT technology, the device utilizes the MQ-2 sensor to detect LPG and CO gases, providing digital notifications via WhatsApp and integrated auditory and visual warnings through a buzzer and LED. This comprehensive approach empowers users to act swiftly, effectively minimizing risks and enhancing household safety.


## Features

- **Detect LPG and toxic carbon monoxide (CO) gases:** Keep an eye Aerolyze utilizes the MQ-2 sensor to detect hazardous gases such as LPG and toxic carbon monoxide (CO), ensuring accurate and continuous monitoring of household air quality.
- **Give real-time alerts using buzzer and red LED:** The device activates an integrated buzzer and red LED to deliver immediate auditory and visual warnings when dangerous gas levels are detected, ensuring residents are promptly alerted to potential threats.
- **Give notification to user by Whatsapp:** Aerolyze sends real-time notifications to the user's WhatsApp, providing instant updates and enabling swift action to address the detected hazard.

# II. Hardware design and implementation details

## Schematic

<img width="7634" alt="Diagram Schematic" src="https://cdn.discordapp.com/attachments/859432769956741121/1316045006137593937/System_Schematic.jpg?ex=67599dec&is=67584c6c&hm=1975338209c7cb83459715ebe985fc83b83820c2fb49fa01dce1de93c829a620&">


The key components in our system are:
- **MQ-2 Gas Sensor**
The sensor detects the presence of gas. The analog output of the sensor is connected to the analog pin of the microcontroller for continuous data sampling
- **ESP32**
The ESP32 serves as the master and the slave node. Both of them have a Wifi module which enables wireless communication using painless mesh. This also makes the notification alert possible through WhatsApp as the master node establishes a connection with the internet.
- **Buzzer and LEDs**
These components act as alerts to notify the user when the gas concentration exceeds the threshold.
- **Power Supply**
The master node is connected to a laptop for the user to be able to read the serial monitor. Meanwhile the slave node is connected to a power bank to ensure the transfer of the sensor data to the master node.

The hardware design focuses on using the MQ-2 gas sensor to detect harmful gases like LPG and Carbon Monoxide (CO). This sensor is connected to a central control unit, called the master, which processes data from the sensor and connects to WiFi for communication. If the gas level goes above a safe limit, the system will activate a buzzer and a red LED to give immediate warnings through sound and light.

The master device also can send notifications to the user's WhatsApp using an API. This ensures users are informed about gas leaks even if they are away from home. The hardware setup combines the MQ-2 sensor, a WiFi-enabled master unit, a buzzer, and an LED to create a system that monitors gas levels and provides quick warnings to improve household safety.

# III. Software Implementation Details
The software implementation for Aerolyze is crafted in C++ and leverages several libraries to enhance its functionality and IoT capabilities. Key libraries include painlessMesh.h for creating a mesh network, WiFi.h for internet connectivity, and HTTPClient.h for interacting with APIs, enabling seamless communication and integration with web services. Additional libraries like UrlEncode.h and time.h handle data formatting and time-related functions. 

The program is divided into two parts: the Leaf Program (manages local sensor data and basic alerts) and the Root Program (coordinates system operations, sends notifications, and triggers alarms). The Leaf Program is responsible for collecting gas sensor data from the MQ-2 sensor and transmitting it to the root node via a mesh network. It uses the painlessMesh library to maintain communication and sets up tasks with FreeRTOS for reading sensor data and sending it across the network. Visual indicators, such as green and red LEDs, provide immediate feedback on gas safety levels. The program also incorporates queues to manage sensor data efficiently and ensures that the mesh network stays updated for real-time responsiveness.

The Root Program serves as the system's central control, handling received data from the leaf nodes, managing alarms, and sending real-time notifications to the user via WhatsApp using the CallMeBot API. Tasks within this program monitor gas levels, trigger alarms when dangerous conditions are detected, and deactivate alerts when conditions return to normal. The program also includes advanced features like a timeout mechanism for alarms and robust error handling for notification delivery. By combining mesh networking, real-time monitoring, and IoT integration, this software ensures a comprehensive and efficient solution for hazardous gas detection and user notification.

Flowchart showing how our project works:
- **Flowchart:**
  <img width="7634" alt="Diagram Schematic" src="https://cdn.discordapp.com/attachments/859432769956741121/1316050910404935750/231674.jpg?ex=6759a36c&is=675851ec&hm=a72bbec3ca8cae1ec66c3eee46ebfeb4e7a8b5d6758acbc92d3cd7bfae9f4e6d&">

# IV. Test results and performance evaluation
The testing process aimed to validate the functionality and reliability of the IoT gas monitoring system in detecting hazardous gases and sending notifications. Both unit and integration testing were conducted using the Arduino IDE and actual hardware to ensure the system met its functional requirements. 

Unit testing:
- Leaf Node Testing: Verified the MQ-2 sensor's ability to detect hazardous gas levels accurately when exposed to smoke.
- Root Node Testing: Confirmed the node's ability to process gas data and send WhatsApp notifications using the CallMeBot API.

Integration testing combined the leaf and root nodes into a complete system and assessed end-to-end operations. The system was exposed to simulated hazardous gas, such as smoke from burning paper, to validate its ability to detect gas, activate alarms, and send real-time WhatsApp notifications. The system successfully performed as expected, with seamless coordination between components and reliable alert delivery.

The system performed effectively in detecting hazardous gases and delivering real-time notifications, with smooth interaction between the leaf and root nodes during testing. However, some performance limitations were noted, including occasional delays or failures in notification delivery due to API server load and variations in response time under different environmental conditions. These findings suggest areas for improvement in notification handling and system responsiveness to enhance reliability in more dynamic real-world scenarios.

# V. Conclusion and Future Work
Aerolyze is an advanced solution designed to enhance household safety by detecting hazardous gas leaks using the MQ-2 sensor and IoT technology. It monitors gases like LPG and carbon monoxide, which can pose significant health risks. The system uses the ESP32 microcontroller, a mesh network for communication, and a dual-alert mechanism (buzzer and LED) to notify users when dangerous gas levels are detected. Real-time notifications are sent via WhatsApp, keeping users informed even when away from home.

The software is structured into the Leaf Program and Root Program, which handle data collection, processing, and communication. The system uses FreeRTOS for efficient task management and scalability. Integration with IoT and mesh networking ensures seamless communication and real-time alerts. Testing showed that Aerolyze effectively detects gas leaks and meets functional requirements. However, further improvements in notification reliability and response time would enhance the system's performance in diverse environments.

## Documentation

- **Main Device:**

  <img width="7634" alt="Diagram Schematic" src="https://cdn.discordapp.com/attachments/859432769956741121/1316045006137593937/System_Schematic.jpg?ex=67599dec&is=67584c6c&hm=1975338209c7cb83459715ebe985fc83b83820c2fb49fa01dce1de93c829a620&">

- **Documentation:**
  <img width="7634" alt="Diagram Schematic" src="https://cdn.discordapp.com/attachments/859432769956741121/1316050253392248904/Kerkom.jpg?ex=6759a2cf&is=6758514f&hm=6c28a3bbe481010115b5924a609447f419e2b88b91d0352ed3d7a52b7d551431&">