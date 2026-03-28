#include "iOSGraphicsContext.h"
#include <wv/Logger.h>

#ifdef PLATFORM_IOS

// iOS SDK includes
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import <QuartzCore/CAEAGLLayer.h>
#import <UIKit/UIKit.h>
#include <mach/mach_time.h>

namespace WillowVox
{
    iOSGraphicsContext::iOSGraphicsContext()
    {
        // Get start time
        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);
        uint64_t time = mach_absolute_time();
        m_startTime = (double)time * (double)timebase.numer / (double)timebase.denom / 1e9;
    }

    iOSGraphicsContext::~iOSGraphicsContext()
    {
        Shutdown();
    }

    bool iOSGraphicsContext::Initialize(int width, int height, const char* title)
    {
        m_width = width;
        m_height = height;

        Logger::EngineLog("Initializing iOS Graphics (OpenGL ES 3.0)");

        // Create EAGLContext with OpenGL ES 3.0
        EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if (!context)
        {
            Logger::EngineError("Failed to create EAGLContext with ES 3.0, trying ES 2.0");
            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
            if (!context)
            {
                Logger::EngineError("Failed to create EAGLContext");
                return false;
            }
        }
        m_eaglContext = (__bridge_retained void*)context;

        // Make context current
        if (![EAGLContext setCurrentContext:context])
        {
            Logger::EngineError("Failed to set current EAGLContext");
            return false;
        }

        // Get the main UIWindow's layer (this is a simplification - in real app, you'd get this from your ViewController)
        UIWindow* window = [[UIApplication sharedApplication] keyWindow];
        if (!window)
        {
            Logger::EngineError("Failed to get UIWindow");
            return false;
        }

        CAEAGLLayer* eaglLayer = (CAEAGLLayer*)window.layer;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = @{
            kEAGLDrawablePropertyRetainedBacking: @NO,
            kEAGLDrawablePropertyColorFormat: kEAGLColorFormatRGBA8
        };
        m_eaglLayer = (__bridge void*)eaglLayer;

        // Create framebuffer
        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

        // Create color renderbuffer
        glGenRenderbuffers(1, &m_colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);

        // Allocate storage for the renderbuffer from the layer
        [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderbuffer);

        // Get actual renderbuffer size
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_width);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_height);

        // Create depth renderbuffer
        glGenRenderbuffers(1, &m_depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);

        // Check framebuffer status
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            Logger::EngineError("Framebuffer is not complete: 0x%X", status);
            return false;
        }

        // Enable OpenGL ES features
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Logger::EngineLog("iOS Graphics initialized (OpenGL ES, %dx%d)", m_width, m_height);
        m_initialized = true;

        return true;
    }

    void iOSGraphicsContext::Shutdown()
    {
        if (m_initialized)
        {
            // Delete framebuffer objects
            if (m_framebuffer)
            {
                glDeleteFramebuffers(1, &m_framebuffer);
                m_framebuffer = 0;
            }
            if (m_colorRenderbuffer)
            {
                glDeleteRenderbuffers(1, &m_colorRenderbuffer);
                m_colorRenderbuffer = 0;
            }
            if (m_depthRenderbuffer)
            {
                glDeleteRenderbuffers(1, &m_depthRenderbuffer);
                m_depthRenderbuffer = 0;
            }

            // Release EAGL context
            if (m_eaglContext)
            {
                EAGLContext* context = (__bridge_transfer EAGLContext*)m_eaglContext;
                if ([EAGLContext currentContext] == context)
                {
                    [EAGLContext setCurrentContext:nil];
                }
                context = nil;
                m_eaglContext = nullptr;
            }

            m_initialized = false;
        }
    }

    void iOSGraphicsContext::BeginFrame()
    {
        // Bind framebuffer for rendering
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
    }

    void iOSGraphicsContext::EndFrame()
    {
        // Nothing specific needed
    }

    void iOSGraphicsContext::SwapBuffers()
    {
        // Present the color renderbuffer
        glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
        EAGLContext* context = (__bridge EAGLContext*)m_eaglContext;
        [context presentRenderbuffer:GL_RENDERBUFFER];
    }

    void iOSGraphicsContext::Clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void iOSGraphicsContext::ClearDepth()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    bool iOSGraphicsContext::ShouldClose() const
    {
        return m_shouldClose;
    }

    void iOSGraphicsContext::SetShouldClose(bool shouldClose)
    {
        m_shouldClose = shouldClose;
    }

    void iOSGraphicsContext::GetFramebufferSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void iOSGraphicsContext::GetWindowSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    void iOSGraphicsContext::SetVSync(bool enabled)
    {
        m_vsyncEnabled = enabled;
        // VSync on iOS is typically controlled at the CADisplayLink level
        // This would require integration with the view controller
    }

    bool iOSGraphicsContext::IsVSyncEnabled() const
    {
        return m_vsyncEnabled;
    }

    float iOSGraphicsContext::GetTime() const
    {
        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);
        uint64_t time = mach_absolute_time();
        double currentTime = (double)time * (double)timebase.numer / (double)timebase.denom / 1e9;
        return static_cast<float>(currentTime - m_startTime);
    }

    void iOSGraphicsContext::SetBackgroundColor(float r, float g, float b, float a)
    {
        m_clearColor[0] = r;
        m_clearColor[1] = g;
        m_clearColor[2] = b;
        m_clearColor[3] = a;
    }

    void iOSGraphicsContext::SetViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
    }
}

#endif // PLATFORM_IOS
