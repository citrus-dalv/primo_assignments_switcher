#pragma once
#include <iostream>
#include <exception>
#include <cstdio>
#include <fstream>
#include <cerrno>

#ifdef _WIN32
#include <windows.h>
#endif

#include "signals.h"
#include "cmd.h"
#include "log.h"
#include "json.h"
#include "config.h"
#include "primoapi.h"