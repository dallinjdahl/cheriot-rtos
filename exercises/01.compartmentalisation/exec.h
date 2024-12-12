#pragma once
#include <compartment.h>

void __cheri_compartment("ex") execute(std::vector<uint8_t> bytecode);