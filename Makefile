CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -I./include

SRCDIR = src
OBJDIR = build
BINDIR = .

TARGET = $(BINDIR)/forensic-tool
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✓ Build successful: $@"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)
	@echo "✓ Cleaned"

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
