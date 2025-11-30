# Adventure Game (Console)

Minimal console text-adventure project initialized for Windows (MinGW), CMake and Boost.

Build & run (PowerShell, MinGW-w64):

1. Create a build directory and run CMake (use the "MinGW Makefiles" generator):

```powershell
mkdir build; cd build
cmake -G "MinGW Makefiles" .. -DBOOST_ROOT="C:/path/to/boost"
mingw32-make
```

2. Run the executable:

```powershell
./adventure_game.exe
```

Notes:
- If CMake cannot find Boost, set the `BOOST_ROOT` environment variable or pass `-DBOOST_ROOT=` on the `cmake` command line.
- Example: `-DBOOST_ROOT=C:/local/boost_1_80_0`.
- This project uses `Boost.Filesystem` to demonstrate linking a Boost component; you can add other components in `CMakeLists.txt`.

Enjoy — type `help` when the game starts.

Save & Load
- Type `save` at the prompt to write the current score to `saves/lastsave.txt`.
- Type `load` to restore the score from the last save file (if present).

Quick build helper (PowerShell)
 - Use the included `build.ps1` script to configure and build with MinGW/CMake.

```powershell
# run from project root; optionally pass Boost path
.\build.ps1 -BoostRoot 'C:/local/boost_1_80_0'
```
# adventure-game