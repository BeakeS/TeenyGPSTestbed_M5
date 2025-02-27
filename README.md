# TeenyGPSTestbed_M5 Documentation

Overview

TeenyGPSTestbed_M5 is an application designed for configuring and evaluating Ublox GPS modules. It runs on the M5Stack Unified IoT Development Kit and supports Ublox M8, M9, and M10 GPS modules connected to the M5Stack Core2/CoreS3 Serial interface. The application also includes a GPS emulation mode for testing navigation applications when GPS signals are unavailable or when a fixed, repeatable GPS source is required.

Menu Navigation

The TeenyGPSTestbed_M5 uses the TeenyMenu library for menu/page navigation, which is controlled via a three-button interface on the M5Stack Core2.

Buttons
 - Top Button: Scroll Up
 - Bottom Button: Scroll Down
 - Middle Button: Select (with additional functionalities based on the menu item type)

Menu Item Behavior
 - MENU LINK ITEM: Tapping the middle button switches to the linked menu page. A long press on the middle button returns to the previous page.
 - PARAMETER ITEM: Allows editing of values. Use the scroll buttons to adjust the value. Tap the middle button to save changes or long-press it to exit without saving.
 - BOOLEAN ITEM: Tapping the middle button toggles the value (e.g., ON/OFF).
 - ACTION ITEM: Executes a function call when selected.
 - EXIT ITEM: Returns to the previous menu page.

Modes of Operation

1. GPSRCV: Basic GPS Receiver
 - Displays PVT (Position, Velocity, Time) packet info, including:
     - Date/Time
     - Latitude/Longitude/Altitude
     - Heading
     - Positional Accuracy

![rcvr](https://github.com/user-attachments/assets/44933f0f-ee60-4185-a83a-6f1d62cc16ba)

2. GPSLOG: UBLOX Packet Logger
 - Logs Ublox NAV-PVT, NAV-STATUS, and NAV-SAT raw packets for further analysis and/or emulation.
 - Can also export data as GPX or KML files for mapping or animation.

![logger](https://github.com/user-attachments/assets/76e60714-89a7-4215-b9e3-44fe045ecd9c)

3. NAVSAT: UBX-NAV-SAT Receiver
 - Displays satellite reception data.
 - Shows data for tracked satellites sorted by signal strength.

![navsat](https://github.com/user-attachments/assets/81b5195e-e7c8-4fdd-8519-e363819dadc6)

4. Satellite Constellation Map
 - Visualizes satellite position data:
     - Satellite status (Used for Navigation: Blue=Yes, Red=No)
     - Signal strength (Color-coded rings: White ≥35dB, Yellow ≥20dB, Orange <20dB)

![satmap](https://github.com/user-attachments/assets/4119fc8f-0d75-4e7a-a513-4fce083ca1e6)

5. SATCAL: Satellite Calibration Tool
 - Scans each GNSS individually and records satellites, signal strength, and status

![gnsscalibration](https://github.com/user-attachments/assets/d76e7595-5d09-44f8-af4b-93cf7d234fab)

6. SATCFG: Satellite Configuration Tool
 - Displays GNSS system configuration and allows enabling/disabling selected GNSS systems.

![gnssconfig](https://github.com/user-attachments/assets/b6c171d3-360c-4afe-a90b-d0e92ae4508c)

7. EMUM8 & EMUM10: GPS Emulation
 - EMUM8: Emulates Beitian BN-880Q (Ublox M8) GPS module.
 - EMUM10: Emulates Beitian BE-880Q (Ublox M10) GPS module.
 - Both modules replay Ublox NAV-PVT, NAV-STATUS, and NAV-SAT packets from ROM or SD card with progressive date/time updates.

![emulate](https://github.com/user-attachments/assets/886de429-d785-4dba-84fc-5d3719fe9072)
