# Building Workflow for Windows

## Requirements

-   Visual Studio
-   [Meson](https://github.com/mesonbuild/meson/releases) (**0.63** or later)

> It also supports mingw-w64 but it makes the binary larger.  
> I recommend that you use msvc.

## Install Meson

Get the installer (*.msi) from [the release page](https://github.com/mesonbuild/meson/releases).  
And launch it on your machine.  

> If you are a Python user, you can also get meson via pip. (`pip3 install meson`)

## Build

```batch
meson setup build --backend=vs --native-file presets\release.ini
meson compile -C build
```

The executable will be generated in `build/`.  

## Cross Compile for ARM64

If you installed ARM64 components in Visual Studio, you can build ARM64 binary on x64 machine.  

```
meson setup build_arm --backend=vs --cross-file presets\windows_arm64.ini --cross-file presets\release.ini
meson compile -C build_arm
```

The executable will be generated in `build_arm/`.  
