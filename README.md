# ferrofluids
Arduino code to read proximity from 2 infrared sensors, drive 2 electromagnets and animate a ferrofluid sculpture.

  * [Web](https://sites.google.com/site/pierredangauthier/noir)
  * [Pictures](https://goo.gl/photos/tqKLKexmqSTodXV98)
  * [Videos](https://www.youtube.com/channel/UCY0LABikxcgyDP2GDgEsV_g)

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
 
Prototypes

  * DC supply directly wired to one electromagnet with a 5 Ohms power resistor
     * ON/OFF switch + 5 Ohm power resistor => 2 Amps.
     * Need a 'power' resistor dissipating 24W. It gets hot.
  * Arduino with home made PWM power board (optocouplers and MOSFET)
     * Removed the hot resistor and added spikes modulation.
     * Power is controlled via Pulse Width Modulation (alpha max = 10% to stay below 2.5A)
     * With analogwrite (500Hz) + optocoupler + a low Rds(on) MOSFET.
     * Field strength is controlled by hand proximity (IR sensor).
  * Addition of a second electromagnet
  * Using a motorshield to be able to inverse polarities

Misc

  * Magnetic fields are difficult to generate and canalize.
  * Better start with a small screw and and a simple design.
     * Make a tight, clean wiring. 
     * First 2 layers are the most important ones.
     * Use glue to fix layers.
     * Do not damage wires with sharp edges or with the glue.
     * Wire loops must be very close to the ferrofluid.            

Parts
  * https://www.supermagnete.de/eng/physics-magnets/ferrofluid-10-ml-magnetic-fluid-for-experiments_FER-01
  * http://www.conrad.fr/ce/fr/product/605237/Bobine-de-fil-de-cuivre-100-mm-28-A-Block-CUL-100100/SHOP_AREA_17416?
  * http://www.arduino.cc/en/Tutorial/PWM
  * http://www.datasheetcatalog.com/datasheets_pdf/4/N/3/5/4N35.shtml
  * https://www.fairchildsemi.com/datasheets/RF/RFP50N06.pdf
  * http://www.farnell.com/datasheets/1496354.pdf
