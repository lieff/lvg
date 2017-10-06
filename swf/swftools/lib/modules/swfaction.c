/* swfaction.c

   Actionscript generation and parsing routines

   Extension module for the rfxswf library.
   Part of the swftools package.

   Copyright (c) 2001 Matthias Kramm <kramm@quiss.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include "../rfxswf.h"

#define MAX_LOOKUP 1024   // make cross references in dumps

struct Action
{
    int version;
    char*name;
    U8 op;
    char*flags;
} static actions[] =
{
    /*
f: frame (word)
u: url (string)
t: target (string)
l: label (string)
C: constant pool header (word)
c: constant pool entry (string)
s: skip (byte) (number of actions)
m: method (byte) swf_GetUrl2:(0=none, 1=get, 2=post)/GotoFrame2:(1=play)
b: branch (word) (number of bytes)
p (push): type(byte), type=0:string, type=1:double
{: define function (name (string), num (word), params (num strings), codesize (word)
o: codesize (word) object (string)
r: register (byte)
 */
{3,"End", 0x00, ""},
{3,"GotoFrame", 0x81, "f"},
{4,"GotoFrame2", 0x9f, "m"}, // -1 (/Movieclip:3)
{3,"GetUrl", 0x83, "ul"},
{4,"GetUrl2", 0x9a, "m"}, //-2
{3,"NextFrame", 0x04, ""},
{3,"PreviousFrame", 0x05, ""},
{3,"Play", 0x06, ""},
{3,"Stop", 0x07, ""},
{3,"ToggleQuality", 0x08, ""},
{3,"StopSounds", 0x09, ""},
{3,"WaitForFrame", 0x8a, "fs"},
{4,"WaitForFrame2", 0x8d, "s"}, // -1
{3,"SetTarget", 0x8b, "t"},
{4,"SetTarget2", 0x20, ""}, //-1
{3,"GotoLabel", 0x8c, "l"},
{4,"Add", 0x0a, ""}, //  -2, +1
{4,"Multiply", 0x0c, ""}, //  -2, +1
{4,"Divide", 0x0d, ""}, //  -2, +1
{4,"Subtract", 0x0b, ""}, //  -2, +1
{4,"Less", 0x0f, ""}, //  -2, +1
{4,"Equals", 0x0e, ""}, //  -2, +1
{4,"And", 0x10, ""}, //  -2, +1
{4,"Or", 0x11, ""}, //  -2, +1
{4,"Not", 0x12, ""}, //  -1, +1
{4,"StringAdd", 0x21, ""}, // -2,+1
{4,"StringLength", 0x14, ""}, // -1, +1
{4,"MBStringLength", 0x31, ""}, // -1, +1
{4,"StringEquals", 0x13, ""}, // -2, +1
{4,"StringLess", 0x29, ""}, //-2, +1
{4,"StringExtract", 0x15, ""}, // -3, +1
{4,"MBStringExtract", 0x35, ""}, //-3 +1
{4,"Push", 0x96, "p"}, //  +1
{4,"Pop", 0x17, ""}, //  -1
{4,"ToInteger", 0x18, ""}, // -1, +1
{4,"CharToAscii", 0x32, ""}, // -1, +1
{4,"AsciiToChar", 0x33, ""}, // -1, +1
{4,"MBCharToAscii", 0x36, ""}, // -1, +1
{4,"MBAsciiToChar", 0x37, ""}, // -1, +1
{4,"Jump", 0x99, "b"},
{4,"If", 0x9d, "b"}, // -1
{4,"Call", 0x9e, ""}, //-1 (frame label/number) (high bit is wrong.)
{4,"GetVariable", 0x1c,""}, // -1, +1
{4,"SetVariable", 0x1d,""}, // -2
{4,"GetProperty", 0x22,""}, //-2, +1
{4,"SetProperty", 0x23, ""}, // -3
{4,"RemoveSprite", 0x25, ""}, //-1
{4,"StartDrag", 0x27, ""}, // -2, -1, (-4)
{4,"EndDrag", 0x28, ""},
{4,"CloneSprite", 0x24, ""}, // -3
{4,"Trace", 0x26, ""}, //-1
{4,"GetTime", 0x34, ""}, //+1
{4,"RandomNumber", 0x30, ""}, //-1,+1
{5,"Modulo", 0x3f,""},
{5,"BitAnd", 0x60,""},
{5,"BitLShift", 0x63,""},
{5,"BitOr", 0x61,""},
{5,"BitRShift", 0x64,""},
{5,"BitURShift", 0x65,""},
{5,"BitXor", 0x62,""},//66?
{5,"Decrement", 0x51,""},
{5,"Increment", 0x50,""},
{5,"PushDuplicate", 0x4c,""},
{5,"StackSwap", 0x4d,""}, //?
{5,"StoreRegister", 0x87,"r"},
{5,"CallFunction", 0x3d,""},
{5,"DefineFunction", 0x9b, "{"},
{5,"Return", 0x3e,""},
{5,"GetMember", 0x4e,""},
{5,"SetMember", 0x4f,""},
{5,"CallMethod", 0x52,""},
{5,"Constantpool", 0x88, "Cc"},
{5,"DefineLocal", 0x3c,""},
{5,"DefineLocal2", 0x41,""},
{5,"Makehash", 0x43, ""}, //??
{5,"Delete", 0x3a,""}, //?
{5,"Delete2", 0x3b,""},
{5,"Enumerate", 0x46,""},
{5,"Equals2", 0x49,""},
{5,"InitArray", 0x42,""}, // InitObject?
{5,"NewMethod", 0x53,""}, //?
{5,"NewObject", 0x40,""},
{5,"TargetPath", 0x45,""}, //?
{5,"With", 0x94, "o"},
{5,"ToNumber", 0x4a,""}, //?
{5,"ToString", 0x4b,""}, //?
{5,"TypeOf", 0x44,""},
{5,"Add2", 0x47,""},
{5,"Less2", 0x48,""},
{6,"Greater", 0x67,""},
{6,"StringGreater", 0x68,""},
{6,"Enumerate2", 0x55,""},
{6,"InstanceOf", 0x54,""},
{6,"StrictEquals", 0x66,""},
{7,"DefineFunction2", 0x8e, "{"}
};
static int definedactions = sizeof(actions)/sizeof(struct Action);

ActionTAG* swf_ActionGet(TAG*tag)
{
    U8 op = 1;
    int length;
    ActionTAG tmp;
    ActionTAG*action = &tmp;
    U8*data;
    while(op)
    {
        action->next = (ActionTAG*)calloc(1, sizeof(ActionTAG));
        action->next->prev = action;
        action->next->next = 0;
        action->next->parent = tmp.next;
        action = action->next;

        op = swf_GetU8(tag);
        if(op<0x80)
            length = 0;
        else
            length = swf_GetU16(tag);

        if(length) {
            data = (U8*)malloc(length);
            swf_GetBlock(tag, data, length);
        } else {
            data = 0;
        }
        action->op = op;
        action->len = length;
        action->data = data;
    }
    return tmp.next;
}

void swf_ActionFree(ActionTAG*action)
{
    if(!action) {
        return;
    }
    action = action->parent;
    if(!action) {
#ifdef _DEBUG
        printf("Warning: freeing zero action (no parent)");
#endif
        return;
    }

    while(action)
    {
        ActionTAG*tmp;
        if(action->data && action->data != action->tmp) {
            free(action->data);
            action->data = 0;
        }
        action->len = 0;

        tmp = action;
        action=action->next;
        free(tmp);
    }
}

void swf_ActionSet(TAG*tag, ActionTAG*action)
{
    if(!action) {
        return;
    }
    action=action->parent;
    while(action)
    {
        swf_SetU8(tag, action->op);
        if(action->op & 128)
            swf_SetU16(tag, action->len);

        swf_SetBlock(tag, action->data, action->len);

        action = action->next;
    }
}

int OpAdvance(ActionTAG *atag, char c, U8*data)
{
    switch (c)
    {
    case 'f':
        return 2;
    case 'u':
        return strlen((const char*)data)+1;
    case 't':
        return strlen((const char*)data)+1;
    case 'l':
        return strlen((const char*)data)+1;
    case 'c':
        return strlen((const char*)data)+1;
    case 'C':
        return 2;
    case 's':
        return 1;
    case 'm':
        return 1;
    case 'b':
        return 2;
    case 'r':
        return 1;
    case 'p': {
        U8 type = *data++;
        if(type == 0) {
            return 1+strlen((const char*)data)+1; //string
        } else if (type == 1) {
            return 1+4; //float
        } else if (type == 2) {
            return 1+0; //NULL
        } else if (type == 3) {
            return 1+0; //Undefined
        } else if (type == 4) {
            return 1+1; //register
        } else if (type == 5) {
            return 1+1; //bool
        } else if (type == 6) {
            return 1+8; //double
        } else if (type == 7) {
            return 1+4; //int
        } else if (type == 8) {
            return 1+1; //lookup
        } else if (type == 9) {
            return 1+2; //lookup 16
        } else return 1;
        break;
    }
    case 'o': {
        return 2;
    }
    case '{': {
        U16 num;
        U16 codesize;
        U8* odata = data;
        int t;
        while(*data++); //name
        num = (*data++); //num
        num += (*data++)*256;
        if(atag->op == 0x8e/*DefineFunction2*/) {
            data += 3; // RegisterCount and flags
        }
        for(t=0;t<num;t++)
        {
            if(atag->op == 0x8e/*DefineFunction2*/)
                data++; // Register
            while(*data++); //param
        }
        codesize = (*data++); //num
        codesize += (*data++)*256;
        return data-odata;
    }
    }
    return 0;
}
#define ATAG_FULLLENGTH(atag) ((atag)->len + 1 + ((atag)->op&0x80?2:0))
#define MAX_LEVELS 16
/* TODO: * this should be in swfdump.c */
void swf_DumpActions(ActionTAG*atag, char*prefix)
{
    int t;
    U8*data;
    char* cp;
    int entry = 0;
    char spaces[MAX_LEVELS*4+1];
    struct {
        char*text;
        int count;
    } counter[MAX_LEVELS];
    int countpos = 0;
#ifdef MAX_LOOKUP
    char * lookup[MAX_LOOKUP];
    memset(lookup,0x00,sizeof(lookup));
#endif
    memset(spaces, 32, sizeof(spaces));
    spaces[sizeof(spaces)-1] = 0;

    if (!prefix)
        prefix="";

    while(atag)
    {
        char*indent = &spaces[sizeof(spaces)-1-countpos*4];
        U16 poollen = 0;
        for(t=0;t<definedactions;t++)
            if(actions[t].op == atag->op)
                break;

        if(t==definedactions) {
            printf("%s (%5d bytes) action:%s unknown[%02x]", prefix, atag->len, indent, atag->op);
        } else {
            printf("%s (%5d bytes) action:%s %s", prefix, atag->len, indent, actions[t].name);
        }
        data = atag->data;
        if(atag->len && t!=definedactions) //TODO: check for consistency: should we have a length?
        {
            cp = actions[t].flags;
            while(*cp)
            {
                switch(*cp)
                {
                case 'f': { //frame
                    printf(" %d", data[0]+256*data[1]);
                } break;
                case 'u': {
                    printf(" URL:\"%s\"", data);
                } break;
                case 't': {
                    printf(" Target:\"%s\"", data);
                } break;
                case 'l': {
                    printf(" Label:\"%s\"", data);
                } break;
                case 'c': {
                    printf(" String:\"%s\"", data);
#ifdef MAX_LOOKUP
                    if (entry<MAX_LOOKUP)
                        lookup[entry++] = strdup((const char*)data);
#endif
                } break;
                case 'C': {
                    poollen = data[0]+256*data[1];
                    entry = 0;
                    printf("(%d entries)", poollen);
                } break;
                case 's': {
                    printf(" +%d", *data);
                } break;
                case 'm': {
                    //m: method (byte) url:(0=none, 1=get, 2=datat)/gf2:(1=play)
                    printf(" %d", *data);
                } break;
                case '{': {
                    U16 num;
                    U16 codesize;
                    int s = 0;
                    int t;
                    printf(" %s(", data);
                    while(data[s++]); //name
                    num = (data[s++]); //num
                    num += (data[s++])*256;
                    if(atag->op == 0x8e/*DefineFunction2*/) {
                        s += 3; // RegisterCount and flags
                    }
                    for(t=0;t<num;t++) {
                        if(atag->op == 0x8e/*DefineFunction2*/)
                            s++; // Register
                        printf("%s",data+s);  // 10/22/04 MD: added +s to
                        if(t<num-1)
                            printf(", ");
                        while(data[s++]); //param
                    }
                    printf(")");
                    codesize = (data[s++]); //num
                    codesize += (data[s++])*256;
                    printf(" codesize:%d ",codesize);
                    printf("\n%s                       %s{", prefix, indent);
                    if(countpos>=15) {
                        printf("Error: nested too deep\n");
                        continue;
                    }
                    counter[countpos].text = "}";
                    counter[countpos].count = codesize + ATAG_FULLLENGTH(atag);
                    countpos++;
                } break;
                case 'o': {
                    int t;
                    U16 codesize = data[0]+256*data[1];
                    printf(" codesize:%d ", codesize);

                    /* the following tries to find the "string"
                       the flash documentation speaks of- I've
                       never actually seen one yet. -mk */
                    for(t=2;t<atag->len;t++)
                        printf("[%02x]", atag->data[t]);

                    printf("\n%s                       %s{", prefix, indent);
                    if(countpos>=15) {
                        printf("Error: nested too deep\n");
                        continue;
                    }
                    counter[countpos].text = "}";
                    counter[countpos].count = codesize + ATAG_FULLLENGTH(atag);
                    countpos++;
                } break;
                case 'b': {
                    printf(" %d", data[0]+256*(signed char)data[1]);
                } break;
                case 'r': {
                    printf(" %d", data[0]);
                } break;
                case 'p': {
                    U8 type = *data;
                    unsigned char*value = data+1;
                    if(type == 0) {
                        printf(" String:\"%s\"", value);
                    } else if (type == 1) {
                        union {
                            U32 i;
                            float f;
                        } u;
                        u.i = value[0] + (value[1] << 8)+
                             (value[2] << 16) + (value[3] << 24);
                        printf(" Float:%f", u.f);
                    } else if (type == 2) {
                        printf(" NULL");
                    } else if (type == 3) {
                        printf(" Undefined");
                    } else if (type == 4) {
                        printf(" register:%d", *value);
                    } else if (type == 5) {
                        printf(" bool:%s", *value?"true":"false");
                    } else if (type == 6) {
                        union {
                            U8 a[8];
                            double d;
                        } u;
                        memcpy(&u.a[4], value, 4);
                        memcpy(u.a, &value[4], 4);
#ifdef WORDS_BIGENDIAN
                        int t;
                        for(t=0;t<4;t++) {
                            U8 tmp = u.a[t];
                            u.a[t] = u.a[7-t];
                            u.a[7-t] = tmp;
                        }
#endif
                        printf(" double:%f", u.d);
                    } else if (type == 7) {
                        printf(" int:%d", value[0]+(value[1]<<8)+
                                (value[2]<<16)+(value[3]<<24));
                    } else if (type == 8) {
                        printf(" Lookup:%d", *value);
#ifdef MAX_LOOKUP
                        if (lookup[*value])
                            printf(" (\"%s\")",lookup[*value]);
#endif
                    } else if (type == 9) {
                        U32 offset = value[0]+(value[1]<<8);
                        printf(" Lookup16:%d", offset);
#ifdef MAX_LOOKUP
                        if (lookup[offset])
                            printf(" (\"%s\")",lookup[offset]);
#endif
                    } else {
                        printf(" UNKNOWN[%02x]",type);
                    }
                } break;
                }
                data += OpAdvance(atag, *cp, data);
                if((*cp!='c' || !poollen) &&
                        (*cp!='p' || !(data<&atag->data[atag->len])))
                    cp++;
                if(poollen)
                    poollen--;
            }
        }

        if(data < atag->data + atag->len)
        {
            int nl = ((atag->data+atag->len)-data);
            int t;
            printf(" (remainder of %d bytes:\"", nl);
            for(t=0;t<nl;t++) {
                if(data[t]<32)
                    printf("\\%d",data[t]);
                else
                    printf("%c", data[t]);
            }
            printf("\")");
        }
        printf("\n");

        for(t=0;t<countpos;t++) {
            counter[t].count -= ATAG_FULLLENGTH(atag);
            if(counter[t].count < 0) {
                printf("===== Error: Oplength errors =====\n");
                countpos = 0;
                break;
            }
        }

        while(countpos && !counter[countpos-1].count)
        {
            printf("%s                   %s%s\n",
                   prefix, indent, counter[countpos-1].text);
            indent += 4;
            countpos--;
        }

        atag = atag->next;
    }

#ifdef MAX_LOOKUP
    for (t=0;t<MAX_LOOKUP;t++) if (lookup[t]) free(lookup[t]);
#endif
}

static const char TYPE_URL = 1;
static const char TYPE_TARGET = 2;
static const char TYPE_STRING = 4;

int swf_ActionEnumerate(ActionTAG*atag, char*(*callback)(char*), int type)
{
    int t;
    U8*data;
    char* cp;
    int count = 0;
    while(atag)
    {
        U16 poollen = 0;
        for(t=0;t<definedactions;t++)
            if(actions[t].op == atag->op)
                break;

        if(t==definedactions) {
            // unknown actiontag
            atag = atag->next;
            count++;
            continue;
        }
        cp = actions[t].flags;
        data = atag->data;
        if(atag->len) {
            while(*cp) {
                U8 * replacepos = 0;
                int replacelen = 0;
                U8 * replacement = 0;
                switch(*cp)
                {
                case 'u': {
                    if(type&TYPE_URL)
                    {
                        replacelen = strlen((const char*)data);
                        replacepos = data;
                        replacement = (U8*)callback((char*)data); // may be null
                    }
                } break;
                case 't': {
                    if(type&TYPE_TARGET)
                    {
                        replacelen = strlen((const char*)data);
                        replacepos = data;
                        replacement = (U8*)callback((char*)data); // may be null
                    }
                } break;
                case 'c': {
                    if(type&TYPE_STRING)
                    {
                        replacelen = strlen((const char*)data);
                        replacepos = data;
                        replacement = (U8*)callback((char*)data); // may be null
                    }
                } break;
                case 'C': {
                    poollen = (data[0]+256*data[1]);
                } break;
                case 'o': {
                } break;
                case 'p': {
                    U8 datatype = *data;
                    char*value = (char*)&data[1];
                    if(datatype == 0) { //string
                        if(type&TYPE_STRING)
                        {
                            replacelen = strlen(value);
                            replacepos = (U8*)value;
                            replacement = (U8*)callback(value); // may be null
                        }
                    } else if (datatype == 8) { //lookup
                    }
                } break;
                }
                data += OpAdvance(atag, *cp, data);
                if(*cp!='c' || !poollen)
                    cp++;
                if(poollen)
                    poollen--;

                if(replacement)
                {
                    int newlen = strlen((const char *)replacement);
                    char * newdata = (char*)malloc(atag->len - replacelen + newlen);
                    int rpos = replacepos - atag->data;
                    memcpy(newdata, atag->data, rpos);
                    memcpy(&newdata[rpos], replacement, newlen);
                    memcpy(&newdata[rpos+newlen], &replacepos[replacelen],
                            &data[atag->len] - &replacepos[replacelen]);
                    free(atag->data);
                    atag->data = (U8*)newdata;
                    data = &atag->data[rpos+newlen+1];
                }
            }
        }
        atag = atag->next;
        count ++;
    }
    return count;
}

void swf_ActionEnumerateTargets(ActionTAG*atag, char*(*callback)(char*))
{
    swf_ActionEnumerate(atag, callback, TYPE_TARGET);
}
void swf_ActionEnumerateStrings(ActionTAG*atag, char*(*callback)(char*))
{
    swf_ActionEnumerate(atag, callback, TYPE_STRING);
}
void swf_ActionEnumerateURLs(ActionTAG*atag, char*(*callback)(char*))
{
    swf_ActionEnumerate(atag, callback, TYPE_URL);
}
