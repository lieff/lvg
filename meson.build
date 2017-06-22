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
incdirs = include_directories('.', 'nanovg', 'swftools/lib', 'mp3')
add_global_arguments('-D_GNU_SOURCE', language : 'c')
executable('lvg', sources, link_args : ['-L..', '-lm', '-lglfw', '-lGL', '-ltcc2', '-ldl', '-lSDL2'], include_directories : incdirs)
