#pragma once
#include <compartment.h>

void __cheri_compartment("run") run(uint8_t *bytecode);
