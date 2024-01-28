NAME		=	webserv

INCDIR		=	./include
SRCDIR		=	./src
SUBDIRNAMES	=	server request response utils config
SUBDIRS		=	$(foreach name, $(SUBDIRNAMES), $(SRCDIR)/$(name))
BUILDDIR	=	./build

CP			=	c++
CPPFLAGS	=	-Werror -Wextra -Wall -std=c++98 -g -fsanitize=address
INCFLAGS	=	-I $(INCDIR)

INCS		=	$(wildcard $(INCDIR)/*.hpp)
SRCS		=	$(SRCDIR)/main.cpp \
				$(foreach dir, $(SUBDIRNAMES), $(wildcard $(SRCDIR)/$(dir)/*.cpp))
OBJS		=	$(addprefix $(BUILDDIR)/, $(SRCS:.cpp=.o))
DEPS		=	$(OBJS:.o=.d)

PINK		=	\033[1;95m
GREEN		=	\033[1;92m
RESET		=	\033[0m

$(NAME): $(OBJS)
	@$(CP) $(CPPFLAGS) $(INCFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Compiled successfully!$(RESET)"

-include $(DEPS)

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CP) $(CPPFLAGS) $(INCFLAGS) -c $< -o $@

all: $(NAME)

clean:
	@/bin/rm -rf $(BUILDDIR)
	@echo "$(PINK)clean successful!$(RESET)"

fclean: clean
	@/bin/rm -f $(NAME)
	@echo "$(PINK)fclean successful!$(RESET)"

re: fclean all

.PHONY: all clean fclean re