project('lvg', 'c', default_options : ['c_std=c99'])
sources = ['adpcm.c', 'lunzip.c', 'lvg_audio.c', 'svg.c', 'svgb.c', 'swf.c',
    'mp3/minimp3.c',
    'nanovg/nanovg.c',
    'swftools/lib/bitio.c',
    'swftools/lib/drawer.c',
    'swftools/lib/graphcut.c',
    'swftools/lib/log.c',
    'swftools/lib/mem.c',
    'swftools/lib/os.c',
    'swftools/lib/rfxswf.c',
    'swftools/lib/modules/swfaction.c',
    'swftools/lib/modules/swfalignzones.c',
    'swftools/lib/modules/swfbits.c',
    'swftools/lib/modules/swfbutton.c',
    'swftools/lib/modules/swfdraw.c',
    'swftools/lib/modules/swfdump.c',
    'swftools/lib/modules/swffilter.c',
    'swftools/lib/modules/swffont.c',
    'swftools/lib/modules/swfobject.c',
    'swftools/lib/modules/swfrender.c',
    'swftools/lib/modules/swfscripts.c',
    'swftools/lib/modules/swfshape.c',
    'swftools/lib/modules/swftext.c',
    'swftools/lib/modules/swftools.c',
    'swftools/lib/action/actioncompiler.c'
]
incdirs = ['.', 'nanovg', 'swftools/lib', 'mp3']
add_global_arguments('-D_GNU_SOURCE', language : 'c')
ext_link_args = ['-L..', '-lm', '-ldl', '-lSDL2']
host_os_family = host_machine.system()
if host_os_family == 'linux'
ext_link_args += ['-lGL', '-lglfw', '-ltcc2']
endif
if host_os_family == 'darwin'
incdirs += ['/usr/local/include']
ext_link_args += ['-L/usr/local/lib', '-lglfw3', '-ltcc', '-framework', 'OpenGL', '-framework', 'Cocoa', '-framework', 'IOKit', '-framework', 'CoreFoundation', '-framework', 'CoreVideo']
endif
executable('lvg', sources, link_args : ext_link_args, include_directories : include_directories(incdirs))
