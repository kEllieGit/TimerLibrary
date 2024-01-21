#include <Core.h>
#include <Misc/AutomationTest.h>
#include "Tests/AutomationEditorCommon.h"
#include "Timers/Public/Timer.h"

BEGIN_DEFINE_SPEC(TimerTests, "Timer", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
    // Test Variabls
    TUniquePtr<Timers::FTimer> Timer;
    TArray<TUniquePtr<Timers::FTimer>> HelperTimers;
    UWorld* World = nullptr;
    int Loops = 0;

    // Times
    const FTimespan Timeout = FTimespan(0, 1, 0);
    /// @brief Time a timer should run for testing
    const float TestTime = 1;
    /// @brief Creates a timer that runs indefinitly (max length is the timeout of the test itself)
    const float TestTimeInfinite = Timeout.GetTotalSeconds();
    /// @brief Max allowed deviation of the requested time
    const float TimeTolerance = 0.2;

    // Helpers
    template<typename TFunc>
    void DoAfter(float time, TFunc func) 
    {
        HelperTimers.Add(MakeUnique<Timers::FTimer>(World, time, false, func));
    }
END_DEFINE_SPEC(TimerTests)

void TimerTests::Define()
{
    World = FAutomationEditorCommonUtils::CreateNewMap();

    Describe("Callbacks", [this]()
        {
            LatentIt("should call TDelegate<>", Timeout, [this](const FDoneDelegate TestDone)
                {
                    auto TestLambda = FTimerDelegate::CreateLambda([this, TestDone]() {
                        TestEqual("Time Elapsed", Timer->GetTimeElapsed(), TestTime, TimeTolerance);
                        TestDone.Execute();
                        });

                    Timer = MakeUnique<Timers::FTimer>(World, TestTime, true, TestLambda);
                    TestTrue("Callback Bound", TestLambda.IsBound());
                });            
            LatentIt("should call Lambda", Timeout, [this](const FDoneDelegate TestDone)
                {
                    Timer = MakeUnique<Timers::FTimer>(World, TestTime, true, [this, TestDone]() 
                        {
                            TestEqual("Time Elapsed", Timer->GetTimeElapsed(), TestTime, TimeTolerance);
                            TestDone.Execute();
                        }
                    );
                });
        });

    LatentIt("should return elapsed/remaining time", Timeout, [this](const FDoneDelegate TestDone)
        {
            Timer = MakeUnique<Timers::FTimer>(World, TestTimeInfinite, false);
            DoAfter(TestTime,[this, TestDone]() 
                {
                    TestEqual("Time Elapsed", Timer->GetTimeElapsed(), TestTime, TimeTolerance);
                    TestEqual("Time Remaining", Timer->GetTimeRemaining(), TestTimeInfinite - TestTime, TimeTolerance);
                    TestDone.Execute();
                }
            );
        }
    );

    LatentIt("should loop", Timeout, [this](const FDoneDelegate TestDone)
        {
            Loops = 0;
            Timer = MakeUnique<Timers::FTimer>(World, TestTime, true, [this]()
                {
                    Loops++;
                });
            DoAfter(TestTime * 2.5, [this, TestDone]() // We want to be abit slower, so that the timer has a chance to complete the 2nd loop
                {
                    TestEqual("Loops", Loops, 2);
                    TestDone.Execute();
                });
        });

    Describe("Pausing",[this]()
        {
            LatentIt("should pause", Timeout, [this](const FDoneDelegate TestDone)
                {
                    // Run -TestTime-> Pause -TestTime-> Check
                    // Expected TimerTime = TestTime
                    Timer = MakeUnique<Timers::FTimer>(World, TestTimeInfinite, false);
                    DoAfter(TestTime, [this, TestDone]()
                        {
                            TestTrue("Timer Active Before Pause", Timer->IsActive());
                            Timer->Pause();
                            TestFalse("Timer Active After Pause",Timer->IsActive());
                        }
                    );
                    DoAfter(TestTime * 2, [this, TestDone]()
                        {
                            TestFalse("Timer Active After Pause 2", Timer->IsActive());
                            TestEqual("Time Elapsed", Timer->GetTimeElapsed(), TestTime, TimeTolerance);
                            TestDone.Execute();
                        }
                    );
                }
            );            
            LatentIt("should resume", Timeout, [this](const FDoneDelegate TestDone)
                {
                    // Run --> Pause -TestTime-> Resume -TestTime-> Check
                    // Expected TimerTime = TestTime
                    Timer = MakeUnique<Timers::FTimer>(World, TestTimeInfinite, false);
                    Timer->Pause();
                    DoAfter(TestTime, [this, TestDone]()
                        {
                            TestFalse("Timer Active Before Resume", Timer->IsActive());                        
                            Timer->Resume();
                            TestTrue("Timer Active After Resume", Timer->IsActive());
                        }
                    );
                    DoAfter(TestTime * 2, [this, TestDone]()
                        {
                            TestTrue("Timer Active After Resume 2", Timer->IsActive());
                            TestEqual("Time Elapsed", Timer->GetTimeElapsed(), TestTime,TimeTolerance);
                            TestDone.Execute();
                        }
                    );
                }
            );
        });
}