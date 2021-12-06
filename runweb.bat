@echo off


echo Open http://localhost:8080/
"../emsdk/emsdk_env.bat" & emrun --no_browser --port 8080 public/
