#pragma once
#ifndef ARCHIE_H
#define ARCHIE_H

#if defined(_WIN32)
#include "archie_win.h"
#elif defined(__linux__)
#include "archie_linux.h"
#else
#error "Unknown compiler Operating System"
#endif

#endif // !ARCHIE_H
