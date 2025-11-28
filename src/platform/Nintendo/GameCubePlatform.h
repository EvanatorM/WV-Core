#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>

namespace WillowVox
{
    class GameCubeGraphicsContext;

    /**
     * Nintendo GameCube Platform (devkitPPC Homebrew)
     *
     * Controller Mapping (GameCube Controller):
     * - Control Stick: Movement
     * - C-Stick: Look
     * - A: Action1 / Jump
     * - B: Action2
     * - X: Action3
     * - Y: Action4
     * - L/R: Cycle items
     * - Z: Crouch
     * - Start: MenuOpen
     *
     * Graphics: GX (libogc)
     * Input: PAD (libogc)
     */
    class GameCubePlatform : public IPlatform
    {
    public:
        GameCubePlatform();
        ~GameCubePlatform() override;

        bool Initialize() override;
        void Shutdown() override;
        void PollInput(InputState& outInputState) override;
        void ResetInputFrameState(InputState& inputState) override;
        void ProcessEvents() override;
        IGraphicsContext* GetGraphicsContext() override;
        const char* GetUserDataPath() const override;
        const char* GetAssetsPath() const override;
        const char* GetPlatformName() const override;
        InputDeviceType GetPrimaryInputDevice() const override;
        bool HasFeature(const char* featureName) const override;

    private:
        std::unique_ptr<GameCubeGraphicsContext> m_graphicsContext;
    };
}
