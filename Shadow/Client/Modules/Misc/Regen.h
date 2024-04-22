#pragma once

class Regen : public Module
{
public:
    Regen(int keybind = Keys::NUM_0, bool enabled = false) :
        Module("Regen", "Misc", "Nukes blocks in a radius around you", keybind, enabled)
    {
        addSlider("Range", "How far around you is regened", &radiusregen, 1, 6);
        addSlider("Delay", "The delay between breaking", &delay, 1, 15);
        addBool("Render", "Render the redstone counter", &render);
        //addSlider("Delay", "The delay between destroying blocs", &delay, 1, 20);
    }

private:
    float radiusregen = 1.5;
    bool render = true;

    int enabledticks;
    bool hasDestroyed = false;
    bool destroy = false;
    bool isregen = false;

    int tick = 0;
    int slot = 0;
    bool firstbreak = false;
    bool continuemine = false;

    float delay = 12;
    bool haspickaxe = true;
public:
    Vector3<int> GetBlockPosition()
    {
        auto localPlayer = Global::getClientInstance()->getLocalPlayer();
        //if (localPlayer == nullptr) return;

        Vector3<float> pos = localPlayer->getAABBShape()->PosLower;

        return { (int)floor(pos.x), (int)floor(pos.y), (int)floor(pos.z) };
    }

    void onEnabled() override {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) return;
        
        firstbreak = false;
        continuemine = false;
        isregen = true;
        enabledticks = 0;
    }

    const static Vector2<float> CalcAngleRegen(Vector3<float> ths, Vector3<float> dst)
    {
        Vector3 diff = dst.submissive(ths);

        diff.y = diff.y / diff.magnitude();
        Vector2<float> angles;
        angles.x = asinf(diff.y) * -(180.0f / 3.1415927f);
        angles.y = (float)-atan2f(diff.x, diff.z) * (180.0f / 3.1415927f);

        return angles;
    }

    void onEvent(RenderContextEvent* event) override
    {
        auto player = Global::getClientInstance()->getLocalPlayer();
        if (player == nullptr) {
            return;
        }
        
        GameMode* gamemode = player->getGamemode();

        if (!gamemode || !player)
            return;

        enabledticks++;

        PlayerInventory* supplies = player->getSupplies();

        isregen = true;
        haspickaxe = true;

        if (enabledticks > delay) {
            isregen = true;
        }
        else {
            isregen = false;
        }

        Vector3<int> playerBlockPos = GetBlockPosition();

        for (int x = -radiusregen; x <= radiusregen; x++) {
            for (int y = -radiusregen; y <= radiusregen; y++) {
                for (int z = -radiusregen; z <= radiusregen; z++) {
                    Vector3<int> blockPos = Vector3<int>(playerBlockPos.x + x, playerBlockPos.y + y, playerBlockPos.z + z);

                    BlockSource* source = Global::getClientInstance()->getBlockSource();

                    if (!source)
                        return;

                    Block* block = source->getBlock(blockPos);

                    if (block->GetBlockLegacy()->getBlockID() == 73 || block->GetBlockLegacy()->getBlockID() == 74)
                    {
                        destroy = true;
                        gamemode->startDestroyBlock(blockPos, 1, true);
                        continuemine = true;
                    }
                    else continue;

                    if (destroy) {
                        if (enabledticks > 12) {
                            gamemode->destroyBlock(blockPos, 1, true); // is safe there
                            hasDestroyed = true;
                            enabledticks = 0;
                            gamemode->stopDestroyBlock(blockPos);
                            continuemine = false;
                        }
                    }

                    if (hasDestroyed) {
                        destroy = false;
                        hasDestroyed = false;
                    }

                    if (!destroy) {
                        isregen = false;
                        if (continuemine) gamemode->stopDestroyBlock(blockPos);
                        continuemine = false;
                        enabledticks = 0;
                    }
                    Vector2<float> angle = CalcAngleRegen(player->getStateVector()->Position, blockPos.ToFloat());

                    player->getMovementInterpolator()->Rotations = angle;
                }
            }
        }
    }

    int cool = 0;
    void onEvent(ImGuiRenderEvent* event) {
        auto instance = Global::getClientInstance();
        auto localPlayer = instance->getLocalPlayer();

        if (!localPlayer || !localPlayer->isAlive()) return;

        float percentage = (enabledticks * 100) / delay;
        if (percentage > 100) percentage = 0;

        cool++;
        if ((int)percentage == 0) {
            if (cool >= 90) cool = 0;
        }
        else {
            if (cool >= 25) cool = 0;
        }

        float renderx = instance->getGuiData()->mcResolution.x / 2;
        float rendery = (instance->getGuiData()->mcResolution.y / 2) + 90;
        Vector2 renderpos = Vector2(renderx, rendery);
        std::string rendertext = "Mining (" + std::to_string((int)percentage) + "/100)";
        if ((int)percentage == 0) {
            rendertext = "Queued";
        }

        float textLen = ImRenderUtil::getTextWidth(&rendertext, 1) / 2;
        float textLenTrue = ImRenderUtil::getTextWidth(&rendertext, 1);
        renderpos.x -= textLen;

        Vector4<float> rect = Vector4<float>(renderpos.x - 19, renderpos.y - 5, (renderpos.x + textLenTrue) + 10, renderpos.y + 22);
        Vector4<float> rectQueued = Vector4<float>(rect.x - 19, rect.y, rect.z + 19, rect.w);
        Vector4<float> rectSmooth = Vector4<float>(rect.x, rect.y, (rect.z - 120) + (cool * 4.4), rect.w);
        if ((int)percentage == 0) {
            rectSmooth = Vector4<float>(rect.x - 19, rect.y, ((rect.z - 90) + cool) + 19, rect.w);
        }

        if (render) {
            if (haspickaxe) {
                if ((int)percentage == 0) {
                    ImRenderUtil::fillRectangle(rectQueued, UIColor(33, 33, 33), 1.f, 10.f);
                }
                else {
                    ImRenderUtil::fillRectangle(rect, UIColor(33, 33, 33), 1.f, 10.f);
                }
                if ((int)percentage == 0) {
                    ImRenderUtil::fillRectangle(rectSmooth, UIColor(201, 62, 220), 1.f, 10.f);
                    ImRenderUtil::fillShadowRectangle(rectSmooth, UIColor(201, 62, 220), 1, 150, 0, 10);
                }
                else {
                    ImRenderUtil::fillRectangle(rectSmooth, UIColor(212, 173, 48), 1.f, 10.f);
                    ImRenderUtil::fillShadowRectangle(rectSmooth, UIColor(212, 173, 48), 1, 150, 0, 10);
                }
                ImRenderUtil::drawText(renderpos, &rendertext, UIColor(255, 255, 255), 1, 1.f, true);
            }
        }
    }

    void onDisabled() override {
        destroy = false;
        isregen = false;
    }
};