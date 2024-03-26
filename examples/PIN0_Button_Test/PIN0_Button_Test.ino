// https://github.com/JulyIghor/PinButtonEvents
/*
MIT License

Copyright (c) 2024 Ighor July julyighor@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "PinButtonEvents.h"

PinButtonEvents button;

void setup()
{
    Serial.begin(115200);
    button.setPin(0, INPUT);
    button.setPinReleased(HIGH);
    button.setPinPressed(LOW);

    // General callback for handling all non sequence events
    button.on([](PinButtonEvents::State state, unsigned char holdCount, unsigned char repeatCount)
    {
        Serial.print("PinButtonEvents: ");
        Serial.print(state == PinButtonEvents::State::Pressed ? "pressed" : "released");
        Serial.print(" | HoldS: ");
        Serial.print(holdCount);
        Serial.print(" | Repeated: ");
        Serial.println(repeatCount);
    });

    // Define callbacks for handling specific button states, hold durations, and repeated press counts:

    button.on(PinButtonEvents::State::Pressed, 5, 4, []()
    {
        Serial.println("Pressed 5 times and held for 5 seconds");
    });

    button.on(PinButtonEvents::State::Pressed, 3, 0, []()
    {
        Serial.println("Held for 3 seconds");
    });

    button.on(PinButtonEvents::State::Pressed, 0, 1, []()
    {
        Serial.println("Double pressed button");
    });

    // Attach any sequences to specific actions via callbacks for an intuitive and versatile interaction framework. Ideal for projects requiring detailed input handling

    button.onSequence({PinButtonEvents::Sequence::Short, PinButtonEvents::Sequence::Short, PinButtonEvents::Sequence::Pause, PinButtonEvents::Sequence::Long}, []()
    {
        Serial.println("Sequence detected: Short, Short, Pause, Long");
    });

    button.onSequence({PinButtonEvents::Sequence::Long, PinButtonEvents::Sequence::Pause, PinButtonEvents::Sequence::Long, PinButtonEvents::Sequence::Short}, []()
    {
        Serial.println("Sequence detected: Long, Pause, Long, Short");
    });
}

void loop()
{
    button.update();
}
