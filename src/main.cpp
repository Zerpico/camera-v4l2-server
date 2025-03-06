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

int main()
{
    auto container = getContainer();
    container->AddSingleton<INtMediaChannels, NtMediaChannels>();
    container->AddSingleton<CDispatcherBase, CDispatcher>();
    container->AddSingleton<IPipeline, Pipeline, INtMediaChannels>();
    container->AddSingleton<IWebServer, WebServer, CDispatcherBase>();
    container->AddSingleton<INtRtspApp, NtRtspApp, CDispatcherBase>();
    set_external_avlogger(spdlog::default_logger());

    // just for test new Dummy device
    // {
    //     auto newChannel = container->Resolve<INtMediaChannels>()->addChannel();
    //     newChannel.type = ChannelSourceType::Dummy;
    //     newChannel.enable = true;
    //     auto isUpdate = container->Resolve<INtMediaChannels>()->updateChannel(newChannel);
    // }
    {
        auto newChannel = container->Resolve<INtMediaChannels>()->addChannel();
        newChannel.type = ChannelSourceType::Dummy;
        newChannel.enable = true;
        newChannel.metadata["width"] = "640";
        auto isUpdate = container->Resolve<INtMediaChannels>()->updateChannel(newChannel);
    }

    container->Resolve<INtRtspApp>()->run();
    container->Resolve<IWebServer>()->run();
}