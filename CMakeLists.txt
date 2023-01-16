cmake_minimum_required(VERSION 3.3)

project(vgmstream)

option(BUILD_CLI "" OFF)
option(BUILD_STATIC "" ON)
option(BUILD_FB2K "" OFF)
option(BUILD_WINAMP "" OFF)
option(BUILD_XMPLAY "" OFF)
option(BUILD_AUDACIOUS "" OFF)
option(USE_JANSSON "" OFF)
option(BUILD_V123 "" OFF)
option(USE_MPEG "" ON)
option(USE_VORBIS "" ON)
option(USE_FFMPEG "" ON)
option(USE_MAIATRAC3PLUS "" OFF)
option(USE_G7221 "" ON)
option(USE_G719 "" ON)
option(USE_ATRAC9 "" ON)
option(USE_CELT "" ON)
option(USE_SPEEX "" ON)

add_subdirectory(vgmstream EXCLUDE_FROM_ALL)

set_target_properties(libvgmstream_shared PROPERTIES LINK_SEARCH_END_STATIC OFF)
target_link_options(libvgmstream_shared PRIVATE "-Wl,--version-script=${CMAKE_SOURCE_DIR}/version.map")
set_target_properties(libvgmstream_shared PROPERTIES PUBLIC_HEADER include/vgmstream.h)

install(TARGETS libvgmstream_shared PUBLIC_HEADER)
