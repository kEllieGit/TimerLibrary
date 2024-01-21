# UE5 C++ TimerLibrary
The Unreal Engine TimerManager uses an outdated C-Style way of handling Timers via handles. We can do things better and in a more modern C++ way. Instead of having to manage a whole bunch of redundant Timer Handles, we can use a singular object for each timer,
and then run whatever changes we want on that singular timer, without having to use handles at all!

That is the problem this plugin is intended to solve.
