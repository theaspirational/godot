# Godot Engine Build Guide for macOS

This guide documents the complete process for building Godot Engine from source on macOS, specifically configured for Android game development.

## 🎯 Build Goals
- **Target**: Editor build for game development
- **Platform**: macOS (Apple Silicon ARM64)
- **Renderer**: OpenGL3 (compatible with both macOS and Android)
- **Output**: Professional .app bundle with proper icons and integration

## 📋 Prerequisites

### System Requirements
- **macOS**: 11.0+ (Big Sur or later)
- **Architecture**: ARM64 (Apple Silicon) or x86_64 (Intel)
- **Python**: 3.8+
- **SCons**: 4.0+
- **Xcode**: Command Line Tools or full Xcode

### Check Prerequisites
```bash
# Check Python version
python3 --version  # Should be 3.8+

# Check SCons
scons --version    # Should be 4.0+

# Check Xcode tools
xcode-select -p    # Should show tools path

# Install SCons if needed
pip3 install scons
```

## 🚀 Build Process

### Step 1: Initial Build Configuration

For Android development, we avoid Vulkan/MoltenVK complexity by using OpenGL3:

```bash
# Build editor with OpenGL3 renderer (recommended for Android dev)
scons target=editor vulkan=no opengl3=yes -j8

# Alternative: If you want both renderers and have MoltenVK SDK
scons target=editor vulkan=yes opengl3=yes -j8
```

**Build Options Explained:**
- `target=editor` - Builds the full editor (not just export templates)
- `vulkan=no` - Disables Vulkan to avoid MoltenVK SDK requirement
- `opengl3=yes` - Enables OpenGL3 (works on both macOS and Android)
- `-j8` - Use 8 parallel jobs (adjust based on CPU cores)

### Step 2: Expected Build Output

After successful build (typically 10-20 minutes):
```
bin/godot.macos.editor.arm64    # ARM64 binary (119MB typical size)
```

### Step 3: Create Official .app Bundle

Use the official Godot method for proper macOS integration:

```bash
# Run the official app bundle creation script
./create_official_app_bundle.sh
```

This script:
1. Uses official template from `misc/dist/macos_tools.app`
2. Copies the editor binary to proper location
3. Applies code signing with entitlements
4. Creates `bin/Godot.app` with proper icons and file associations

## 🛠️ Build Scripts Created

### 1. `create_official_app_bundle.sh`
**Purpose**: Creates proper .app bundle using official Godot template  
**Based on**: Official Godot documentation  
**Output**: `bin/Godot.app` with proper icons and macOS integration

### 2. `godot` (launcher script)
**Purpose**: Simple command-line launcher  
**Usage**: `./godot --version` instead of long binary paths

## ✅ Verification Steps

### Test Binary
```bash
# Test the raw binary
./bin/godot.macos.editor.arm64 --version

# Test the app bundle
./bin/Godot.app/Contents/MacOS/Godot --version

# Test the launcher
./godot --version
```

### Expected Output
```
4.5.beta.custom_build.45509c284
```

### Test App Bundle Integration
```bash
# Open in Finder to verify icon
open -R bin/Godot.app

# Launch the app
open bin/Godot.app
```

## 🔧 Troubleshooting

### Common Issues

#### 1. MoltenVK SDK Error
**Error**: `MoltenVK SDK installation directory not found`  
**Solution**: Use `vulkan=no opengl3=yes` build configuration

#### 2. Missing Dependencies
**Error**: Build fails with missing tools  
**Solution**: 
```bash
# Reinstall Xcode Command Line Tools
sudo rm -rf /Library/Developer/CommandLineTools
sudo xcode-select --install

# Install/update SCons
pip3 install --upgrade scons
```

#### 3. App Bundle Icon Issues
**Error**: Generic grid icon instead of Godot icon  
**Solution**: Use official template method, not custom icon conversion

## 🎮 Android Development Setup

This build is optimized for Android game development:

### Why OpenGL3?
- **Cross-platform**: Works on both macOS (development) and Android (deployment)
- **Compatibility**: Supported by virtually all Android devices
- **No SDK dependencies**: Avoids MoltenVK complexity during development

### Export Configuration
When exporting to Android:
1. **Renderer**: Use OpenGL3 for maximum compatibility
2. **Fallback**: Can still use Vulkan on modern Android devices (7.0+)
3. **Testing**: Test on actual Android devices or emulator

## 📁 File Structure After Build

```
godot/
├── bin/
│   ├── godot.macos.editor.arm64      # Raw binary
│   └── Godot.app/                    # macOS app bundle
│       ├── Contents/
│       │   ├── Info.plist           # App metadata
│       │   ├── MacOS/Godot          # Executable
│       │   └── Resources/           # Icons and localizations
├── create_official_app_bundle.sh     # Official app creation
├── godot                             # Command-line launcher
├── BUILD_GUIDE.md                    # This guide
└── CLAUDE.md                         # AI assistant guide
```

## 🚀 Quick Build Commands

### Full Clean Build
```bash
# Clean previous build
scons -c

# Build editor for Android development
scons target=editor vulkan=no opengl3=yes -j8

# Create app bundle
./create_official_app_bundle.sh

# Test
./godot --version
```

### Development Build (faster rebuilds)
```bash
# Development mode with extra debugging
scons target=editor vulkan=no opengl3=yes dev_mode=yes -j8
```

## 📚 References

- **Official Docs**: [Compiling for macOS](https://github.com/godotengine/godot-docs/blob/master/contributing/development/compiling/compiling_for_macos.rst)
- **Build System**: [SCons Documentation](https://scons.org/documentation.html)
- **Android Export**: [Exporting for Android](https://docs.godotengine.org/en/latest/tutorials/export/exporting_for_android.html)

## 💡 Tips

1. **Parallel Builds**: Use `-j<cores>` for faster compilation
2. **Development Mode**: Add `dev_mode=yes` for debugging builds
3. **Clean Builds**: Use `scons -c` to clean before major rebuilds
4. **App Bundle**: Always use official template for proper macOS integration
5. **Testing**: Test both command-line and GUI launching methods