**Idea List for Defcon 29 w/ TAMU**

Roving ESP32/8266's with a static webpage (SPIFFS) and an image + description of the person carrying it
	- Goal is to ID the aggie carrying the device to win a sticker
	- Fun scavenger hunt, 1-4 people/devices
	- "You may be looking at an Aggie if they are wearing maroon, have a TAMU class ring, or by some other means"
	
Technical Information:
- ESP's are running a static webpage in Access Point (or is it STA_tion mode??)
- Captive portal + DNS redirection so that when you connect you get a page

Challenge Information:
- ESP could also have 'baby's first RE challenge on the SPIFFS (think it's like 32 MB max??)
- so serve an ELF or other file
- SSTV wav?
- "You know an Aggie by their Ring, by their (honor code), or by someone who wears Maroon and is always quick to say 'Howdy!' "

- Could also have the beacon spammer running but that one is less fun than something in the spirit of DefCon



**Setup**
- `sudo ln -s /usr/bin/python3 /usr/bin/python` since the esp32 libraries want python in the path
- `sudo apt install python3-pip` to install pip3
- `pip install pyserial` since esp32 libs use pyserial ('import serial')

-----



 - following https://iotespresso.com/esp32-captive-portal-fetching-html-from-spiffs/
