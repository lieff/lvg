project('lvg', 'c', default_options : ['c_std=gnu99'])
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
    'swf/adpcm.c',
    'swf/avm1.c',
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
    'swf/swftools/lib/modules/swfscripts.c',
    'swf/swftools/lib/modules/swfshape.c',
    'swf/swftools/lib/modules/swftext.c',
    'swf/swftools/lib/modules/swftools.c',
    'video/ffmpeg/ffmpeg_dec.c'
]

incdirs = ['.', 'src', 'nanovg', 'scripting/tcc', 'swf/swftools/lib', 'mp3']
add_global_arguments('-D_GNU_SOURCE', '-DNDEBUG', '-Os', '-flto', '-fno-asynchronous-unwind-tables', '-fno-stack-protector', '-ffunction-sections', '-fdata-sections', language : 'c')
add_global_link_arguments('-ffunction-sections', '-fdata-sections', language : 'c')
ext_link_args = ['-L../scripting/tcc', '-lm', '-lSDL2', '-lavcodec', '-lavutil']
host_os_family = host_machine.system()

if host_os_family == 'linux'
ext_link_args += ['-lGL', '-lglfw', '-ltcc2', '-ldl']
add_global_link_arguments('-s', '-Wl,--gc-sections', '-Wl,-Map=lvg.map', language : 'c')
endif

if host_os_family == 'darwin'
incdirs += ['/usr/local/include']
ext_link_args += ['-L/usr/local/lib', '-lglfw3', '-ltcc', '-ldl', '-framework', 'OpenGL', '-framework', 'Cocoa', '-framework', 'IOKit', '-framework', 'CoreFoundation', '-framework', 'CoreVideo']
add_global_link_arguments('-Wl,-dead_strip', '-Wl,-map,lvg.map', language : 'c')
endif

executable('lvg', sources, link_args : ext_link_args, include_directories : include_directories(incdirs))
