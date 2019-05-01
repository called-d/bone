#include "string_pool.h"
#include <string.h>
#include "../bone.h"

static void free_ptr_array_entry(gpointer data);

typedef struct bnStringPool {
        GHashTable* table;
        GPtrArray* map;
        int count;
} bnStringPool;

bnStringPool* bnNewStringPool() {
        bnStringPool* ret = BN_MALLOC(sizeof(bnStringPool));
        ret->table = g_hash_table_new(g_str_hash, g_str_equal);
        ret->map = g_ptr_array_new();
        ret->count = 2;
        g_ptr_array_add(ret->map, BN_NULL_STRING);
        g_ptr_array_add(ret->map, BN_EMPTY_STRING);
        g_ptr_array_set_free_func(ret->map, free_ptr_array_entry);
        return ret;
}

bnStringView bnIntern(bnStringPool* self, const char* str) {
        if (str == BN_NULL_STRING) {
                return BN_NULL_VIEW;
        }
        if (!strcmp(str, BN_EMPTY_STRING)) {
                return BN_EMPTY_VIEW;
        }
        gpointer val = g_hash_table_lookup(self->table, (gconstpointer)str);
        // key is not found
        if (val != NULL) {
                return (bnStringView)val;
        }
        const char* dupstr = strdup(str);
        bnStringView ret = self->count;
        g_hash_table_insert(self->table, dupstr, GINT_TO_POINTER(self->count));
        g_ptr_array_add(self->map, dupstr);
        self->count++;
        return ret;
}

bnStringView bnStr2View(bnStringPool* self, const char* str) {
        if (str == BN_NULL_STRING) {
                return BN_NULL_VIEW;
        }
        if (!strcmp(str, BN_EMPTY_STRING)) {
                return BN_EMPTY_VIEW;
        }
        gpointer val = g_hash_table_lookup(self->table, (gconstpointer)str);
        assert(val != NULL);
        return (bnStringView)val;
}

const char* bnView2Str(bnStringPool* self, bnStringView view) {
        assert(view < self->count);
        return (const char*)g_ptr_array_index(self->map, view);
}

void bnDeleteStringPool(bnStringPool* self) {
        g_hash_table_destroy(self->table);
        g_ptr_array_free(self->map, TRUE);
        BN_FREE(self);
}

static void free_ptr_array_entry(gpointer data) {
        if (data == NULL || !strcmp(data, "")) {
                return;
        }
        free(data);
}