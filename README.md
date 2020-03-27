<img src="https://github.com/csteamengine/YoutubeCounter/blob/master/Images/IMG_3743.png" width="250">

# Modern YouTube Counter
There is nothing quite as exciting as having a new viewer subscribe to your YouTube channel. For content creators trying to get
monetized, every new subscriber gets you one step closer to the 1000 subs needed to reach your goal. I wanted a way to view a live
count of my subscribers so I can tell, at a glance, how close I am to being monetized, and so I can celebrate every time a new viewer 
subscribes. I have been so happy with the final result that I wanted to document how I did everything so that others can do it too!

If you are interested in building this, check out my YouTube channel for for the video, and as always, be sure to subscribe!

Video Link: https://youtu.be/5DM0E9tf7Ko

# Build
To do this, I built a giant seven segment display made out of sections of WS2811 LEDs with an ESP32 for the brains.
I designed and 3d printed an enclosure made up of a black section to hold all the LEDs as well as a white panel to act 
as a diffusor. The last part of the display, is a small detail piece to help separate each of the segments and improve the
appearance a bit.

## 3d Printed Parts
All 3d printed parts for this project can be downloaded here: https://www.thingiverse.com/thing:4216665

![3d Printed Parts](https://github.com/csteamengine/YoutubeCounter/blob/master/Images/Parts.png)

The brains of this project are powered by an ESP32 Module that connects to the YouTube API to pull channel statistics and decides which LEDs to turn on and which to turn off. For the lights, I used a strip of WS2811 LEDs, which can be cut into sections of 3 pixels. This ended up being perfect for the size I wanted the display to be. 

The sections of LEDs are wired together in the configuration shown in the image below. This way I could connect multiple digits together in series, without needing a controller for each digit. The LEDs run off 12v while the ESP32 can run off pretty much anything under 12v. It can handle 12v, but as that's the upper limit of its voltage regulator, I wanted to be sure I didn't burn it out, which has happened to me before. To do this, I added a 5v regulator I pulled out of a car USB charger and wired that to the vin on the ESP32. I designed a small YouTube logo box to store all the electronics for this project, and I added another section of LEDs to the box, so I could light up the play button, just to spice things up a bit. The last part I added, was a 12v plug so I could power everything from a common plug type. The output of that is sent to the LEDs, the onboard LED, and the 5v Voltage Regulator, which then powers the ESP32. 

![Wiring for LEDs](https://github.com/csteamengine/YoutubeCounter/blob/master/Images/IMG_9061.png)
![Schematic](https://github.com/csteamengine/YouTubeCounter/blob/master/Images/Schematic_YouTubeCounter_Sheet_1_20200311172836.png)
![Electronics](https://github.com/csteamengine/YouTubeCounter/blob/master/Images/IMG_3974.png)

The last thing to note on the electronics, is that I added some stand off female pins so that I could replace the ESP32 or remove
it in the event that either something went wrong, or if I wanted to change the code on it easily. 

# Code
The code for this projet is incredibly simple, but there are a few steps you'll need to take to get it working. Follow these steps to get the ESP32 working with your Arduino IDE:

https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/

Once you're up and running, download this repo, and make a copy of the `credentials_example.h` file, and rename it to just
`credentials.h`. In the new `credentials.h` file, change the SSID and PASS arrays to your WiFi network name and password.
Once that is updated, go ahead and upload it to the ESP32 and plug it in. With that done, simply wire the LEDs to pin 26 on the ESP32, and the on-board LED inside the play button, to pin 27 on the ESP32. 

That's it! Once you wire everything up and assemble it, you should have a working YouTube subscription counter too!

![YouTube Play Counter](https://github.com/csteamengine/YoutubeCounter/blob/master/Images/IMG_3743.png)
![YouTube Play Counter](https://github.com/csteamengine/YoutubeCounter/blob/master/Images/IMG_5599.png)
![YouTube Play Counter](https://github.com/csteamengine/YoutubeCounter/blob/master/Images/IMG_8685.png)



