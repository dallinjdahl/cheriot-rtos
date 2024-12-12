#include "cdefs.h"
#include "microvium-ffi.hh"
#include "secret.h"
#include <compartment.h>
#include <cstdint>
#include <cstdlib>
#include <debug.hh>
#include <riscvreg.h>
#include <type_traits>
#include <vector>
#include "exec.h"

// Expose debugging features unconditionally for this compartment.
using Debug = ConditionalDebug<true, "Javascript compartment">;
using CHERI::Capability;
enum ErrorRecoveryBehaviour
compartment_error_handler(struct ErrorState *frame,
                          size_t             mcause,
                          size_t             mtval) {
    heap_free_all(MALLOC_CAPABILITY);
    return ForceUnwind;
}

void execute(std::vector<uint8_t> bytecode)
{
    Debug::log("{} bytes of heap available",
		           heap_quota_remaining(MALLOC_CAPABILITY));
	// Allocate the space for the VM capability registers on the stack and
	// record its location.
	// **Note**: This must be on the stack and in same compartment as the
	// JavaScript interpreter, so that the callbacks can re-derive it from
	// csp.
	AttackerRegisterState state;
	attackerRegisterStateAddress = Capability{&state}.address();

	mvm_TeError                         err;
	std::unique_ptr<mvm_VM, MVMDeleter> vm;
	// Create a Microvium VM from the bytecode.
	{
		mvm_VM *rawVm;
		err = mvm_restore(
		  &rawVm,            /* Out pointer to the VM */
		  bytecode.data(),   /* Bytecode data */
		  bytecode.size(),   /* Bytecode length */
		  MALLOC_CAPABILITY, /* Capability used to allocate memory */
		  ::resolve_import); /* Callback used to resolve FFI imports */
		// If this is not valid bytecode, give up.
		Debug::Assert(
		  err == MVM_E_SUCCESS, "Failed to parse bytecode: {}", err);
		vm.reset(rawVm);
	}

	// Get a handle to the JavaScript `run` function.
	mvm_Value run;
	err = mvm_resolveExports(vm.get(), &ExportRun, &run, 1);
	if (err != MVM_E_SUCCESS)
	{
		Debug::log("Failed to get run function: {}", err);
	}
	else
	{
		// Call the function:
		err = mvm_call(vm.get(), run, nullptr, nullptr, 0);
		if (err != MVM_E_SUCCESS)
		{
			Debug::log("Failed to call run function: {}", err);
		}
	}
}