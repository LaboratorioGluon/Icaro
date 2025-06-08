#pragma once

namespace Data
{

    enum class AppState
    {
        BOOTING = 0,
        INITIALIZING,
        FULL_POWER,
        POWER_SAVE,
        BEACON,
        RECOVERY,
        TOTAL_FAILURE,
    };

    enum class ThreadState
    {
        STARTING = 0,
        RUNNING,
        SLEEPING,
        STOPPED,
    };
        
}
