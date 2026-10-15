# Tests unitaires de la libft, avec AddressSanitizer.
# Les sources ../ft_*.c sont recompilées ici (avec ASan) dans build/ :
# rien n'est écrit à la racine du dépôt.
#
# Linux / WSL :   make -C tests run        (V=1 : détail de chaque cas)
# Couleurs : auto si terminal ; NO_COLOR=1 pour couper, FORCE_COLOR=1 pour forcer.

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -g -fsanitize=address -fno-omit-frame-pointer
LDFLAGS		= -fsanitize=address

LIB_DIR		= ..
BUILD		= build
NAME		= $(BUILD)/run_tests

LIB_SRCS	= $(wildcard $(LIB_DIR)/ft_*.c)
LIB_OBJS	= $(patsubst $(LIB_DIR)/%.c,$(BUILD)/lib/%.o,$(LIB_SRCS))

TEST_SRCS	= runner.c test_ctype.c test_mem.c test_str.c test_conv.c \
			  test_alloc.c test_fd.c test_lst.c test_ub.c
TEST_OBJS	= $(patsubst %.c,$(BUILD)/%.o,$(TEST_SRCS))

all: $(NAME)

$(NAME): $(LIB_OBJS) $(TEST_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD)/lib/%.o: $(LIB_DIR)/%.c $(LIB_DIR)/libft.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(LIB_DIR) -c $< -o $@

$(BUILD)/%.o: %.c test.h $(LIB_DIR)/libft.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(LIB_DIR) -c $< -o $@

run: $(NAME)
	@mkdir -p $(BUILD)/logs
	./$(NAME) $(if $(V),-v)

clean:
	rm -rf $(BUILD)

re: clean all

.PHONY: all run clean re
