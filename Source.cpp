#include <assert.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define NAPI_EXTERN
#include <node_api.h>

#define DECLARE_NAPI_METHOD(name, func) \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

#define DECLARE_SYMBOL(symbol) decltype(::symbol)* symbol

#ifdef _WIN32
#define LOAD_SYMBOL(module, symbol) js::symbol = (decltype(::symbol)*)GetProcAddress(module, #symbol)
#endif

#define NODE_GYP_MODULE_NAME addon

namespace js
{
    // Alias napi symbols locally
    DECLARE_SYMBOL(napi_typeof);
    DECLARE_SYMBOL(napi_get_cb_info);
    DECLARE_SYMBOL(napi_throw_type_error);
    DECLARE_SYMBOL(napi_get_value_double);
    DECLARE_SYMBOL(napi_create_double);
    DECLARE_SYMBOL(napi_define_properties);
    DECLARE_SYMBOL(napi_module_register);
}

namespace
{
    struct Init
    {
        Init()
        {
            // Resolve napi symbols
            auto module = GetModuleHandle(nullptr);
            LOAD_SYMBOL(module, napi_typeof);
            LOAD_SYMBOL(module, napi_get_cb_info);
            LOAD_SYMBOL(module, napi_throw_type_error);
            LOAD_SYMBOL(module, napi_get_value_double);
            LOAD_SYMBOL(module, napi_create_double);
            LOAD_SYMBOL(module, napi_define_properties);
            LOAD_SYMBOL(module, napi_module_register);
        }
    };
}

static Init s_init;

namespace js
{
    // This is really https://github.com/nodejs/node-addon-examples/tree/main/2_function_arguments/nan
    extern "C" napi_value Add(napi_env env, napi_callback_info info)
    {
        napi_status status;

        size_t argc = 2;
        napi_value args[2];
        status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
        assert(status == napi_ok);

        if (argc < 2) {
            napi_throw_type_error(env, NULL, "Wrong number of arguments");
            return NULL;
        }

        napi_valuetype valuetype0;
        status = napi_typeof(env, args[0], &valuetype0);
        assert(status == napi_ok);

        napi_valuetype valuetype1;
        status = napi_typeof(env, args[1], &valuetype1);
        assert(status == napi_ok);

        if (valuetype0 != napi_number || valuetype1 != napi_number) {
            napi_throw_type_error(env, NULL, "Wrong arguments");
            return NULL;
        }

        double value0;
        status = napi_get_value_double(env, args[0], &value0);
        assert(status == napi_ok);

        double value1;
        status = napi_get_value_double(env, args[1], &value1);
        assert(status == napi_ok);

        napi_value sum;
        status = napi_create_double(env, value0 + value1, &sum);
        assert(status == napi_ok);

        return sum;
    }

    extern "C" napi_value Init(napi_env env, napi_value exports)
    {
        napi_status status;
        napi_property_descriptor addDescriptor = DECLARE_NAPI_METHOD("add", Add);
        status = napi_define_properties(env, exports, 1, &addDescriptor);
        assert(status == napi_ok);
        return exports;
    }

    NAPI_MODULE(addon, Init)
}
