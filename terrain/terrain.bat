REM bmp = new CBmpImage(argv[1]);
REM	primSize = (INT)atoi(argv[2]);
REM	vheight = (INT)atoi(argv[3]);
REM	vertCount = (INT)atoi(argv[4]);
REM	useUV = (INT)atoi(argv[5]);
REM	average = (INT)atoi(argv[6]);
REM	vertices = (INT)atoi(argv[7]);
REM
REM terrain.exe field.bmp 1 1.0 1 1 1 1
REM terrain.exe flat64x64.bmp 8 1.0 64 1 1 1
terrain.exe heightmap64x64.bmp 1 0.0125 64 1 1 1
REM terrain.exe heightmap264x64.bmp 32 1.0 64 1 1 1
REM terrain.exe heightmap512x512.bmp 8 1.0 8 1 1 1
REM terrain.exe oceancliffs.bmp 32 2.0 32 1 1 1
REM terrain.exe heightmap8x8.bmp 256 2.0 8 1 1 1
