#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "NtRtspApp.h"
#include "NtDummyVideoDevice.h"
#include <spdlog/spdlog.h>
#include "avcodec_utils.h"
#include "Observer.h"
#include "ThreadsafeQueue.h"
#include "NtMediaChannels.h"
#include "mioc/mioc.h"
#include "Pipeline.h"

int main()
{
    // init container and services
    auto container = getContainer();
    container->AddSingleton<INtMediaChannels, NtMediaChannels>();
    container->AddSingleton<CDispatcherBase, CDispatcher>();
    container->AddSingleton<IPipeline, Pipeline, INtMediaChannels>();
    container->AddSingleton<IWebServer, WebServer, CDispatcherBase>();
    container->AddSingleton<INtRtspApp, NtRtspApp, CDispatcherBase>();
    set_external_avlogger(spdlog::default_logger());

    // just for test
    // {
    //     auto channels = container->Resolve<INtMediaChannels>();
    //     channels->addChannel("hello");
    //     channels->addChannel("hello-world");
    // }

    container->Resolve<INtRtspApp>()->Start();
    container->Resolve<IWebServer>()->run();
}