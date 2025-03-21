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
    container->AddSingleton<INtChannelManager, NtChannelManager, INtFactoryDevice>();

    container->AddSingleton<Observer, EventSource>();
    container->AddSingleton<IWebServer, WebServer, Observer>();
    container->AddSingleton<INtRtspApp, NtRtspApp, Observer>();
    set_external_avlogger(spdlog::default_logger());

    auto disp = container->Resolve<Observer>();

    // just for test new Dummy device
    {
        auto newChannel = container->Resolve<INtChannelManager>()->addChannel();
        newChannel.type = ChannelSourceType::Dummy;
        newChannel.enable = true;
        auto isUpdate = container->Resolve<INtChannelManager>()->updateChannel(newChannel);
    }
    // {
    //     auto newChannel = container->Resolve<INtMediaChannels>()->addChannel();
    //     newChannel.type = ChannelSourceType::Dummy;
    //     newChannel.enable = true;
    //     newChannel.metadata["width"] = "640";
    //     auto isUpdate = container->Resolve<INtMediaChannels>()->updateChannel(newChannel);
    // }

    // Создаем EventSource (реализация Observer)
    // auto eventSource = std::make_shared<EventSource>();

    // // Создаем Listeners
    // auto listener1 = std::make_shared<DataProcessor>("Processor 1");
    // auto listener2 = std::make_shared<DataProcessor>("Processor 2");

    // // Подписываем Listeners на EventSource
    // eventSource->subscribe(listener1);
    // eventSource->subscribe(listener2);

    // // Создаем EventGenerator
    // auto eventGenerator = std::make_shared<EventGenerator>(eventSource);

    // // Запускаем генерацию событий
    // eventGenerator->startGeneratingEvents();

    // // Даем системе немного времени для работы
    // std::this_thread::sleep_for(std::chrono::seconds(5));

    container->Resolve<INtRtspApp>()->run();
    container->Resolve<IWebServer>()->run();
}