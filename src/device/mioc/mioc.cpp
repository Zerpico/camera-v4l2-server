#include "mioc.h"

extern mioc::ServiceContainerPtr getContainer()
{
    return mioc::SingletonContainer::GetContainer(false);
}