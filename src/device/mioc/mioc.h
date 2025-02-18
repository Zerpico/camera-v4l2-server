// Copyright (C) 2023 - 2024 Tony's Studio. All rights reserved.

#pragma once

#ifndef _MIOC_H_
#define _MIOC_H_

#ifdef _WIN32
#define MYCONTAINER_API __declspec(dllexport)
#else
#define MYCONTAINER_API __declspec(dllimport)
#endif

#include "ServiceContainer.h"
#include "SingletonContainer.h"

extern MYCONTAINER_API mioc::ServiceContainerPtr getContainer();
#endif // _MIOC_H_
