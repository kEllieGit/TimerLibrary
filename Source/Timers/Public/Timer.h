#pragma once

#include <Core.h>
#include <Templates/UnrealTemplate.h>
#include <concepts>

namespace Timers
{

    /// @brief Simple timer that calls the provided callback after a time period.
    class TIMERS_API FTimer
    {
        using CallbackVariant = TVariant<FTimerDelegate, FTimerDynamicDelegate>;

    public:
        /// @brief Initializes this timer without a Callback.
        /// @param World context required for the timer to function.
        /// @param Delay (in seconds) until we call the provided callback.
        /// @param If this timer should loop or not.
        FTimer(UWorld* InWorld, float delay, bool loop)
            : World{ InWorld }, Delay{ delay }, Loop{ loop }, Callback {  }
        {
            check(InWorld != nullptr);

            World->GetTimerManager().SetTimer(Handle, Delay, Loop);
        };

        /// @brief Initializes this timer with a FTimerDelegate Callback.
        /// @param World context required for the timer to function.
        /// @param Delay (in seconds) until we call the provided callback.
        /// @param If this timer should loop or not.
        /// @param Callback to call when this timer is finished.
        FTimer(UWorld* InWorld, float delay, bool loop, FTimerDelegate callback)
            : World{ InWorld }, Delay{ delay }, Loop{ loop }, Callback{ InPlace, TInPlaceType<FTimerDelegate>(), MoveTemp(callback) }
        {
            check(InWorld != nullptr);

            World->GetTimerManager().SetTimer(Handle, Callback.GetValue().Get<FTimerDelegate>(), Delay, Loop);
        };

        /// @brief Initializes this timer with a FTimerDynamicDelegate Callback.
        /// @param World context required for the timer to function.
        /// @param Delay (in seconds) until we call the provided callback.
        /// @param If this timer should loop or not.
        /// @param Callback to call when this timer is finished.
        FTimer(UWorld* InWorld, float delay, bool loop, FTimerDynamicDelegate callback)
            : World{ InWorld }, Delay{ delay }, Loop{ loop }, Callback{ InPlace, TInPlaceType<FTimerDynamicDelegate>(), MoveTemp(callback) }
        {
            check(InWorld != nullptr);

            World->GetTimerManager().SetTimer(Handle, Callback.GetValue().Get<FTimerDynamicDelegate>(), Delay, Loop);
        };

        /// @brief Initializes this timer with a functor as a callback
        /// @param World context required for the timer to function.
        /// @param Delay (in seconds) until we call the provided callback.
        /// @param If this timer should loop or not.
        template<typename TCallback>
        FTimer(UWorld* InWorld, float delay, bool loop, TCallback callback)
            : FTimer(InWorld,delay,loop, FTimerDelegate::CreateLambda(callback))
        {};

        /// @todo https://docs.unrealengine.com/5.0/en-US/API/Runtime/Engine/FTimerManager/SetTimer/5/
        /// @todo https://docs.unrealengine.com/5.0/en-US/API/Runtime/Engine/FTimerManager/SetTimer/6/

        ~FTimer();

        /// @brief Pauses the timer.
        void Pause();

        /// @brief Resumes the timer if it's paused.
        void Resume();

        /// @brief Sets the delay time of this timer to the specified value.
        /// @param Delay (in seconds) until we call the callback.
        void SetTime(float newTimeInSeconds);

        /// @brief Returns the elapsed time since this timer's creation. If the timer has finished, returns -1.
        float GetTimeElapsed() const;

        /// @brief Returns the remaining time until the callback is called.
        float GetTimeRemaining() const;

        /// @brief Returns whether the timer is valid and not paused.
        bool IsActive() const;

        /// @brief Returns whether this timer exists in the TimerManager.
        bool IsValid() const;
    private:
        /// @brief The delay until the callback gets called when this timer is created.
        float Delay;

        /// @brief If this timer is set to loop.
        bool Loop;
            
        /// @brief Type of Callback used for this timer.
        TOptional<CallbackVariant> Callback;

        /// @brief Unreal handle for this timer to interface with TimerManager.
        FTimerHandle Handle;

        /// @brief World context required for the timer to function.
        UWorld* World;
    };
}
