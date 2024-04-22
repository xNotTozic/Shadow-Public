#pragma once

class ChestStealer : public Module
{
public:
    ChestStealer(int keybind = 7, bool enabled = true) :
        Module("ChestStealer", "Player", "Steal items out of chests", keybind, enabled)
    {
        addSlider("SPS", "How many items are moved a second", &SPS, 1, 60);
    }

    float SPS = 20;

    int item = 0;
    int maxItem = 56;

    void onEvent(ContainerTickEvent* event) override
    {
        if (TimeUtils::hasTimeElapsed("Cs", 500, false))
            item = 0;

        ContainerScreenController* controller = event->Controller;

        if (!controller)
            return;

        if (SPS == 60)
        {
            for (int i = 0; i < 56; ++i)
            {
                controller->stealItems(Containers::Container, i);
            }

            return;
        }

        if (TimeUtils::hasTimeElapsed("Cs", 1000 / SPS, true))
        {
            controller->stealItems(Containers::Container, item);

            if (item == maxItem)
            {
                item = -1;
            }

            item++;
        }
    }
};