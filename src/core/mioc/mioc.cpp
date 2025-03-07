#include "mioc.h"

mioc::ServiceContainerPtr getContainer()
{
    return mioc::SingletonContainer::GetContainer(false);
}