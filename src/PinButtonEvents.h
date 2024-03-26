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

#ifndef PINBUTTONEVENTS_H
#define PINBUTTONEVENTS_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <functional>
#include <map>
#include <tuple>

class PinButtonEvents
{
public:
    enum class State : bool
    {
        Pressed = 0,
        Released = 1
    };

    void setPin(unsigned char pin, unsigned char mode = INPUT);
    void update();

    void on(std::function<void(State state, unsigned char holdS, unsigned char repeatCount)> callback);
    void on(State state, unsigned char holdS, unsigned char repeatCount, std::function<void()> callback);
    void on(State state, std::function<void()> callback);

    unsigned long debounceDelay() const;
    void setDebounceDelay(unsigned long newDebounceDelay = 30);

    int getPinReleased() const;
    void setPinReleased(int newPinReleased = HIGH);

    int getPinPressed() const;
    void setPinPressed(int newPinPressed = LOW);

private:
    unsigned char _pin = 0;
    std::function<void(State state, unsigned char holdS, unsigned char repeatCount)> _callback;
    std::map<std::tuple<State, unsigned char, unsigned char>, std::function<void()>> _callbackMap;

    State _state = State::Released;
    unsigned char _holdCount = 0;
    unsigned char _repeatCount = 0;

    unsigned long _pressTime = 0;
    unsigned long _releaseTime = 0;
    unsigned long _triggerTime = 0;
    unsigned long _debounceDelay = 30;

    int _pinReleased = HIGH;
    int _pinPressed = LOW;

    void triggerEvent();
};

#endif // PINBUTTONEVENTS_H
