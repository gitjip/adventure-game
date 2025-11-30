# Adventure Game (Console)

> 中文译本: `README.zh-CN.md`

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

Troubleshooting Boost on MinGW
--------------------------------

If CMake fails to find Boost or you hit missing-header / undefined-reference errors when building with MinGW, follow these steps:

- Preferred: pass the MinGW Boost installation with `-DBOOST_ROOT=` to the `cmake` command or set the `BOOST_ROOT` environment variable.
	Example (PowerShell):

```powershell
cmake -G "MinGW Makefiles" .. -DBOOST_ROOT="D:/Users/damin/mingw64"
```

- Common MinGW layout: Boost headers live under `BOOST_ROOT/x86_64-w64-mingw32/include` and libraries under `BOOST_ROOT/lib`.
	If you installed Boost for MinGW-w64, CMake's automatic detection may point to a truncated path or try to use Boost "config" packages that are absent for some distributions. The included `CMakeLists.txt` and `build.ps1` try to handle this by:

	- Detecting and adding the triplet-prefixed include dir (e.g. `x86_64-w64-mingw32/include`).
	- Manually selecting filesystem library files from `BOOST_ROOT/lib` when CMake's config-mode lookup doesn't provide imported targets.

- Architecture mismatch: if your toolchain is x64, ensure the Boost libs you link are x64 variants (file names often include `x64` or `x32`). If you see linker "undefined reference" errors to Boost.Filesystem symbols, it's often because a 32-bit Boost library was picked while building with a 64-bit toolchain — install/use the matching Boost package or set `BOOST_ROOT` to the correct distribution.

- If you want to rely exclusively on Boost's CMake config mode (modern, `Boost::filesystem` targets), make sure your Boost installation provides per-component CMake config files (e.g. `boost_systemConfig.cmake`). Otherwise prefer the compatibility/manual detection approach used here.

If you still have trouble, paste the CMake configure output (`cmake` output) and the exact linker/compiler errors and I can help diagnose further.

## Development / Change Log

- `2025-12-01` — Default: `ENABLE_SCENELOADER` is now ON by default. This enables the `SceneLoader` implementation and its unit tests when you configure the project with CMake.

	- To disable the SceneLoader build (for example on constrained toolchains), pass the option when configuring:

		```powershell
		cmake -G "MinGW Makefiles" .. -DENABLE_SCENELOADER=OFF
		```

	- Tests including the SceneLoader test are executed by `ctest` (or `cmake --build build --target test` depending on your workflow). Example:

		```powershell
		# from project root
		cmake -S . -B build -G "MinGW Makefiles"
		cmake --build build --config Debug
		ctest --test-dir build -V
		```

	- Rationale: enabling the loader by default helps developers run the scene/event integration tests locally. If you encounter platform-specific toolchain issues (e.g., MinGW temp-file or antivirus interference), disable the loader temporarily as shown above.
