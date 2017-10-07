#include <assert.h>
#include "../types.h"
#include "../rfxswf.h"
#include "assets.h"

void swf_ResolveAssets(asset_resolver_t*assets, abc_file_t*file)
{
    int resolved = 0;
    int t;
    for(t=0;t<file->classes->num;t++) {
        abc_class_t*cls = (abc_class_t*)array_getvalue(file->classes, t);
        char*fullname = abc_class_fullname(cls);
        abc_asset_t*a = (abc_asset_t*)dict_lookup(assets->name2asset, fullname);
        if(a) {
            resolved++;
            cls->asset = a;
        }
        free(fullname);
    }
}

static void dump_asset(FILE*fo, abc_asset_t*a, const char*prefix)
{
    asset_tag_list_t*t = a->tags;
    while(t) {
        TAG*tag = t->asset_tag->tag;
        fprintf(fo, "%s[tag] %d defines ID %d\n", prefix, tag->id, swf_GetDefineID(tag));
        char*prefix2 = allocprintf("%s    ", prefix);
        int i;
        for(i=0;i<t->asset_tag->num_deps;i++) {
            asset_dependency_t*d = &t->asset_tag->deps[i];
            dump_asset(fo, d->asset, prefix2);
        }
        free(prefix2);
        t = t->next;
    }
}

void swf_DumpAsset(FILE*fo, abc_asset_t*asset, const char*prefix)
{
    dump_asset(fo, asset, prefix);
}
