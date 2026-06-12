CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -fPIC
LDFLAGS  = -ldl -lstdc++fs
TARGET   = cryptoAppRGR
SO_DIR   = .

# ── Исходники main-приложения ──────────────────────────────────────────────
MAIN_SRCS = \
    src/main.cpp \
    src/console_utils.cpp \
    src/auth.cpp

MAIN_OBJS = $(MAIN_SRCS:.cpp=.o)

# ── validation.o — линкуется в каждую .so статически ──────────────────────
VALIDATION_OBJ = src/validation.o

# ── libaes.so ──────────────────────────────────────────────────────────────
AES_SRCS = \
    libs/lib_aes.cpp \
    src/AES128.cpp \
    src/CFBmode.cpp \
    src/Utilities.cpp

AES_OBJS = $(AES_SRCS:.cpp=.o)

# ── liba51.so ──────────────────────────────────────────────────────────────
A51_SRCS = \
    libs/lib_a51.cpp \
    src/A51Cipher.cpp \
    src/A51EncryptDecryptBlock.cpp \
    src/A51utilities.cpp

A51_OBJS = $(A51_SRCS:.cpp=.o)

# ── librsa.so ──────────────────────────────────────────────────────────────
RSA_SRCS = \
    libs/lib_rsa.cpp \
    src/RSAblock.cpp \
    src/RSAutilities.cpp

RSA_OBJS = $(RSA_SRCS:.cpp=.o)

# ── libhill_vernam.so ──────────────────────────────────────────────────────
HV_SRCS = \
    libs/lib_hill_vernam.cpp \
    src/hill.cpp \
    src/vernam.cpp \
    src/filef.cpp \
    src/textproc.cpp

HV_OBJS = $(HV_SRCS:.cpp=.o)

# ── libbifid_adfgx.so ─────────────────────────────────────────────────────
BA_SRCS = \
    libs/lib_bifid_adfgx.cpp \
    src/bifid.cpp \
    src/adfgx.cpp \
    src/hex_utils.cpp \
    src/console_utils.cpp

BA_OBJS = $(BA_SRCS:.cpp=.o)

# ── libaffine.so ──────────────────────────────────────────────────────────
AFFINE_SRCS = \
    libs/lib_affine.cpp \
    src/affine_cipher.cpp \
    src/file_processor.cpp \
    src/key_generator.cpp

AFFINE_OBJS = $(AFFINE_SRCS:.cpp=.o)

# ── libplayfair.so ────────────────────────────────────────────────────────
PF_SRCS = \
    libs/lib_playfair.cpp \
    src/playfair_cipher.cpp \
    src/file_processor.cpp \
    src/key_generator.cpp

PF_OBJS = $(PF_SRCS:.cpp=.o)

ALL_SO = \
    $(SO_DIR)/libaes.so \
    $(SO_DIR)/liba51.so \
    $(SO_DIR)/librsa.so \
    $(SO_DIR)/libhill_vernam.so \
    $(SO_DIR)/libbifid_adfgx.so \
    $(SO_DIR)/libaffine.so \
    $(SO_DIR)/libplayfair.so

all: $(ALL_SO) $(TARGET)

$(TARGET): $(MAIN_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SO_DIR)/libaes.so: $(AES_OBJS) $(VALIDATION_OBJ)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

$(SO_DIR)/liba51.so: $(A51_OBJS) $(VALIDATION_OBJ)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

$(SO_DIR)/librsa.so: $(RSA_OBJS) $(VALIDATION_OBJ)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ -lgmp

$(SO_DIR)/libhill_vernam.so: $(HV_OBJS) $(VALIDATION_OBJ)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

$(SO_DIR)/libbifid_adfgx.so: $(BA_OBJS) $(VALIDATION_OBJ)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

$(SO_DIR)/libaffine.so: $(AFFINE_OBJS) $(VALIDATION_OBJ)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

$(SO_DIR)/libplayfair.so: $(PF_OBJS) $(VALIDATION_OBJ)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

libs/%.o: libs/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o libs/*.o $(ALL_SO) $(TARGET)

.PHONY: all clean
