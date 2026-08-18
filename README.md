# AquaGuardian — Smart Water Drinking Reminder System

**AquaGuardian** is an ARM7-based smart water drinking reminder system developed using the **LPC2148/LPC2138 microcontroller** and Embedded C. The system helps users maintain their daily water intake by setting a target number of glasses and configurable reminder intervals. It uses an RTC for time-based operation, a 16x2 LCD for real-time information display, a 4x4 matrix keypad for user interaction, and external interrupts for recording water consumption.
The project was developed using **Keil µVision**, simulated in **Proteus**, and tested on physical **LPC2148 hardware**. The generated HEX file is programmed into the microcontroller using **Flash Magic**.

## Features
* **Real-Time Clock:** Maintains and displays the current time and date using the DS1307 RTC.
* **Water Intake Goal:** Allows the user to configure a daily water consumption target.
* **Reminder Interval:** Allows the user to configure the interval between water-drinking reminders.
* **Water Consumption Tracking:** Records the number of glasses consumed using external interrupt functionality.
* **Hydration Progress:** Displays the consumed quantity, target goal, and water intake percentage/progress on the 16x2 LCD.
* **Automatic Reminder:** Generates periodic reminders using the buzzer and LEDs when the configured reminder interval is reached.
* **Daily Goal Monitoring:** Continues tracking water consumption until the configured daily goal is achieved.
* **24-Hour Reset:** Automatically resets the daily water intake tracking after completion of the 24-hour cycle.
* **Password-Protected Configuration:** Protects RTC configuration and user settings using a 4-digit password.
* **Menu-Based Interface:** Provides menu navigation and configuration through the 4x4 matrix keypad and menu button.
* **Visual Status Indication:** Uses green and red LEDs to indicate hydration/status conditions.
* **Audio Indication:** Uses a buzzer for reminder, error, and user-interaction feedback.
* **External Interrupt Handling:** Uses EINT0/EINT1 for event-driven user input and water consumption logging.

## System Security
* **Default RTC Configuration Password:** `1111`
* The password-protected menu restricts access to RTC configuration and protected user settings.
* The system provides password verification before allowing protected parameters to be modified.

## Hardware Requirements
* **Microcontroller:** NXP LPC2148/LPC2138 ARM7 microcontroller
* **Development Language:** Embedded C
* **IDE:** Keil µVision
* **Programming Tool:** Flash Magic
* **Simulation Tool:** Proteus
* **Display:** 16x2 Character LCD (LM016L)
* **RTC:** DS1307 RTC
* **Input:** 4x4 Matrix Keypad
* **Menu Input:** External Push Button
* **Status Indicators:** Yellow LED, Green LED, Red LED
* **Audio Output:** Buzzer
* **External Interrupts:** EINT0/EINT1

## Hardware Connections

### LPC2148/LPC2138
* **P0.1:** Menu Button
* **P0.5:** Yellow LED (D1)
* **P0.6:** Buzzer
* **P0.22:** Green LED (D2)
* **P0.23:** Red LED (D3)

### 16x2 LCD
* **RS:** P0.19
* **RW:** P0.20
* **EN:** P0.21
* **D0-D7:** P0.8-P0.15
* **VSS:** GND
* **VDD:** Supply line
* **VEE:** Contrast control through RV1 (1k)

### 4x4 Matrix Keypad
* **Rows R1-R4:** P1.16-P1.19
* **Columns C1-C4:** P1.20-P1.23
The keypad is used for password entry, menu navigation, RTC configuration, water goal setting, reminder interval configuration, and other user inputs.

### RTC
* **RTC:** DS1307
* The RTC is used to maintain time and date information required for reminder scheduling and daily water tracking.

## Project Operation
1. The system initializes the LPC2148/LPC2138, LCD, RTC, keypad, GPIO, and interrupt modules.
2. The AquaGuardian initialization screen is displayed on the LCD.
3. The current RTC time and date are displayed during normal operation.
4. The user can access the protected configuration menu using the menu interface.
5. The default password `1111` is required to access protected RTC settings.
6. The user can configure the RTC time and date.
7. The user sets the required daily water intake goal using the keypad.
8. The user configures the desired reminder interval.
9. The system monitors the configured reminder interval using the RTC.
10. When the reminder interval is reached, the buzzer and LEDs provide a water-drinking reminder.
11. When the user drinks water, the external interrupt is used to update the water consumption count.
12. The LCD displays the current water intake, target goal, remaining glasses, and hydration percentage/progress.
13. The green LED indicates that the configured daily water goal has been achieved.
14. The red LED provides a warning/status indication when the configured reminder conditions are not met.
15. After completion of the 24-hour tracking period, the daily water intake count and related status are reset for the next cycle.

## LCD Status Display
The 16x2 LCD provides information such as:
* Current RTC time and date
* Current water consumption
* Daily target goal
* Remaining glasses
* Hydration percentage/progress
* Configuration menus
* Reminder messages
* Password and system status messages

## Repository / Project Structure
```text
├── main.c
├── water.c / water.h
├── password.c / password.h
├── rtc.c / rtc.h
├── lcd.c / lcd.h
├── KPM.c / KPM.h
├── interrupt.c / interrupt.h
├── led_buzzer.c / led_buzzer.h
└── delay.c / delay.h
```

### Module Description
* **main.c:** System entry point and main application control.
* **water.c / water.h:** Water goal setting, water consumption tracking, reminder management, and daily reset functionality.
* **password.c / password.h:** Password verification and protected configuration access.
* **rtc.c / rtc.h:** RTC initialization, time/date configuration, and RTC data handling.
* **lcd.c / lcd.h:** 16x2 LCD initialization and display functions.
* **KPM.c / KPM.h:** 4x4 matrix keypad scanning and key detection.
* **interrupt.c / interrupt.h:** External interrupt configuration and interrupt service routines.
* **led_buzzer.c / led_buzzer.h:** LED and buzzer control functions.
* **delay.c / delay.h:** Software delay functions used by the embedded application.

## Development and Programming Procedure
* The Embedded C source code is developed and compiled using **Keil µVision**.
* After successful compilation, a HEX file is generated.
* The HEX file is programmed into the LPC2148/LPC2138 hardware using **Flash Magic**.
* The circuit and system operation are verified using **Proteus** simulation.
* The final system is tested on physical LPC2148 hardware.

## Testing
The AquaGuardian system was successfully tested in both **Proteus simulation** and on **physical LPC2148 hardware**.

The following modules were verified:
* Microcontroller initialization
* GPIO configuration
* LCD interfacing
* 4x4 keypad interfacing
* RTC operation
* Password-protected configuration
* RTC time and date configuration
* Water goal setting
* Reminder interval configuration
* Water consumption tracking
* External interrupt operation
* Hydration percentage/progress display
* Buzzer operation
* LED status indication
* 24-hour daily reset
* Complete system integration

## Author
**Aditya Pachkor**

## Project Summary
AquaGuardian integrates an LPC2148/LPC2138 microcontroller, DS1307 RTC, 16x2 LCD, 4x4 matrix keypad, external interrupts, LEDs, and buzzer to create a smart water drinking reminder system. The system provides configurable daily water goals and reminder intervals, tracks water consumption, displays hydration progress, and provides visual and audible reminders. Password protection is implemented for protected RTC and system configuration.

The project demonstrates practical implementation of **Embedded C, ARM7 microcontroller programming, GPIO, LCD interfacing, keypad interfacing, RTC interfacing, external interrupts, timers/time-based control, and peripheral integration**.
