const PCA9685Ramp = require('./build/Release/PCA9685Ramp');
const GATE = [
    0,0,        90,100,      90,100,    150,100,
    0,100,      150,100,     90,100,    90,0,
    0,100,      150,100,     0,40,      90,100,
    100,100,    90,40,       0,100,     150,100,
];
var gate_index = 0;

Math.map = function (value, in_min, in_max, out_min, out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

console.log("running servo test.");
PCA9685Ramp.Initialize(200, 500, "/dev/i2c-1", 0x40,
    [   
        127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127
    ]);
process.on('exit',() => {
    PCA9685Ramp.Shutdown();
});

setInterval(() => {
    for (var i=0; i < 8; i++) {
        var degrees = GATE[gate_index++];
        var servoIndex = i;

        // if we're on the left side, reverse.
        if (servoIndex >= 8 / 2) {
            degrees = Math.map(degrees, 180, 0.0, 0, 180);
        }

        // FRONT_RIGHT_FOOT, FRONT_LEFT_FOOT, are both inverted.
        if (servoIndex == 1 || servoIndex == 5) {
            degrees = Math.map(degrees, 180, 0.0, 0, 180);
        }

        // if we're more then half servo count, jump the servo gap.
        if (servoIndex >= 8 / 2) {
            servoIndex += 8;
        }
        PCA9685Ramp.SetPWM(servoIndex, Math.map(degrees, 0, 180, 0, 255), 250);
    }

    if ( gate_index >= GATE.length)  {
        gate_index = 0;
    }

}, 250);