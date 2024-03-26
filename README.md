# PinButtonEvents Library

Library for handling button events with debouncing support and a vast number of combinations.

## Features

- Support for handling button press, release, hold, and repeated presses.
- Debouncing to avoid unintended button events.
- Flexible callback system for defining actions based on button states, hold durations, and repeated press counts.

## Installation

To use the PinButtonEvents library in your Arduino projects, simply download the latest version from the [GitHub repository](https://github.com/JulyIghor/PinButtonEvents) and install it using the Arduino IDE.

## Usage
Define callbacks for handling specific button states, hold durations, and repeated press counts:
```cpp
#include <PinButtonEvents.h>

PinButtonEvents button;

void setup()
{
    Serial.begin(115200);
    button.setPin(0, INPUT); // Initialize button on pin 0
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
}

void loop()
{
    button.update();
}
```

## Advanced Usage
Define callbacks for handling all events:

```cpp
#include <PinButtonEvents.h>

PinButtonEvents button;

void setup()
{
    Serial.begin(115200);
    button.setPin(0, INPUT); // Initialize button on pin 0
    button.on([](PinButtonEvents::State state, unsigned char holdCount, unsigned char repeatCount)
    {
        Serial.print("PinButtonEvents: ");
        Serial.print(state == PinButtonEvents::State::Pressed ? "pressed" : "released");
        Serial.print(" | HoldS: ");
        Serial.print(holdCount);
        Serial.print(" | Repeated: ");
        Serial.println(repeatCount);
    });
}

void loop()
{
    button.update();
}
```

## Class Methods

### void setPin(unsigned char pin, unsigned char mode = INPUT)
Set the button pin and pin mode.

- Parameters:
  - `pin`: The GPIO pin connected to the button.
  - `mode`: The pinMode to set for the button pin (default: INPUT).

### `void update()`

Updates the state of the button and triggers any associated callbacks.

### `void on(std::function<void(State state, unsigned char holdS, unsigned char repeatCount)> callback)`

Registers a callback function for handling button events.

- Parameters:
  - `callback`: The callback function to be executed when a button event occurs.

### `void on(State state, unsigned char holdS, unsigned char repeatCount, std::function<void()> callback)`

Registers a callback function for handling specific button states, hold durations, and repeated press counts.

- Parameters:
  - `state`: The button state to trigger the callback (Pressed or Released).
  - `holdS`: The number of seconds the button must be held for the callback to trigger (-1 for any duration).
  - `repeatCount`: The number of repeated presses required for the callback to trigger (-1 for any count).
  - `callback`: The callback function to be executed when the specified button event occurs.

### `void on(State state, std::function<void()> callback)`

Registers a callback function for handling button events with a specific state.

- Parameters:
  - `state`: The button state to trigger the callback (Pressed or Released).
  - `callback`: The callback function to be executed when the specified button event occurs.

### `unsigned long debounceDelay() const`

Returns the debounce delay in milliseconds.

### `void setDebounceDelay(unsigned long newDebounceDelay = 30)`

Sets the debounce delay in milliseconds.

- Parameters:
  - `newDebounceDelay`: The new debounce delay value (default: 30).

### `int getPinReleased() const`

Returns the value of the pin when released.

### `void setPinReleased(int newPinReleased = HIGH)`

Sets the value of the pin when released.

- Parameters:
  - `newPinReleased`: The new pin value when released (default: HIGH).

### `int getPinPressed() const`

Returns the value of the pin when pressed.

### `void setPinPressed(int newPinPressed = LOW)`

Sets the value of the pin when pressed.

- Parameters:
  - `newPinPressed`: The new pin value when pressed (default: LOW).
