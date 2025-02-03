#include "CameraDeviceSource.h"
#include "WebServer.h"
#include "NtRtspApp.h"
#include "NtDummyVideoDevice.h"
#include <spdlog/spdlog.h>
#include "avcodec_utils.h"
#include "Observer.h"
#include "glob.h"
#include "ThreadsafeQueue.h"

std::unique_ptr<CDispatcher> g_Dispatcher;

int main()
{
    g_Dispatcher = std::make_unique<CDispatcher>();

    auto web = std::make_unique<WebServer>(g_Dispatcher.get());
    set_external_avlogger(spdlog::default_logger());
    auto rtsp = std::make_unique<NtRtspApp>(g_Dispatcher.get(), 8554);

    auto channel = std::shared_ptr<NtDeviceInterface>(NtDummyVideoDevice::createNew(DummyVideoDeviceParameters{}));
    channel->start();
    // rtsp->addSession

    int val = 5;
    auto f = &val;
    // auto queue = std::make_unique<ThreadsafeQueue<int>>(5);
    // ThreadsafeQueue<int> queue(5);
    // queue->push(val);
    // queue->push(val + 8);
    // std::shared_ptr<int> frame = queue->wait_and_pop();
    // std::shared_ptr<int> frame2 = queue->wait_and_pop();
    // rtsp->Start();
    web->run();
}