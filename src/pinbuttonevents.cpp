/* MIT License

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

#include "pinbuttonevents.h"
#include <limits>

#define HOLD_INTERVAL 1000
#define REPEAT_INTERVAL 500
#define STATE_ANY -1

PinButtonEvents::PinButtonEvents(unsigned char pin, unsigned char setPinMode, unsigned long debounceDelay) :
    _pin(pin), _debounceDelay(debounceDelay)
{
#if defined(ARDUINO_ARCH_STM32F1)
    pinMode(pin, (WiringPinMode)setPinMode);
#else
    pinMode(pin, setPinMode);
#endif
}

void PinButtonEvents::on(std::function<void(State state, unsigned char holdS, unsigned char repeatCount)> callback)
{
    _callback = callback;
}

void PinButtonEvents::on(State state, unsigned char holdS, unsigned char repeatCount, std::function<void()> callback)
{
    auto key = std::make_tuple(state, holdS, repeatCount);
    if (callback)
        _callbackMap[key] = callback;
    else
        _callbackMap.erase(key);
}

void PinButtonEvents::on(State state, std::function<void()> callback)
{
    on(state, STATE_ANY, STATE_ANY, callback);
}

void PinButtonEvents::triggerEvent()
{
    _triggerTime = millis();

    if (_callback)
        _callback(_state, _holdCount, _repeatCount);

    auto call = _callbackMap.find(std::make_tuple(_state, _holdCount, _repeatCount));
    if (call != _callbackMap.end())
        call->second();

    if (!_holdCount)
    {
        call = _callbackMap.find(std::make_tuple(_state, STATE_ANY, STATE_ANY));
        if (call != _callbackMap.end())
            call->second();

        call = _callbackMap.find(std::make_tuple(_state, STATE_ANY, _repeatCount));
        if (call != _callbackMap.end())
            call->second();
    }

    call = _callbackMap.find(std::make_tuple(_state, _holdCount, STATE_ANY));
    if (call != _callbackMap.end())
        call->second();
}

void PinButtonEvents::update()
{
    if (millis() - _triggerTime < _debounceDelay)
        return;

    switch (_state)
    {
    case State::Pressed:
        if (digitalRead(_pin) == _pinReleased)
        {
            _pressTime = 0;
            _releaseTime = millis();
            _state = State::Released;
            triggerEvent();
            _holdCount = 0;
        }
        else if (_pressTime && digitalRead(_pin) == _pinPressed)
        {
            auto ml = millis();
            if (ml - _pressTime >= HOLD_INTERVAL)
            {
                if (_holdCount < std::numeric_limits<unsigned char>::max())
                {
                    _pressTime = ml;
                    ++_holdCount;
                }
                else
                    _pressTime = 0;
                triggerEvent();
            }
        }
        break;
    case State::Released:
        if (digitalRead(_pin) != _pinPressed)
            break;
        _pressTime = millis();
        _state = State::Pressed;
        if (_releaseTime)
        {
            if (_pressTime - _releaseTime <= REPEAT_INTERVAL)
            {
                _releaseTime = 0;
                if (_repeatCount < std::numeric_limits<unsigned char>::max())
                    ++_repeatCount;
                triggerEvent();
                break;
            }
            _repeatCount = 0;
            _releaseTime = 0;
        }
        triggerEvent();
        break;
    }
}

unsigned long PinButtonEvents::debounceDelay() const
{
    return _debounceDelay;
}

void PinButtonEvents::setDebounceDelay(unsigned long newDebounceDelay)
{
    _debounceDelay = newDebounceDelay;
}

int PinButtonEvents::getPinReleased() const
{
    return _pinReleased;
}

void PinButtonEvents::setPinReleased(int newPinReleased)
{
    _pinReleased = newPinReleased;
}

int PinButtonEvents::getPinPressed() const
{
    return _pinPressed;
}

void PinButtonEvents::setPinPressed(int newPinPressed)
{
    _pinPressed = newPinPressed;
}
