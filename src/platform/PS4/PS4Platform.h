#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>

namespace WillowVox
{
    class PS4GraphicsContext;

    /**
     * PlayStation 4 Platform (OpenOrbis Homebrew)
     *
     * Controller: DualShock 4 (same mapping as PS3)
     * Graphics: GNM (low-level) or Gnm wrapper
     * Input: libSceUserService + libScePad
     *
     * See PS3Platform for controller mapping (similar button layout)
     */
    class PS4Platform : public IPlatform
    {
    public:
        PS4Platform();
        ~PS4Platform() override;

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
        std::unique_ptr<PS4GraphicsContext> m_graphicsContext;
        // PS4-specific pad handling (similar to PS3)
    };
}
