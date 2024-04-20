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
#include <limits>
#include <vector>

#define HOLD_INTERVAL 1000
#define REPEAT_INTERVAL 500
#define STATE_ANY -1

struct PinButtonSequence
{
    void addSequence(PinButtonEvents::Sequence);
    std::map<std::vector<PinButtonEvents::Sequence>, std::function<void()>> _callback;
    std::vector<PinButtonEvents::Sequence> _history;
    size_t _maxSize = 0;
};

inline uint32_t elapsed(uint32_t startMillis, uint32_t currentMillis)
{
    return currentMillis >= startMillis ? currentMillis - startMillis : (ULONG_MAX - startMillis) + currentMillis + 1;
}

inline uint32_t elapsed(uint32_t startMillis)
{
    return elapsed(startMillis, millis());
}

void PinButtonEvents::setPin(unsigned char pin, unsigned char mode)
{
    _pin = pin;
#if defined(ARDUINO_ARCH_STM32F1)
    pinMode(pin, (WiringPinMode)mode);
#else
    pinMode(pin, mode);
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

void PinButtonSequence::addSequence(PinButtonEvents::Sequence state)
{
    if (_callback.empty())
        return;

    _history.push_back(state);
    while (_maxSize < _history.size())
        _history.erase(_history.begin());

    for (const auto& pair : _callback)
    {
        const auto& key = pair.first;
        if (key.size() > _history.size())
            continue;

        if (!std::equal(key.begin(), key.end(), _history.end() - key.size()))
            continue;

        pair.second();
        break;
    }
}

void PinButtonEvents::onSequence(const std::initializer_list<Sequence>& sequence, std::function<void()> callback)
{
    if (sequence.size() == 0)
        return;

    if (callback)
    {
        if (!_sequence)
            _sequence.reset(new PinButtonSequence);

        _sequence->_callback[std::vector<Sequence>(sequence)] = callback;
        _sequence->_maxSize = std::max<size_t>(_sequence->_maxSize, sequence.size());
    }
    else if (_sequence)
    {
        _sequence->_callback.erase(std::vector<Sequence>(sequence));
        if (_sequence->_maxSize == _sequence->_callback.size() + 1)
        {
            _sequence->_maxSize = 0;
            for (const auto& pair : _sequence->_callback)
                _sequence->_maxSize = std::max<size_t>(_sequence->_maxSize, pair.first.size());
        }
        if (_sequence->_callback.empty())
            _sequence.reset();
    }
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
    if (elapsed(_triggerTime) < _debounceDelay)
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
            if (_holdCount)
                _repeatCount = 0;
            else if (_sequence)
                _sequence->addSequence(Sequence::Short);

            _holdCount = 0;
        }
        else if (_pressTime && digitalRead(_pin) == _pinPressed)
        {
            auto ml = millis();
            if (elapsed(_pressTime, ml) >= HOLD_INTERVAL)
            {
                if (_holdCount < std::numeric_limits<unsigned char>::max())
                {
                    _pressTime = ml;
                    ++_holdCount;

                    if (_sequence && !_sequence->_callback.empty() && _holdCount == 1)
                        _sequence->addSequence(Sequence::Long);
                }
                else
                    _pressTime = 0;
                triggerEvent();
            }
        }
        break;
    case State::Released:
        if (digitalRead(_pin) != _pinPressed)
        {
            if (_sequence && !_sequence->_history.empty() && *_sequence->_history.rbegin() != Sequence::Pause)
            {
                if (elapsed(_releaseTime, millis()) > HOLD_INTERVAL)
                    _sequence->addSequence(Sequence::Pause);
            }
            break;
        }
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
