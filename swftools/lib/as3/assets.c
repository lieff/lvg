#include <assert.h>
#include "../types.h"
#include "../rfxswf.h"
#include "assets.h"

static void add_dependencies(asset_resolver_t*assets, abc_asset_t*asset, asset_tag_t*atag)
{
    TAG*tag = atag->tag;
    atag->num_deps = swf_GetNumUsedIDs(tag);
    int*positions = malloc(sizeof(int)*atag->num_deps);
    atag->deps = malloc(atag->num_deps*sizeof(atag->deps[0]));
    swf_GetUsedIDs(tag, positions);
    int t;
    for(t=0;t<atag->num_deps;t++) {
        asset_dependency_t*d = &atag->deps[t];
        d->patch_pos = positions[t];
        U16 id = GET16(&tag->data[positions[t]]);
        d->asset = assets->id2asset[id];
        if(!d->asset) {
            fprintf(stderr, "Error: ID %d referenced, but not defined\n", id);
        }
    }
    free(positions);
}

asset_resolver_t* swf_ParseAssets(SWF*swf)
{
    NEW(asset_resolver_t,assets);
    assets->name2asset = dict_new2(&charptr_type);
    assets->id2asset = rfx_calloc(sizeof(abc_asset_t*)*65536);

    TAG*tag = swf->firstTag;
    while(tag) {
        if(swf_isDefiningTag(tag)) {
            NEW(abc_asset_t, asset);
            assets->id2asset[swf_GetDefineID(tag)] = asset;
        }
        tag = tag->next;
    }

    tag = swf->firstTag;
    while(tag) {
        if(swf_isDefiningTag(tag)) {
            abc_asset_t*asset = assets->id2asset[swf_GetDefineID(tag)];
            assert(asset);
            if(asset) {
                NEW(asset_tag_t,t);
                t->tag = tag;
                list_append(asset->tags, t);
                add_dependencies(assets, asset, t);
            }
        } else if(swf_isPseudoDefiningTag(tag)) {
            abc_asset_t*asset = assets->id2asset[swf_GetDefineID(tag)];
            if(asset) {
                NEW(asset_tag_t,t);
                t->tag = tag;
                list_append(asset->tags, t);
                add_dependencies(assets, asset, t);
            }
        } else if(tag->id == ST_SYMBOLCLASS) {
            int t, num = swf_GetU16(tag);
            for(t=0;t<num;t++) {
                U16 id = swf_GetU16(tag);
                if(!id) {
                    assets->mainclass_id = id;
                } else {
                    abc_asset_t*asset = assets->id2asset[id];
                    if(!asset) {
                        fprintf(stderr, "Error: ID %d referenced, but not defined.\n", id);
                    } else {
                        char*name = swf_GetString(tag);
                        dict_put(assets->name2asset, name, asset);
                    }
                }
            }
        }
        tag = tag->next;
    }
    return assets;
}

void swf_ResolveAssets(asset_resolver_t*assets, abc_file_t*file)
{
    int num = assets->name2asset->num;
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
        fprintf(fo, "%s[tag] %s defines ID %d\n", prefix, swf_TagGetName(tag), swf_GetDefineID(tag));
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
