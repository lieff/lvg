project('lvg', 'c', default_options : ['c_std=gnu99', 'buildtype=release'])

conf = configuration_data()
conf.set('version', '0.0.5')
conf.set('RENDER_NVPR', get_option('RENDER_NVPR') ? 1 : 0)
conf.set('RENDER_NANOVG', get_option('RENDER_NANOVG') ? 1 : 0)
conf.set('AUDIO_SDL', get_option('AUDIO_SDL') ? 1 : 0)
conf.set('VIDEO_FFMPEG', get_option('VIDEO_FFMPEG') ? 1 : 0)
conf.set('SCRIPT_TCC', get_option('SCRIPT_TCC') ? 1 : 0)
conf.set('ENABLE_AUDIO', get_option('ENABLE_AUDIO') ? 1 : 0)
conf.set('ENABLE_VIDEO', get_option('ENABLE_VIDEO') ? 1 : 0)
conf.set('ENABLE_SCRIPT', get_option('ENABLE_SCRIPT') ? 1 : 0)
configure_file(input : 'config.h.in', output : 'config.h', configuration : conf)

sources = [
    'audio/sdl/lvg_audio.c',
    'src/lunzip.c',
    'src/lvg.c',
    'src/svgb.c',
    'mp3/minimp3.c',
    'nanovg/nanovg.c',
    'render/common.c',
    'render/render_nanovg.c',
    'render/render_nvpr.c',
    'render/render_null.c',
    'swf/adpcm.c',
    'swf/avm1.c',
    'swf/avm1_globals.c',
    'scripting/tcc/script_tcc.c',
    'swf/swf.c',
    'swf/swftools/lib/bitio.c',
    'swf/swftools/lib/drawer.c',
    'swf/swftools/lib/graphcut.c',
    'swf/swftools/lib/rfxswf.c',
    'swf/swftools/lib/q.c',
    'swf/swftools/lib/as3/abc.c',
    'swf/swftools/lib/as3/assets.c',
    'swf/swftools/lib/as3/code.c',
    'swf/swftools/lib/as3/opcodes.c',
    'swf/swftools/lib/as3/pool.c',
    'swf/swftools/lib/modules/swfaction.c',
    'swf/swftools/lib/modules/swfalignzones.c',
    'swf/swftools/lib/modules/swfbits.c',
    'swf/swftools/lib/modules/swfdraw.c',
    'swf/swftools/lib/modules/swfdump.c',
    'swf/swftools/lib/modules/swffilter.c',
    'swf/swftools/lib/modules/swffont.c',
    'swf/swftools/lib/modules/swfobject.c',
    'swf/swftools/lib/modules/swfshape.c',
    'swf/swftools/lib/modules/swftext.c',
    'swf/swftools/lib/modules/swftools.c',
    'video/ffmpeg/ffmpeg_dec.c'
]

incdirs = ['.', 'src', 'nanovg', 'scripting/tcc', 'swf/swftools/lib', 'mp3']
if get_option('buildtype') == 'debug'
    add_global_arguments('-D_GNU_SOURCE', '-D_DEBUG', language : 'c')
else
    add_global_arguments('-D_GNU_SOURCE', '-DNDEBUG', '-Os', '-flto', '-fno-asynchronous-unwind-tables', '-fno-stack-protector', '-ffunction-sections', '-fdata-sections', language : 'c')
    add_global_link_arguments('-ffunction-sections', '-fdata-sections', language : 'c')
endif
ext_link_args = ['-L../scripting/tcc', '-lm', '-lavcodec', '-lavutil']
host_os_family = host_machine.system()

if get_option('buildtype') != 'debug'
    if host_os_family == 'darwin'
        add_global_link_arguments('-Wl,-dead_strip', '-Wl,-map,lvg.map', language : 'c')
    else
        add_global_link_arguments('-s', '-Wl,--gc-sections', '-Wl,-Map=lvg.map', language : 'c')
    endif
endif

if host_os_family == 'linux'
    ext_link_args += ['-lGL', '-lglfw', '-lSDL2', '-ltcc2', '-ldl']
endif

if host_os_family == 'darwin'
    incdirs += ['/usr/local/include']
    ext_link_args += ['-L/usr/local/lib', '-lglfw3', '-lSDL2', '-ltcc', '-ldl', '-framework', 'OpenGL', '-framework', 'Cocoa', '-framework', 'IOKit', '-framework', 'CoreFoundation', '-framework', 'CoreVideo']
endif

if host_os_family == 'windows'
    incdirs += ['glfw/include', 'glfw/deps', 'SDL/include', 'video/ffmpeg/FFmpeg']
    sources += ['windows/mman.c', 'glfw/deps/glad.c']
    ext_link_args += ['-L../glfw/build/src', '-L../SDL/build', '-L../video/ffmpeg/FFmpeg/libavcodec', '-L../video/ffmpeg/FFmpeg/libavutil']
    ext_link_args += ['-lSDL2-static', '-lglfw3', '-lopengl32', '-luser32', '-lgdi32', '-lwinmm', '-limm32', '-lole32', '-loleaut32', '-lshell32', '-lversion']
endif

executable('lvg', sources, link_args : ext_link_args, include_directories : include_directories(incdirs))

executable('lvg_test', sources, c_args: '-D_TEST', link_args : ext_link_args, include_directories : include_directories(incdirs))
