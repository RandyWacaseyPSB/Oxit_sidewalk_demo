# Oxit_sidewalk_demo
sidewalk demo hardware from oxit

RW 04012025 - updated platformio.ini to include serial terminal setup, this will allow the use of Serial Monitor feature in VScode however a physical reset of the board is first required after programming (button closest to the USB C port).  Otherwise you get a usersideexception error indicating that the port cannot open.

RW - Would like to add a feature that will update the UI to change the color of the LED to represent the protocol upon switch prior to going into not connected (red).  This will allow the user to verify that the button press was acknowledged

RW - Would like to add a scanning feature in which the device will scan for all 3 protocols back to back (simulating the manual button press).  After cycling through all 3 it would show a green blinking or red blinking status to indicate if there was any successful packet transfer.  Then start the process over again.  Need to determine how to enter this mode or if this would just be a FW image.