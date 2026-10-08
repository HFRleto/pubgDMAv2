# Recompile en Release avec symboles, lance le programme sous le debogueur Windows (cdb)
# pendant -Seconds secondes, puis affiche la pile d'appels en cas de plantage et la sortie console.
#
#   powershell -File tools\debug_run.ps1 [-Seconds 60] [-NoBuild]
param(
    [int]$Seconds = 60,
    [switch]$NoBuild
)

$root = Split-Path -Parent $PSScriptRoot
$rel = Join-Path $root 'x64\Release'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe'
$cdb = 'C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe'
$buildLog = Join-Path $env:TEMP 'kaka_build.log'
$runLog = Join-Path $env:TEMP 'kaka_cdb.log'

if (-not $NoBuild) {
    # la configuration Release ne genere pas de symboles : on les demande a l'editeur de liens
    $env:LINK = '/DEBUG:FULL'
    & $msbuild (Join-Path $root 'Jax Kane.sln') /p:Configuration=Release /p:Platform=x64 /m /nologo /v:m /clp:Summary 2>&1 |
        Out-File -Encoding utf8 $buildLog
    $env:LINK = $null
    $build = Get-Content $buildLog
    $build | Select-String -Pattern ' error ' | Select-Object -First 15 | ForEach-Object { $_.Line }
    $build | Select-String -Pattern 'Warning\(s\)|Error\(s\)|Elapsed' | ForEach-Object { $_.Line.Trim() }
    if ($LASTEXITCODE -ne 0) { 'COMPILATION ECHOUEE'; exit 1 }
}

$cdbArgs = @('-g', '-G', '-lines', '-y', $rel, '-c', '".echo ===CRASH===; .lastevent; kn 40; .echo ===END===; q"',
    (Join-Path $rel 'KAKA-PRO.exe'))
$p = Start-Process -FilePath $cdb -ArgumentList $cdbArgs -WorkingDirectory $rel -RedirectStandardOutput $runLog `
    -RedirectStandardError "$runLog.err" -PassThru -WindowStyle Hidden

if (-not $p.WaitForExit($Seconds * 1000)) {
    "RESULTAT : toujours en vie apres $Seconds s, pas de plantage"
    Get-Process KAKA-PRO -ErrorAction SilentlyContinue | Stop-Process -Force
    Start-Sleep 1
    if (-not $p.HasExited) { $p.Kill() }
}
else {
    "RESULTAT : arret avant $Seconds s"
}

$t = Get-Content $runLog -Encoding UTF8
$crash = ($t | Select-String -SimpleMatch '===CRASH===' | Select-Object -Last 1).LineNumber
if ($crash) {
    '--- plantage :'
    $t[($crash - 5)..([Math]::Min($t.Count - 1, $crash + 34))] |
        Where-Object { $_ -notmatch 'Inline Function' } |
        ForEach-Object { $_.Substring(0, [Math]::Min(210, $_.Length)) }
}
'--- sortie du programme :'
$t | Where-Object { $_ -match '\[\+\]|\[X\]|\[-\]' -and $_ -notmatch '=====|=    ' } |
    ForEach-Object { $_ -replace "\x1b\[[0-9;]*m", '' } |
    ForEach-Object { $_.Substring(0, [Math]::Min(300, $_.Length)) } |
    Group-Object | ForEach-Object { if ($_.Count -gt 1) { "(x$($_.Count)) $($_.Name)" } else { $_.Name } } |
    Select-Object -Last 40
