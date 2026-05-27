# LPOD
Landfill POD firmware both under Arduino &amp; Particle firmware as we continue doing testing.

# Using this Firmware
This firmware has three use-cases. Arudino MKR, Particle Electron, and Particle Boron 404x. I can only vouch for the Boron 404x's functionality. If the file folder is titled with MKR, you must run the firmware from the Arduino IDE. If the file folder is titled with Particle, it must be run from Particle Workbench on VSCode. If you need help setting up either, please don't hesitate to reach out to Percy or Sascha.

# Update Tracker
Thanks to Izzy for this suggestion! Here we will be tracking each version of the firmware. 
| Microcontroller         | Version       | "Named" Ver.       | Pilot         | Date               | Description & Purpose                  |
| ----------------------- | ------------- | ------------------ | ------------- | -------------      | -------------------------------------- |
| Particle Boron404x      | V6            | Migrate to Boron   | Percy         | August 2024        | Moves to Boron & optimize cell         |
|                         | V7            | Repair B4 Meas.    | Percy         | October 2024       | Changes measurement of AS-B4 sensors   |
|                         | V8            | Adds Neg Meas.     | Percy         | July 2025          | Negative meas on AS-B4 being deranged  |
|                         | V9            | Change B4 Naming   | Percy         | May 2026           | Corrects Aux/Worker naming to hw comp  |

## Legacy Versions (Out-of-Use Microcontrollers)
| Microcontroller         | Version       | "Named" Ver.       | Pilot         | Date               | Description & Purpose                  |
| ----------------------- | ------------- | ------------------ | ------------- | -------------      | -------------------------------------- |
| Arduino MKR 1500        | V1.0.0        | Initial Firmware   | Percy         | July 2024          | Very first LPOD firmware for testing   |
|                         | V1.1.0        | MKR updated        | Percy         | July 2024          | Minor bug fixes & development from 1   |
| Particle Electron       | POC           | Proof-of-concept   | Percy         | July 2024          | Testing LPOD off Particle              |
|                         | V2            | Refining usability | Percy         | August 2024        | Refining UI & running new sensors      |
|                         | V3            | Continuing edits   | Percy         | August 2024        | Updates and continued edits            |
|                         | V4            | Integrate cellular | Percy         | August 2024        | Logs to SD, Cell, and all sensors      |
|                         | V5            | Prep for Boron     | Percy         | August 2024        | Preparing to move to Boron 404x        |
