#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "NtRtspApp.h"
#include "NtDummyVideoDevice.h"
#include <spdlog/spdlog.h>
#include "avcodec_utils.h"
#include "Observer.h"
#include "glob.h"

std::unique_ptr<CDispatcher> g_Dispatcher;

int main()
{
    g_Dispatcher = std::make_unique<CDispatcher>();

    auto web = new WebServer(g_Dispatcher.get());
    set_external_avlogger(spdlog::default_logger());
    auto rtsp = new NtRtspApp(g_Dispatcher.get());

    // channel.AddChannel(std::shared_ptr<NtDeviceInterface>(NtDummyVideoDevice::createNew(DummyVideoDeviceParameters{})));

    // for (int i = 1000; i < 1020; i++)
    //     g_Dispatcher->SendMessageLP(&i);

    // Sleep(100);

    rtsp->Start();
    web->run();
}