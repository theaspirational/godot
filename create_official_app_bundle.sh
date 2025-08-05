#!/bin/bash

# Official Godot .app bundle creation script
# Based on official documentation: https://github.com/godotengine/godot-docs/blob/bae4de3b5bb34bdfbb67a0bb1519e41ea60c1fe8/contributing/development/compiling/compiling_for_macos.rst

set -e  # Exit on any error

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Creating official Godot .app bundle..."

# Check if we have the editor binary
EDITOR_BINARY=""
if [ -f "$SCRIPT_DIR/bin/godot.macos.editor.arm64" ]; then
    EDITOR_BINARY="$SCRIPT_DIR/bin/godot.macos.editor.arm64"
    ARCH="arm64"
elif [ -f "$SCRIPT_DIR/bin/godot.macos.editor.x86_64" ]; then
    EDITOR_BINARY="$SCRIPT_DIR/bin/godot.macos.editor.x86_64"
    ARCH="x86_64"
elif [ -f "$SCRIPT_DIR/bin/godot.macos.editor.universal" ]; then
    EDITOR_BINARY="$SCRIPT_DIR/bin/godot.macos.editor.universal"
    ARCH="universal"
else
    echo "Error: No Godot editor binary found!"
    echo "Expected one of:"
    echo "  - bin/godot.macos.editor.arm64"
    echo "  - bin/godot.macos.editor.x86_64"
    echo "  - bin/godot.macos.editor.universal"
    echo ""
    echo "Build Godot first with: scons target=editor vulkan=no opengl3=yes"
    exit 1
fi

echo "Found editor binary: $EDITOR_BINARY (arch: $ARCH)"

# Check if the official template exists
TEMPLATE_DIR="$SCRIPT_DIR/misc/dist/macos_tools.app"
if [ ! -d "$TEMPLATE_DIR" ]; then
    echo "Error: Official template not found at $TEMPLATE_DIR"
    echo "This script requires the complete Godot source repository."
    exit 1
fi

echo "Using official template: $TEMPLATE_DIR"

# Remove existing app bundle if it exists
APP_BUNDLE="$SCRIPT_DIR/bin/Godot.app"
if [ -d "$APP_BUNDLE" ]; then
    echo "Removing existing Godot.app..."
    rm -rf "$APP_BUNDLE"
fi

# Step 1: Copy the official template
echo "Copying official template to create .app bundle..."
cp -r "$TEMPLATE_DIR" "$APP_BUNDLE"

# Step 2: Create MacOS directory and copy binary
echo "Setting up bundle structure..."
mkdir -p "$APP_BUNDLE/Contents/MacOS"
cp "$EDITOR_BINARY" "$APP_BUNDLE/Contents/MacOS/Godot"
chmod +x "$APP_BUNDLE/Contents/MacOS/Godot"

# Step 3: Code signing (optional, using ad-hoc signature)
echo "Applying code signature..."
if command -v codesign >/dev/null 2>&1; then
    # Check if entitlements file exists
    ENTITLEMENTS_FILE="$SCRIPT_DIR/misc/dist/macos/editor.entitlements"
    if [ -f "$ENTITLEMENTS_FILE" ]; then
        echo "Using official entitlements file..."
        codesign --force --timestamp --options=runtime --entitlements "$ENTITLEMENTS_FILE" -s - "$APP_BUNDLE" 2>/dev/null || {
            echo "Warning: Code signing with entitlements failed, trying without entitlements..."
            codesign --force --timestamp --options=runtime -s - "$APP_BUNDLE" 2>/dev/null || {
                echo "Warning: Code signing failed, but app should still work for local use"
            }
        }
    else
        echo "Entitlements file not found, using basic signing..."
        codesign --force --timestamp --options=runtime -s - "$APP_BUNDLE" 2>/dev/null || {
            echo "Warning: Code signing failed, but app should still work for local use"
        }
    fi
else
    echo "codesign not found, skipping code signing (app should still work)"
fi

# Verify the app bundle was created correctly
if [ -f "$APP_BUNDLE/Contents/MacOS/Godot" ]; then
    echo ""
    echo "✅ Successfully created official Godot.app bundle!"
    echo ""
    echo "Bundle location: $APP_BUNDLE"
    echo "Binary size: $(du -h "$EDITOR_BINARY" | cut -f1)"
    echo "Bundle size: $(du -sh "$APP_BUNDLE" | cut -f1)"
    echo "Architecture: $ARCH"
    echo ""
    echo "The app bundle was created using the official Godot template from:"
    echo "  $TEMPLATE_DIR"
    echo ""
    echo "Usage:"
    echo "  • Double-click Godot.app to launch"
    echo "  • Drag to Applications folder if desired"
    echo "  • Run from command line: ./bin/Godot.app/Contents/MacOS/Godot"
    echo ""
    
    # Test the binary
    echo "Testing the app bundle..."
    if "$APP_BUNDLE/Contents/MacOS/Godot" --version >/dev/null 2>&1; then
        echo "✅ App bundle binary is working correctly"
    else
        echo "⚠️  Warning: App bundle binary test failed"
    fi
    
else
    echo "❌ Error: Failed to create app bundle"
    exit 1
fi