-- Copyright Microsoft and CHERIoT Contributors.
-- SPDX-License-Identifier: MIT

set_project("CHERIoT Final Project")
sdkdir = "../../sdk"
includes(sdkdir)
set_toolchains("cheriot-clang")

option("board")
    set_default("sail")

compartment("hello")
    -- memcpy
    add_deps("freestanding", "debug")
    add_files("hello.cc")

compartment("run")
    add_files("run.cc")

-- Firmware image for the example.
firmware("hello_world")
    add_deps("hello")
    add_deps("run")
    on_load(function(target)
        target:values_set("board", "$(board)")
        target:values_set("threads", {
            {
                compartment = "hello",
                priority = 1,
                entry_point = "say_hello",
                stack_size = 0x200,
                trusted_stack_frames = 2
            }
        }, {expand = false})
    end)
