#!/bin/bash

# Godot Engine Build Automation Script
# Builds Godot Engine from source with optimal settings for Android development
# 
# Usage:
#   ./build_godot.sh                    # Standard build
#   ./build_godot.sh --clean           # Clean build
#   ./build_godot.sh --dev             # Development build
#   ./build_godot.sh --help            # Show help

set -e  # Exit on any error

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_CORES=$(sysctl -n hw.ncpu)  # Auto-detect CPU cores
BUILD_TARGET="editor"
BUILD_PLATFORM="macos"
VULKAN_ENABLED="no"
OPENGL3_ENABLED="yes"
DEV_MODE="no"
CLEAN_BUILD="no"
CREATE_APP_BUNDLE="yes"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

show_help() {
    cat << EOF
Godot Engine Build Script

USAGE:
    $0 [OPTIONS]

OPTIONS:
    --clean         Clean previous build before compiling
    --dev           Enable development mode (extra debugging, warnings)
    --vulkan        Enable Vulkan rendering (requires MoltenVK SDK)
    --no-opengl3    Disable OpenGL3 rendering
    --no-app        Skip .app bundle creation
    --cores N       Use N cores for compilation (default: auto-detect)
    --help          Show this help message

EXAMPLES:
    $0                      # Standard build for Android development
    $0 --clean --dev        # Clean development build
    $0 --vulkan --cores 4   # Build with Vulkan using 4 cores

NOTES:
    - Default configuration is optimized for Android game development
    - OpenGL3 renderer works on both macOS and Android
    - Vulkan requires MoltenVK SDK installation
    - App bundle is created using official Godot template

EOF
}

check_prerequisites() {
    log_info "Checking build prerequisites..."
    
    # Check Python
    if ! command -v python3 &> /dev/null; then
        log_error "Python 3 is required but not installed"
        exit 1
    fi
    
    PYTHON_VERSION=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
    if [[ $(echo "$PYTHON_VERSION < 3.8" | bc -l) -eq 1 ]]; then
        log_error "Python 3.8+ required, found $PYTHON_VERSION"
        exit 1
    fi
    log_success "Python $PYTHON_VERSION found"
    
    # Check SCons
    if ! command -v scons &> /dev/null; then
        log_error "SCons is required but not installed"
        log_info "Install with: pip3 install scons"
        exit 1
    fi
    
    SCONS_VERSION=$(scons --version 2>&1 | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n1)
    log_success "SCons $SCONS_VERSION found"
    
    # Check Xcode tools
    if ! command -v clang &> /dev/null; then
        log_error "Xcode Command Line Tools required but not installed"
        log_info "Install with: xcode-select --install"
        exit 1
    fi
    log_success "Xcode Command Line Tools found"
    
    # Check for MoltenVK if Vulkan is enabled
    if [[ "$VULKAN_ENABLED" == "yes" ]]; then
        if [[ -z "$VULKAN_SDK" ]]; then
            log_warning "Vulkan enabled but VULKAN_SDK environment variable not set"
            log_info "Download MoltenVK SDK from: https://vulkan.lunarg.com/sdk/home"
        else
            log_success "Vulkan SDK found at: $VULKAN_SDK"
        fi
    fi
}

parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --clean)
                CLEAN_BUILD="yes"
                shift
                ;;
            --dev)
                DEV_MODE="yes"
                shift
                ;;
            --vulkan)
                VULKAN_ENABLED="yes"
                shift
                ;;
            --no-opengl3)
                OPENGL3_ENABLED="no"
                shift
                ;;
            --no-app)
                CREATE_APP_BUNDLE="no"
                shift
                ;;
            --cores)
                BUILD_CORES="$2"
                shift 2
                ;;
            --help)
                show_help
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

build_godot() {
    log_info "Starting Godot Engine build..."
    
    # Build command construction
    BUILD_CMD="scons"
    BUILD_CMD="$BUILD_CMD platform=$BUILD_PLATFORM"
    BUILD_CMD="$BUILD_CMD target=$BUILD_TARGET"
    BUILD_CMD="$BUILD_CMD vulkan=$VULKAN_ENABLED"
    BUILD_CMD="$BUILD_CMD opengl3=$OPENGL3_ENABLED"
    BUILD_CMD="$BUILD_CMD -j$BUILD_CORES"
    
    if [[ "$DEV_MODE" == "yes" ]]; then
        BUILD_CMD="$BUILD_CMD dev_mode=yes"
    fi
    
    # Clean build if requested
    if [[ "$CLEAN_BUILD" == "yes" ]]; then
        log_info "Cleaning previous build..."
        scons -c
    fi
    
    # Show build configuration
    log_info "Build Configuration:"
    echo "  Platform: $BUILD_PLATFORM"
    echo "  Target: $BUILD_TARGET"
    echo "  Vulkan: $VULKAN_ENABLED"
    echo "  OpenGL3: $OPENGL3_ENABLED"
    echo "  Cores: $BUILD_CORES"
    echo "  Dev Mode: $DEV_MODE"
    echo "  Clean: $CLEAN_BUILD"
    echo ""
    
    log_info "Build command: $BUILD_CMD"
    echo ""
    
    # Execute build
    START_TIME=$(date +%s)
    
    if $BUILD_CMD; then
        END_TIME=$(date +%s)
        BUILD_TIME=$((END_TIME - START_TIME))
        log_success "Build completed in ${BUILD_TIME}s"
    else
        log_error "Build failed"
        exit 1
    fi
}

verify_build() {
    log_info "Verifying build output..."
    
    # Expected binary paths
    ARM64_BINARY="$SCRIPT_DIR/bin/godot.macos.editor.arm64"
    X86_64_BINARY="$SCRIPT_DIR/bin/godot.macos.editor.x86_64"
    UNIVERSAL_BINARY="$SCRIPT_DIR/bin/godot.macos.editor.universal"
    
    FOUND_BINARY=""
    
    if [[ -f "$ARM64_BINARY" ]]; then
        FOUND_BINARY="$ARM64_BINARY"
        log_success "ARM64 binary found: $(basename "$FOUND_BINARY")"
    elif [[ -f "$X86_64_BINARY" ]]; then
        FOUND_BINARY="$X86_64_BINARY"
        log_success "x86_64 binary found: $(basename "$FOUND_BINARY")"
    elif [[ -f "$UNIVERSAL_BINARY" ]]; then
        FOUND_BINARY="$UNIVERSAL_BINARY"
        log_success "Universal binary found: $(basename "$FOUND_BINARY")"
    else
        log_error "No Godot binary found in bin/ directory"
        exit 1
    fi
    
    # Test binary
    log_info "Testing binary..."
    if VERSION_OUTPUT=$("$FOUND_BINARY" --version 2>&1); then
        log_success "Binary test passed: $VERSION_OUTPUT"
        
        # Show binary info
        BINARY_SIZE=$(du -h "$FOUND_BINARY" | cut -f1)
        BINARY_ARCH=$(file "$FOUND_BINARY" | grep -oE "(arm64|x86_64|universal)")
        log_info "Binary size: $BINARY_SIZE"
        log_info "Architecture: $BINARY_ARCH"
    else
        log_error "Binary test failed"
        exit 1
    fi
    
    echo ""
}

create_app_bundle() {
    if [[ "$CREATE_APP_BUNDLE" == "yes" ]]; then
        log_info "Creating .app bundle..."
        
        if [[ -f "$SCRIPT_DIR/create_official_app_bundle.sh" ]]; then
            if "$SCRIPT_DIR/create_official_app_bundle.sh"; then
                log_success ".app bundle created successfully"
                
                # Test app bundle
                APP_BUNDLE="$SCRIPT_DIR/bin/Godot.app"
                if [[ -d "$APP_BUNDLE" ]]; then
                    APP_SIZE=$(du -sh "$APP_BUNDLE" | cut -f1)
                    log_info "App bundle size: $APP_SIZE"
                    
                    # Test app bundle binary
                    APP_BINARY="$APP_BUNDLE/Contents/MacOS/Godot"
                    if [[ -x "$APP_BINARY" ]]; then
                        if "$APP_BINARY" --version >/dev/null 2>&1; then
                            log_success "App bundle binary test passed"
                        else
                            log_warning "App bundle binary test failed"
                        fi
                    fi
                fi
            else
                log_error ".app bundle creation failed"
                exit 1
            fi
        else
            log_warning "create_official_app_bundle.sh not found, skipping .app bundle creation"
        fi
    else
        log_info "Skipping .app bundle creation (--no-app specified)"
    fi
}

show_summary() {
    echo ""
    log_success "=== Build Summary ==="
    echo ""
    echo "✅ Godot Engine build completed successfully!"
    echo ""
    echo "📁 Output files:"
    
    # List binary
    for binary in "$SCRIPT_DIR/bin/godot.macos.editor"*; do
        if [[ -f "$binary" && -x "$binary" ]]; then
            size=$(du -h "$binary" | cut -f1)
            name=$(basename "$binary")
            echo "   Binary: $name ($size)"
        fi
    done
    
    # List app bundle
    if [[ -d "$SCRIPT_DIR/bin/Godot.app" ]]; then
        size=$(du -sh "$SCRIPT_DIR/bin/Godot.app" | cut -f1)
        echo "   App Bundle: Godot.app ($size)"
    fi
    
    echo ""
    echo "🚀 Usage:"
    echo "   Command line: ./godot --version"
    echo "   GUI: open bin/Godot.app"
    echo "   Development: Ready for Android game development!"
    echo ""
    echo "📚 Next steps:"
    echo "   1. Move Godot.app to Applications folder (optional)"
    echo "   2. Create your first game project"
    echo "   3. Configure Android export settings"
    echo ""
}

# Main execution
main() {
    echo "🎮 Godot Engine Build Script"
    echo "============================"
    echo ""
    
    parse_arguments "$@"
    check_prerequisites
    build_godot
    verify_build
    create_app_bundle
    show_summary
}

# Run main function with all arguments
main "$@"