#Arduino Uploader V2.0
#Automagically download required libs, compile ino then upload the binary to an Arudino
#Cal.W 2020

<#
1) If wanted attempt to pull from git
2) Download CLI
    2.1) Extract Zip
3) Download Libs
4) Grab Board Info for uploader #[TODO] Support Mulitple Baords
    4.1) Grab Board Type and Port
5) Run Prebuild
6) Upload / Verifiy
#>

#Extra Compiler Args
##Printf Command Args
$printfCMDArgs = @{
    CMD = "--pref compiler.c.elf.extra_flags="
    Default = $null;
    Full = "-Wl,-u,vfprintf -lprintf_flt";
    Minimal = "-Wl,-u,vfprintf -lprintf_min"
}

##MacroExspansion Command Args
$macroExspansionArgs = @{
    CMD = "--pref compiler.cpp.extra_flags="
    Default = $null; 
    Full = "ftrack-macro-expansion=2";
    Minimal = "ftrack-macro-expansion=1";
    None = "ftrack-macro-expansion=0"
}

##Diagnostic Caret Command Args
$showDiagCaretArgs = @{
    CMD = "--pref compiler.cpp.extra_flags=";
    Default = $null;
    None = "-fno-diagnostics-show-caret"
}

##Upload Args
$uplodeArg = @('--verify', '--upload')

##Standard Choices
$Choices = [System.Management.Automation.Host.ChoiceDescription[]] @("&Yes", "&No", "&Cancel")



#Setup User Vars
$inoFile = "./main.ino"
$preBuildCommand = "python ./genHeaders.py"
$includedLibs = @() #@('Adafruit BMP280 Library', 'SdFat')

$doGitPull = $false
$doPreBuild = $true

$doUpload = $true
$waitForUser = $false
$dieOnPreBuildFail = $false

$printfOpt = "Full"; $macroExpOpt = "Default"; $caretOpt = "None"

#Setup Program Vars
$idePath = "C:\Program Files (x86)\Arduino\"
$libPath = [System.Environment]::GetEnvironmentVariable('userprofile')+"\Documents\Arduino\libraries"

if ([System.Environment]::Is64BitOperatingSystem) {$arch = "64"} else {$arch = "32"}
$cliPath = "./arduino-cli"
$cliURL  = "https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_"+$arch+"bit.zip"
$cliZipName = "arduino-cli.zip"

$serialPort = $null
$boardType = $null

#1) If wanted, check if git is installed then attempt to update.
if ($doGitPull){
    if (Get-Command "git" -ErrorAction SilentlyContinue) {
        Write-Output "Git installed, checking if repo out of date..."
        #Write-Output "Updating remote...."
        $pGitChange = Start-Process git -ArgumentList "diff origin/master --quiet" -wait -NoNewWindow -PassThru
        if ($pGitChange.ExitCode -eq 1){
            Write-Output "Diffrence in remote and local branch detected!"
            Write-Output "Here is what differs:"
            Start-Process git -ArgumentList "--no-pager diff origin/master --color-words" -wait -NoNewWindow

            Write-Output "You have the option to burn any local changes or ignore any changes made on the remote side."
            Write-Output "If you select [C]ancel then $inoFile will not be uploaded, allowing for a manual backup."
            Write-Output "WARNING: Selecting [Y]es WILL deleted anything you have done. As git tells you, you should stash any changes you make."
            switch ($host.UI.PromptForChoice(
                        "", "Would you like to pull the latest version removing any LOCAL changes?",
                        $Choices, 1)
                    ) {
                0 {
                    Write-Output "Nuking Local changes...."
                    Start-Process git -ArgumentList "reset --hard" -wait -NoNewWindow
                    Start-Process git -ArgumentList "pull" -wait -NoNewWindow
                    Write-Output "All local changes reset!"
                }
                1 { Write-Output "Ignoring Remote changes....." }
                2 {
                    Write-Output "Canceling upload!"
                    Write-Output "You need to upload the code to the Arduino before any changes you made will be present!"
                    if ($waitForUser) { Read-Host 'Press Enter to continue' }
                    exit
                }
            }
        } else {
            Write-Output "Runing on the latest verison - I hope"
        }
    } else {
        Write-Output "Git is not installed. You have to manually update this folder..."
    }
} else {
    Write-Output "Not attemping to download latest version from git!"
}

#2.0) Download the CLI if Needed
if (-not (Test-Path $cliPath)){
    Write-Output "Arduino CLI not present. Downloading...."
    New-Item -Path $cliPath -ItemType Directory
    Invoke-WebRequest $cliURL -OutFile "$cliPath/$cliZipName"
    Write-Output "CLI Downloaded. Extracting...."
    Expand-Archive -Force "$cliPath/$cliZipName" $cliPath
    Write-Output "CLI Extracted. Cleaning Up...."
    Remove-Item "$cliPath/$cliZipName"
    Write-Output "Arduino-CLI Downloaded and Setup!"
}

#3) Install Libs
Write-Output "Checking libs present..."
foreach ($lib in $includedLibs){
    $libPathName = $lib -replace " ", "_"
    if (-not (Test-Path "$libPath`\$libPathName")) {
        Write-Output "Installing $lib..."
        Start-Process "$cliPath/arduino-cli.exe" -ArgumentList "lib install `"$lib`"" -wait -NoNewWindow
    } else {
        Write-Output "$lib already installed."
    }
}

#4) Grab board info
#[TODO] Force board selection
Write-Output "Detecting boards..."
#blach who desgined this function???
$procStartInfo = New-object System.Diagnostics.ProcessStartInfo 
$procStartInfo.CreateNoWindow = $true 
$procStartInfo.UseShellExecute = $false 
$procStartInfo.RedirectStandardOutput = $true 
$procStartInfo.RedirectStandardError = $true 
$procStartInfo.FileName = "$cliPath`/arduino-cli.exe"
$procStartInfo.Arguments = @("board list", "--format json") 
$process = New-Object System.Diagnostics.Process 
$process.StartInfo = $procStartInfo 
[void]$process.Start()
$output = $process.StandardOutput.ReadToEnd() 
$process.WaitForExit()

$boards = $output | ConvertFrom-Json
if ($null -eq $boards[0].address){
    Write-Output "No Arduino found! Only going to verifiy the program."
    $doUpload = $false
} else {
    Write-Output ("Found a " + $boards[0].boards.name + " on " + $boards[0].address)
    $serialPort = "--port " + $boards[0].address
    $boardType = "--board " + $boards[0].boards.FQBN;
}

#5) Run PreBuild
if ($doPreBuild){
    $pbSplit = $preBuildCommand.split(" ")
    $cmdHead = $pbSplit[0]
    $cmdArgs = [String]::Join(" ", $pbSplit[1..($pbSplit.Length-1)])
    if (Get-Command $cmdHead -ErrorAction SilentlyContinue) {
        Write-Output "Attemping to exicute prebuild command: `"$preBuildCommand`""
        Write-Debug "Exicuting as '$cmdHead' '$cmdArgs'"
        $preBuildProc = Start-Process $cmdHead -ArgumentList $cmdArgs -wait -NoNewWindow  -PassThru
        if ($preBuildProc.ExitCode -ne 0){
            Write-Warning ":( Something failed in the preBuild script!"
            if ($dieOnPreBuildFail){ throw "PreBuild Command Failed!"}
        }
    } else {
        Write-Warning "Could not exicute prebuild command: `"$preBuildCommand`""
    }
}

#6) Uploading / Verifiying
if ($doUpload) { Write-Output "Attempting to Upload $inoFile...." } 
    else { Write-Output "Starting verification of $inoFile" }

$argList = ""

if ($null -ne $printfCMDArgs[$printfOpt])
    {$argList += ($printfCMDArgs["CMD"] + '"' + $printfCMDArgs[$printfOpt] + "`" ")}
if ($null -ne $macroExspansionArgs[$macroExpOpt]) 
    {$argList += ($macroExspansionArgs["CMD"] + '"' + + $macroExspansionArgs[$macroExpOpt] + "`" ")}
if ($null -ne $showDiagCaretArgs[$caretOpt])
    {$argList += ($showDiagCaretArgs["CMD"] + '"' + $showDiagCaretArgs[$caretOpt] + "`" ")}

$argList += "$serialPort $boardType "
$argList += $uplodeArg[$doUpload] + " $inoFile"

$arduinoDebug = Start-Process "$idePath`\arduino_debug.exe" -ArgumentList $argList -wait -NoNewWindow -PassThru

if ($arduinoDebug.ExitCode -ne 0){
    Write-Warning ":( Something went very wrong when uploading!"
} else {
    Write-Host "Done!" -ForegroundColor Green
}

if ($waitForUser) { Read-Host 'Press Enter to continue' }
exit
