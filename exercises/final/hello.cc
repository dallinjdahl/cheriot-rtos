// Copyright Microsoft and CHERIoT Contributors.
// SPDX-License-Identifier: MIT

#include <compartment.h>
#include <debug.hh>
#include <fail-simulator-on-error.h>
#include "run.h"

/// Expose debugging features unconditionally for this compartment.
using Debug = ConditionalDebug<true, "Hello world compartment">;

uint8_t mem[256] = {0, 1, 'o', 1, 'k', 1, '\n', 1, 3, 2, 5};
uint8_t bad[128] = {0, 1, 0, 7, 1, 'o', 1, 'k', 1, 2, 2, 5};

void __cheri_compartment("hello") say_hello()
{
	// Print hello world, along with the compartment's name to the default UART.
	Debug::log("Hello world");
	run(bad);
	run(mem);
}
