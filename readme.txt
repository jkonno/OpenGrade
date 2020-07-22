------


By Pat

Modified by nut (Juho K 22.7.2020)


------

This is with the blade control edited into the UI and also connected to the machine port instead.


Modifications in soure code:
Serch for
cutDelta : this is the value used from the original code
cutValve : this is the value added for the output


cutDelta value:
9999 when not in use
- to + form line in centimeters when in use

cutValve  value:
100 when not in use
1 to 199 when in use, 100 is on the line, else is the distance from line in mm. 
	 max is 99 mm even if farter from line.


Lasermode:
interface:
use value from push to zero btnZeroAltitude
make button on and off make "cboxLaserModeOnOff" from "cboxRecLastOnOff"

A new INO file with both propo control and also on/off valve control included.

Arduino is sending back up/down status and current cutValue, or in propomode up/down signal and PWM value
