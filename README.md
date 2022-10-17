## Description

Small raycaster in C, allowing the user to cast light on edges and curves.

The user can draw curves by placing points on the screen. Each fragment of curve is modelled by a Cubic Bézier curve which is connecter with one another by a C^2 junction. 

The user can also move the curve as he wants by sliding the white points.

## Prerequisites

In order to compile this program, you need to install the following libraries :

- the **SDL2 library** (available [here](https://github.com/libsdl-org/SDL/releases/tag/release-2.24.1) : don't forget to choose the version according to your OS)

- the **SDL_ttf library** which isn't included in the default SDL2 library and therefore needs to be downloaded separately (available [here](https://github.com/libsdl-org/SDL_ttf/releases). This library handles the use of fonts.

- the **GSL library** which is a C library for numerical computations that I used to calculate cubic roots (available [here](https://solarianprogrammer.com/2020/01/26/getting-started-gsl-gnu-scientific-library-windows-macos-linux/#gsl_installation_macos)).


