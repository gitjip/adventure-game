param(
  [string]$BoostRoot = $env:BOOST_ROOT
)

if (-not $BoostRoot) {
  Write-Host "Note: BOOST_ROOT not provided and not set in environment."
  Write-Host "If CMake fails to find Boost, re-run with -BoostRoot 'C:/path/to/boost'"
}

$buildDir = "build"
if (-not (Test-Path $buildDir)) { New-Item -ItemType Directory -Path $buildDir | Out-Null }
Push-Location $buildDir

$generator = "MinGW Makefiles"
$sourceDir = ".."
$cmakeCmd = "cmake"
$cmakeParams = @("-G", $generator, $sourceDir)
if ($BoostRoot) { $cmakeParams += "-DBOOST_ROOT=$BoostRoot" }

Write-Host "Running: $cmakeCmd $($cmakeParams -join ' ')"
& $cmakeCmd @cmakeParams
if ($LASTEXITCODE -ne 0) { Write-Error "CMake failed."; Pop-Location; exit $LASTEXITCODE }

Write-Host "Building with mingw32-make..."
mingw32-make
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed."; Pop-Location; exit $LASTEXITCODE }

Pop-Location
Write-Host "Build complete. Run .\build\adventure_game.exe"
