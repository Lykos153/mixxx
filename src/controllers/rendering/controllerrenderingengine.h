#pragma once

#include <GL/gl.h>

#include <QMutex>
#include <QObject>
#include <QWaitCondition>
#include <chrono>

#include "controllers/legacycontrollermapping.h"
#include "controllers/scripting/controllerscriptenginebase.h"
#include "preferences/configobject.h"
#include "util/time.h"

class Controller;
class QOffscreenSurface;
class QOpenGLContext;
class QOpenGLFramebufferObject;
class QQmlEngine;
class QQuickRenderControl;
class QQuickWindow;
class QThread;

/// @brief This class is used to host the rendering of a screen controller,
/// using and existing QML Engine running under a ControllerScriptEngineBase.
class ControllerRenderingEngine : public QObject {
    Q_OBJECT
  public:
    ControllerRenderingEngine(const LegacyControllerMapping::ScreenInfo& info,
            ControllerScriptEngineBase* parent);
    ~ControllerRenderingEngine();

    bool event(QEvent* event) override;

    QSize screenSize() const {
        return m_screenInfo.size;
    }

    bool isValid() const {
        return m_isValid;
    }

    bool isRunning() const;

    // pointer lives as long as the `ControllerRenderingEngine` instance it is retrieved from.
    QQuickWindow* quickWindow() const {
        return m_quickWindow.get();
    }

    const LegacyControllerMapping::ScreenInfo& info() const {
        return m_screenInfo;
    }

  public slots:
    // Request sending frame data to the device. The task will be run in the
    // rendering event loop. This method should only be called once received the
    // `frameRendered` signal
    virtual void requestSendingFrameData(Controller* controller, const QByteArray& frame);
    // Request setting up the rendering context for QML engine and wait till it
    // is completed. The task will be run in the rendering event loop to ensure
    // thread affinity of engine components. `isValid` can be used to ensure
    // that the setup was successful
    void requestEngineSetup(std::shared_ptr<QQmlEngine> qmlEngine);
    void start();
    virtual bool stop();

  private slots:
    void finish();
    void renderFrame();
    void setup(std::shared_ptr<QQmlEngine> qmlEngine);
    void send(Controller* controller, const QByteArray& frame);

  signals:
    void frameRendered(const LegacyControllerMapping::ScreenInfo& screeninfo,
            QImage frame,
            const QDateTime& timestamp);
    void engineSetupRequested(std::shared_ptr<QQmlEngine> engine);
    void stopping();
    /// @brief Request the screen thread to send a frame to the device
    /// @param controller the controller to send the frame to
    /// @param frame the frame data, ready to be sent
    void sendFrameDataRequested(Controller* controller, const QByteArray& frame);

  private:
    virtual void prepare();

    std::chrono::time_point<std::chrono::steady_clock> m_nextFrameStart;

    LegacyControllerMapping::ScreenInfo m_screenInfo;

    std::unique_ptr<QThread> m_pThread;

    std::unique_ptr<QOpenGLContext> m_context;
    std::unique_ptr<QOffscreenSurface> m_offscreenSurface;
    std::unique_ptr<QQuickRenderControl> m_renderControl;
    std::unique_ptr<QQuickWindow> m_quickWindow;

    std::unique_ptr<QOpenGLFramebufferObject> m_fbo;

    GLenum m_GLDataFormat;
    GLenum m_GLDataType;

    bool m_isValid;

    // These mutexes components are used to ensure internal object synchronicity
    QWaitCondition m_waitCondition;
    QMutex m_mutex;

    ControllerScriptEngineBase* m_pControllerEngine;
};
