#pragma once
#include <map>
#include <string>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <shared_mutex>

using namespace std;

#define UPLOADPATH "UploadPath"

namespace global
{
    class GlobalDictionary
    {
    private:
        class Impl
        {
        public:
            std::unordered_map<std::string, std::string> data;
            std::shared_mutex mutex;
        };

        std::unique_ptr<Impl> impl;

        GlobalDictionary() : impl(std::make_unique<Impl>()) {}
        ~GlobalDictionary() = default;

    public:
        GlobalDictionary(const GlobalDictionary &) = delete;
        GlobalDictionary &operator=(const GlobalDictionary &) = delete;

        static GlobalDictionary &getInstance();

        std::string operator[](const std::string &key) const
        {
            std::shared_lock<std::shared_mutex> lock(impl->mutex);
            auto it = impl->data.find(key);
            if (it != impl->data.end())
                return it->second;

            static const std::string emptyString;
            return emptyString;
        }

        std::string operator[](const char *key) const
        {
            std::shared_lock<std::shared_mutex> lock(impl->mutex);
            auto it = impl->data.find(key);
            if (it != impl->data.end())
                return it->second;

            static const std::string emptyString;
            return emptyString;
        }

        std::string &operator[](const std::string &key)
        {
            std::unique_lock<std::shared_mutex> lock(impl->mutex);
            return impl->data[key];
        }

        std::string &operator[](const char *key)
        {
            std::unique_lock<std::shared_mutex> lock(impl->mutex);
            return impl->data[key]; // Implicit conversion from const char* to std::string
        }

        bool contains(const std::string &key) const
        {
            std::shared_lock<std::shared_mutex> lock(impl->mutex);
            return impl->data.count(key) > 0;
        }

        void erase(const std::string &key)
        {
            std::unique_lock<std::shared_mutex> lock(impl->mutex);
            impl->data.erase(key);
        }
    };
};

extern global::GlobalDictionary &getGlobalDictionary();