#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#if !defined(NDEBUG) || NDEBUG == 0
#define LOG(formatter, ...) printf(formatter", %s, ln %d\n", __VA_ARGS__, __FUNCTION__, __LINE__)
#else
#define LOG(formatter, ...)
#endif

#endif  /* LOGGING_H */
