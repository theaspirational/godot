# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is the **Godot Engine** repository - a feature-packed, cross-platform game engine for creating 2D and 3D games. Godot is written primarily in C++ and uses SCons as its build system.

## Build System

Godot uses **SCons** (Software Construction tool) for building:

### Quick Build Commands

**For Android Development (Recommended):**
```bash
# Automated build script
./build_godot.sh

# Manual build command
scons target=editor vulkan=no opengl3=yes -j8

# Create official .app bundle
./create_official_app_bundle.sh
```

**Build Options:**
- `target=editor` - Full editor build
- `vulkan=no` - Avoids MoltenVK SDK requirement on macOS
- `opengl3=yes` - Cross-platform renderer (works on macOS + Android)
- `-j8` - Parallel build with 8 cores (adjust for your CPU)

### Available Build Scripts

1. **`build_godot.sh`** - Comprehensive automated build
   - Checks prerequisites automatically
   - Supports various build configurations
   - Creates .app bundle using official method
   - Usage: `./build_godot.sh [--clean] [--dev] [--help]`

2. **`create_official_app_bundle.sh`** - Official .app bundle creation
   - Uses official Godot template from `misc/dist/macos_tools.app`
   - Proper code signing and entitlements
   - Creates professional macOS integration

### Manual Build Process

**Prerequisites Check:**
- Python 3.8+, SCons 4.0+, Xcode Command Line Tools
- For Vulkan: MoltenVK SDK (optional for Android dev)

**Build Steps:**
1. `scons target=editor vulkan=no opengl3=yes -j8` - Compile engine
2. `./create_official_app_bundle.sh` - Create .app bundle
3. `./godot --version` - Test installation

**Advanced Options:**
- `dev_mode=yes` - Development build with extra debugging
- `tests=yes` - Enable unit tests
- `verbose=yes` - Verbose compilation output
- `compiledb=yes` - Generate compile_commands.json for IDE integration

### Build Output
- **Binary**: `bin/godot.macos.editor.arm64` (~119MB)
- **App Bundle**: `bin/Godot.app` (with proper icons and integration)
- **Launcher**: `./godot` (command-line shortcut)

## Code Architecture

### Core Structure
- **`core/`** - Engine core systems (object system, variants, math, I/O, templates)
- **`scene/`** - Scene system with 2D/3D nodes, GUI, animation, resources
- **`editor/`** - Complete editor implementation with plugins and tools
- **`servers/`** - Low-level servers (rendering, physics, audio, navigation)
- **`modules/`** - Optional feature modules (can be enabled/disabled at build time)
- **`platform/`** - Platform-specific implementations
- **`drivers/`** - Hardware/API drivers
- **`main/`** - Application entry point and main loop
- **`tests/`** - Unit and integration tests
- **`thirdparty/`** - External dependencies

### Key Systems
- **Object System**: Everything inherits from `Object` class with signals, properties, and methods
- **Node System**: Scene tree based on `Node` hierarchy with 2D/3D specializations
- **Resource System**: Asset management through `Resource` class
- **Variant System**: Dynamic typing system for scripting integration
- **Extension System**: GDExtension for C++ plugins and GDScript for scripting

### Module System
Modules in `modules/` can be:
- **Enabled/disabled** at build time using `module_<name>_enabled=no`
- **Custom modules** can be added via `custom_modules=path/to/modules`
- Each module has its own `SCsub`, `config.py`, and `register_types.cpp`

## Testing

- **Run unit tests**: Build with `tests=yes` then run the binary with `--test`
- **Test files**: Located in `tests/` directory with `test_*.h` pattern
- Tests use a custom framework defined in `test_macros.h`
- Tests are automatically run in CI for each platform

## Code Standards

The project uses:
- **clang-format** for C++ code formatting (config in `.clang-format`)
- **clang-tidy** for static analysis
- **Pre-commit hooks** configured in `.pre-commit-config.yaml`
- Set up pre-commit: Follow instructions in CONTRIBUTING.md

### Commit Guidelines
- Use imperative mood in commit titles (e.g., "Fix", "Add", "Update")
- Keep first line under 72 characters
- Include area prefix when relevant (e.g., "Core: Fix Object::has_method()")
- Reference issues with "Fixes #1234" in PR description (not commit message)

## Development Workflow

1. **Setup**: Install SCons 4.0+, Python 3.8+, and platform-specific dependencies
2. **Build for development**: `scons target=editor dev_mode=yes`
3. **Running**: The built editor binary will be in `bin/` directory
4. **Testing**: Build with `tests=yes` and run with `--test` flag
5. **Documentation**: Update XML class documentation when adding/changing APIs

## Platform Support

Godot supports multiple platforms with specific build configurations:
- **Desktop**: Linux, Windows, macOS
- **Mobile**: Android, iOS
- **Web**: WebAssembly
- **Consoles**: Various console platforms

Each platform has its own directory in `platform/` with platform-specific code.

## Extension Development

- **GDScript**: Built-in scripting language (see `modules/gdscript/`)
- **C# Support**: Available through `modules/mono/`
- **GDExtension**: Native extension system for C++ plugins
- **Custom Modules**: Can be built directly into the engine

## Documentation

- **Class Reference**: Auto-generated from XML files in `doc/classes/`
- **Update docs**: Use `--doctool` flag to update XML from code
- All public APIs must be documented in the XML files