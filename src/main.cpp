#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "NtRtspApp.h"
#include <spdlog/spdlog.h>
#include "avcodec_utils.h"
#include "Observer.h"
#include "ThreadsafeQueue.h"
#include "NtMediaChannels.h"
#include "mioc/mioc.h"
#include "Pipeline.h"
#include "globs.h"
#include "NtFactoryDevice.h"
#include "NtChannelManager.h"

int main()
{
    auto container = getContainer();

    container->AddSingleton<INtFactoryDevice, NtFactoryDevice>();
    auto manager = std::make_shared<NtChannelManager>(std::make_shared<NtFactoryDevice>());
    // container->AddSingleton<INtChannelManager, NtChannelManager, INtFactoryDevice>();
    container->AddSingleton<INtChannelManager>(manager);
    container->AddSingleton<IDeviceManager>(manager);
    container->AddSingleton<IObserverEvent, ObserverEventSource>();
    container->AddSingleton<IWebServer, WebServer, IObserverEvent>();
    container->AddSingleton<INtRtspApp, NtRtspApp, IObserverEvent, INtChannelManager, IDeviceManager>();

    // spdlog::default_logger()->set_level(spdlog::level::debug);
    set_external_avlogger(spdlog::default_logger());

    // just for test new Dummy device
    {
        auto newChannel = container->Resolve<INtChannelManager>()->addChannel();
        newChannel.type = ChannelSourceType::Dummy;
        newChannel.enable = true;
        auto isUpdate = container->Resolve<INtChannelManager>()->updateChannel(newChannel);
    }
    // {
    //     auto newChannel = container->Resolve<INtChannelManager>()->addChannel();
    //     newChannel.type = ChannelSourceType::Dummy;
    //     newChannel.enable = true;
    //     newChannel.metadata["width"] = "640";
    //     auto isUpdate = container->Resolve<INtChannelManager>()->updateChannel(newChannel);
    // }

    container->Resolve<INtRtspApp>()->run();
    container->Resolve<IWebServer>()->run();
}