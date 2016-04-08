# ferrofluids
Arduino code to drive a ferrofluid sculpture

[Pictures and videos](https://sites.google.com/site/pierredangauthier/home/noir)


Ferrofluid sculptures

  * Ferrofluids + 2 electromagnets + IR proximity sensors + Arduino.

Electromagnet

  * 10cm * 1cm screws, smoothed edges.
  * 4-layer coil, 60 turns per layer. Insulated copper wire (1mm / 2.8 Amps).
  * 12V / 2Amps DC supply from an IDE-to-USB adaptor.
  * Magnetic field shall be strong at the top, its gradient pulls the fluid up.
  * Tight wiring to canalize it though the screw.
  * First 2 layers are the most important ones. Top loops must be as close as possible to the fluid.

Power control

  * Arduino's Pulse Width Modulation (500Hz, max ratio = 10% to stay < 2.5A).
  * R3 motorshield or optocoupler + low Rds(on) MOSFET.

Code
  * Last version : ferro3states2FerrosMotorShieldNoSin.ino
  * Edited on [codebender.cc](https://codebender.cc/sketch:23274)

https://codebender.cc/sketch:23274
