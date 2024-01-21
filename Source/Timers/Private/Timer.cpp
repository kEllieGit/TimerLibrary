#include "Timer.h"

Timers::FTimer::~FTimer()
{
    if (Handle.IsValid() && World != nullptr && &World->GetTimerManager() != nullptr)
    {
        World->GetTimerManager().ClearTimer(Handle);
    }
}

void Timers::FTimer::Pause()
{
    if (!World->GetTimerManager().IsTimerPaused(Handle))
    {
        World->GetTimerManager().PauseTimer(Handle);
    }
}

void Timers::FTimer::Resume()
{
    if (World->GetTimerManager().IsTimerPaused(Handle))
    {
        World->GetTimerManager().UnPauseTimer(Handle);
    }
}

void Timers::FTimer::SetTime(float newTimeInSeconds)
{
    // Clear the old handle and make a new one with the set time.
    // This is stupid but unreal doesn't allow us to
    // edit the internal time of timers so I have to do this idiocy.
    World->GetTimerManager().ClearTimer(Handle);

    if (Callback.IsSet())
    {
        Visit([this, newTimeInSeconds](auto& callback)
            {
                World->GetTimerManager().SetTimer(Handle, callback, newTimeInSeconds, Loop);
            }, Callback.GetValue());
    }
    else
    {
        World->GetTimerManager().SetTimer(Handle, newTimeInSeconds, Loop);
    }
}

float Timers::FTimer::GetTimeElapsed() const
{
    return World->GetTimerManager().GetTimerElapsed(Handle);
}

float Timers::FTimer::GetTimeRemaining() const
{
    return World->GetTimerManager().GetTimerRemaining(Handle);
}

bool Timers::FTimer::IsActive() const
{
    return World->GetTimerManager().IsTimerActive(Handle);
}

bool Timers::FTimer::IsValid() const
{
    return World->GetTimerManager().TimerExists(Handle);
}
