# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/02 13:15:43 by jmeirele          #+#    #+#              #
#    Updated: 2025/06/04 15:29:52 by jmeirele         ###   ########.fr        #
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
SRC_PATH    = src
SERVER_PATH = $(SRC_PATH)/server
UTILS_PATH  = $(SRC_PATH)/utils

SRCS        = $(SRC_PATH)/main.cpp \
              $(SERVER_PATH)/Server.cpp \
              $(SERVER_PATH)/ServerConfig.cpp \
              $(UTILS_PATH)/Logger.cpp \

INCLUDES    = -I.
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