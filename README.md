# minivgmstream

This project builds vgmstream as shared library and embeds all 3rd party libraries in it, while keeping API exposure to minimal.

Supported platform is only for linux. Win32/MinGW/Msys2 are not supported and probably will never be.

Currently this library is built for `Ubuntu 22.04 (Jammy)` distro, but should work for all distros with the same kernel version.

## Using minivgmstream in another project

```cmake
if (UNIX)
    if (NOT EXISTS libvgmstream)
        message("Downloading libvgmstream")
        file(DOWNLOAD https://github.com/PredatorCZ/libvgmstream/releases/download/<VERSION>/libvgmstream-linux-amd64.tar.xz libvgmstream)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/vgmstream)
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar x ../libvgmstream WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/vgmstream)
    endif()

    add_library(libvgmstream INTERFACE)
    target_include_directories(libvgmstream INTERFACE ${CMAKE_BINARY_DIR}/vgmstream/include)
    target_link_directories(libvgmstream INTERFACE ${CMAKE_BINARY_DIR}/vgmstream/lib)
    target_link_libraries(libvgmstream INTERFACE vgmstream)
endif()
```

Replace `<VERSION>` with release name found in releases for this project.
Link `libvgmstream` to your target via `target_link_libraries`.

## License

Library is licensed under GPLv2. Although most of the external libraries are licensed under permisive licenses like BSD or MIT. FFmpeg and MPG are using GPLv2 or LGPLv2 (depends on configuration).
