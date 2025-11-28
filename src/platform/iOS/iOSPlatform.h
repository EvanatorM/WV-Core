#pragma once

#include <wv/platform/IPlatform.h>
#include <memory>

namespace WillowVox
{
    class iOSGraphicsContext;

    /**
     * iOS Platform Implementation
     *
     * Virtual Touch Controls (similar to Android):
     * - Left virtual joystick: Movement
     * - Right side: Look control (drag)
     * - Bottom right buttons: A (Action1), B (Action2), Jump
     * - Top left: Menu button
     */
    class iOSPlatform : public IPlatform
    {
    public:
        iOSPlatform();
        ~iOSPlatform() override;

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
        std::unique_ptr<iOSGraphicsContext> m_graphicsContext;
        // Touch input tracking (similar to Android)
        // Implementation details omitted for brevity
    };
}
