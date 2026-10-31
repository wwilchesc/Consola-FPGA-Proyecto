CC ?= gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
ENGINE = firmware/console.c firmware/controls.c firmware/maze.c firmware/snake.c firmware/draw.c firmware/services.c firmware/platform.c
HEADERS = $(wildcard firmware/*.h)

.PHONY: all test demo clean
all: test demo

build:
	mkdir -p build

build/test_console: sim/test_console.c $(ENGINE) $(HEADERS) | build
	$(CC) $(CFLAGS) -o $@ sim/test_console.c $(ENGINE)

test: build/test_console
	./build/test_console

build/demo_host: firmware/demo_host.c $(ENGINE) $(HEADERS) | build
	$(CC) $(CFLAGS) -o $@ firmware/demo_host.c $(ENGINE)

demo: build/demo_host
	./build/demo_host

clean:
	rm -rf build

RISCV_CC ?= riscv64-unknown-elf-gcc
RISCV_SIZE ?= riscv64-unknown-elf-size
RVFLAGS = -std=c11 -Os -Wall -Wextra -Werror -march=rv32i -mabi=ilp32 -ffreestanding -fno-builtin -fstack-usage
RVOBJECTS = $(patsubst firmware/%.c,build/rv32/%.o,$(ENGINE))

build/rv32/%.o: firmware/%.c $(HEADERS)
	mkdir -p build/rv32
	$(RISCV_CC) $(RVFLAGS) -c $< -o $@

.PHONY: size-rv32 sanitize
size-rv32: $(RVOBJECTS)
	$(RISCV_SIZE) $(RVOBJECTS)

sanitize: | build
	$(CC) -std=c11 -g -O1 -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -o build/test_sanitize sim/test_console.c $(ENGINE)
	ASAN_OPTIONS=detect_leaks=0 ./build/test_sanitize

build/render_reference: sim/render_reference.c $(ENGINE) $(HEADERS) | build
	$(CC) $(CFLAGS) -o $@ sim/render_reference.c $(ENGINE)

.PHONY: compare-html
compare-html: build/render_reference
	node sim/compare_html.cjs
