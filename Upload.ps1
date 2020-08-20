#Arduino Uploader V2.0
#Automagically download required libs, compile ino then upload the binary to an Arudino
#Cal.W 2020

<#
1) If wanted attempt to pull from git
2) Download CLI & IDE if needed :/
    2.1) Extract Zip
3) Download Libs
4) Grab Board Info for uploader #[TODO] Support Mulitple Baords
    4.1) Grab Board Type and Port
5) Run Prebuild
6) Upload / Verifiy
#>

param (
    [string]$jsonFilePath   = $PSScriptRoot + "/upload_config.json",
    [switch]$forceNoInput   = $false,
    [switch]$forceGitIgnore = $false,
    [switch]$forceGitNuke   = $false,
    [switch]$forceIniUpdate = $false,
    [switch]$generateJSON   = $false
)

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

##Verbosity
$verbosityArg = @{
    Default = $null;
    Full = "--verbose";
    Upload = "--verbose-upload";
    Build = "--verbose-build"
}

##Upload Args
$uplodeArg = @('--verify', '--upload')

##Standard Choices
$ChoiceYN =  [System.Management.Automation.Host.ChoiceDescription[]] @("&Yes", "&No")
$ChoiceYNC = [System.Management.Automation.Host.ChoiceDescription[]] @("&Yes", "&No", "&Cancel")

##Get the git branch
$gitBranch = ((git symbolic-ref --short HEAD) | Out-String).Trim()


#Setup User Vars
$settings = @{
    inoFile = "./main.ino";
    preBuildCommand = ""; #"python ./genHeaders.py";
    includedLibs = @(); # @('Adafruit BMP280 Library', 'SdFat', 'MPU6050');
    
    doGitPull = $true;
    doPreBuild = $true;
    
    doUpload = $true;
    waitForUser = $false;
    dieOnPreBuildFail = $false;
    
    verbosity = "Upload";
    
    printfOpt = "Full";
    macroExpOpt = "Default";
    caretOpt = "None";

    cacheZips = $true;
    
    askIniUpdate = $true;

    #Default Values - If this is null then the default value will be used :/
    buildPath = $null;
    libPath = $null;
    cliPath = $null;
    cliURL = $null;
    cliZipName = $null;
    cliJSONURL = $null;
    cliJSONPath = $null;
    globalIDEPath = $null;
    localIDEPath = $null;
    idePath = $null;
    ideZipName = $null;
    
    serialPort  = $null;
    boardType = $null;
}

if (Test-Path $jsonFilePath) {
    $jsonData = Get-Content $jsonFilePath | ConvertFrom-Json
    foreach ($kp in $jsonData.PSObject.Properties){
        if ($settings.ContainsKey($kp.Name) -And -not [string]::IsNullOrEmpty($kp.Value)){
            $settings[$kp.Name] =  $kp.Value
        }    
    }
} elseif ($generateJSON) {
    $settings | ConvertTo-Json -depth 32 | Out-File $jsonFilePath
}

#Will use "../.build/Folder_Name-1" if the current folder name is "Folder Name-1"
$buildPath = $(if(-not [string]::IsNullOrEmpty($settings["buildPath"])) {$settings["buildPath"]} Else {($PSScriptRoot + "\..\.build\" + ((Split-Path $PSScriptRoot -Leaf) -replace " ", "_"))})

#Setup Program Vars
$libPath = $(if(-not [string]::IsNullOrEmpty($settings["libPath"])) {$settings["libPath"]} Else {[System.Environment]::GetEnvironmentVariable('userprofile')+"\Documents\Arduino\libraries"})

$arch = $(if ([System.Environment]::Is64BitOperatingSystem) {"64"} else {"32"})

$cliPath = $(if(-not [string]::IsNullOrEmpty($settings["cliPath"])) {$settings["cliPath"]} Else {$PSScriptRoot+"/arduino-files/cli"})
$cliURL  = $(if(-not [string]::IsNullOrEmpty($settings["cliURL"])) {$settings["cliURL"]} Else {"https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_"+$arch+"bit.zip"})
$cliZipName = $(if(-not [string]::IsNullOrEmpty($settings["cliZipName"])) {$settings["cliZipName"]} Else {"arduino-cli.zip"})

$cliJSONURL = $(if(-not [string]::IsNullOrEmpty($settings["cliJSONURL"])) {$settings["cliJSONURL"]} Else {"https://downloads.arduino.cc/packages/package_index.json"})
$cliJSONPath = $(if(-not [string]::IsNullOrEmpty($settings["cliJSONPath"])) {$settings["cliJSONPath"]} Else {[System.Environment]::GetEnvironmentVariable('userprofile')+"\AppData\Local\Arduino15\package_index.json"})

$globalIDEPath = $(if(-not [string]::IsNullOrEmpty($settings["globalIDEPath"])) {$settings["globalIDEPath"]} Else {"C:\Program Files (x86)\Arduino\"})
$localIDEPath = $(if(-not [string]::IsNullOrEmpty($settings["localIDEPath"])) {$settings["localIDEPath"]} Else {$PSScriptRoot+"/arduino-files/ide"})

$idePath = $globalIDEPath
#Download the IDE if we can't find it on the system
if (-not (Test-Path "$globalIDEPath`\arduino_debug.exe") -Or (Test-Path "$localIDEPath`\arduino_debug.exe")){
     $idePath = $localIDEPath
}

$ideURL = $(if(-not [string]::IsNullOrEmpty($settings["ideURL"])) {$settings["ideURL"]} Else {"https://downloads.arduino.cc/arduino-1.8.13-windows.zip"})
$ideZipName = $(if(-not [string]::IsNullOrEmpty($settings["ideZipName"])) {$settings["ideZipName"]} Else {"arduino-ide.zip"})

$serialPort = $(if(-not [string]::IsNullOrEmpty($settings["serialPort"])) {$settings["serialType"]} Else {$null})
$boardType = $(if(-not [string]::IsNullOrEmpty($settings["boardType"])) {$settings["boardType"]} Else {$null})

if ($forceNoInput) { $settings["waitForUser"] = $false }

#1) If wanted, check if git is installed then attempt to update.
if ($settings["doGitPull"]){
    if (Get-Command "git" -ErrorAction SilentlyContinue) {
        Write-Output "Git installed, checking if current branch '$gitBranch' is out of date..."
        Write-Debug "Updating remote...."
        git fetch --quiet
        $pGitChange = Start-Process git -ArgumentList "diff origin/$gitBranch --quiet" -wait -NoNewWindow -PassThru
        if ($pGitChange.ExitCode -eq 1){
            Write-Output "Diffrence in remote and local branch detected!"
            Write-Output "Here is what differs:"
            Start-Process git -ArgumentList "--no-pager diff origin/$gitBranch --color-words" -wait -NoNewWindow

            Write-Output "You have the option to burn any local changes or ignore any changes made on the remote side."
            Write-Output ("If you select [C]ancel then " + $settings["inoFile"] + " will not be uploaded, allowing for a manual backup.")
            Write-Output "WARNING: Selecting [Y]es WILL deleted anything you have done. As git tells you, you should stash any changes you make."
            $gitOption = $(if ($forceGitNuke -Or $forceGitIgnore) {if ($forceGitIgnore) {1} else {0}} #Always take the non-nuke option
                            else {$host.UI.PromptForChoice(
                                    "", "Would you like to pull the latest version removing any LOCAL changes?",
                                    $ChoiceYNC, 1)}
                        )
            switch ($gitOption) {
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
                    if ($settings["waitForUser"]) { Read-Host 'Press Enter to continue' }
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

$tmpProgressPreference = $global:ProgressPreference
$global:ProgressPreference = "SilentlyContinue"
#2.-1) Download the IDE Locally
if (-not (Test-Path "$idePath`\arduino_debug.exe")){
    Write-Output "Arduino IDE not present."
    if (-not (Test-Path $idePath)) { [void](New-Item -Path $idePath -ItemType Directory) }
    if (-not $settings["cacheZips"] -Or ($settings["cacheZips"] -And -not (Test-Path "$idePath/$ideZipName"))){
        Write-Output "Attemping to Download, This will hang...."
        Write-Output "Please Wait..."
        Invoke-WebRequest $ideURL -OutFile "$idePath/$ideZipName"
        Write-Output "IDE Downloaded."
    }
    Write-Output "Extracting...."
    Expand-Archive -Force "$idePath/$ideZipName" $idePath
    Write-Output "IDE Extracted."
    Write-Output "Moving all files...."
    $ideFolder = ((Get-ChildItem -dir $idePath)[0].FullName)
    Get-ChildItem -Path $ideFolder | Move-Item -Destination $idePath
    Remove-Item $ideFolder
    Write-Output "Moved all files"
    Add-Content -Path "$idePath`\arduino_debug.l4j.ini" -Value "`n-DDEBUG=false"
    if (-not $settings["cacheZips"]){
        Write-Output "Not Caching Zips, Cleaning Up...."
        Remove-Item "$idePath/$ideZipName"
    }
    Write-Output "Arduino-IDE Downloaded and Setup!"
}

#2.0) Download the CLI if Needed
if (-not (Test-Path "$cliPath`/arduino-cli.exe")){
    Write-Output "Arduino CLI not present."
    if (-not (Test-Path $cliPath)) { [void](New-Item -Path $cliPath -ItemType Directory) }
    if (-not $settings["cacheZips"] -Or ($settings["cacheZips"] -And -not (Test-Path "$cliPath/$cliZipName"))){
        Write-Output "Attemping to Download, This will hang...."
        Write-Output "Please Wait..."
        Invoke-WebRequest $cliURL -OutFile "$cliPath/$cliZipName"
        Write-Output "CLI Downloaded. Extracting...."
    }
    Expand-Archive -Force "$cliPath/$cliZipName" $cliPath
    Write-Output "CLI Extracted."
    if (-not $settings["cacheZips"]){
        Write-Output "Not Caching Zips, Cleaning Up...."
        Remove-Item "$cliPath/$cliZipName"
    }
    if (-not (Test-Path $cliJSONPath)){
        Write-Output "Could not find CLI-JSON File. Grabbing it..."
        Invoke-WebRequest $cliJSONURL -OutFile $cliJSONPath
    }

    Write-Output "Arduino-CLI Downloaded and Setup!"
}

$global:ProgressPreference = $tmpProgressPreference

#3) Install Libs
Write-Output "Checking libs present..."
foreach ($lib in $settings["includedLibs"]){
    $libPathName = $lib -replace " ", "_"
    if (-not (Test-Path "$libPath`\$libPathName")) {
        Write-Output "Installing $lib..."
        Start-Process "$cliPath\arduino-cli.exe" -ArgumentList "lib install `"$lib`"" -wait -NoNewWindow
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
    $settings["doUpload"] = $false
} else {
    Write-Output ("Found a " + $boards[0].boards.name + " on " + $boards[0].address)
    $serialPort = "--port " + $boards[0].address
    $boardType = "--board " + $boards[0].boards.FQBN;
}

#5) Run PreBuild
if ($settings["doPreBuild"] -And -not [string]::IsNullOrEmpty($settings["preBuildCommand"])){
    $pbSplit = $settings["preBuildCommand"].split(" ")
    $cmdHead = $pbSplit[0]
    $cmdArgs = [String]::Join(" ", $pbSplit[1..($pbSplit.Length-1)])
    if (Get-Command $cmdHead -ErrorAction SilentlyContinue) {
        Write-Output ("Attemping to exicute prebuild command: `"" + $settings["preBuildCommand"] + "`"")
        Write-Debug "Exicuting as '$cmdHead' '$cmdArgs'"
        $preBuildProc = Start-Process $cmdHead -ArgumentList $cmdArgs -wait -NoNewWindow  -PassThru
        if ($preBuildProc.ExitCode -ne 0){
            Write-Warning ":( Something failed in the preBuild script!"
            if ($settings["dieOnPreBuildFail"]){ throw "PreBuild Command Failed!"}
        }
    } else {
        Write-Warning ("Could not exicute prebuild command: `"" + $settings["preBuildCommand"] + "`"")
    }
}

#6) Uploading / Verifiying

#6.1) Check if build path exsists and create it if it doesn't
if ($settings["doUpload"]) { Write-Output ("Attempting to Upload " + $settings["inoFile"] + "....") } 
    else { Write-Output ("Starting verification of " + $settings["inoFile"]) }

$argList = ""

if ($null -ne $buildPath){
    if (-not (Test-Path -Path $buildPath -PathType Container)){
        Write-Output "Creating Build Path: `"$buildPath`""
        New-Item -path $buildPath -type directory > $null
    }
	Write-Output "Using Build Path: `"$buildPath`""
    $buildPath = Convert-Path $buildPath
	$argList += "--pref build.path=`"$buildPath`" "
}

if ($null -ne $printfCMDArgs[$settings["printfOpt"]])
    {$argList += ($printfCMDArgs["CMD"] + '"' + $printfCMDArgs[$settings["printfOpt"]] + "`" ")}
if ($null -ne $macroExspansionArgs[$settings["macroExpOpt"]]) 
    {$argList += ($macroExspansionArgs["CMD"] + '"' + + $macroExspansionArgs[$settings["macroExpOpt"]] + "`" ")}
if ($null -ne $showDiagCaretArgs[$settings["caretOpt"]])
    {$argList += ($showDiagCaretArgs["CMD"] + '"' + $showDiagCaretArgs[$settings["caretOpt"]] + "`" ")}

if ($null -ne $serialPort)
    {$argList += "$serialPort $boardType "}
if ($null -ne $verbosityArg[$settings["verbosity"]])
    {$argList += ($verbosityArg[$settings["verbosity"]] + " ")}

$argList += ($uplodeArg[$settings["doUpload"]] + " " + $settings["inoFile"])

#Ask to disable the verbose output.
if ($settings["askIniUpdate"] -And (Test-Path "$idePath`\arduino_debug.l4j.ini") -And -not (Select-String -Path "$idePath`\arduino_debug.l4j.ini" -Pattern "-DDEBUG=false" -SimpleMatch -Quiet)) {
    Write-Output "It's detected that verbose output is enabled. This can be very anyoing / cluttering so disableing it is highly recomened."
    Write-Output "Note: Depending on the arduino install location, a request for administrative privliges may be requested."
    $updateIniFile = $(if ($forceIniUpdate) {0} else {$host.UI.PromptForChoice("", "Would you like to disable the *very* verbose arduino console output?", $ChoiceYN, 0)})

    if ($updateIniFile -eq 0) {
        Try {
            Write-Output "Attempting to disable verbose debug output..."
            Add-Content -LiteralPath ("$idePath`\arduino_debug.l4j.ini") -Value "`n-DDEBUG=false" -ErrorAction Stop
        } Catch {
            Write-Output "The required file is protected so an eleveation request was required."
            Try {
                Start-Process PowerShell -ErrorAction Stop -Wait -Verb RunAs "-NoProfile -ExecutionPolicy Bypass -Command `"cd '$pwd'; Add-Content -LiteralPath '$idePath`\arduino_debug.l4j.ini'  -Value '`n-DDEBUG=false';`"";
            } catch {
                Write-Output "Failed to open file."
            }
        } Finally {
            if (Select-String -Path "$idePath`\arduino_debug.l4j.ini" -Pattern "-DDEBUG=false" -SimpleMatch -Quiet) {
                if ((Select-String -Path "$idePath`\arduino_debug.l4j.ini" -Pattern "-DDEBUG=true" -SimpleMatch -Quiet)) {
                    Try {
                        Write-Output "Debug enable flag detected in file, attempting to remove...."
                        (Get-Content -LiteralPath ("$idePath`\arduino_debug.l4j.ini")) -replace "-DDEBUG=true","" | Out-File -LiteralPath ("$idePath`\arduino_debug.l4j.ini") -ErrorAction Stop
                    } catch {
                        Write-Output "The required file is protected so an eleveation request was required."
                        Try {
                            Start-Process PowerShell -ErrorAction Stop -Wait -Verb RunAs "-NoProfile -ExecutionPolicy Bypass -Command `"cd '$pwd'; (Get-Content -LiteralPath '$idePath`\arduino_debug.l4j.ini') -replace '-DDEBUG=true','' | Out-File -LiteralPath '$idePath`\arduino_debug.l4j.ini';`"";
                        } catch {
                            Write-Output "Failed to open file."
                        }
                    }
                }
                Write-Output "Verbose output disabled."
            } else {
                Write-Output "Verbose output has not been disabled. Unless you set 'askIniUpdate' to true in the JSON file you will not see this promt again."
            }
        }
    } else {
        Write-Output "Verbose output has not been disabled. Unless you set 'askIniUpdate' to true in the JSON file you will not see this promt again."
    }

    $tempJsonData = Get-Content $jsonFilePath | ConvertFrom-Json
    Try{
        $tempJsonData.askIniUpdate = $false
    } Catch {
        $tempJsonData | Add-Member -Name "askIniUpdate" -value $false -MemberType NoteProperty    
    }
    $tempJsonData | ConvertTo-Json -depth 32 | Out-File $jsonFilePath
}

$arduinoDebug = Start-Process "$idePath`\arduino_debug.exe" -ArgumentList $argList -wait -NoNewWindow -PassThru

switch ($arduinoDebug.ExitCode) {
    0 { Write-Host ("`n"+"Done!") -ForegroundColor Green }
    1 { Write-Host ("`n"+":( Build or Upload Failed!") -ForegroundColor Red}
    2 { Write-Host ("`n"+":{ Sketch `"" + $settings["inoFile"] + "`" not found!") -ForegroundColor Red}
    3 { Write-Host ("`n"+":[ Invalid argument in `"[" + $argList + "]`"") -ForegroundColor Red}
    4 { Write-Host ("`n"+":$ Preferance passed to `"--get-pref`" does not exsit!") -ForegroundColor Red}
    Default { Write-Error ("`n"+":O Something went very very wrong! You should never see this!") -ForegroundColor Red}
}

if ($settings["waitForUser"]) { Read-Host 'Press Enter to continue' }

exit $arduinoDebug.ExitCode