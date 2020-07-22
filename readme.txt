------


By Pat


------

Install the original opengrade from Github the replace the OpenGrade.exe with this in this folder

plug the arduino in the AutoSteer port




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

code :

in "OpenGL.Designer.cs line 415

if (cboxLaserModeOnOff.Checked)

//in cm
cutDelta = (pn.altitude - ct.zeroAltitude)*100
else


to change distance from line to use valve/lightbar

in OpenGL.Designer.cs line 526:
if (minDist < 9000) // original number is 15(3.9 meter). meter2 form the line, For 5 meter put 25.9000 is 95 meters