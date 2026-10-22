CC = cc

CFLAGS = -std=c23 -Wall -Wextra -Werror -Wno-unused-parameter -fno-asm -Istd -Iinclude -ggdb -g -fsanitize=address -Wno-implicit-fallthrough -Ofast -march=native -flto -funroll-loops -fomit-frame-pointer -fno-semantic-interposition
LDFLAGS = -ggdb -g -fsanitize=address -lm -flto -Wl,-O3 -Wl,--gc-sections

TARGET = target/csvm.out
BUILD_DIR = build
TARGET_DIR = target

SRCS = $(shell find . -name "*.c")
OBJS = $(patsubst ./%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(patsubst ./%.c,$(BUILD_DIR)/%.d,$(SRCS))

RED     := \033[31m
GREEN   := \033[32m
YELLOW  := \033[33m
MAGENTA := \033[35m
CYAN    := \033[36m
RESET   := \033[0m
BOLD    := \033[1m


.PHONY: clean all clear

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	@printf "\n$(BOLD)$(GREEN)Compilation finished with code %d ✓$(RESET)" $$status;
	@printf "\n$(BOLD)$(CYAN)Linking ↺$(RESET)\n"
	@if ! $(CC) -o $@ $^ $(LDFLAGS) > /dev/null ; then \
		printf "\n$(BOLD)$(RED)Linking failed ✘$(RESET)\n"; \
		exit 1;\
	fi

	@printf "$(BOLD)$(GREEN)Linking finished ✓$(RESET)"
	@printf "\n$(BOLD)$(CYAN)Executable dumped:$(RESET) $(BOLD)$(YELLOW)$(TARGET) ✓$(RESET)\n\n"

$(BUILD_DIR)/%.o: ./%.c
	@printf "\n$(BOLD)$(CYAN)Compiling $< ⧗$(RESET)"

	@mkdir -p $(dir $@)

	@if ! $(CC) $(CFLAGS) -c $< -o $@ > /dev/null; then \
		printf "\n$(BOLD)$(RED)Compilation exited abnormally with code %d ✘ $(RESET)\n\n" $$status;\
		exit 1;\
	fi


run:
	@rlwrap ./$(TARGET)

clean:
	@echo "Cleaning build directory 🗑️"
	rm -rf $(BUILD_DIR)

clear:
	@echo "Clearing project directory 🗑️"
	rm -rf $(BUILD_DIR) $(TARGET) $(TARGET_DIR)

-include $(DEPS)




