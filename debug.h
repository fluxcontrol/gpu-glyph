#pragma once

#include <stdio.h>

#define STRINGIFY_LOWLEVEL(X) #X
#define STRINGIFY(X) STRINGIFY_LOWLEVEL(X)
#ifdef DEBUG
#define DEBUG_ON 1
#else
#define DEBUG_ON 0
#endif
#define TRACE_ARGFIRST(...) TRACE_ARGFIRST0(__VA_ARGS__, 0)
#define TRACE_ARGFIRST0(X, ...) X
#define TRACE_ARGREMAIN(...) TRACE_ARGREMAIN0(__VA_ARGS__, 0)
#define TRACE_ARGREMAIN0(X, ...) __VA_ARGS__
#define TRACE_DEBUG(F, ...) \
	do { \
		if (DEBUG_ON)  \
			fprintf(stderr, "DEBUG: (%s)%s:" STRINGIFY(__LINE__) ": " F "\n", \
				__FILE__, \
				__func__, \
				__VA_ARGS__ \
			); \
	} while (0)
#define debug(...) TRACE_DEBUG(TRACE_ARGFIRST(__VA_ARGS__) "%.0d", TRACE_ARGREMAIN(__VA_ARGS__))
