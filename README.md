A Node Addon API for the raspberry pi to communcate with the PCA9685 16-bit servo driver board, however... it also handles ramping over a duration which allows for smooth movement of servos without doing loops in nodejs [loop is handled within c++ on another thread and is there for not blocking or costly].


```
const PCA9685Ramp = require('./build/Release/PCA9685Ramp');
PCA9685Ramp.Initialize(200, 500, "/dev/i2c-1", 0x40, () => {});
PCA9685Ramp.SetPWM(servoIndex, 0, 3000);
```