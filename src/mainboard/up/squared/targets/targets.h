#ifndef TARGETS_H
#define TARGETS_H

#include <stdint.h>
#include <console/uart8250mem.h>
#include <commonlib/loglevel.h>

#include "../red_unlock.h"
#include "../lib-micro-x86/lib-micro-minimal.h"

#define REP10(BODY) BODY BODY BODY BODY BODY BODY BODY BODY BODY BODY
#define REP100(BODY) REP10(REP10(BODY))

inline static __attribute__((always_inline)) void target_loop(void* uart_base);

#endif
