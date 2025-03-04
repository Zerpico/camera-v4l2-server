#include "globs.h"

global::GlobalDictionary &global::GlobalDictionary::getInstance()
{
    static GlobalDictionary instance;
    return instance;
}

// Определение функции, возвращающей экземпляр
global::GlobalDictionary &getGlobalDictionary()
{
    return global::GlobalDictionary::getInstance();
}
