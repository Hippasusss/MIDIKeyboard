# MIDIKeyboard
A new arduino MIDI controller for the Casio cdp130 keybed (and potentialy other scan matrix based velocity sensitive keybaords). 

To use this sketch you require:
- [Arduino Due (for native USB support and lots of digital IO)](https://smile.amazon.co.uk/Arduino-A000062-Due-Microcontroller/dp/B00A6C3JN2/ref=sr_1_3?keywords=arduino+due&qid=1584032351&sr=8-3)
- [1.0mm Pitch 30Pin AWM 80C 60V VW-1 Flexible Flat Cable](https://smile.amazon.co.uk/gp/product/B016DRXVB8/ref=ppx_yo_dt_b_asin_title_o09_s00?ie=UTF8&psc=1)
- [FFC FPC 30 Pin 0.5mm Pitch to DIP 1.0mm PCB Converter Board](https://smile.amazon.co.uk/gp/product/B07RWMJYNR/ref=ppx_yo_dt_b_asin_title_o01_s01?ie=UTF8&psc=1)
- A Broken Casio CDP130 Keyboard :(

The casio scan matrix is accesible through the 30 pin FFC cable near the centre of the keybed's mother board. It uses pins 11-18 for input.
It then uses pins 1-10 and 19-30 for output. Odd outputs are triggered when the key is first pressed. Even outputs are triggered when the key
is fully pressed. The time between these yields the velocity. 

Hopefully this will save someone else the time spent reverse engineering this!

Looks pretty smart when you're done:

![IMG_3679](https://user-images.githubusercontent.com/8618294/100522854-d6511380-31a3-11eb-9b38-7a63d9348d85.jpg)

