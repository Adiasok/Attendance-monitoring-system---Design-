# Attendance-monitoring-system---Design-
This project is a Attendance Monitoring System built around a custom-designed PCB using the ESP32-S3-WROOM-2. The system automates attendance logging by integrating biometric fingerprint authentication, RFID-based identification, and a display interface, making it suitable for academic or organizational environments.




![image alt](https://github.com/Adiasok/Attendance-monitoring-system---Design-/blob/fb9c81271bf0663167ea8faac8529da7dc7ce840/Screenshot%20(28).png)


The above image of the schematic represents the top-level architecture of the Smart Attendance Monitoring System. It shows the interconnection between major functional blocks including the ESP32-S3 MCU, power supply, battery, storage, RFID module, fingerprint sensor, display, RTC, and audio subsystem. Communication protocols such as UART, SPI, I²C, and I²S are used and distinguished, providing a clean hierarchical view of the system design.



![image alt](https://github.com/Adiasok/Attendance-monitoring-system---Design-/blob/fb9c81271bf0663167ea8faac8529da7dc7ce840/Screenshot%20(29).png)


This schematic implements the audio output subsystem using the MAX98357A I²S digital audio amplifier. The ESP32-S3 provides I²S signals (BCLK, LRCLK, DIN), which are converted to a differential speaker output. Proper decoupling capacitors and shutdown/gain configuration resistors are included to ensure stable operation and low-noise audio playback. 



![image alt](https://github.com/Adiasok/Attendance-monitoring-system---Design-/blob/fb9c81271bf0663167ea8faac8529da7dc7ce840/Screenshot%20(30).png)


This block uses the DS3231M real-time clock to provide accurate timekeeping for timestamping attendance records. The RTC communicates with the MCU via the I²C interface and includes a backup coin cell for time retention during power loss. Pull-up resistors, decoupling capacitors, and an optional interrupt/output clock pin are included for reliable operation.

![image alt](https://github.com/Adiasok/Attendance-monitoring-system---Design-/blob/fb9c81271bf0663167ea8faac8529da7dc7ce840/Screenshot%20(31).png)

This schematic shows the MicroSD card interface used for local storage of attendance logs. The SD card operates at 3.3 V and communicates with the ESP32-S3 using the SD/SPI protocol. Proper power decoupling and grounding of the shield are implemented to improve signal integrity and EMI performance. It also contains all the information of the fingerprints of the employees which will be used to monitor them.


![image alt](https://github.com/Adiasok/Attendance-monitoring-system---Design-/blob/fb9c81271bf0663167ea8faac8529da7dc7ce840/Screenshot%20(32).png)


This block interfaces an EM-18 RFID reader module with the MCU. RFID is used when using a fingerprint is not possible at the moment, instead the ID card has a RFID reciever that will authorize them to enter, this is done by the RFID reader block in our design. A level shifter (SN74LVT34) is used to ensure voltage compatibility between the 5 V RFID module and the 3.3 V ESP32-S3 UART input. Decoupling capacitors and a dedicated connector make the design modular and robust. 

![image alt](https://github.com/Adiasok/Attendance-monitoring-system---Design-/blob/fb9c81271bf0663167ea8faac8529da7dc7ce840/Screenshot%20(33).png)


This schematic contains the core microcontroller section built around the ESP32-S3-WROOM-2 module. This is the brain of the whole system, it controls all the functions properly so that it will work accordingly. It includes boot configuration circuitry, reset and enable switches, external crystal support, USB interface, and pin assignments for all peripherals such as RFID, fingerprint sensor, display, RTC, audio, and storage. Careful pin labeling improves firmware–hardware traceability. 


![image alt](https://github.com/Adiasok/Attendance-monitoring-system---Design-/blob/fb9c81271bf0663167ea8faac8529da7dc7ce840/Screenshot%20(34).png)


This schematic implements the power regulation and filtering stage of the system. It contains the supply for both 5V and 3.3V so that it will satisfy the needs of every component in our design. Input power is filtered and regulated using AMS1117 linear regulators to generate stable 5 V and 3.3 V rails. Bulk and ceramic capacitors are placed for ripple reduction and transient response, ensuring reliable operation of sensitive digital and analog components.


![image alt](https://github.com/Adiasok/Attendance-monitoring-system---Design-/blob/fb9c81271bf0663167ea8faac8529da7dc7ce840/Screenshot%20(38).png)



This is the PCB layout design of our Schematic design, it consists of a compact, robust design. The PCB for the Smart Attendance Monitoring System was designed with a strong emphasis on signal integrity, power stability, and modular peripheral integration. A multi-layer approach was used to ensure clean routing of high-speed digital signals while maintaining a low-noise power distribution network.

The ESP32-S3-WROOM-2 is placed centrally to minimize trace lengths to critical peripherals such as the fingerprint sensor, RFID module, display, RTC, audio amplifier, and MicroSD card. High-speed interfaces including SPI, I²C, UART, USB, and I²S were routed with controlled impedance considerations and minimal stubs.

Dedicated power planes and solid ground referencing were used to reduce EMI and improve overall system reliability. Decoupling capacitors are placed close to each IC’s power pins, and analog and digital sections are logically separated to prevent noise coupling, particularly in the audio and RTC circuits.




