CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -I./include
LDFLAGS = -lstdc++fs

SRCDIR = src
OBJDIR = build
BINDIR = .

TARGET = $(BINDIR)/forensic-tool

# All source files
SOURCES = \
	$(SRCDIR)/main.cpp \
	$(SRCDIR)/menu.cpp \
	$(SRCDIR)/device_manager.cpp \
	$(SRCDIR)/recovery_manager.cpp \
	$(SRCDIR)/erasure_manager.cpp \
	$(SRCDIR)/verifier.cpp \
	$(SRCDIR)/audit_logger.cpp \
	$(SRCDIR)/report_generator.cpp

OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
HEADERS = $(wildcard include/*.hpp)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
	@echo "\n✓ Build successful: $@"
	@echo "✓ Run with: sudo ./$(TARGET)"

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJDIR)
	@echo "[*] Compiling $<"
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Create build directory
$(OBJDIR):
	@mkdir -p $(OBJDIR)
	@echo "[*] Created build directory"

# Create bin directory
$(BINDIR):
	@mkdir -p $(BINDIR)

# Clean build artifacts
clean:
	@echo "[*] Cleaning build artifacts..."
	@rm -rf $(OBJDIR) $(TARGET)
	@echo "✓ Cleaned successfully"

# Clean everything including logs and reports
distclean: clean
	@echo "[*] Removing logs and reports..."
	@rm -rf logs/ reports/ recovery/
	@echo "✓ Full clean complete"

# Run the tool
run: $(TARGET)
	@echo "[*] Starting Forensic Data Tool..."
	@echo "[!] Note: This tool requires root privileges"
	sudo ./$(TARGET)

# Run with valgrind (memory profiling)
valgrind: $(TARGET)
	@echo "[*] Running with Valgrind..."
	sudo valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Build and run in one command
build-run: clean all run

# Display build information
info:
	@echo "\n═══════════════════════════════════════════════════════════════"
	@echo "     FORENSIC DATA TOOL - Build Information"
	@echo "═══════════════════════════════════════════════════════════════"
	@echo "Compiler:        $(CXX)"
	@echo "C++ Standard:    C++17"
	@echo "Optimization:    -O2"
	@echo "Target:          $(TARGET)"
	@echo "Source Dir:      $(SRCDIR)"
	@echo "Object Dir:      $(OBJDIR)"
	@echo "───────────────────────────────────────────────────────────────"
	@echo "Source Files:    $(words $(SOURCES))"
	@echo "───────────────────────────────────────────────────────────────"
	@echo "Available targets:"
	@echo "  make all          - Build the tool"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make distclean    - Clean everything (logs, reports, builds)"
	@echo "  make run          - Build and run with sudo"
	@echo "  make build-run    - Clean, build, and run"
	@echo "  make info         - Show this information"
	@echo "  make valgrind     - Run with memory profiling"
	@echo "═══════════════════════════════════════════════════════════════\n"

# Help target
help: info

# Mark phony targets
.PHONY: all clean distclean run valgrind build-run info help

# Default goal
.DEFAULT_GOAL := all
