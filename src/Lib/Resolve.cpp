/*
 * Copyright (c) 2021, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdlib>
#include <dlfcn.h>
#include <filesystem>
#include <format>
#include <iostream>
#include <mutex>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

#include <Lib/Lib.h>
#include <Lib/Logging.h>
#include <Lib/Resolve.h>
#include <Lib/Result.h>

namespace Lib {

std::mutex g_resolve_mutex;

/* ------------------------------------------------------------------------ */

Resolver::Library::Library(std::string_view img)
    : m_image(img)
{
}

Resolver::Library::~Library()
{
    if (m_handle) {
        dlclose(m_handle);
    }
}

std::string Resolver::Library::to_string()
{
    return (!m_image.empty()) ? m_image : "Main Program Image";
}

fs::path Resolver::Library::platform_image(std::string_view const &image)
{
    if (image.empty()) {
        return "";
    }
    std::string img { image };
    for (auto backslash = img.find_first_of('\\');
        backslash != std::string::npos;
        backslash = img.find_first_of('\\')) {
        img[backslash] = '/';
    }
    fs::path platform_image { img };
#ifdef __APPLE__
    platform_image.replace_extension("dylib");
#else
    platform_image.replace_extension("so");
#endif
    return platform_image;
}

LibOpenResult Resolver::Library::try_open(fs::path const &dir)
{
    auto     image = platform_image(m_image);
    fs::path path;
    if (!image.empty()) {
        if (!dir.empty()) {
            path = dir / image;
        } else {
            path = image;
        }
    }
    dlerror();
    if (auto lib_handle = dlopen((!image.empty()) ? path.c_str() : nullptr, RTLD_NOW | RTLD_GLOBAL); lib_handle != nullptr) {
        return lib_handle;
    }
    return ResolveError { ResolveErrorCode::CouldNotFindLibrary, std::format("Could not find library '{}' at location '{}'", m_image, dir.string()) };
}

LibOpenResult Resolver::Library::search_and_open()
{
    fs::path arwen_dir { getenv(ARWEN_DIR) ? getenv(ARWEN_DIR) : "" };
    if (arwen_dir.empty()) {
        arwen_dir = fs::path { "/usr" } / "share" / "arwen";
    }
    fs::path paths[] = {
        { arwen_dir / "lib" },
        { arwen_dir / "bin" },
        { arwen_dir },
        { arwen_dir / "share" / "lib" },
        { fs::path { "lib" } },
        { fs::path { "bin" } },
        { fs::path { "share" } / "lib" },
        { fs::path { "." } },
    };
    for (auto ix = 0; ix < sizeof(paths) / sizeof(fs::path); ++ix) {
        LibOpenResult ret = try_open(arwen_dir / "lib");
        if (ret.has_value()) {
            return ret;
        }
    }
    return ResolveError { ResolveErrorCode::CouldNotFindLibrary, std::format("Could not find library '{}'", m_image) };
}

LibOpenResult Resolver::Library::open()
{
    auto image { platform_image(m_image) };
    m_handle = nullptr;
    LibOpenResult ret = (!image.empty()) ? search_and_open() : try_open("");
    if (!ret.is_error()) {
        m_handle = ret.value();
        if (!image.empty()) {
            auto result = TRY_EVAL(get_function(ARWEN_INIT));
            if (result != nullptr) {
                result();
            }
        }
        return m_handle;
    } else {
        std::println(std::cerr, "Resolver::Library::open('{}') FAILED", to_string());
        m_my_error = ret.error();
        return ret;
    }
}

ResolveResult Resolver::Library::get_function(std::string const &function_name)
{
    if (m_my_error)
        return *m_my_error;
    dlerror();
    if (m_functions.contains(function_name))
        return m_functions[function_name];
    auto res = dlsym(m_handle, function_name.c_str());

    if (!res) {
        std::string err = dlerror();
        // 'undefined symbol' is returned with an empty result pointer
#ifdef __APPLE__
        if (err.find("symbol not found") == std::string::npos) {
#else
        if (err.find("undefined symbol") == std::string::npos) {
#endif
        std:;
            println(std::cerr, "Resolver::Library::get_function('{}') Error finding function: {}", function_name, err);
            m_my_error = { ResolveErrorCode::DLError, std::format("Error finding function '{}': {}", function_name, err) };
            return *m_my_error;
        } else {
            // std:;println(std::cerr, "Resolver::Library::get_function('{}') dlerror(): {}", function_name, err);
            return ResolveResult {};
        }
    }

    auto fnc = reinterpret_cast<void_t>(res);
    m_functions[function_name] = fnc;
    return fnc;
}

/* ------------------------------------------------------------------------ */

LibOpenResult Resolver::open(std::string_view const &image)
{
    auto platform_image = Library::platform_image(image);
    if (!m_images.contains(platform_image)) {
        m_images.emplace(platform_image, image);
        if (auto res = m_images.at(platform_image).open(); res.is_error()) {
            m_images.erase(platform_image);
            return res.error();
        }
    }
    return m_images.at(platform_image).handle();
}

ResolveResult Resolver::resolve_(FunctionName const &func_name)
{
    std::lock_guard<std::mutex> const lock(g_resolve_mutex);
    if (auto result = open(func_name.library); result.has_value()) {
        auto lib = m_images.at(Library::platform_image(func_name.library));
        return lib.get_function(func_name.function);
    } else {
        return result.error();
    }
}

Resolver &Resolver::get_resolver() noexcept
{
    static std::optional<Resolver>    resolver {};
    std::lock_guard<std::mutex> const lock(g_resolve_mutex);
    if (!resolver) {
        resolver = Resolver();
    }
    return *resolver;
}

}
