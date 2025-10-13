# Program names
CLIENT = client
SERVER = server

# Directories
SRC_DIR = .
SRC_BONUS_DIR = src_bonus
INCLUDE_DIR = .
LIBFT_DIR = libft
OBJ_DIR = obj

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -I $(INCLUDE_DIR)
AR = ar rcs
RM = rm -rf

# Source files
SRC_CLIENT = client.c
SRC_SERVER = server.c

# Bonus source files
# SRC_CLIENT_BONUS = client_bonus.c
# SRC_SERVER_BONUS = server_bonus.c

OBJS_CLIENT = $(addprefix $(OBJ_DIR)/, $(SRC_CLIENT:.c=.o))
OBJS_SERVER = $(addprefix $(OBJ_DIR)/, $(SRC_SERVER:.c=.o))
# OBJS_CLIENT_BONUS = $(addprefix $(OBJ_DIR)/, $(SRC_CLIENT_BONUS:.c=.o))
# OBJS_SERVER_BONUS = $(addprefix $(OBJ_DIR)/, $(SRC_SERVER_BONUS:.c=.o))

# Libraries
LIBFT = $(LIBFT_DIR)/libft.a

all: $(CLIENT) $(SERVER)

$(CLIENT): $(OBJS_CLIENT) $(LIBFT)
	@$(CC) $(CFLAGS) $(OBJS_CLIENT) $(LIBFT) -o $(CLIENT)

$(SERVER): $(OBJS_SERVER) $(LIBFT)
	@$(CC) $(CFLAGS) $(OBJS_SERVER) $(LIBFT) -o $(SERVER)
	@$(MAKE) banner

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR) --no-print-directory

# Banner rules
banner:
	@echo "╔════════════════════════════════════════╗"
	@echo "║       MINITALK COMPILED! 🚀            ║"
	@echo "╔════════════════════════════════════════╗"

bonus:
	@echo "Bonus not implemented yet"

.PHONY: all clean fclean re normi banner bonus

clean:
	@$(MAKE) -C $(LIBFT_DIR) clean
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@$(RM) $(CLIENT) $(SERVER)

re: fclean all

normi:
	@norminette -R CheckForbiddenSourceHeader
