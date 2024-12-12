// Copyright Microsoft and CHERIoT Contributors.
// SPDX-License-Identifier: MIT

#include <compartment.h>
#include <debug.hh>
#include <fail-simulator-on-error.h>
#include "run.h"

/// Expose debugging features unconditionally for this compartment.
using Debug = ConditionalDebug<true, "run compartment">;


uint8_t stack[32] = {0};
uint8_t rstack[32] = {0};

uint8_t *ip = nullptr;
uint8_t sp = 0;
uint8_t rsp = 0;
uint8_t done = 0;
uint8_t reg = 0;

void exec(uint8_t op, uint8_t *bytecode) {
	switch (op) {
		case 0:
			ip++;
			return;
		case 1:
			ip++;
			stack[sp++] = *(ip++);
			return;
		case 2:
			ip++;
			reg = stack[--sp];
			stack[sp] = 0;
			sp -= reg;
			Debug::log("{}", (const char *)(stack + sp));
			stack[sp] = 0;
			return;
		case 3:
			ip++;
			rstack[rsp++] = ip - bytecode;
			ip = bytecode + stack[--sp];
			stack[sp] = 0;
			return;
		case 4:
			ip = bytecode + rstack[--rsp];
			return;
		case 5:
			done = 1;
			return;
		case 6:
			ip++;
			((void (*)())(bytecode + stack[--sp]))();
						Debug::log("here");

			stack[sp] = 0;
			return;
		case 7:
			reg = *((volatile uint8_t *)0);
		default:
			ip++;
			return;
	}
}

void run(uint8_t *bytecode) {
	ip = bytecode;
	done = 0;
	while(!done) {
		Debug::log("{} {}", (unsigned int)(ip - bytecode), *ip);
		exec(*ip, bytecode);
	}
}