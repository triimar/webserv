NAME = webserv

INCDIR		=	./include
SRCDIR		=	./src
SUBDIRNAMES	=	./
SUBDIRS		=	$(foreach name, $(SUBDIRNAMES), $(SRCDIR)/$(name))
OBJDIR		=	./obj

INCS		=	$(wildcard $(INCDIR)/*.hpp)
SRCS		=	$(foreach dir, $(SUBDIRS), $(wildcard $(dir)/*.cpp))
OBJS		=	$(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

CP = c++
FLAGS = -Werror -Wextra -Wall -std=c++98 -g -fsanitize=address

PINK = \033[1;95m
GREEN = \033[1;92m
RESET = \033[0m

$(NAME): $(OBJS)
	@$(CP) $(OBJS) $(FLAGS) -o $(NAME)
	@echo "$(GREEN)Compiled successfully!$(RESET)"

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CC) -c -o $@ $< $(FLAGS)

all: $(NAME)

clean:
	@/bin/rm -rf $(OBJ_DIR)
	@echo "$(PINK)clean successful!$(RESET)"

fclean: clean
	@/bin/rm -f $(NAME)
	@echo "$(PINK)fclean successful!$(RESET)"

re: fclean all

.PHONY: all clean fclean re