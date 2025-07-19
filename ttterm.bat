:: Quick launcher to avoid writing 
:: the full path to the executable every time.

@echo off
cd /d %~dp0
.\build\debug\ttterm.exe %* 
exit /b %errorlevel%