del "*.suo" /S/Q/F/A
del "*.pdb" /S/Q/F/A

for /F %%i in ('dir /A:d /S/B "Sources\Core\cpp\Release*"') do rd /S/Q %%i
for /F %%i in ('dir /A:d /S/B "Sources\Core\cpp\Debug*"') do rd /S/Q %%i
del "Sources\Core\cpp\*.vcxproj.user" /S/Q/F/A


for /F %%i in ('dir /A:d /S/B "Sources\Core\cpp_cli\Release*"') do rd /S/Q %%i
for /F %%i in ('dir /A:d /S/B "Sources\Core\cpp_cli\Debug*"') do rd /S/Q %%i
del "Sources\Core\cpp_cli\*.vcxproj.user" /S/Q/F/A


for /F %%i in ('dir /A:d /S/B "Sources\Core\csharp\bin*"') do rd /S/Q %%i
for /F %%i in ('dir /A:d /S/B "Sources\Core\csharp\obj*"') do rd /S/Q %%i
del "Sources\Core\csharp\*.csproj.user" /S/Q/F/A

for /F %%i in ('dir /A:d /S/B "Sources\Service\bin*"') do rd /S/Q %%i
for /F %%i in ('dir /A:d /S/B "Sources\Service\obj*"') do rd /S/Q %%i
del "Sources\Service\*.csproj.user" /S/Q/F/A

for /F %%i in ('dir /A:d /S/B "Sources\TextMining\bin*"') do rd /S/Q %%i
for /F %%i in ('dir /A:d /S/B "Sources\TextMining\obj*"') do rd /S/Q %%i
del "Sources\TextMining\*.csproj.user" /S/Q/F/A

for /F %%i in ('dir /A:d /S/B "Sources\Tests\bin*"') do rd /S/Q %%i
for /F %%i in ('dir /A:d /S/B "Sources\Tests\obj*"') do rd /S/Q %%i
del "Sources\Tests\*.csproj.user" /S/Q/F/A

pause
