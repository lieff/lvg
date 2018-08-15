#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include "glad.h"
#include "jfes/jfes.h"
#include "gl.h"
#include "minishadertoy.h"
#include "src/stb_image.h"
#include "src/lvg_header.h"

static const char *shader_header =
    "#version 300 es\n"
    "#extension GL_EXT_shader_texture_lod : enable\n"
    "#extension GL_OES_standard_derivatives : enable\n"
    "#define iGlobalTime iTime\n"
    "precision highp float;\n"
    "precision highp int;\n"
    "precision highp sampler2D;\n"
    "out vec4 fragmentColor;\n"
    "uniform vec3      iResolution;\n"
    "uniform float     iTime;\n"
    "uniform float     iTimeDelta;\n"
    "uniform int       iFrame;\n"
    "uniform float     iChannelTime[4];\n"
    "uniform vec3      iChannelResolution[4];\n"
    "uniform vec4      iMouse;\n"
    "uniform vec4      iDate;\n"
    "uniform float     iSampleRate;\n"
    "uniform sampler%s iChannel0;\n"
    "uniform sampler%s iChannel1;\n"
    "uniform sampler%s iChannel2;\n"
    "uniform sampler%s iChannel3;\n";

static const char *shader_footer =
    "\nvoid main(void) {\n"
    "    vec4 color = vec4(0.0,0.0,0.0,1.0);\n"
    "    mainImage(color, gl_FragCoord.xy);\n"
    "    color.w = 1.0;\n"
    "    fragmentColor = color;\n"
    "}\n";


static int codepoint_utf8(uint32_t codepoint, char **buf)
{
    int len = 0;
    char *p = *buf;
    if (codepoint >= 0x110000u)
        return 0;
    if (codepoint < 0x80u)
    {
        p[len++] = (char)codepoint;
    } else if (codepoint < 0x800u)
    {
        p[len++] = (char)(0xC0 | (codepoint >> 6 & 0x1F));
        p[len++] = (char)(0x80 | (codepoint & 0x3F));
    } else if (codepoint < 0x10000u)
    {
        p[len++] = (char)(0xE0 | (codepoint >> 12 & 0xF));
        p[len++] = (char)(0x80 | (codepoint >> 6 & 0x3F));
        p[len++] = (char)(0x80 | (codepoint & 0x3F));
    } else
    {
        p[len++] = (char)(0xF0 | (codepoint >> 18 & 0x7));
        p[len++] = (char)(0x80 | (codepoint >> 12 & 0x3F));
        p[len++] = (char)(0x80 | (codepoint >> 6 & 0x3F));
        p[len++] = (char)(0x80 | (codepoint & 0x3F));
    }
    return len;
}

static int unescape_json(char *buf, int buf_len, char *out_buf)
{
    enum {
        JSON_INITIAL,
        JSON_ESCAPE,
        JSON_UNICODE,
        JSON_UTF16_I1,
        JSON_UTF16_I2,
        JSON_UTF16_LS
    };
    int i, ubuf_offset, len = 0, state = JSON_INITIAL;
    uint32_t u1, codepoint;
    char ch, *p = out_buf, *pt;
    char ubuf[5];
    ubuf[4] = 0;
    for (i = 0; i < buf_len; i++)
    {
        ch = buf[i];
        if (state == JSON_INITIAL)
        {
            if (ch != '\\')
                p[len++] = ch;
            else
                state = JSON_ESCAPE;
        } else if (state == JSON_ESCAPE || state == JSON_UTF16_I2)
        {
            int old_state = state;
            state = JSON_INITIAL;
            switch(ch)
            {
                case 'n': p[len++] = '\n'; break;
                case 't': p[len++] = '\t'; break;
                case 'r': p[len++] = '\r'; break;
                case 'b': p[len++] = '\b'; break;
                case 'f': p[len++] = '\f'; break;
                case 'u': state = (JSON_ESCAPE == old_state) ? JSON_UNICODE : JSON_UTF16_LS; ubuf_offset = 0; break;
                default: p[len++] = ch; break;
            }
        } else if (state == JSON_UNICODE)
        {
            ubuf[ubuf_offset++] = ch;
            if (ubuf_offset == 4)
            {
                codepoint = strtol(ubuf, NULL, 16);
                if ((codepoint & 0xFC00u) == 0xD800u)
                {
                    u1 = codepoint;
                    state = JSON_UTF16_I1;
                } else
                {
                    pt = &p[len];
                    len += codepoint_utf8(codepoint, &pt);
                    state = JSON_INITIAL;
                }
            }
        } else if (state == JSON_UTF16_I1)
        {
            if (ch != '\\')
            {
                p[len++] = ch;
                state = JSON_INITIAL;
            } else
                state = JSON_UTF16_I2;
        } else if (state == JSON_UTF16_LS)
        {
            ubuf[ubuf_offset++] = ch;
            if (ubuf_offset == 4)
            {
                codepoint = strtol(ubuf, NULL, 16);
                if ((codepoint & 0xFC00u) == 0xDC00u)
                    codepoint = ((u1 & 0x3FFu) << 10 | (codepoint & 0x3FFu)) + 0x10000u;
                pt = &p[len];
                len += codepoint_utf8(codepoint, &pt);
                state = JSON_INITIAL;
            }
        }
    }
    p[len] = 0;
    return len;
}

static void CheckGLErrors(const char *func, int line)
{
    int lasterror = glGetError();
    if (lasterror)
    {
        printf("OpenGL error in %s, %i: err=%i\n", func, line, lasterror); fflush(stdout);
    }
}

static void load_image(const char *data, int len, SHADER_INPUT *inp, int is_cubemap)
{
    int n;
    SAMPLER *s = &inp->sampler;
    stbi_set_flip_vertically_on_load(s ? s->vflip : 0);
    unsigned char *pix = stbi_load_from_memory((const stbi_uc *)data, len, &inp->w, &inp->h, &n, 4);
    if (!pix)
        return;
    glGenTextures(1, &inp->tex); GLCHK;
    int tgt = is_cubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
    glBindTexture(tgt, inp->tex); GLCHK;
#ifndef USE_GLES3
    glTexParameteri(tgt, GL_GENERATE_MIPMAP, GL_TRUE); GLCHK;
#endif
    int clamp = GL_CLAMP_TO_EDGE, min_filter = GL_LINEAR_MIPMAP_LINEAR, mag_filter = GL_LINEAR;
    if (s)
    {
        clamp = (s->wrap && !is_cubemap) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
        if (!s->filter)
        {
            min_filter = GL_NEAREST;
            mag_filter = GL_NEAREST;
        } else if (1 == s->filter)
            min_filter = GL_LINEAR;
    }
    glTexParameteri(tgt, GL_TEXTURE_MIN_FILTER, min_filter); GLCHK;
    glTexParameteri(tgt, GL_TEXTURE_MAG_FILTER, mag_filter); GLCHK;
    glTexParameteri(tgt, GL_TEXTURE_WRAP_S, clamp); GLCHK;
    glTexParameteri(tgt, GL_TEXTURE_WRAP_T, clamp); GLCHK;
    if (is_cubemap)
    {
        for (int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, inp->w, inp->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix); GLCHK;
        }
    } else
        glTexImage2D(tgt, 0, GL_RGBA8, inp->w, inp->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix); GLCHK;
#ifdef USE_GLES3
    glGenerateMipmap(tgt); GLCHK;
#endif
    glBindTexture(tgt, 0); GLCHK;
}

static void update_cubemap(const char *data, int len, SHADER_INPUT *inp, int i)
{
    int n, w, h;
    SAMPLER *s = &inp->sampler;
    stbi_set_flip_vertically_on_load(s ? s->vflip : 0);
    unsigned char *pix = stbi_load_from_memory((const stbi_uc *)data, len, &w, &h, &n, 4);
    if (!pix)
        return;
    assert(inp->w == w && inp->h == h);
    glBindTexture(GL_TEXTURE_CUBE_MAP, inp->tex); GLCHK;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, inp->w, inp->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix); GLCHK;
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); GLCHK;
}

void fb_delete(FBO *f)
{
    if (f->framebuffer)
        glDeleteFramebuffers(1, &f->framebuffer); GLCHK;
    if (f->framebufferTex)
        glDeleteTextures(1, &f->framebufferTex); GLCHK;
}

void fb_init(FBO *f, int width, int height, int float_tex)
{
    f->floatTex = float_tex;
    glGenFramebuffers(1, &f->framebuffer); GLCHK;
    glBindFramebuffer(GL_FRAMEBUFFER, f->framebuffer); GLCHK;
    glGenTextures(1, &f->framebufferTex); GLCHK;
    glBindTexture(GL_TEXTURE_2D, f->framebufferTex); GLCHK;
    if (f->floatTex)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, 0);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    GLCHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); GLCHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GLCHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); GLCHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); GLCHK;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, f->framebufferTex, 0); GLCHK;
    glBindTexture(GL_TEXTURE_2D, 0); GLCHK;
    glBindFramebuffer(GL_FRAMEBUFFER, 0); GLCHK;
}

void shader_delete(SHADER *s)
{
    if (s->shader)
        glDeleteShader(s->shader);
    if (s->prog)
        glDeleteProgram(s->prog);
}

int shader_init(SHADER *s, const char *pCode, const char *pCommonCode/*, int is_compute*/)
{
    char header[1024];
    snprintf(header, sizeof(header), shader_header, s->inputs[0].is_cubemap ? "Cube" : "2D",
        s->inputs[1].is_cubemap ? "Cube" : "2D", s->inputs[2].is_cubemap ? "Cube" : "2D", s->inputs[3].is_cubemap ? "Cube" : "2D");
    size_t hdr_len = strlen(header);
    size_t source_len = strlen(pCode);
    size_t common_len = pCommonCode ? strlen(pCommonCode) : 0;
    size_t footer_len = strlen(shader_footer);
    GLchar *sh = (GLchar *)malloc(hdr_len + source_len + common_len + footer_len + 1);
    GLchar *psh = sh;
    memcpy(psh, header, hdr_len); psh += hdr_len;
    if (pCommonCode)
    {
        memcpy(psh, pCommonCode, common_len); psh += common_len;
    }
    memcpy(psh, pCode, source_len); psh += source_len;
    *psh = 0;
    if (!strstr(sh, "void main(") && !strstr(sh, "void main "))
    {
        memcpy(psh, shader_footer, footer_len); psh += footer_len;
        *psh = 0;
    }

    s->prog = glCreateProgram(); GLCHK;
    s->shader = glCreateShader(/*is_compute ? GL_COMPUTE_SHADER : */GL_FRAGMENT_SHADER); GLCHK;
    glShaderSource(s->shader, 1, (const GLchar **)&sh, 0); GLCHK;
    glCompileShader(s->shader); GLCHK;
#ifdef _DEBUG
    GLint isCompiled = 0;
    glGetShaderiv(s->shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(s->shader, GL_INFO_LOG_LENGTH, &maxLength);
        GLchar *errorLog = (GLchar *)malloc(maxLength);
        glGetShaderInfoLog(s->shader, maxLength, &maxLength, &errorLog[0]);
        printf("compile error: %s", errorLog);
        printf("code: %s", sh);
        exit(1);
    }
#endif
    free(sh);
    glAttachShader(s->prog, s->shader); GLCHK;
    glLinkProgram(s->prog); GLCHK;
#ifdef _DEBUG
    GLint isLinked = 0;
    glGetProgramiv(s->prog, GL_LINK_STATUS, &isLinked); GLCHK;
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(s->prog, GL_INFO_LOG_LENGTH, &maxLength); GLCHK;
        GLchar *errorLog = (GLchar *)malloc(maxLength);
        glGetProgramInfoLog(s->prog, maxLength, &maxLength, &errorLog[0]); GLCHK;
        printf("link error: %s", errorLog);
        exit(1);
    }
#endif
    s->iResolution = glGetUniformLocation(s->prog, "iResolution"); GLCHK;
    s->iTime       = glGetUniformLocation(s->prog, "iTime"); GLCHK;
    s->iTimeDelta  = glGetUniformLocation(s->prog, "iTimeDelta"); GLCHK;
    s->iFrame      = glGetUniformLocation(s->prog, "iFrame"); GLCHK;
    s->iMouse      = glGetUniformLocation(s->prog, "iMouse"); GLCHK;
    s->iDate       = glGetUniformLocation(s->prog, "iDate"); GLCHK;
    s->iSampleRate = glGetUniformLocation(s->prog, "iSampleRate"); GLCHK;
    for (int i = 0; i < 4; i++)
    {
        char buf[64];
        sprintf(buf, "iChannel%d", i);
        s->iChannel[i] = glGetUniformLocation(s->prog, buf); GLCHK;
        sprintf(buf, "iChannelTime[%d]", i);
        s->iChannelTime[i] = glGetUniformLocation(s->prog, buf); GLCHK;
        sprintf(buf, "iChannelResolution[%d]", i);
        s->iChannelResolution[i] = glGetUniformLocation(s->prog, buf); GLCHK;
    }
    return 1;
}

void shadertoy_renderpass(SHADER *s, PLATFORM_PARAMS *p)
{
    glUseProgram(s->prog); GLCHK;
    glUniform3f(s->iResolution, (float)p->winWidth, (float)p->winHeight, 1.0f); GLCHK;
    glUniform1f(s->iTime, p->cur_time); GLCHK;
    glUniform1f(s->iTimeDelta, p->cur_time - p->time_last); GLCHK;
    glUniform1i(s->iFrame, p->frame++); GLCHK;
    if(p->cx > -0.5f)
        glUniform4f(s->iMouse, p->mx, p->my, p->cx, p->cy); GLCHK;
    glUniform4f(s->iDate, p->tm->tm_year, p->tm->tm_mon, p->tm->tm_mday, p->tm->tm_hour*3600 + p->tm->tm_min*60 + p->tm->tm_sec); GLCHK;
    glUniform1f(s->iSampleRate, 0); GLCHK;

    glActiveTexture(GL_TEXTURE0); GLCHK;
    glBindTexture(GL_TEXTURE_2D, 0); GLCHK;
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f); GLCHK;
    for (int i = 0, tu = 1; i < 4; i++)
    {
        glUniform1f(s->iChannelTime[i], p->cur_time); GLCHK;
        //int w = 0, h = 0;
        if (s->inputs[i].tex)
        {
            int tgt = s->inputs[i].is_cubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
            glActiveTexture(GL_TEXTURE0 + tu); GLCHK;
            glBindTexture(tgt, s->inputs[i].tex); GLCHK;
            glUniform1i(s->iChannel[i], tu); GLCHK;
            tu++;
        } else
            glUniform1i(s->iChannel[i], 0); GLCHK;
        glUniform3f(s->iChannelResolution[i], s->inputs[i].w, s->inputs[i].h, 1.0f); GLCHK;
    }

    glRecti(1, 1, -1, -1); GLCHK;
    glUseProgram(0); GLCHK;
}

static int switch_val(jfes_value_t *str, const char **vals)
{
    for (int i = 0; *vals; i++, vals++)
        if (!strcmp(*vals, str->data.string_val.data))
            return i;
    perror(str->data.string_val.data);
    assert(0);
    return -1;
}

int load_json(SHADER *shaders, char *buffer, int buf_size)
{
    jfes_config_t config;
    config.jfes_malloc = (jfes_malloc_t)malloc;
    config.jfes_free = free;

    jfes_value_t value;
    jfes_status_t status = jfes_parse_to_value(&config, buffer, buf_size, &value);
    if (!jfes_status_is_good(status))
       return 1;
    jfes_value_t *root = value.data.array_val->items[0];
    jfes_value_t *rp = jfes_get_child(root, "renderpass", 0);

    char *common_code = 0;
    for (int i = 0; i < rp->data.array_val->count; i++)
    {
        SHADER *s = &shaders[i];
        jfes_value_t *pass = rp->data.array_val->items[i];
        jfes_value_t *type = jfes_get_child(pass, "type", 0);
        static const char *rp_types[] = { "image", "common", "buffer", "cubemap", "sound", 0 };
        s->type = switch_val(type, rp_types);
        if (1 == s->type)
        {
            if (common_code)
            {
                printf("error: common code already exists.");
                exit(1);
            }
            jfes_value_t *code = jfes_get_child(pass, "code", 0);
            common_code = strdup(code->data.string_val.data);
            unescape_json(code->data.string_val.data, code->data.string_val.size, common_code);
        }
    }
    for (int i = 0; i < rp->data.array_val->count; i++)
    {
        SHADER *s = &shaders[i];
        jfes_value_t *pass = rp->data.array_val->items[i];
        jfes_value_t *inputs  = jfes_get_child(pass, "inputs", 0);
        jfes_value_t *outputs = jfes_get_child(pass, "outputs", 0);
        jfes_value_t *code    = jfes_get_child(pass, "code", 0);
        if (s->type)
            continue;
        int j;
        for (j = 0; j < inputs->data.array_val->count; j++)
        {
           static const char *types[] = { "texture", "buffer", "cubemap", "musicstream", "music", "keyboard", 0 };
           jfes_value_t *input = inputs->data.array_val->items[j];
           jfes_value_t *iid   = jfes_get_child(input, "id", 0);
           int itype = switch_val(jfes_get_child(input, "type", 0), types);
           jfes_value_t *ichannel = jfes_get_child(input, "channel", 0);
           jfes_value_t *filepath = jfes_get_child(input, "filepath", 0);
           jfes_value_t *sampler  = jfes_get_child(input, "sampler", 0);
           SHADER_INPUT *inp = s->inputs + ichannel->data.int_val;
           SAMPLER *smp = &inp->sampler;
           inp->id = iid->data.string_val.data;
           inp->is_cubemap = (2 == itype);
           if (sampler)
           {
              static const char *filter[] = { "nearest", "linear", "mipmap", 0 };
              static const char *wrap[]   = { "clamp", "repeat", 0 };
              static const char *bools[]  = { "false", "true", 0 };
              static const char *internal[] = { "byte", 0 };
              smp->filter = switch_val(jfes_get_child(sampler, "filter", 0), filter);
              smp->wrap   = switch_val(jfes_get_child(sampler, "wrap", 0), wrap);
              smp->vflip  = switch_val(jfes_get_child(sampler, "vflip", 0), bools);
              smp->srgb   = switch_val(jfes_get_child(sampler, "srgb", 0), bools);
              smp->internal = switch_val(jfes_get_child(sampler, "internal", 0), internal);
           }
           if (filepath && (0 == itype || inp->is_cubemap))
           {
                int components = inp->is_cubemap ? 6 : 1;
                char *buf = malloc(filepath->data.string_val.size + 26 + 2);
                for (j = 0; j < components; j++)
                {
                    strcpy(buf, "https://www.shadertoy.com");
                    unescape_json(filepath->data.string_val.data, filepath->data.string_val.size, buf + 25);
                    if (j)
                    {
                        char *s = strrchr(buf, '.');
                        if (s)
                        {
                            int len = strlen(s);
                            s[len + 2] = 0;
                            for (; len; len--)
                                s[len + 1] = s[len - 1];
                            s[0] = '_';
                            s[1] = '0' + j;
                        }
                    }
                    char *img = lvgGetFileContents(buf + 26, (uint32_t *)&buf_size);
#ifdef HAVE_CURL
                    if (!img)
                    {
                        img = load_url(buf, &buf_size, 0);
                        printf("load %s (%d bytes)\n", buf, buf_size);
                        mkpath(buf + 26);
                        FILE *f = fopen(buf + 26, "wb");
                        if (f)
                        {
                            fwrite(img, 1, buf_size, f);
                            fclose(f);
                        }
                    }
#endif
                     if (img)
                     {
                         if (0 == j)
                             load_image(img, buf_size, inp, inp->is_cubemap);
                         else
                             update_cubemap(img, buf_size, inp, j);
                         free(img);
                     }
                }
                free(buf);
           }
           //printf("i type=%d, id=%s, channel=%d\n", itype, inp->id, ichannel->data.int_val);
        }
        for (j = 0; j < outputs->data.array_val->count; j++)
        {
           jfes_value_t *output = outputs->data.array_val->items[j];
           jfes_value_t *oid    = jfes_get_child(output, "id", 0);
           //jfes_value_t *ochannel = jfes_get_child(output, "channel", 0);
           s->output.id = oid->data.string_val.data;
           //printf("o id=%s, channel=%d\n", s->output.id, ochannel->data.int_val);
        }
        //printf("type=%s\n", type->data.string_val.data);
        char *unesc_buf = strdup(code->data.string_val.data);
        unescape_json(code->data.string_val.data, code->data.string_val.size, unesc_buf);
        shader_init(s, unesc_buf, common_code);
        free(unesc_buf);
    }
    jfes_free_value(&config, &value);
    return 0;
}

/*int main(int argc, char **argv)
{
    SHADER shaders[5];
    memset(shaders, 0, sizeof(shaders));
    load_json(shaders, buffer, buf_size);

    PLATFORM_PARAMS p;
    memset(&p, 0, sizeof(p));
    p.cur_time = glfwGetTime() - time_start;
    time_t rawtime;
    time(&rawtime);
    p.tm = localtime(&rawtime);

    shadertoy_renderpass(&shaders[0], &p);
    p.time_last = p.cur_time;
    return 0;
}*/
