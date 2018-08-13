project('lvg', 'c', default_options : ['c_std=gnu99', 'buildtype=release'])

conf = configuration_data()
conf.set('version', '0.0.5')
conf.set('RENDER_NVPR', get_option('RENDER_NVPR') ? 1 : 0)
conf.set('RENDER_NANOVG', get_option('RENDER_NANOVG') ? 1 : 0)
conf.set('AUDIO_SDL', get_option('AUDIO_SDL') ? 1 : 0)
conf.set('VIDEO_FFMPEG', get_option('VIDEO_FFMPEG') ? 1 : 0)
conf.set('PLATFORM_GLFW', get_option('PLATFORM_GLFW') ? 1 : 0)
conf.set('PLATFORM_SDL', get_option('PLATFORM_SDL') ? 1 : 0)
conf.set('SCRIPT_TCC', get_option('SCRIPT_TCC') ? 1 : 0)
conf.set('SCRIPT_PICOC', get_option('SCRIPT_PICOC') ? 1 : 0)
conf.set('ENABLE_AUDIO', get_option('ENABLE_AUDIO') ? 1 : 0)
conf.set('ENABLE_VIDEO', get_option('ENABLE_VIDEO') ? 1 : 0)
conf.set('ENABLE_SCRIPT', get_option('ENABLE_SCRIPT') ? 1 : 0)
configure_file(input : 'config.h.in', output : 'config.h', configuration : conf)

sources = [
    'src/lunzip.c',
    'src/lvg.c',
    'audio/audio_null.c',
    'render/common.c',
    'render/render_null.c',
    'video/ffmpeg/ffmpeg_dec.c'
]

host_os_family = host_machine.system()
incdirs = [ '.', 'src' ]
ext_link_args = [ '-lm', '-lavcodec', '-lavutil' ]

if get_option('ENABLE_AUDIO')
    sources += [ 'audio/common.c' ]
    if get_option('AUDIO_SDL')
        sources += [ 'audio/audio_sdl.c' ]
    endif
endif

if get_option('ENABLE_VIDEO')
    sources += [ 'render/glad.c' ]
    if get_option('RENDER_NVPR')
        sources += [ 'render/render_nvpr.c' ]
    endif
    if get_option('RENDER_NANOVG')
        sources += [ 'nanovg/nanovg.c', 'render/render_nanovg.c' ]
        incdirs += [ 'nanovg' ]
    endif
endif

if get_option('PLATFORM_GLFW')
    sources += [ 'platform/platform_glfw.c' ]
    if host_os_family == 'linux'
        ext_link_args += [ '-lglfw' ]
    else
        ext_link_args += [ '-lglfw3' ]
    endif
    if host_os_family == 'windows'
        incdirs += [ 'glfw/include', 'glfw/deps' ]
        ext_link_args += [ '-L../glfw/build/src' ]
    endif
endif

if get_option('PLATFORM_SDL') or get_option('AUDIO_SDL')
    sources += [ 'platform/platform_sdl.c' ]
    if host_os_family == 'windows'
        ext_link_args += [ '-lSDL2-static' ]
    else
        ext_link_args += [ '-lSDL2' ]
    endif
endif

if get_option('ENABLE_SCRIPT')
    if get_option('SCRIPT_TCC')
        sources += [ 'scripting/tcc/script_tcc.c' ]
        incdirs += [ 'scripting/tcc' ]
        ext_link_args += [ '-ltcc2' ]
    endif
    if get_option('SCRIPT_PICOC')
        sources += [
            'scripting/picoc/clibrary.c',
            'scripting/picoc/debug.c',
            'scripting/picoc/expression.c',
            'scripting/picoc/heap.c',
            'scripting/picoc/include.c',
            'scripting/picoc/lex.c',
            'scripting/picoc/parse.c',
            'scripting/picoc/picoc.c',
            'scripting/picoc/platform.c',
            'scripting/picoc/table.c',
            'scripting/picoc/type.c',
            'scripting/picoc/variable.c',
            'scripting/picoc/cstdlib/ctype.c',
            'scripting/picoc/cstdlib/errno.c',
            'scripting/picoc/cstdlib/math.c',
            'scripting/picoc/cstdlib/stdbool.c',
            'scripting/picoc/cstdlib/stdio.c',
            'scripting/picoc/cstdlib/stdlib.c',
            'scripting/picoc/cstdlib/string.c',
            'scripting/picoc/cstdlib/time.c',
            'scripting/picoc/cstdlib/unistd.c',
            'scripting/picoc/platform/platform_unix.c'
        ]
        add_global_arguments('-DUNIX_HOST', language : 'c')
    endif
endif

if get_option('ENABLE_SWF')
    sources += [
        'swf/adpcm.c',
        'swf/avm1.c',
        'swf/avm1_globals.c',
        'swf/swf.c',
        'swf/swftools/lib/bitio.c',
        'swf/swftools/lib/rfxswf.c',
        'swf/swftools/lib/q.c',
        'swf/swftools/lib/as3/abc.c',
        'swf/swftools/lib/as3/assets.c',
        'swf/swftools/lib/as3/code.c',
        'swf/swftools/lib/as3/opcodes.c',
        'swf/swftools/lib/as3/pool.c',
        'swf/swftools/lib/modules/swfbits.c',
        'swf/swftools/lib/modules/swffilter.c',
        'swf/swftools/lib/modules/swfobject.c',
        'swf/swftools/lib/modules/swfshape.c',
        'swf/swftools/lib/modules/swftext.c',
        'swf/swftools/lib/modules/swftools.c',
    ]
    incdirs += [ 'swf/swftools/lib' ]
endif

if get_option('buildtype') == 'debug'
    add_global_arguments('-D_GNU_SOURCE', '-D_DEBUG', language : 'c')
else
    add_global_arguments('-D_GNU_SOURCE', '-DNDEBUG', '-Os', '-flto', '-fno-asynchronous-unwind-tables', '-fno-stack-protector', '-ffunction-sections', '-fdata-sections', language : 'c')
    add_global_link_arguments('-ffunction-sections', '-fdata-sections', language : 'c')
endif

if get_option('buildtype') != 'debug'
    if host_os_family == 'darwin'
        add_global_link_arguments('-Wl,-dead_strip', '-Wl,-map,lvg.map', language : 'c')
    else
        add_global_link_arguments('-s', '-Wl,--gc-sections', '-Wl,-Map=lvg.map', language : 'c')
    endif
endif

if host_os_family == 'linux'
    ext_link_args += [ '-ldl' ]
endif

if host_os_family == 'darwin'
    incdirs += [ '/usr/local/include' ]
    ext_link_args += [ '-L/usr/local/lib', '-ldl', '-framework', 'OpenGL', '-framework', 'Cocoa', '-framework', 'IOKit', '-framework', 'CoreFoundation', '-framework', 'CoreVideo' ]
endif

if host_os_family == 'windows'
    incdirs += [ 'SDL/include', 'video/ffmpeg/FFmpeg', '-Ivideo/ffmpeg/FFmpeg/build-win' ]
    sources += [ 'windows/mman.c' ]
    ext_link_args += [ '-L../SDL/build', '-L../video/ffmpeg/FFmpeg/build-win/libavcodec', '-L../video/ffmpeg/FFmpeg/build-win/libavutil' ]
    ext_link_args += [ '-luser32', '-lgdi32', '-lwinmm', '-limm32', '-lole32', '-loleaut32', '-lshell32', '-lversion' ]
endif

executable('lvg', sources, link_args : ext_link_args, include_directories : include_directories(incdirs))

executable('lvg_test', sources, c_args: '-D_TEST', link_args : ext_link_args, include_directories : include_directories(incdirs))
