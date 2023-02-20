#pragma once
#include <linux/types.h>
#include <iostream>

class Ramp 
{

private:

    __u16 A;
    __u16 B;
    __u16 val;
    __u16 dur;
    __u16 pos;
    __u16 grain;
    __u32 time;

public:

    Ramp() {
        A = 0;
        B = 0;
        val = 0;
        dur = 0;
        pos = 0;
        grain = 10;
        time = 0;
    }

    __u16 update(__u32 now) {
        if (time == 0) {
            time = now;
        }

        __u32 delta = now - time;
        const bool doUpdate = delta > grain;
        if (doUpdate) {
            time = now;
            if (pos + delta < dur) {
                pos += delta;
            } else {
                pos = dur;
            }
            if (dur > 0 && A != B) {
                float k = (float)pos / (float)dur;
                val = B >= A ? (A + (B-A)*k) : (A - (A-B) * k);
            } else {
                val = B;
            }
        }
        return val; 
    }

    __u16 go(__u16 value, __u16 duration) {
        A = val;
        B = value;
        dur = duration;
        time = 0;

        pos = 0;
        if (dur == 0) val = B;
        return val;
    }

    bool isFinished() {
        return pos == dur;
    }

    __u16 getValue() {
        return val;
    }

};