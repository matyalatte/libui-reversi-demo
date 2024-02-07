# Building Workflow for Linux

## Requirements

-   Build tools (e.g. `build-essential` for Ubuntu)
-   GTK+ 3.10 or later (e.g. `libgtk-3-dev` for Ubuntu)
-   Ninja
-   [Meson](https://github.com/mesonbuild/meson) (**0.63** or later)

## Install Meson and Ninja

You can install meson and ninja via apt. (`sudo apt install meson ninja`)  

> If you are a Python user, you can also get them via pip. (`sudo pip3 install meson ninja`)

## Build

```shell
meson setup build --native-file presets/release.ini
meson compile -C build
strip --strip-all ./build/libui_rubiks_demo
```

The executable will be generated in `build`.  
