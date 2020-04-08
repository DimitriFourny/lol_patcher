#pragma once

#include <stdio.h>

#ifndef NO_DEBUG
#define debug_printf(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug_printf while (0)
#endif