NAME		=	webserv

INCDIR		=	include
SRCDIR		=	src
SUBDIRNAMES	=	server request response utils config
SUBDIRS		=	$(foreach name, $(SUBDIRNAMES), $(SRCDIR)/$(name))
BUILDDIR	=	./build

CP			=	c++
CPPFLAGS	=	-Werror -Wextra -Wall -std=c++98 -MMD -MP -g -fsanitize=address 
INCFLAGS	=	-I $(INCDIR)

INCS		=	$(wildcard $(INCDIR)/*.hpp)
SRCS		:=	$(SRCDIR)/main.cpp \
				$(foreach dir, $(SUBDIRNAMES), $(wildcard $(SRCDIR)/$(dir)/*.cpp))
OBJS		=	$(addprefix $(BUILDDIR)/, $(SRCS:.cpp=.o))
DEPS		=	$(OBJS:.o=.d)

PINK		=	\033[1;95m
GREEN		=	\033[1;92m
RED			=	\033[0;31m
RESET		=	\033[0m

COMP = compiler

$(NAME): $(OBJS)
	@$(CP) $(CPPFLAGS) $(INCFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Compiled successfully!$(RESET)"

-include $(DEPS)

# $(COMP):
# 	@printf "$(RED)Compiling."

$(BUILDDIR)/%.o: %.cpp
	@$(shell if [ ! -d "$(@D)" ]; then mkdir -p $(@D); fi)
	@$(CP) $(CPPFLAGS) $(INCFLAGS) -c $< -o $@
# @printf "."

all: $(NAME)

clean:
	@/bin/rm -rf $(BUILDDIR)
	@echo "$(PINK)clean successful!$(RESET)"

fclean: clean
	@/bin/rm -f $(NAME)
	@echo "$(PINK)fclean successful!$(RESET)"

re: fclean all

.PHONY: all clean fclean re