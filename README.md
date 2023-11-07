## HyPlantBox!
HyPlantBox! project represents a user-friendly IoT-based hydroponic system controlled by an automation box. This system features a web-based user interface, enabling users to both monitor and adjust vital system parameters with ease. Originating from the students' Innovation Project (see [innoproject](https://github.com/aalicc/innoproject) repository) , this project received financial support of UrbanFarmLab and Metropolia UAS, underscoring its potential for practical greenhouse applications.
<br />
## Components
* CONTROLLINO MAXI
* Raspberry Pi 4B
* Power supply: DRS-240-12
* 5 x Water level meter: HC-SR04
* Waterproof temperature sensor: DS18B20
* 2 x Humidity & temperature sensor: RuuviTag
* pH sensor: DFRobot Industrial pH Sensor v2
* TDS sensor: Grove – TDS Sensor
* 4 x Dosing pump: EZO-PMP-BX
* Main pump: BILTEMA art. 25-999
* 2 x Fan: Attwood turbo 4000
* 2-channel motor driver: MDD3A
* Joy-IT Raspberry Pi case with coolers
* Protected cables of different thickness
* Automation box
* 2 x DIN rail
* Connectors
* 3D printed holders
## Web interface
### Built with
![Bootstrap](https://img.shields.io/badge/bootstrap-%238511FA.svg?style=for-the-badge&logo=bootstrap&logoColor=white)
<br />
![jQuery](https://img.shields.io/badge/jquery-%230769AD.svg?style=for-the-badge&logo=jquery&logoColor=white)
<br />
![NodeJS](https://img.shields.io/badge/node.js-6DA55F?style=for-the-badge&logo=node.js&logoColor=white)
<br />
![Express.js](https://img.shields.io/badge/express.js-%23404d59.svg?style=for-the-badge&logo=express&logoColor=%2361DAFB)
<br />
![SQLite](https://img.shields.io/badge/sqlite-%2307405e.svg?style=for-the-badge&logo=sqlite&logoColor=white)
## Getting Started
The system includes typical sensors used for water and air quality monitoring i.e, pH, EC, temperature and humidity meters. These in turn are responsible for triggering the activation of mechanical devices, such as pumps and fans. The significance of pumps lies not only in delivering water to the growing channels but also in dispensing the necessary nutrients automatically.
<br />
<br />
![Pumps working](https://media.giphy.com/media/20IqPq7Z8fLSujgLiN/giphy.gif)
<br />
Gif 1. Dosing pumps working
<br />
<br />
As was already mentioned the system has certain dependencies. Due to waterproof temperature sensor, temperature compensation is applied to the DFRobot pH and Grove TDS meters, enhancing the accuracy of their readings. These readings, in turn, regulate the activation of the dosing pumps. The specified threshold for pH and TDS triggers the pumps to either decrease or increase respective parameters in the main water tank by pumping up the appropriate solution (5 ml at a time by default). 
In addition to its ability to monitor and manage water parameters, the system is also equipped with fans to respond to changes in air humidity and temperature. The motor driver is a critical component in this regard, as it enables precise control of the ventilation speed.  By adjusting the connections, the first fan's rotation direction can be reversed, and the same approach can be used for the second fan, providing flexible control over both fan units.
<br />
The user can easily access connections and do neccessary changes from the automation box. Figure 1 below shows an example of the box, demonstrating its straightforward design for users to make any modifications. 
<br />
<br />
<img src="https://github.com/aalicc/HyPlantBox/assets/56063288/0b0a01fc-e512-4a89-8ac6-4af4699bf235" alt="Picture of the box" height="425px" width="342px">
<br />
Figure 1. Automation box controlled by CONTROLLINO MAXI
<br />
<br />
The web interface runs on the Raspberry Pi which is serially connected to the CONTROLLINO MAXI and the touchscreen on top of the control box. It gives the end user the ability to observe the current state of the system, and it gives the user warnings when the amount of some solution depletes beyond a threshold of 40%. Also, the user can change how the system works by interacting with the included control panel, from where they can adjust the fan speed, the main water pump cycle, and the thresholds which trigger the solution pumps and the fans.
<br />
<br />
![GIF](https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExYmh6MmplZ3J1MjJrZXE5NDB6cWd0bTN0eGJ0MnhrN3A5a3I2aGFtdyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/Gzvg4E2RT3h02zhKSJ/giphy-downsized-large.gif)
<br />
Gif 2. Web Interface demo
<br />
### Hardware
The system features a variety of sensors and components dedicated to measuring water and air properties, with their parameters being dynamically regulated by mechanical components. The block diagram of the system is demonstrated in Figure 2 below.
<br />
<br />
![HyPlantBox_block_diagram](https://github.com/aalicc/HyPlantBox/assets/105237164/fc39680a-d0df-4925-a436-07f476e72fa8)
<br />
Figure 2. HyPlantBox! block diagram
<br />
<br />
CONTROLLINO MAXI, as the core element, is located in the center of the diagram, interconnecting with the components from both sides. As shown on the diagram all the sensors are located on the left side, while the mechanical equipment is positioned on the right side. 
<br />
<br />
Although the most of the system connections are straightforward, it's important to pay special attention to certain aspects.
<br />
<br />
For example, the waterproof temperature sensor DS18B20 is connected utilizing One Wire technology. This means that the sensor is powered only with data and ground line. To perform this type of connection a pull-up resistor of 4.7 kOhm must be integrated between VCC and D0 pins and then as a single data line connected to CONTROLLINO. 
<br />
<br />
The EZO-PMP dosing pumps functions with either UART or I2C mode. It is important to note that the pin names on the original product correspond to I2C configuration. The SCL pin is equivalent to RX, and SDA to TX in case of UART. Consequently, when operating in UART mode, the TX of the pumps is connected to the RX of the PLC, and the RX is connected to the TX of the PLC.
<br />
<br />
Another noteworthy component is MDD3A, serving as the motor driver within the system. This component enables precise control over fan speed by utilizing PWM signals. The M1A, M2A and M1B, M2B pins determine rotational direction of the motors, where each number corresponds to a specific fan unit, and the letter indicating the direction. By changing the connection from A to B the fans start rotate in opposite direction. 
<br />
<br />
The Raspberry Pi is linked to the CONTROLLINO via its hardware serial port, with the TX of CONTROLLINO connected to the RX of the Raspberry Pi and vice versa. The interface is displayed on a touchscreen, connected simply through microHDMI. Furthermore, data from RuuviTags, which includes air humidity and temperature measurements, is transmitted using Bluetooth Low Energy (LE).

### Software
#### Controllino
1. Install Arduino IDE on your computer
2. Run Arduino IDE
3. Install CONTROLLINO library
    - Go to Sketch -> Include Library -> Manage Libraries
    - Type CONTROLLINO in search box
    - Press Install
    - Navigate to File -> Preferences
    - Insert the link into Additional boards manager URLs and press OK:
      https://raw.githubusercontent.com/CONTROLLINO-PLC/CONTROLLINO_Library/master/Boards/package_ControllinoHardware_index.json
    - Go to Tools -> Board:  -> Boards Manager
    - Search for CONTROLLINO
    - Install CONTROLLINO boards
4. Open state_machine.ino
5. Download and install ‘DFRobot_PH.h’ library from: https://github.com/DFRobot/DFRobot_PH
6. Check if other libraries specified in the code are installed on your computer (usually they're istalled together with Arduino software)
7. Run state_machine.ino


#### Raspberry Pi
1.	Download to your local repository
2.	Open the startup.sh script with a text editor
3.	Change the paths, so that they point to the repository
4.	Save the changes
5.	In your local repository, run “npm install” to install dependencies
6.	In “server.js” file, change the Serial port to the desired one and change the baud rate
7.	(Optional) Install SQLite3
8.	Run the startup.sh script

## Possible improvements
Although the project in its current state meets its initial goals, there are opportunities to enhance its performance and reliability. This chapter addresses the following potential challenges that users may encounter:
<br />
* Immediate activation of pumps and fans upon system startup.
      </br>Upon plugging in the system, the pumps and fans start operating immediately,
      contrary to the expected behavior where they should remain off until the user
      configures the settings. Resolution of the problem may involve triggering the “Maintenance Mode” within
      the code right after the application starts.
* Inconsistent operation of Python script for Ruuvitags data retrieval.
     </br>The Python script designed to retrieve the data from Ruuvitags exhibits occasional
    failures, which did not occur during standalone testing. The error indicates a
    connection timeout, possibly due to interference with the Bluetooth connection
    from other electronics near the Raspberry Pi within enclosure.
    Resolution of the issue would involve investigation and mitigation of the source of
    interference. Another option would be complete replacement of Ruuvitags for a
    different sensor with a different communication type.
* Occasional website crashes due to database Locking.
     </br>Periodic crashes of the website resulting from database congestion caused by
    multiple concurrent script accesses.Resolution of this problem would be to reboot the Raspberry Pi, or manual restart
    of the web application, but this is not recommended.
* Exposure to water may disrupt the accuracy of water level measurements.
     </br>If the water exposure is unavoidable, it is recommended to consider replacing the
    HC-SR04 with the waterproof version.

## Contact info
andreileb23@gmail.com
<br />
alissa.subina@gmail.com
## Links
**GitHub** :octocat:<br/>
Andrei: https://github.com/IoTAndrew<br/>
Alissa: https://github.com/aalicc<br/>
innoproject: https://github.com/aalicc/innoproject<br/>
<br/>**CONTROLLINO MAXI** :green_apple:<br/>
Official webpage: https://www.controllino.com/product/controllino-maxi/<br/>
**Raspberry Pi 4B** :grapes:<br/>
Official webpage: https://www.raspberrypi.com/products/raspberry-pi-4-model-b/<br/>
<br/>**Sensors’ datasheets & instructions** :eyeglasses:<br />
HC-SR04: https://www.amazon.co.uk/HC-SR04-Ultrasonic-Distance-Arduino-MEGA2560/dp/B07XF4815H<br/>
DS18B20: https://www.analog.com/media/en/technical-documentation/data-sheets/DS18B20.pdf<br/>
DFRobot Industrial pH Sensor v2: https://www.dfrobot.com/product-2069.html<br/>
Grove - TDS Sensor: https://wiki.seeedstudio.com/Grove-TDS-Sensor/<br/>
RuuviTag: https://ruuvi.com/ruuvitag/<br/>
<br/>**Motor driven devices & accessories** :zap:<br/>
EZO-PMP-BX: https://atlas-scientific.com/peristaltic/sgl-pmp-bx/<br/>
MDD3A: https://www.cytron.io/p-3amp-4v-16v-dc-motor-driver-2-channels<br/>
Attwood turbo 4000: <br/>
BILTEMA art. 25-999: <br/>
<br/>**Power supply** :electric_plug:<br/>
DRS-240-12: https://www.meanwell.com/webapp/product/search.aspx?prod=DRS-240
