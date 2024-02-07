# Building Workflow for macOS

## Requirements

-   Xcode
-   Ninja
-   [Meson](https://github.com/mesonbuild/meson) (**0.63** or later)

## Install Meson and Ninja

You can install meson and ninja via Homebrew. (`brew install meson ninja`)

> If you are a Python user, you can also get them via pip. (`pip3 install meson ninja`)

## Build

```shell
meson setup build --native-file presets/release.ini
meson compile -C build
strip ./build/libui_rubiks_demo
```

The executable will be generated in `build`.  
