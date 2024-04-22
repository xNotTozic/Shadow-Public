#pragma once

class Killaura : public Module
{
public:
    Killaura(int keybind = 80, bool enabled = false) :
        Module("Killaura", "Combat", "Hits every entity around you.", keybind, enabled)
    {

    }

    bool TargetMobs = true; // lol
    bool Swing = true; // enum
    bool Strafe = false;

    int switchMode = 0;
    float range = 4;
    float APS = 10;
    bool rotations = true;

    void onEvent(RenderContextEvent* event) override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (!player)
        {
            return;
        }

        auto list = player->getLevel()->getRuntimeActorList3();

        GameMode* gm = player->getGamemode();

        if (!gm)
            return;

        gm->attack(*list);

        //for (Actor* actor : list) {
          //  gm->attack(*actor);

            //player->swing();
       // }
    }

    std::string getModeName() override {
        return " " + std::string("Multi");
    }
};