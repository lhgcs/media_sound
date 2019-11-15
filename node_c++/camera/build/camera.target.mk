# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := camera
DEFS_Debug := \
	'-DNODE_GYP_MODULE_NAME=camera' \
	'-DUSING_UV_SHARED=1' \
	'-DUSING_V8_SHARED=1' \
	'-DV8_DEPRECATION_WARNINGS=1' \
	'-D_LARGEFILE_SOURCE' \
	'-D_FILE_OFFSET_BITS=64' \
	'-DBUILDING_NODE_EXTENSION' \
	'-DDEBUG' \
	'-D_DEBUG' \
	'-DV8_ENABLE_CHECKS'

# Flags passed to all source files.
CFLAGS_Debug := \
	-fPIC \
	-pthread \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-g \
	-O0

# Flags passed to only C files.
CFLAGS_C_Debug :=

# Flags passed to only C++ files.
CFLAGS_CC_Debug := \
	-fno-rtti \
	-fno-exceptions \
	-std=gnu++1y

INCS_Debug := \
	-I/home/rock/.cache/node-gyp/10.16.0/include/node \
	-I/home/rock/.cache/node-gyp/10.16.0/src \
	-I/home/rock/.cache/node-gyp/10.16.0/deps/openssl/config \
	-I/home/rock/.cache/node-gyp/10.16.0/deps/openssl/openssl/include \
	-I/home/rock/.cache/node-gyp/10.16.0/deps/uv/include \
	-I/home/rock/.cache/node-gyp/10.16.0/deps/zlib \
	-I/home/rock/.cache/node-gyp/10.16.0/deps/v8/include \
	-I$(srcdir)/../node_modules/nan

DEFS_Release := \
	'-DNODE_GYP_MODULE_NAME=camera' \
	'-DUSING_UV_SHARED=1' \
	'-DUSING_V8_SHARED=1' \
	'-DV8_DEPRECATION_WARNINGS=1' \
	'-D_LARGEFILE_SOURCE' \
	'-D_FILE_OFFSET_BITS=64' \
	'-DBUILDING_NODE_EXTENSION'

# Flags passed to all source files.
CFLAGS_Release := \
	-fPIC \
	-pthread \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-O3 \
	-fno-omit-frame-pointer

# Flags passed to only C files.
CFLAGS_C_Release :=

# Flags passed to only C++ files.
CFLAGS_CC_Release := \
	-fno-rtti \
	-fno-exceptions \
	-std=gnu++1y

INCS_Release := \
	-I/home/rock/.cache/node-gyp/10.16.0/include/node \
	-I/home/rock/.cache/node-gyp/10.16.0/src \
	-I/home/rock/.cache/node-gyp/10.16.0/deps/openssl/config \
	-I/home/rock/.cache/node-gyp/10.16.0/deps/openssl/openssl/include \
	-I/home/rock/.cache/node-gyp/10.16.0/deps/uv/include \
	-I/home/rock/.cache/node-gyp/10.16.0/deps/zlib \
	-I/home/rock/.cache/node-gyp/10.16.0/deps/v8/include \
	-I$(srcdir)/../node_modules/nan

OBJS := \
	$(obj).target/$(TARGET)/camera.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Debug := \
	-pthread \
	-rdynamic

LDFLAGS_Release := \
	-pthread \
	-rdynamic

LIBS :=

$(obj).target/camera.node: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(obj).target/camera.node: LIBS := $(LIBS)
$(obj).target/camera.node: TOOLSET := $(TOOLSET)
$(obj).target/camera.node: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,solink_module)

all_deps += $(obj).target/camera.node
# Add target alias
.PHONY: camera
camera: $(builddir)/camera.node

# Copy this to the executable output path.
$(builddir)/camera.node: TOOLSET := $(TOOLSET)
$(builddir)/camera.node: $(obj).target/camera.node FORCE_DO_CMD
	$(call do_cmd,copy)

all_deps += $(builddir)/camera.node
# Short alias for building this executable.
.PHONY: camera.node
camera.node: $(obj).target/camera.node $(builddir)/camera.node

# Add executable to "all" target.
.PHONY: all
all: $(builddir)/camera.node

