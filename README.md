# 'Evelike'
This is a (highly likely) abandoned 2023 rewrite/rethinking of my earlier 'Blue'
project. This code focuses on a newer C++ style, and contains mostly a glTF
loader and viewer which uses SDL (instead of hand-rolled engine code). This code
contains some nice window code, shader lighting, skyboxes and trackball model
manipulation.

![glTF model with skybox](/screenshots/projecttitle.jpg "Evelike displaying a glTF model with skybox")

This codebase is a branch of the original 'Blue' code, with some large files
removed from the history, hence this is a very large repo with a **LOT** of
history.

## Building
Requires CMake 3.24+. Tested on Linux Mint 21.3 (virginia).
`python3 ./build.py`

## Note
This project contains within it the following **external** dependancies:

1. GLEW
    - https://glew.sourceforge.net/
    - https://sourceforge.net/projects/glew/files/glew/2.1.0/
2. FreeImage CMake Files:
    - https://github.com/Max-ChenFei/FreeImage-CMake
3. FreeImage:
    - https://freeimage.sourceforge.io/
    - https://deac-fra.dl.sourceforge.net/project/freeimage/Source%20Distribution/3.18.0/FreeImage3180.zip
4. SDL2
    - https://www.libsdl.org/
    - https://github.com/libsdl-org/SDL/releases/tag/release-2.26.3



