#pragma once

class Timer : public Module
{
public:
    Timer(int keybind = 86, bool enabled = false) :
        Module("Timer", "Misc", "Increase your game tick rate.", keybind, enabled)
    {
        addSlider("Timer", "The games speed", &timerSpeed, 0, 60);
    }

    float timerSpeed = 40;

    void onEvent(RenderContextEvent* event) override {
        if (!Global::getClientInstance())
            return;

        if (!Global::getClientInstance()->getTimerClass())
            return;

        Global::getClientInstance()->getTimerClass()->setMainTimerSpeed(timerSpeed);
    }

    void onDisabled() override {
        if (!Global::getClientInstance())
            return;

        if (!Global::getClientInstance()->getTimerClass())
            return;

        Global::getClientInstance()->getTimerClass()->setMainTimerSpeed(20);
    }

    std::string getModeName() override {
        char str[10];
        sprintf_s(str, 10, "%.1f", timerSpeed);
        return " " + std::string(str);
    }
};