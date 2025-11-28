#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>

namespace WillowVox
{
    class XboxSeriesGraphicsContext;

    /**
     * Xbox Series X/S Platform (Dev Mode / GDK)
     *
     * Controller: Xbox Series Controller (standard Xbox layout)
     * - Left Stick: Movement
     * - Right Stick: Look
     * - A: Action1 / Jump
     * - B: Action2 / MenuBack
     * - X: Action3
     * - Y: Action4
     * - LB/RB: Cycle items
     * - LT/RT: Crouch/Sprint
     * - Menu (≡): MenuOpen
     * - View (::): MenuBack
     *
     * Graphics: DirectX 12 (via GDK)
     * Input: GameInput API or XInput
     */
    class XboxSeriesPlatform : public IPlatform
    {
    public:
        XboxSeriesPlatform();
        ~XboxSeriesPlatform() override;

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
        std::unique_ptr<XboxSeriesGraphicsContext> m_graphicsContext;
    };
}
