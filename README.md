# AeroSim - 1.6 Initial Release
This simulation calcualtes coefficient of drag, base drag, skin friction, and pressure, along with Reynold's Numbers, Mach speeds, and Drag force at a certain velocity of a rocket with any params. 

Coefficient of pressure drag and axial drag interpolation still need some revisement. 
As of now this sim only accounts for the rocket's drag at one single point in time.

# Properties to measure
The surface height of the rocket is needed. OpenRocket defaults to 60micrometers. This depends on the rocket's material
Length of the nosecone and length of body tube. - This sim only allows for conical nose cones (for now)
Diameter of the rocket. 
This sim assumes the nose cone and body tube have the same diameter and surfaceheight.

More details with further revisions!

# Note
This is a rewrite of the OpenRocket flight code in c++. I have tried my best to improve it.
The purpose of this library is to be used for in-flight drag calcs in the future. 



