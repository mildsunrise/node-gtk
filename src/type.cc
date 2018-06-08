/*
 * type.cc
 * Copyright (C) 2016 romgrk <romgrk@Romgrk-ARCH>
 *
 * Distributed under terms of the GPL license.
 */

#include "gi.h"
#include "type.h"
#include "util.h"

using v8::FunctionTemplate;
using v8::Persistent;

namespace GNodeJS {

void ClassDestroyed(const v8::WeakCallbackInfo<GIBaseInfo> &info) {
    GIBaseInfo *gi_info = info.GetParameter ();
    GType gtype = g_registered_type_info_get_g_type ((GIRegisteredTypeInfo *) gi_info);
    void *type_data = g_type_get_qdata (gtype, GNodeJS::template_quark());

    Persistent<FunctionTemplate> *persistent = (Persistent<FunctionTemplate> *) type_data;
    delete persistent;

    g_type_set_qdata (gtype, GNodeJS::template_quark(), NULL);
    g_base_info_unref (gi_info);
}


gsize GetTypeSize (GITypeInfo *type_info) {
    gsize size = 0;

    GITypeTag type_tag;

    type_tag = g_type_info_get_tag (type_info);
    switch (type_tag) {
        case GI_TYPE_TAG_BOOLEAN:
        case GI_TYPE_TAG_INT8:
        case GI_TYPE_TAG_UINT8:
        case GI_TYPE_TAG_INT16:
        case GI_TYPE_TAG_UINT16:
        case GI_TYPE_TAG_INT32:
        case GI_TYPE_TAG_UINT32:
        case GI_TYPE_TAG_INT64:
        case GI_TYPE_TAG_UINT64:
        case GI_TYPE_TAG_FLOAT:
        case GI_TYPE_TAG_DOUBLE:
        case GI_TYPE_TAG_GTYPE:
        case GI_TYPE_TAG_UNICHAR:
            size = GetTypeTagSize (type_tag);
            break;
        case GI_TYPE_TAG_INTERFACE:
        {
            GIBaseInfo *info;
            GIInfoType info_type;

            info = g_type_info_get_interface (type_info);
            info_type = g_base_info_get_type (info);

            switch (info_type) {
                case GI_INFO_TYPE_STRUCT:
                    if (g_type_info_is_pointer (type_info)) {
                        size = sizeof (gpointer);
                    } else {
                        size = g_struct_info_get_size ( (GIStructInfo *) info);
                    }
                    break;
                case GI_INFO_TYPE_UNION:
                    if (g_type_info_is_pointer (type_info)) {
                        size = sizeof (gpointer);
                    } else {
                        size = g_union_info_get_size ( (GIUnionInfo *) info);
                    }
                    break;
                case GI_INFO_TYPE_ENUM:
                case GI_INFO_TYPE_FLAGS:
                    if (g_type_info_is_pointer (type_info)) {
                        size = sizeof (gpointer);
                    } else {
                        GITypeTag type_tag;

                        type_tag = g_enum_info_get_storage_type ( (GIEnumInfo *) info);
                        size = GetTypeTagSize (type_tag);
                    }
                    break;
                case GI_INFO_TYPE_BOXED:
                case GI_INFO_TYPE_OBJECT:
                case GI_INFO_TYPE_INTERFACE:
                case GI_INFO_TYPE_CALLBACK:
                    size = sizeof (gpointer);
                    break;
                case GI_INFO_TYPE_VFUNC:
                case GI_INFO_TYPE_INVALID:
                case GI_INFO_TYPE_FUNCTION:
                case GI_INFO_TYPE_CONSTANT:
                case GI_INFO_TYPE_VALUE:
                case GI_INFO_TYPE_SIGNAL:
                case GI_INFO_TYPE_PROPERTY:
                case GI_INFO_TYPE_FIELD:
                case GI_INFO_TYPE_ARG:
                case GI_INFO_TYPE_TYPE:
                case GI_INFO_TYPE_UNRESOLVED:
                default:
                    g_assert_not_reached();
                    break;
            }

            g_base_info_unref (info);
            break;
        }
        case GI_TYPE_TAG_ARRAY:
        case GI_TYPE_TAG_VOID:
        case GI_TYPE_TAG_UTF8:
        case GI_TYPE_TAG_FILENAME:
        case GI_TYPE_TAG_GLIST:
        case GI_TYPE_TAG_GSLIST:
        case GI_TYPE_TAG_GHASH:
        case GI_TYPE_TAG_ERROR:
            size = sizeof(void*);
            break;
    }

    g_assert (size > 0);

    return size;
}

gsize GetTypeTagSize (GITypeTag type_tag) {
    gsize size = 0;

    switch (type_tag) {
        case GI_TYPE_TAG_BOOLEAN:
            size = sizeof (gboolean);
            break;
        case GI_TYPE_TAG_INT8:
        case GI_TYPE_TAG_UINT8:
            size = sizeof (gint8);
            break;
        case GI_TYPE_TAG_INT16:
        case GI_TYPE_TAG_UINT16:
            size = sizeof (gint16);
            break;
        case GI_TYPE_TAG_INT32:
        case GI_TYPE_TAG_UINT32:
            size = sizeof (gint32);
            break;
        case GI_TYPE_TAG_INT64:
        case GI_TYPE_TAG_UINT64:
            size = sizeof (gint64);
            break;
        case GI_TYPE_TAG_FLOAT:
            size = sizeof (gfloat);
            break;
        case GI_TYPE_TAG_DOUBLE:
            size = sizeof (gdouble);
            break;
        case GI_TYPE_TAG_GTYPE:
            size = sizeof (GType);
            break;
        case GI_TYPE_TAG_UNICHAR:
            size = sizeof (gunichar);
            break;
        case GI_TYPE_TAG_VOID:
        case GI_TYPE_TAG_UTF8:
        case GI_TYPE_TAG_FILENAME:
        case GI_TYPE_TAG_ARRAY:
        case GI_TYPE_TAG_INTERFACE:
        case GI_TYPE_TAG_GLIST:
        case GI_TYPE_TAG_GSLIST:
        case GI_TYPE_TAG_GHASH:
        case GI_TYPE_TAG_ERROR:
            char * msg = g_strdup_printf(
                    "Unable to know the size (assuming %s is not a pointer)",
                    g_type_tag_to_string (type_tag));
            Nan::ThrowTypeError(msg);
            g_free(msg);
            break;
    }

    return size;
}

};
