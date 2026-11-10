# ============================================================
#  Makefile — Yo-kai Watch 2 Decompilation (Nintendo 3DS)
# ============================================================

PYTHON   := python3
TOOLS    := tools
EXTRACT  := $(PYTHON) $(TOOLS)/extract_rom.py
PROGRESS := $(PYTHON) $(TOOLS)/audit_progress.py
CLASSES  := $(PYTHON) $(TOOLS)/extract_classes.py
CHECK    := $(PYTHON) $(TOOLS)/check_match.py
DUMP     := $(PYTHON) $(TOOLS)/dump_asm.py
REA      := $(PYTHON) $(TOOLS)/rea_evidence.py
INJECT   := $(PYTHON) $(TOOLS)/inject_code.py
LIVE     := $(PYTHON) $(TOOLS)/export_live.py
AZAHAR_MOD_DIR := $(HOME)/.local/share/azahar-emu/load/mods/00040000001B2900/exefs

CC       := arm-none-eabi-gcc
CXX      := arm-none-eabi-g++
AS       := arm-none-eabi-as
ASFLAGS  := -mcpu=mpcore -mfpu=vfpv2
CFLAGS   := -c -mcpu=mpcore -O3 -I . -I include
CXXFLAGS := -c -mcpu=mpcore -O3 -I . -I include -fno-rtti -fno-exceptions

C_SRCS   := $(shell find src -name '*.c' 2>/dev/null)
CPP_SRCS := $(shell find src -name '*.cpp' 2>/dev/null)
S_SRCS   := $(shell find src -name '*.s' 2>/dev/null)
OBJS     := $(patsubst src/%.c, build/src/%.o, $(C_SRCS)) \
            $(patsubst src/%.cpp, build/src/%.o, $(CPP_SRCS)) \
            $(patsubst src/%.s, build/src/%.o, $(S_SRCS))

.PHONY: all build extract progress classes check inject dump rea report info clean distclean live hooks

all: build

build: $(OBJS)

report: $(OBJS)
	@mkdir -p build expected
	@python3 tools/generate_report.py

hooks:
	@mkdir -p .git/hooks tools/hooks
	@chmod +x tools/hooks/pre-commit tools/hooks/pre-push
	@cp tools/hooks/pre-commit .git/hooks/pre-commit
	@cp tools/hooks/pre-push .git/hooks/pre-push
	@chmod +x .git/hooks/pre-commit .git/hooks/pre-push
	@git config core.hooksPath tools/hooks 2>/dev/null || true
	@echo "✅ Git pre-commit y pre-push hooks instalados y configurados correctamente."

info:
	@echo "============================================================"
	@echo "  Yo-kai Watch 2: Psychic Specters — Decompilation (3DS)"
	@echo "============================================================"
	@echo "Available targets:"
	@echo "  make check      - Compile and verify byte-matching of implemented functions"
	@echo "  make hooks      - Install Git pre-commit & pre-push hooks (CI & README checks)"
	@echo "  make inject     - Build 'build/code.bin' and install to Azahar emulator mods"
	@echo "  make dump       - Dump clean assembly for decomp.me (e.g. make dump FUNC=0x0010B6D8)"
	@echo "  make rea        - Run REA 3DS evidence inspector (make rea FUNC=0x001389CC or UNIT=CTR_Api.cpp)"
	@echo "  make progress   - Audit progress and update README tables"
	@echo "  make extract    - Inspect ROM and decompress 'code.bin'"
	@echo "  make clean      - Clean temporary build artifacts"
	@echo "  make distclean  - Also remove extracted binaries in 'extracted/'"
	@echo ""
	@if [ -f "extracted/code.raw.bin" ]; then \
		echo "✓ Decompressed executable present: extracted/code.raw.bin"; \
	elif [ -f "extracted/code.bin" ]; then \
		echo "✓ Executable 'code.bin' detected in extracted/"; \
	else \
		echo "⚠️ Extraction pending: run 'make extract' with a decrypted ROM"; \
	fi

extract:
	@$(EXTRACT)

progress:
	@if [ -f "extracted/code.raw.bin" ]; then \
		$(PYTHON) $(TOOLS)/generate_expected.py >/dev/null; \
	fi
	@$(PROGRESS) --update-readme
	@$(LIVE)

live:
	@$(LIVE)

classes:
	@$(CLASSES)

check: $(OBJS)
	@$(CHECK)

inject: $(OBJS)
	@$(INJECT)
	@if [ -d "$(AZAHAR_MOD_DIR)" ]; then \
		cp build/code.bin "$(AZAHAR_MOD_DIR)/code.bin"; \
		echo "✅ Installed build/code.bin -> $(AZAHAR_MOD_DIR)/code.bin"; \
	fi

dump:
	@if [ -z "$(FUNC)" ]; then \
		echo "Usage: make dump FUNC=<address_or_symbol> [e.g. make dump FUNC=0x0010B6D8]"; \
	else \
		$(DUMP) $(FUNC) --plain; \
	fi

rea:
	@if [ -n "$(FUNC)" ]; then \
		$(REA) --func $(FUNC); \
	elif [ -n "$(UNIT)" ]; then \
		$(REA) --unit $(UNIT); \
	else \
		echo "Usage: make rea FUNC=<address_or_symbol> | make rea UNIT=<unit.cpp>"; \
	fi


build/src/game/loops/func_0010C870.o: CFLAGS += -ffixed-r3 -ffixed-r2
build/src/game/loops/func_0010C8A0.o: CFLAGS += -ffixed-r3 -ffixed-r2
build/src/game/loops/func_0010CA68.o: CFLAGS += -ffixed-r3 -ffixed-r2

WIBO     := tools/wibo
ARMCC    := $(WIBO) tools/armcc/4.0/b821/bin/armcc.exe
ARMFLAGS := --cpu=mpcore -O3 --gnu -c -I . -I include
build/src/ctr/nn/nex/qResult.o: ARMFLAGS += -Otime
build/src/ctr/nn/applet/applet.o: ARMFLAGS += --split_sections
build/src/ctr/nn/dsp/dsp_DSP.o: ARMFLAGS += --split_sections
build/src/ctr/nn/dsp/CTR_Api.o: ARMFLAGS += --split_sections
build/src/ctr/nn/ndm/ndm_Api.o: ARMFLAGS += --split_sections
build/src/ctr/nn/os/os_Api.o: ARMFLAGS += --split_sections
build/src/ctr/nn/y2r/y2r_Y2r.o: ARMFLAGS += --split_sections --cpu=arm1136j-s
build/src/ctr/nn/y2r/detail_Api.o: ARMFLAGS += --split_sections
build/src/ctr/nn/nex/nex_RootObject.o: ARMFLAGS += --split_sections
build/src/engine/lfw/lfwCLoopEmpty.o: ARMFLAGS += --split_sections
build/src/engine/lfw/lfwCLoop.o: ARMFLAGS += --split_sections
build/src/engine/lxp/lxpCMemory.o: ARMFLAGS += --split_sections
build/src/engine/core/lfwCObjManager.o: ARMFLAGS += --split_sections

build/src/game/loops/func_0010C880.o: ARMFLAGS += -Otime
build/src/game/loops/func_0010C880.o: src/game/loops/func_0010C880.c
	@mkdir -p $(dir $@)
	@if [ -f "tools/armcc/4.0/b821/bin/armcc.exe" ]; then \
		$(ARMCC) $(ARMFLAGS) $< -o $@; \
	else \
		echo "⚠️ Warning: ARMCC not found, compiling with GCC fallback..."; \
		$(CC) $(CFLAGS) $< -o $@; \
	fi

build/src/game/loops/func_0011054C.o: src/game/loops/func_0011054C.c
	@mkdir -p $(dir $@)
	@if [ -f "tools/armcc/4.0/b821/bin/armcc.exe" ]; then \
		$(ARMCC) $(ARMFLAGS) $< -o $@; \
	else \
		echo "⚠️ Warning: ARMCC not found, compiling with GCC fallback..."; \
		$(CC) $(CFLAGS) $< -o $@; \
	fi

build/src/ctr/nn/dsp/dsp_DSP.o: src/ctr/nn/dsp/dsp_DSP.cpp
	@mkdir -p $(dir $@)
	@if [ -f "tools/armcc/4.1/b1049/bin/armcc.exe" ]; then \
		$(WIBO) tools/armcc/4.1/b1049/bin/armcc.exe $(ARMFLAGS) --split_sections --cpp $< -o $@; \
	elif [ -f "tools/armcc/4.0/b821/bin/armcc.exe" ]; then \
		$(ARMCC) $(ARMFLAGS) --split_sections --cpp $< -o $@; \
	else \
		$(CXX) $(CXXFLAGS) -ffunction-sections $< -o $@; \
	fi

build/src/ctr/nn/dsp/CTR_Api.o: src/ctr/nn/dsp/CTR_Api.cpp
	@mkdir -p $(dir $@)
	@if [ -f "tools/armcc/4.0/b821/bin/armcc.exe" ] && [ -f "tools/armcc/4.1/b1049/bin/armcc.exe" ]; then \
		$(ARMCC) $(ARMFLAGS) --split_sections --cpp $< -o $@.p1 && \
		$(WIBO) tools/armcc/4.1/b1049/bin/armcc.exe $(ARMFLAGS) -DCTR_API_PASS_B1049 -Otime --split_sections --cpp $< -o $@.p2 && \
		arm-none-eabi-ld -r $@.p1 $@.p2 -o $@ && rm -f $@.p1 $@.p2; \
	elif [ -f "tools/armcc/4.0/b821/bin/armcc.exe" ]; then \
		$(ARMCC) $(ARMFLAGS) --split_sections --cpp $< -o $@; \
	else \
		$(CXX) $(CXXFLAGS) -ffunction-sections $< -o $@; \
	fi

build/src/ctr/nn/snd/snd_Dspsnd.o: src/ctr/nn/snd/snd_Dspsnd.cpp
	@mkdir -p $(dir $@)
	@if [ -f "tools/armcc/4.0/b821/bin/armcc.exe" ] && [ -f "tools/armcc/4.1/b1049/bin/armcc.exe" ] && [ -f "tools/armcc/4.1/b561/bin/armcc.exe" ]; then \
		$(ARMCC) $(ARMFLAGS) --split_sections --cpp $< -o $@.p1 && \
		$(WIBO) tools/armcc/4.1/b1049/bin/armcc.exe --cpu=mpcore -O2 -Otime -Ono_tailcall --gnu -c -I . -I include -DSND_DSPSND_PASS_B1049 --split_sections --cpp $< -o $@.p2 && \
		$(WIBO) tools/armcc/4.1/b561/bin/armcc.exe $(ARMFLAGS) -Otime -DSND_DSPSND_PASS_B561 --split_sections --cpp $< -o $@.p3 && \
		arm-none-eabi-ld -r $@.p1 $@.p2 $@.p3 -o $@ && rm -f $@.p1 $@.p2 $@.p3; \
	elif [ -f "tools/armcc/4.0/b821/bin/armcc.exe" ]; then \
		$(ARMCC) $(ARMFLAGS) --split_sections --cpp $< -o $@; \
	else \
		$(CXX) $(CXXFLAGS) -ffunction-sections $< -o $@; \
	fi

build/src/engine/lfw/lfwCLoop.o: src/engine/lfw/lfwCLoop.cpp
	@mkdir -p $(dir $@)
	@if [ -f "tools/armcc/4.1/b1049/bin/armcc.exe" ]; then \
		$(WIBO) tools/armcc/4.1/b1049/bin/armcc.exe $(ARMFLAGS) --split_sections --cpp $< -o $@; \
	elif [ -f "tools/armcc/4.0/b821/bin/armcc.exe" ]; then \
		$(ARMCC) $(ARMFLAGS) --split_sections --cpp $< -o $@; \
	else \
		$(CXX) $(CXXFLAGS) -ffunction-sections $< -o $@; \
	fi

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	@if [ -f "tools/armcc/4.0/b821/bin/armcc.exe" ]; then \
		$(ARMCC) $(ARMFLAGS) --cpp $< -o $@; \
	else \
		$(CXX) $(CXXFLAGS) $< -o $@; \
	fi

build/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	@rm -rf build/
	@echo "Build cleaned."

distclean: clean
	@rm -rf extracted/
	@echo "Extracted binaries removed."
