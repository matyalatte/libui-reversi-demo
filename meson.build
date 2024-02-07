project('libui_reversi_demo', 'c',
    meson_version: '>=0.63.0',          # diff_files requires 0.63.0
    default_options: [
        'buildtype=debug',              # build debug by default
        'default_library=shared',       # build shared libraries by default
        'warning_level=3',              # always max warnings
        'b_pch=false',                  # we don't want precompiled headers
        'b_staticpic=true',             # use PIC even for static libraries
        'c_std=c99',                    # strict C99
        'c_winlibs=',                   # we define our own Windows libraries
        'cpp_std=c++11',                # strict C++11
        'cpp_eh=sc',                    # shut the compiler up in some cases
        'cpp_winlibs=',                 # likewise as with c_winlibs
    ],)

proj_OS = host_machine.system()
proj_compiler = meson.get_compiler('c').get_id()
proj_is_release = get_option('buildtype').startswith('release')

proj_sources = ['src/main.c']
proj_manifest = []
proj_link_args = []
proj_cpp_args = []

if proj_OS == 'windows'
    windows = import('windows')
    proj_manifest += [
        windows.compile_resources('src/app.rc',
            depend_files: ['src/app.manifest']),
    ]
    if proj_compiler == 'msvc'
        proj_link_args = [
            '/LARGEADDRESSAWARE',
            '/INCREMENTAL:NO',
            '/MANIFEST:NO',
            '/ENTRY:mainCRTStartup'
        ]
    else
        proj_link_args += [
            '-static',
            '-static-libgcc',
        ]
    endif
elif proj_OS == 'darwin'
    add_languages('objc', required: true)
    languages = ['c', 'cpp', 'objc']
    macosx_version_min = '-mmacosx-version-min=10.9'
    add_global_arguments(macosx_version_min, language: languages)
    add_global_link_arguments(macosx_version_min, language: languages)

    arch = ['-arch', 'x86_64', '-arch', 'arm64']
    if get_option('osx_build_universal')
        add_global_arguments(arch, language: languages)
        add_global_link_arguments(arch, language: languages)
    endif
else
    proj_link_args += ['-no-pie']
endif

libui_dep = dependency('libui', fallback : ['libui', 'libui_dep'])
reversi_dep = dependency('reversi', fallback : ['reversi', 'reversi_dep'])

executable('libui_reversi_demo',
    proj_manifest + proj_sources,
    dependencies: [libui_dep, reversi_dep],
    link_args: proj_link_args,
    install: false,
    win_subsystem: 'windows')
