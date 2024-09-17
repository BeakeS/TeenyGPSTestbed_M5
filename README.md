# TeenyGPSTestbed_M5

TeenyGPSTestbed_M5 is an application for configuring and evaluating Ublox GPS modules.  This application runs on M5Stack Unified IoT Development Kit.
The application was developed for Ublox M8 and M10 GPS modules connected to the Core2's Serial2 interface.  The application also has a Ublox GPS emulation mode which can be used for testing navigation applications when no GPS signals can be acquired or when a fixed, repeatable GPS source is desired.

Menu Navigation:
TeenyGPSTestbed_M5 utilizes the TeenyMenu library for menu/page navigation via a three button interface.
The TeenyMenu library was derived from the GEM (Good Enough Menu) Arduino library in 2021.

Navigating the TeenyGPSTestbed_M5 menu pages on the Core2 utilizes the Core2's three touch buttons adjacent to the display as follows:
- The top button single touch is SCROLL_UP.
- The bottom button single touch is SCROLL_DOWN.
- The middle button single touch is SELECT.
  - If the menu item you selected is a MENU LINK item (with '>' on the right), it switches to the linked menu page.
    - Middle button long touch returns you to the previous menu page.
  - If the menu item you selected is a PARAMETER ITEM (name:value) the item switches to edit mode (with '+/-' on the right).
    - Use scroll up and down to change the value.
    - Middle button single touch will save the selected value and exit edit mode.
    - Middle button long touch will exit edit mode without saving.
  - If the menu item you selected is a BOOLEAN ITEM, select will toggle the item's value.
  - If the menu item you selected is an ACTION ITEM (with '>>' on the left side), the action (function call) is executed.
  - Some menus also have an optional EXIT ITEM ('<exit') which returns to the previous menu page just like middle button long touch does.

TeenyGPSTestbed_M5 offers five modes of operation:

GPSRCV: Basic GPS Receiver - Displays PVT packet info including date/time, lat/lon/alt, heading, and positional accuracy data.
![rcvr](https://github.com/user-attachments/assets/44933f0f-ee60-4185-a83a-6f1d62cc16ba)

GPSLOG: UBLOX Packet Logger - For logging Ublox NAV-PVT, NAV-STATUS, and NAV-SAT raw packets for analysis and/or emulation. Logger can also output GPX or KML files for mapping or animation.
![logger](https://github.com/user-attachments/assets/76e60714-89a7-4215-b9e3-44fe045ecd9c)

NAVSAT: UBX-NAV-SAT Receiver - For testing satellite reception.  Displays data for tracked satellites sorted by signal strength.
![navsat](https://github.com/user-attachments/assets/81b5195e-e7c8-4fdd-8519-e363819dadc6)

Satellite Constellation Map (via UBX-NAV-SAT Receiver) - View of satellite position data including individual satellite status - usedForNav (sat color blue=true,red=false) and signal strength (sat ring white>=35db,yellow>=20db,orange<20db).
![satmap](https://github.com/user-attachments/assets/4119fc8f-0d75-4e7a-a513-4fce083ca1e6)

SATCFG: Satellite Configuration Tool - View GNSS system configuration and enable/disable selected GNSS.
![gnssconfig](https://github.com/user-attachments/assets/b6c171d3-360c-4afe-a90b-d0e92ae4508c)

EMUM8 & EMUM10: GPS Emulation - EMUM8 emulates Beitian BN-880Q (Ublox M8) GPS module and EMUM10 emulates BE-880Q (Ublox M10) GPS module including replay of Ublox NAV-PVT, NAV-STATUS, and NAV-SAT packets from ROM or SDCARD with progressive date/time updates.
![emulate](https://github.com/user-attachments/assets/886de429-d785-4dba-84fc-5d3719fe9072)
