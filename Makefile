# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/02 13:15:43 by jmeirele          #+#    #+#              #
#    Updated: 2025/06/03 16:39:16 by meferraz         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = webserv

#==============================================================================#
#                                FLAGS & COMMANDS                              #
#==============================================================================#

CPP         = c++
CPPFLAGS    = -Wall -Wextra -Werror -g -std=c++98
RM          = rm -fr
MKDIR       = mkdir -p

#==============================================================================#
#                                 SOURCE FILES                                 #
#==============================================================================#

BUILD_PATH  = .build
# SERVER_PATH = src/server

SRCS        = main.cpp src/server/Server.cpp

INCLUDES    = -Isrc/server
OBJS        = $(SRCS:%.cpp=$(BUILD_PATH)/%.o)

#==============================================================================#
#                                    RULES                                     #
#==============================================================================#

all: $(NAME)

$(BUILD_PATH)/%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

#==============================================================================#
#                                  DEBUGGING                                   #
#==============================================================================#

GDB_FILE     = gdb.txt

gdb: all $(NAME)
	tmux split-window -h "gdb --tui --args ./$(NAME)"
	tmux resize-pane -L 5
	make get_log

get_log:
	rm -f $(GDB_FILE)
	touch $(GDB_FILE)
	@if command -v lnav; then \
		lnav $(GDB_FILE; \)
	else \
		tail -f $(GDB_FILE; \)
	fi

#==============================================================================#
#                                CLEANING RULES                                #
#==============================================================================#

clean:
	$(RM) $(BUILD_PATH) $(GDB_FILE)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
