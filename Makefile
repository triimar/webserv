NAME = webservto

SRC = main.cpp Server.cpp Config.cpp

CP = c++

OBJ = $(SRC:%.cpp=$(OBJ_DIR)%.o)

OBJ_DIR = obj/

FLAGS = -Werror -Wextra -Wall -std=c++98 -g -fsanitize=address

PINK = \033[1;95m
GREEN = \033[1;92m
RESET = \033[0m

all: $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ_DIR) $(OBJ)
	@$(CP) $(OBJ) $(FLAGS) -o $(NAME)
	@echo "$(GREEN)Compiled successfully!$(RESET)"

$(OBJ_DIR)%.o: %.cpp
	@$(CP) $(FLAGS) -c $< -o $@

clean:
	@/bin/rm -rf $(OBJ_DIR)
	@echo "$(PINK)Clean successful!$(RESET)"

fclean: clean
	@/bin/rm -f $(NAME)

re: fclean all