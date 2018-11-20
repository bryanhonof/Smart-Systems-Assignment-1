# SimpleLink Wi-Fi CC3200 LaunchPad

Assignment 1 of the Smart Systems course lectured by [Vincent Claes](https://www.linkedin.com/in/vincentclaes/) @ [PXL University College](https://www.pxl.be).
The goal of this assignment is to write firmware for the [SimpleLink Wi-Fi CC3200 LaunchPad](http://www.ti.com/tool/CC3200-LAUNCHXL) that collects
and uploads the data it gathers from its temperature sensor. 
Using the collected data the goal is to make a prediction of temperatures. 
This prediction is made in Jupyter Notebooks using the Python scripting language.
A flask server backend is provided on an heroku server together with an heroku PostgreSQL.

<p align="center"><img src="../misc/cc3200.png"></p>


## Features

* CC3200 Wi-Fi wireless microcontroller (MCU) in QFN package
* Industry’s first devices to be Wi-Fi CERTIFIED™ at the chip level by the Wi-Fi Alliance™
* USB interface to PC for CCS/IAR using FTDI USB drivers
* Flash update over the USB using SimpleLink Programmer
* 2 20-pin connectors enables compatibility with BoosterPacks with added functions (BoosterPack headers)
* Standalone development platform featuring sensors, LEDs and push-buttons
* Power from USB for the LaunchPad as well as external BoosterPack
* Operates from 2 AA alkaline batteries
* On-board antenna and U.FL connector selectable using a capacitor re-work 
* Supports 4 wire JTAG and 2 Wire SWD
* GNU Debugger (GDB) support over Open On chip debugger (OpenOCD)

## Parametrics

|                                 | CC3200                                                                 |
|---------------------------------|------------------------------------------------------------------------|
| Processor                       | ARM Cortex-M4                                                          |
| Technology                      | Wi-Fi                                                                  |
| Flash (KB)                      |                                                                        |
| RAM (KB)                        | 256                                                                    |
| Throughput (Max) (Mbps)         | 16                                                                     |
| Wi-Fi RX sensitivity (dBm)      | -96                                                                    |
| Wi-Fi TX current (mA)           | 229                                                                    |
| Security Enabler                | Cryptographic acceleration Networking security Secure FW and SW update |
| Operating temperature range (C) | -40 to 85                                                              |
| Package (mm)                    | 9 9 QFN, 0.5 Pitch, 64-pin                                             |





## Built With

* [Python](https://www.python.org/) - The programming language used
* [Flask](http://flask.pocoo.org/) - The web framework used
> See server/requirements.txt for a full list of used python modules to run the server.
* [Heroku](https://www.heroku.com/home) - The deployment server
* [SimpleLink Wi-Fi CC3200 LaunchPad](http://www.ti.com/tool/CC3200-LAUNCHXL) - The development board used

## Authors

* *Vincent Claes*    - Lecturer - [LinkedIn](https://www.linkedin.com/in/vincentclaes/)
* *Jeffrey Gorissen* - Student  - [LinkedIn](https://www.linkedin.com/in/jeffrey-gorissen-6120a2142/)
* *Bryan Honof*      - Student  - [LinkedIn](https://www.linkedin.com/in/bryan-honof/)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## Acknowledgments

* Hat tip to anyone whose code was used
* Inspiration

