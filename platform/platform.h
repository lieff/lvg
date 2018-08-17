#pragma once

#define MOUSE_BUTTON_LEFT    1
#define MOUSE_BUTTON_RIGHT   2
#define MOUSE_BUTTON_MIDDLE  4

typedef struct platform_params
{
    int winWidth, winHeight;
    int width, height;
    int mkeys, last_mkeys;
    double mx, my;
    double time;
} platform_params;

typedef struct platform
{
    int (*init)(void **ctx, platform_params *params, void (*onFrame)(void *), void *user, int audio_only);
    void (*release)(void *ctx);
    void (*pull_events)(void *ctx);
    void (*main_loop)(void *ctx);
    void (*set_exit)(void *ctx);
    void (*swap_buffers)(void *ctx);
    void (*fullscreen)(void *ctx, int b_fullscreen);
    double (*get_time)(void *ctx);
    int (*get_key)(void *ctx, int key);
    void *(*get_proc_address)(const char *procname);
    int (*extension_supported)(const char *ext);
} platform;

extern platform_params g_params;

// USB HID standard keys
#define KEY_MOD_LCTRL  0x01
#define KEY_MOD_LSHIFT 0x02
#define KEY_MOD_LALT   0x04
#define KEY_MOD_LMETA  0x08
#define KEY_MOD_RCTRL  0x10
#define KEY_MOD_RSHIFT 0x20
#define KEY_MOD_RALT   0x40
#define KEY_MOD_RMETA  0x80

#define KEY_A 0x04
#define KEY_B 0x05
#define KEY_C 0x06
#define KEY_D 0x07
#define KEY_E 0x08
#define KEY_F 0x09
#define KEY_G 0x0a
#define KEY_H 0x0b
#define KEY_I 0x0c
#define KEY_J 0x0d
#define KEY_K 0x0e
#define KEY_L 0x0f
#define KEY_M 0x10
#define KEY_N 0x11
#define KEY_O 0x12
#define KEY_P 0x13
#define KEY_Q 0x14
#define KEY_R 0x15
#define KEY_S 0x16
#define KEY_T 0x17
#define KEY_U 0x18
#define KEY_V 0x19
#define KEY_W 0x1a
#define KEY_X 0x1b
#define KEY_Y 0x1c
#define KEY_Z 0x1d

#define KEY_1 0x1e
#define KEY_2 0x1f
#define KEY_3 0x20
#define KEY_4 0x21
#define KEY_5 0x22
#define KEY_6 0x23
#define KEY_7 0x24
#define KEY_8 0x25
#define KEY_9 0x26
#define KEY_0 0x27

#define KEY_ENTER      0x28
#define KEY_ESC        0x29
#define KEY_BACKSPACE  0x2a
#define KEY_TAB        0x2b
#define KEY_SPACE      0x2c
#define KEY_MINUS      0x2d
#define KEY_EQUAL      0x2e
#define KEY_LEFTBRACE  0x2f
#define KEY_RIGHTBRACE 0x30
#define KEY_BACKSLASH  0x31
#define KEY_HASHTILDE  0x32
#define KEY_SEMICOLON  0x33
#define KEY_APOSTROPHE 0x34
#define KEY_GRAVE      0x35
#define KEY_COMMA      0x36
#define KEY_DOT        0x37
#define KEY_SLASH      0x38
#define KEY_CAPSLOCK   0x39

#define KEY_F1  0x3a
#define KEY_F2  0x3b
#define KEY_F3  0x3c
#define KEY_F4  0x3d
#define KEY_F5  0x3e
#define KEY_F6  0x3f
#define KEY_F7  0x40
#define KEY_F8  0x41
#define KEY_F9  0x42
#define KEY_F10 0x43
#define KEY_F11 0x44
#define KEY_F12 0x45

#define KEY_SYSRQ      0x46
#define KEY_SCROLLLOCK 0x47
#define KEY_PAUSE      0x48
#define KEY_INSERT     0x49
#define KEY_HOME       0x4a
#define KEY_PAGEUP     0x4b
#define KEY_DELETE     0x4c
#define KEY_END        0x4d
#define KEY_PAGEDOWN   0x4e
#define KEY_RIGHT      0x4f
#define KEY_LEFT       0x50
#define KEY_DOWN       0x51
#define KEY_UP         0x52

#define KEY_NUMLOCK    0x53
#define KEY_KPSLASH    0x54
#define KEY_KPASTERISK 0x55
#define KEY_KPMINUS    0x56
#define KEY_KPPLUS     0x57
#define KEY_KPENTER    0x58
#define KEY_KP1   0x59
#define KEY_KP2   0x5a
#define KEY_KP3   0x5b
#define KEY_KP4   0x5c
#define KEY_KP5   0x5d
#define KEY_KP6   0x5e
#define KEY_KP7   0x5f
#define KEY_KP8   0x60
#define KEY_KP9   0x61
#define KEY_KP0   0x62
#define KEY_KPDOT 0x63

#define KEY_102ND   0x64
#define KEY_COMPOSE 0x65
#define KEY_POWER   0x66
#define KEY_KPEQUAL 0x67

#define KEY_F13 0x68
#define KEY_F14 0x69
#define KEY_F15 0x6a
#define KEY_F16 0x6b
#define KEY_F17 0x6c
#define KEY_F18 0x6d
#define KEY_F19 0x6e
#define KEY_F20 0x6f
#define KEY_F21 0x70
#define KEY_F22 0x71
#define KEY_F23 0x72
#define KEY_F24 0x73

#define KEY_OPEN  0x74
#define KEY_HELP  0x75
#define KEY_PROPS 0x76
#define KEY_FRONT 0x77
#define KEY_STOP  0x78
#define KEY_AGAIN 0x79
#define KEY_UNDO  0x7a
#define KEY_CUT   0x7b
#define KEY_COPY  0x7c
#define KEY_PASTE 0x7d
#define KEY_FIND  0x7e
#define KEY_MUTE  0x7f
#define KEY_VOLUMEUP   0x80
#define KEY_VOLUMEDOWN 0x81

#define KEY_KPCOMMA 0x85

#define KEY_RO        0x87
#define KEY_KATAKANAHIRAGANA 0x88
#define KEY_YEN       0x89
#define KEY_HENKAN    0x8a
#define KEY_MUHENKAN  0x8b
#define KEY_KPJPCOMMA 0x8c

#define KEY_HANGEUL  0x90
#define KEY_HANJA    0x91
#define KEY_KATAKANA 0x92
#define KEY_HIRAGANA 0x93
#define KEY_ZENKAKUHANKAKU 0x94

#define KEY_KPLEFTPAREN  0xb6
#define KEY_KPRIGHTPAREN 0xb7

#define KEY_LEFTCTRL   0xe0
#define KEY_LEFTSHIFT  0xe1
#define KEY_LEFTALT    0xe2
#define KEY_LEFTMETA   0xe3
#define KEY_RIGHTCTRL  0xe4
#define KEY_RIGHTSHIFT 0xe5
#define KEY_RIGHTALT   0xe6
#define KEY_RIGHTMETA  0xe7

#define KEY_MEDIA_PLAYPAUSE  0xe8
#define KEY_MEDIA_STOPCD     0xe9
#define KEY_MEDIA_PREVIOUSSONG 0xea
#define KEY_MEDIA_NEXTSONG   0xeb
#define KEY_MEDIA_EJECTCD    0xec
#define KEY_MEDIA_VOLUMEUP   0xed
#define KEY_MEDIA_VOLUMEDOWN 0xee
#define KEY_MEDIA_MUTE       0xef
#define KEY_MEDIA_WWW        0xf0
#define KEY_MEDIA_BACK       0xf1
#define KEY_MEDIA_FORWARD    0xf2
#define KEY_MEDIA_STOP       0xf3
#define KEY_MEDIA_FIND       0xf4
#define KEY_MEDIA_SCROLLUP   0xf5
#define KEY_MEDIA_SCROLLDOWN 0xf6
#define KEY_MEDIA_EDIT       0xf7
#define KEY_MEDIA_SLEEP      0xf8
#define KEY_MEDIA_COFFEE     0xf9
#define KEY_MEDIA_REFRESH    0xfa
#define KEY_MEDIA_CALC       0xfb
