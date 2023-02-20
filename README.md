A Node Addon API for the raspberry pi to communcate with the PCA9685 16-bit servo driver board, however... it also handles ramping over a duration which allows for smooth movement of servos without doing loops in nodejs [loop is handled within c++ on another thread and is there for not blocking or costly].


```
const PCA9685Ramp = require('PCA9685Ramp');
// parameters, minPWM, maxPWM, i2c device, i2c address. array of 16 elements of values between 0-255 for initializing the servos to a known home position.
PCA9685Ramp.Initialize(200, 500, "/dev/i2c-1", 0x40,
    [   
        127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127
    ]);
// parameters, servoIndex, targetDegrees [between 0-255 remapped to the minPWM/maxPWM], duration in milliseconds to arrive at the new degrees.
PCA9685Ramp.SetPWM(servoIndex, 0, 3000);
// closes the i2c_bus connection, will need to be re-initialize before using it again.
PCA9685Ramp.Shutdown();
```