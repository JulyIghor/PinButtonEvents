# PinButtonEvents Library

Library for handling button events with debouncing support and a vast number of combinations.

The project introduces a versatile class designed for advanced button event management on Arduino-compatible devices. It enables detection and handling of complex button press sequences, akin to Morse code, allowing users to define specific patterns of short, long presses, and pauses to trigger designated functions. This feature is particularly useful for applications requiring nuanced user input without relying on multiple buttons or external input devices, streamlining interaction and enhancing user experience.

## Features

The project boasts several key features designed to enhance user interaction through a single button:

1. **Advanced Button Event Management**: Detects and handles multiple button press states.
2. **Custom Sequence Detection**: Allows defining and recognizing complex press patterns, similar to Morse code.
3. **Dynamic Callback Association**: Links specific button sequences to user-defined callback functions for tailored responses.
4. **Debounce Handling**: Incorporates debounce logic to ensure accurate press detection and prevent false triggers.
5. **Flexible Configuration**: Offers adjustable settings for press durations and intervals, accommodating different user preferences and applications.
6. **Minimal Hardware Requirements**: Operates with a single button, reducing the need for additional input hardware.

## Installation

To use the PinButtonEvents library in your Arduino projects, simply download the latest version from the [GitHub repository](https://github.com/JulyIghor/PinButtonEvents) or install it using the Arduino IDE Library Registry.

## Usage

```cpp
#include <PinButtonEvents.h>

PinButtonEvents button;

void setup()
{
    Serial.begin(115200);
    button.setPin(0, INPUT); // Initialize button on pin 0

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

### `void onSequence(const std::initializer_list<PinButtonEvents::Sequence>& sequence, std::function<void()> callback);`

Registers a callback function for handling button sequences. Implement intricate input patterns recognition by defining a series of `Short`, `Long`, and `Pause` button events.

- Parameters:
  - `callback`: The callback function to be executed when a button sequence event occurs.

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

## Contact
- **Author:** Ighor July
- **Email:** [julyighor@gmail.com](mailto:julyighor@gmail.com)
- **Project Repository:** [GitHub - JulyIghor/PinButtonEvents](https://github.com/JulyIghor/PinButtonEvents)
