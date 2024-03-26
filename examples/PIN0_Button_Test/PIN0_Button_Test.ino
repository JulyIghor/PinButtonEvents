// https://github.com/JulyIghor/PinButtonEvents

#include "PinButtonEvents.h"

PinButtonEvents button(0);

void setup()
{
    Serial.begin(115200);
    button.on([](PinButtonEvents::State state, unsigned char holdCount, unsigned char repeatCount)
              {
                  // General callback for handling all events
                  Serial.print("PinButtonEvents: ");
                  Serial.print(state == PinButtonEvents::State::Pressed ? "pressed" : "released");
                  Serial.print(" | HoldS: ");
                  Serial.print(holdCount);
                  Serial.print(" | Repeated: ");
                  Serial.println(repeatCount); });
}

void loop()
{
    button.update();
}
