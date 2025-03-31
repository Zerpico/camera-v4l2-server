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
#include <fstream>

// Function to display usage information
void printUsage()
{
    std::cout << "Usage: program_name [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -i <filename>  Specify the input filename.\n";
    std::cout << "  -d <value>     Specify a dummy value.\n";
    std::cout << "  -h             Display this help/usage information.\n";
}

// Function to parse command-line arguments
std::unordered_map<std::string, std::vector<std::string>> parseArguments(int argc, char **argv)
{
    std::unordered_map<std::string, std::vector<std::string>> arguments;

    for (int i = 1; i < argc; ++i)
    {
        std::string key = argv[i];

        if (key == "-h")
        { // Special case: handle help option immediately
            printUsage();
            exit(0);
        }

        if (key[0] == '-')
        { // Check if it's a key (starts with '-')
            if (i + 1 < argc)
            { // Check if there's a value after the key
                std::string value = argv[i + 1];
                if (value[0] != '-')
                { // check that the next param is not another key.
                    arguments[key].push_back(value);
                    ++i; // Skip the value in the next iteration
                }
                else
                {
                    std::cerr << "Error: Key '" << key << "' is missing a value." << std::endl;
                    printUsage();
                    exit(1);
                }
            }
            else
            {
                std::cerr << "Error: Key '" << key << "' is missing a value." << std::endl;
                printUsage();
                exit(1);
            }
        }
        else
        {
            std::cerr << "Error: Invalid argument '" << key << "'. Keys must start with '-'." << std::endl;
            printUsage();
            exit(1);
        }
    }

    return arguments;
}

// Function to check if a file exists
bool fileExists(const std::string &filename)
{
    std::ifstream file(filename);
    return file.good(); // Check if the file can be opened successfully
}

// Function to register all parsed channels in ChannelManager
int generateChannels(mioc::ServiceContainerPtr container, std::unordered_map<std::string, std::vector<std::string>> args)
{
    auto manager = container->Resolve<INtChannelManager>();
    // Access the arguments
    if (args.count("-i"))
    {
        std::vector<std::string> &filenames = args["-i"];
        for (const std::string &filename : filenames)
        {
            if (fileExists(filename))
            {
                // Use the filename
                auto newChannel = container->Resolve<INtChannelManager>()->addChannel();
                newChannel.type = ChannelSourceType::File;
                newChannel.enable = true;
                newChannel.source = filename;
                auto isUpdate = container->Resolve<INtChannelManager>()->updateChannel(newChannel);
            }
            else
            {
                std::cerr << "Error: File '" << filename << "' does not exist." << std::endl;
                return 1; // Exit with an error code
            }
        }
    }

    if (args.count("-d"))
    {
        std::vector<std::string> &dummyValues = args["-d"];
        for (const std::string &dummyValue : dummyValues)
        {
            auto newChannel = container->Resolve<INtChannelManager>()->addChannel();
            newChannel.type = ChannelSourceType::Dummy;
            newChannel.enable = true;
            auto isUpdate = container->Resolve<INtChannelManager>()->updateChannel(newChannel);
            // Use the dummy value
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    // Parse the command-line arguments
    std::unordered_map<std::string, std::vector<std::string>> args = parseArguments(argc, argv);

    // register all services in ioc
    auto container = getContainer();
    auto manager = std::make_shared<NtChannelManager>(std::make_shared<NtFactoryDevice>());
    container->AddSingleton<INtChannelManager>(manager);
    container->AddSingleton<IDeviceManager>(manager);
    container->AddSingleton<IObserverEvent, ObserverEventSource>();
    container->AddSingleton<IWebServer, WebServer, IObserverEvent>();
    container->AddSingleton<INtRtspApp, NtRtspApp, IObserverEvent, INtChannelManager, IDeviceManager>();

    // set external logger for ffmpeg
    set_external_avlogger(spdlog::default_logger());

    if (generateChannels(container, args) != 0)
        exit(1);

    {
        auto newChannel = container->Resolve<INtChannelManager>()->addChannel();
        newChannel.type = ChannelSourceType::File;
        newChannel.enable = true;
        newChannel.source = "E:\\testVideo\\shapes\\cut_files\\yellowobjects.mkv";
        auto isUpdate = container->Resolve<INtChannelManager>()->updateChannel(newChannel);
    }
    {
        auto newChannel = container->Resolve<INtChannelManager>()->addChannel();
        newChannel.type = ChannelSourceType::File;
        newChannel.enable = true;
        newChannel.source = "E:\\testVideo\\shapes\\cut_files\\circleobjects.mkv";
        ;
        auto isUpdate = container->Resolve<INtChannelManager>()->updateChannel(newChannel);
    }

    // start service and loop
    container->Resolve<INtRtspApp>()->run();
    container->Resolve<IWebServer>()->run();
}