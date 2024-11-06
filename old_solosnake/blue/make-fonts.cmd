
set CODEFONTNAME=FSEX300.ttf
set RUSSIAN1=GOPIYAT_R.ttf

rem U+0000..U+007F U+0400..U+04FF
fontmaker.x64.exe -f ./data/fonts/%RUSSIAN1% -p 1 -r U+0000..U+007F U+0400..U+04FF -s 12 -t ./data/textures/RUSSIAN1_12.bmp -x ./data/fonts/RUSSIAN1_12.xml
fontmaker.x64.exe -f ./data/fonts/%RUSSIAN1% -p 1 -r U+0000..U+007F U+0400..U+04FF -s 18 -t ./data/textures/RUSSIAN1_18.bmp -x ./data/fonts/RUSSIAN1_18.xml
fontmaker.x64.exe -f ./data/fonts/%RUSSIAN1% -p 1 -r U+0000..U+007F U+0400..U+04FF -s 24 -t ./data/textures/RUSSIAN1_24.bmp -x ./data/fonts/RUSSIAN1_24.xml
fontmaker.x64.exe -f ./data/fonts/%RUSSIAN1% -p 1 -r U+0000..U+007F U+0400..U+04FF -s 32 -t ./data/textures/RUSSIAN1_32.bmp -x ./data/fonts/RUSSIAN1_32.xml
fontmaker.x64.exe -f ./data/fonts/%RUSSIAN1% -p 1 -r U+0000..U+007F U+0400..U+04FF -s 36 -t ./data/textures/RUSSIAN1_36.bmp -x ./data/fonts/RUSSIAN1_36.xml
fontmaker.x64.exe -f ./data/fonts/%RUSSIAN1% -p 1 -r U+0000..U+007F U+0400..U+04FF -s 48 -t ./data/textures/RUSSIAN1_48.bmp -x ./data/fonts/RUSSIAN1_48.xml
fontmaker.x64.exe -f ./data/fonts/%RUSSIAN1% -p 1 -r U+0000..U+007F U+0400..U+04FF -s 60 -t ./data/textures/RUSSIAN1_60.bmp -x ./data/fonts/RUSSIAN1_60.xml

fontmaker.x64.exe -f ./data/fonts/%CODEFONTNAME% -p 1 -r 0..255 -s 12 -t ./data/textures/code12.bmp -x ./data/fonts/code12.xml
fontmaker.x64.exe -f ./data/fonts/%CODEFONTNAME% -p 1 -r 0..255 -s 18 -t ./data/textures/code18.bmp -x ./data/fonts/code18.xml
fontmaker.x64.exe -f ./data/fonts/%CODEFONTNAME% -p 1 -r 0..255 -s 24 -t ./data/textures/code24.bmp -x ./data/fonts/code24.xml
fontmaker.x64.exe -f ./data/fonts/%CODEFONTNAME% -p 1 -r 0..255 -s 32 -t ./data/textures/code32.bmp -x ./data/fonts/code32.xml
fontmaker.x64.exe -f ./data/fonts/%CODEFONTNAME% -p 1 -r 0..255 -s 36 -t ./data/textures/code36.bmp -x ./data/fonts/code36.xml
fontmaker.x64.exe -f ./data/fonts/%CODEFONTNAME% -p 1 -r 0..255 -s 48 -t ./data/textures/code48.bmp -x ./data/fonts/code48.xml
