param(
  [string]$BuildExe = "$PSScriptRoot\..\build\adventure_game.exe"
)

if (-not (Test-Path $BuildExe)) {
  Write-Error "Executable not found: $BuildExe. Build the project first."
  exit 1
}

Write-Host "Running smoke test..."

$commands = @(
  "help",
  "score",
  "quit"
)

$temp = Join-Path $PSScriptRoot "commands.txt"
$commands -join "`n" | Set-Content -Path $temp -Encoding UTF8

Get-Content $temp | & $BuildExe

Remove-Item $temp -ErrorAction SilentlyContinue

Write-Host "Smoke test finished."
