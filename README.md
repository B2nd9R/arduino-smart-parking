
# Smart Parking System

![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green.svg)

## Overview

The Smart Parking System is an Arduino-based solution designed to automate and manage parking spaces efficiently. This system monitors parking spot availability, controls entry/exit gates, and provides real-time status updates through an LCD display.

## Features

- **Real-time Monitoring**: Tracks the status of 3 parking spots using IR sensors  
- **Automated Gates**: Controls entry and exit gates using servo motors  
- **User Interface**: Displays available spots and system status on an LCD screen  
- **Alert System**: Provides audio notifications through a buzzer when the parking is full or a spot becomes available  
- **Efficient Design**: Optimized code with minimal processing overhead  

## Hardware Requirements

- Arduino board (Uno/Mega/Nano)  
- 5 IR sensors (3 for parking spots, 2 for entry/exit gates)  
- 2 Servo motors (for entry and exit gates)  
- I2C LCD display (16x2)  
- Buzzer  
- Jumper wires  
- Power supply  

## Circuit Diagram

```
+------------------------+
|                        |
|        Arduino         |
|                        |
| IR Spot 1  -> D2       |
| IR Spot 2  -> D3       |
| IR Spot 3  -> D4       |
| IR Entry   -> D5       |
| IR Exit    -> D6       |
| LCD (I2C: SDA/SCL)     |
| Buzzer      -> D7      |
| Entry Servo -> D8      |
| Exit Servo  -> D9      |
+------------------------+
```

## Installation

1. Connect all components according to the circuit diagram  
2. Install the required libraries:  
   - `LiquidCrystal_I2C`  
   - `Servo`  
3. Upload the `parking_system.ino` sketch to your Arduino board  

## Libraries Used

- `Wire.h` – For I2C communication  
- `Servo.h` – For controlling servo motors  
- `LiquidCrystal_I2C.h` – For interfacing with the LCD display  

## Configuration

You can customize the system by modifying these constants in the code:

```cpp
#define TOTAL_SPOTS 3         // Total number of parking spots
#define GATE_OPEN_ANGLE 90    // Servo angle for open gate
#define GATE_CLOSED_ANGLE 0   // Servo angle for closed gate
#define GATE_OPEN_TIME 3000   // Gate open duration in milliseconds
#define REFRESH_RATE 250      // System refresh rate
```

## Usage

Once powered on, the system will:

1. Display a welcome message  
2. Initialize all components  
3. Close all gates  
4. Begin monitoring parking spots  

The system will automatically:

- Open the entry gate when a car arrives (if spots are available)  
- Open the exit gate when a car wants to leave  
- Close gates after a set duration  
- Update the display with current parking status  
- Sound the buzzer when the parking becomes full or a spot becomes available  

## How It Works

The system uses IR sensors to detect the presence of vehicles. When a vehicle approaches the entry gate and there are available spots, the entry gate opens automatically. Similarly, when a vehicle approaches the exit gate, it opens to allow the vehicle to leave.

The LCD display shows real-time information about available parking spots. When all spots are occupied, the system activates the buzzer to indicate that the parking is full.

## Future Enhancements

- Add RFID/NFC for authorized access  
- Implement data logging capabilities  
- Develop a mobile app interface  
- Add time-based parking fee calculation  
- Connect to cloud for remote monitoring  

## Troubleshooting

- Gates not responding : Check servo connections and power supply  
- Sensors not detecting vehicles : Adjust IR sensor sensitivity or positioning  
- LCD not displaying : Verify I2C address and connections  
- System restarting : Ensure adequate power supply capacity  

## Author

Developed by: Bandar Aljameely
