# THMXParser

A C++20 library for parsing **THMX** files (THERM 8 model exports) used in LBNL window thermal analysis tools. Built on top of [XMLParser](https://github.com/LBNL-ETA/XMLParser) for the underlying XML parsing.

## Requirements

- C++20 compatible compiler (`g++` 11+, `clang++` 14+, MSVC 19.30+)
- CMake 3.15+ (3.21+ if you want to use the shipped CMake presets)
- [XMLParser](https://github.com/LBNL-ETA/XMLParser) (fetched automatically)

## Consuming THMXParser

### Using FetchContent in CMake (recommended)

```cmake
include(FetchContent)
FetchContent_Declare(
    THMXParser
    GIT_REPOSITORY https://github.com/LBNL-ETA/THMXParser.git
    GIT_TAG v1.1.6
)
FetchContent_MakeAvailable(THMXParser)

target_link_libraries(MyTarget PRIVATE THMXParser)
```

Update `GIT_TAG` to the desired release tag.

## Building (developers)

`XMLParser` is downloaded automatically via CMake FetchContent on first configure. Tests build by default when THMXParser is the top-level project (`-DBUILD_THMX_Parser_Tests=OFF` to disable).

On MSVC, `/MP` (multi-processor compilation) is enabled automatically so the library, tests, and dependencies compile in parallel. On GCC/Clang, pass `--parallel` to `cmake --build`.

### Presets

`CMakePresets.json` ships four visible configure presets, plus two hidden inheritance bases:

| Preset | When to use it |
|---|---|
| `default-debug` / `default-release` | Standard configure on any platform; CI and releases. **Always** fetches every dependency from its declared remote — never touches local disk. Picks the system default compiler (MSVC on Windows, system `cc`/`c++` on Linux/macOS). |
| `local-debug` / `local-release` | Development mode: build against sibling working copies on disk when present (see below). |

Examples:

```
cmake --preset default-release
cmake --build build/default-release --parallel
ctest --test-dir build/default-release -C Release --output-on-failure
```

#### Local development mode (`local-*`)

The `local` preset sets a single cache flag, `LBNL_LOCAL_SIBLINGS=ON`. When it is on, each repo's CMakeLists prefers a **sibling working copy** of its *direct* dependencies — e.g. `../XMLParser`, `../googletest` next to `../THMXParser` — over fetching them:

| Direct dependency | Expected sibling path |
|-------------------|----------------------|
| XMLParser | `../XMLParser` |
| googletest | `../googletest` |

Key properties:

- **`default` never uses local repos.** The flag is off, so `default-*` builds are always pure-remote and reproducible — use them for CI and releases. (Build dirs differ per preset, so a prior `local` configure can't leak into a `default` one.)
- **Per-dependency fallback.** A sibling that isn't checked out falls back to its declared remote independently, so `local-*` is safe to run with any subset of siblings present.
- **It propagates.** `LBNL_LOCAL_SIBLINGS` is a cache variable, so it cascades into dependency sub-builds. A sibling dependency that also honors the flag wires up *its own* direct deps in turn — so the whole graph can be built from disk. Each repo only ever names its own direct deps; it never needs to know another repo's internals.
- **Develop, then release.** Local builds intentionally use your working copies, which may differ from the pinned `GIT_TAG`s. Get the graph green locally, then bump each repo's pinned version and release one at a time.

#### Per-machine compiler presets (`CMakeUserPresets.json`)

To use a specific compiler (`vs2022-release`, `gcc-13-debug`, `clang-18-release`, etc.), each developer maintains their own `CMakeUserPresets.json` next to `CMakePresets.json`. It is gitignored, read automatically by CMake (and CLion, VS Code, etc.), and stays on the developer's machine.

Personal presets `inherit` from one of the shipped presets (usually `local`, which gives you sibling-repo overrides for free) and override whatever they want. A complete realistic example — building with WSL Clang on a Windows machine, with CLion 2023.2+ routed through the WSL toolchain automatically:

```json
{
    "version": 6,
    "configurePresets": [
        {
            "name": "clang-release",
            "displayName": "clang (Release)",
            "inherits": "local",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/clang-release",
            "cacheVariables": {
                "CMAKE_C_COMPILER":   "clang",
                "CMAKE_CXX_COMPILER": "clang++",
                "CMAKE_BUILD_TYPE":   "Release"
            },
            "vendor": {
                "jetbrains.com/clion": {
                    "toolchain": "WSL"
                }
            }
        }
    ]
}
```

A few things going on in that one preset:

- `"inherits": "local"` → picks up sibling-repo overrides (when present) and the rest of the framework setup.
- Bare compiler names (`clang`, `clang++`) rather than `/usr/bin/clang` → portable to any machine that has that toolchain on `PATH`. Use absolute paths only if the compiler isn't on `PATH` (e.g. `C:/Program Files/LLVM/bin/clang.exe` — forward slashes work in JSON, no escaping needed).
- `"vendor.jetbrains.com/clion.toolchain"` → tells CLion (2023.2+) which configured toolchain to route this preset through. Standard names are `WSL`, `Visual Studio`, `MinGW`; whatever you see in `Settings → Build, Execution, Deployment → Toolchains`. The hint is silently ignored if the name doesn't match — no configure-time error.

Add as many of those blocks as you have toolchains you want explicit presets for (one per compiler × build type). Each gets its own `binaryDir` so Debug and Release artifacts don't clobber each other.

Alternative if you don't want a personal preset at all: set `CC` and `CXX` environment variables in your shell rc (`~/.bashrc`, PowerShell profile) before invoking `cmake --preset default-release`. CMake picks them up.

### Manual configure (without presets)

```
cmake -B build
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

### Clean rebuild

Delete the `build/` directory and re-run the configure and build commands above.

## License

See the [LICENSE](LICENSE) file.
