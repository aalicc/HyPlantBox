## HyPlantBox!
HyPlantBox! project represents a user-friendly IoT-based hydroponic system controlled by an automation box. This system features a web-based user interface, enabling users to both monitor and adjust vital system parameters with ease. Originating from the students' Innovation Project (see [innoproject](https://github.com/aalicc/innoproject) repository) , this project received financial support of UrbanFarmLab and Metropolia UAS, underscoring its potential for practical greenhouse applications.
<br />
## Components
* CONTROLLINO MAXI
* Raspberry Pi 4B
* Power supply: DRS-240-12
* Water level meter: HC-SR04
* Waterproof temperature sensor: DS18B20
* Humidity & temperature sensor: RuuviTag
* pH sensor: DFRobot Industrial pH Sensor v2
* TDS sensor: Grove – TDS Sensor
* Dosing pump: EZO-PMP-BX
* Main pump: BILTEMA art. 25-999
* Fan: Attwood turbo 4000
* 2-channel motor driver: MDD3A
* Joy-IT Raspberry Pi case with coolers
* Protected cables of different thickness
* Automation box
* 2 DIN rails
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
The user can easily access connections and do neccessary changes from the automation box. Figure 1 below shows an example of the box, demonstrating its straightforward design for users to make any modifications. 
<br />
<br />
<img src="https://github.com/aalicc/HyPlantBox/assets/105237164/27f5cd0e-d3e8-40ba-ba3e-f67ace51745e" alt="Picture of the box" height="425px" width="342px">
<br />
<br />
The web interface runs on the Raspberry Pi which is serially connected to the CONTROLLINO MAXI. It gives the end user the ability to observe the current state of the system, and it gives the user warnings when the amount of some solution depletes beyond a threshold of 40%. Also, the user can change how the system works by interacting with the included control panel, from where they can adjust the fan speed, the main water pump cycle, and the thresholds which trigger the solution pumps and the fans.
<br />
<br />
![GIF](https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExYmh6MmplZ3J1MjJrZXE5NDB6cWd0bTN0eGJ0MnhrN3A5a3I2aGFtdyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/Gzvg4E2RT3h02zhKSJ/giphy-downsized-large.gif)
<br />
Gif 2. Web Interface demo
<br />


### Hardware
The system assembly requires some knowledge of serial communication.
<br />
<br />
![HyPlantBox_block_diagram](https://github.com/aalicc/HyPlantBox/assets/105237164/98f8f401-8a26-4b9f-9e56-a6b9018e6e29)
<br />
Figure 1. HyPlantBox! block diagram
<br />
<br />
As shown on the diagram, all the sensors are located on the left and the mechanical equipment is on the right. In the center of the picture are the elements of the control panel. For a comprehensive pinout diagram specific to the CONTROLLINO MAXI, please refer to the Controllino website.<br />
<br />

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
6. Check if other libraries specified in the code are installed on your computer
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
.
## Contact info
andreileb23@gmail.com
<br />
alissa.subina@gmail.com
## Links
