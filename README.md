# HyPlantBox!

HyPlantBox! project represents a user-friendly IoT-based hydroponic system controlled by an automation box. This system features a web-based user interface, enabling users to both monitor and adjust vital system parameters with ease. Originating from the students' Innovation Project (see "innoproject" repository) , this project received financial support of UrbanFarmLab and Metropolia UAS, underscoring its potential for practical greenhouse applications.
<br />What's cool about it? Everything!
<br />In our system CONTROLLINO MAXI is the brain, and the end user gives it orders by using the web interface, that runs on a Raspberry Pi.
<br />
<br />
#Features
.
#Getting Started
The system includes typical sensors used for water and air quality monitoring i.e, pH, EC, temperature and humidity meters. These in turn are responsible for triggering the activation of mechanical devices, such as pumps and fans. The significance of pumps lies not only in delivering water to the growing channels but also in dispensing the necessary nutrients automatically.

![HyPlantBox_block_diagram](https://github.com/aalicc/HyPlantBox/assets/105237164/98f8f401-8a26-4b9f-9e56-a6b9018e6e29)
<br />
<br />
<img src="https://github.com/aalicc/HyPlantBox/assets/105237164/27f5cd0e-d3e8-40ba-ba3e-f67ace51745e" alt="Picture of the box" height="425px" width="342px">


The web interface runs on the Raspberry Pi which is serially connected to the CONTROLLINO MAXI. It gives the end user the ability to observe the current state of the system, and it gives the user warnings when the amount of some solution depletes beyond a threshold of 40%. Also, the user can change how the system works by interacting with the included control panel, from where they can adjust the fan speed, the main water pump cycle, and the thresholds which trigger the solution pumps and the fans.

![GIF](https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExYmh6MmplZ3J1MjJrZXE5NDB6cWd0bTN0eGJ0MnhrN3A5a3I2aGFtdyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/Gzvg4E2RT3h02zhKSJ/giphy-downsized-large.gif)
