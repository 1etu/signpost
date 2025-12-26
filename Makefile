CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I include

SRCDIR = src
OBJDIR = build
TARGET = signpost

SOURCES = $(SRCDIR)/main.cpp \
          $(SRCDIR)/config.cpp \
          $(SRCDIR)/network/server.cpp \
          $(SRCDIR)/network/client.cpp \
          $(SRCDIR)/protocol/buffer.cpp \
          $(SRCDIR)/protocol/packet.cpp \
          $(SRCDIR)/handler/handshake.cpp \
          $(SRCDIR)/handler/status.cpp \
          $(SRCDIR)/handler/login.cpp

OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)
