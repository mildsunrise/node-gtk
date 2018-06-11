#ifndef GI_H
#define GI_H

#pragma once

#include <node.h>
#include <nan.h>
#include <girepository.h>
#include "boxed.h"


using v8::Object;
using v8::Local;
using v8::Value;


#define UTF8(s)         Nan::New<v8::String> (s).ToLocalChecked()
#define STRING(s)       Nan::New<v8::String> (s).ToLocalChecked()
#define RETURN(s)       info.GetReturnValue().Set(s)

namespace GNodeJS {

// GQuark object_quark (void) ;
// GQuark template_quark (void) ;
// G_DEFINE_QUARK(gnode_js_object,   object);
// G_DEFINE_QUARK(gnode_js_template, template);

class BaseInfo {
public:
    BaseInfo (GIBaseInfo *info) : _info(info) { };
    BaseInfo (Local<Value> value) {
        Local<Object> object = value.As<Object>();
        _info = g_base_info_ref(
                (GIBaseInfo *) GNodeJS::BoxedFromWrapper(object));
    };
    ~BaseInfo () {
        g_base_info_unref(_info);
    };

    inline GIBaseInfo * operator* () {
        return _info;
    }

    inline GIBaseInfo * info() {
        return _info;
    }

    inline GIInfoType type() {
        return g_base_info_get_type(_info);
    }

    inline GITypeTag tag() {
        return g_type_info_get_tag(_info);
    }

    inline const char* name() {
        return g_base_info_get_name(_info);
    }

    inline const char* ns() {
        return g_base_info_get_namespace(_info);
    }

private:
    GIBaseInfo * _info;
};

} /*  GNodeJS  */

#endif
