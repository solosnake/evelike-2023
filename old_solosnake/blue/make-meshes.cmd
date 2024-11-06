mkdir buildlog
cd buildlog

set RESULTWINDOW=false
set SHIPSCALE=0.0018
set SHIPY=0.0

REM SHIPS (3DS MAX MODELS)

..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_1\transport_space_ship_1_cargo_1.3DS -o ts1_1x1 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship1x1.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_1\transport_space_ship_1_cargo_2.3DS -o ts1_1x2 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship1x2.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_1\transport_space_ship_1_cargo_3.3DS -o ts1_1x3 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship1x3.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_1\transport_space_ship_1_cargo_4.3DS -o ts1_1x4 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship1x4.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_1\transport_space_ship_1_cargo_5.3DS -o ts1_1x5 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship1x5.txt

..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_2\transport_space_ship_2_cargo_1.3DS -o ts2_2x1 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship2x1.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_2\transport_space_ship_2_cargo_2.3DS -o ts2_2x2 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship2x2.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_2\transport_space_ship_2_cargo_3.3DS -o ts2_2x3 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship2x3.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_2\transport_space_ship_2_cargo_4.3DS -o ts2_2x4 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship2x4.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_2\transport_space_ship_2_cargo_5.3DS -o ts2_2x5 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship2x5.txt

..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_3\transport_space_ship_3_cargo_1.3DS -o ts3_3x1 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship3x1.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_3\transport_space_ship_3_cargo_2.3DS -o ts3_3x2 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship3x2.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_3\transport_space_ship_3_cargo_3.3DS -o ts3_3x3 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship3x3.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_3\transport_space_ship_3_cargo_4.3DS -o ts3_3x4 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship3x4.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_3\transport_space_ship_3_cargo_5.3DS -o ts3_3x5 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship3x5.txt

..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_4\transport_space_ship_4_cargo_1.3DS -o ts4_4x1 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship4x1.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_4\transport_space_ship_4_cargo_2.3DS -o ts4_4x2 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship4x2.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_4\transport_space_ship_4_cargo_3.3DS -o ts4_4x3 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship4x3.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_4\transport_space_ship_4_cargo_4.3DS -o ts4_4x4 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship4x4.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_4\transport_space_ship_4_cargo_5.3DS -o ts4_4x5 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship4x5.txt

..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_5\transport_space_ship_5_cargo_1.3DS -o ts5_5x1 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship5x1.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_5\transport_space_ship_5_cargo_2.3DS -o ts5_5x2 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship5x2.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_5\transport_space_ship_5_cargo_3.3DS -o ts5_5x3 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship5x3.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_5\transport_space_ship_5_cargo_4.3DS -o ts5_5x4 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship5x4.txt
..\meshmaker.x64.exe -i ..\art_src\transport_space_ship_5\transport_space_ship_5_cargo_5.3DS -o ts5_5x5 -m ..\data\meshes -e ..\data\models -t ..\data\textures -y %SHIPY% -s %SHIPSCALE% -x true -w %RESULTWINDOW% > .\ship5x5.txt

REM ASTEROIDS                                          
..\meshmaker.x64.exe -i ..\art_src\dae\Asteroid_500.dae -o Panguite  -m ..\data\meshes -e ..\data\models -t ..\data\textures -y 0.40 -s 0.90 -w %RESULTWINDOW% > .\panguite.txt
..\meshmaker.x64.exe -i ..\art_src\dae\Asteroid_500.dae -o Kamacite  -m ..\data\meshes -e ..\data\models -t ..\data\textures -y 0.35 -s 0.80 -w %RESULTWINDOW% > .\kamacite.txt
..\meshmaker.x64.exe -i ..\art_src\dae\Asteroid_500.dae -o Ataxite   -m ..\data\meshes -e ..\data\models -t ..\data\textures -y 0.30 -s 0.75 -w %RESULTWINDOW% > .\ataxite.txt
..\meshmaker.x64.exe -i ..\art_src\dae\Asteroid_500.dae -o Chondrite -m ..\data\meshes -e ..\data\models -t ..\data\textures -y 0.25 -s 0.60 -w %RESULTWINDOW% > .\chondrite.txt

cd ..
