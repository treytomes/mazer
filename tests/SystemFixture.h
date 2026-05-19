#pragma once

#include "SYSTEM/SYSTEM.h"
#include "OEM437.h"
#include <cstdlib>
#include <cstring>

// RAII guard that allocates SYSTEM::memory with the OEM437 font loaded,
// satisfying drawChar without running SDL_Init.
struct SystemFixture
{
    SystemFixture()
    {
        if (SYSTEM::memory == nullptr)
        {
            SYSTEM::memory = static_cast<uint8_t*>(calloc(SYSTEM::MEM_SIZE, 1));
            for (int i = 0; i < 256 * 8; i++)
                SYSTEM::memory[SYSTEM::MEM_ASCII + i] = OEM437::DATA[i];
        }
    }

    ~SystemFixture()
    {
        // Don't free here — tests share the same process-global memory.
        // Catch2 allocates all fixtures before teardown, so leave it alive.
    }
};
