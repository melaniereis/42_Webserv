# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/02 13:15:43 by jmeirele          #+#    #+#              #
<<<<<<< HEAD
#    Updated: 2025/06/05 17:10:53 by meferraz         ###   ########.fr        #
=======
#    Updated: 2025/06/05 17:40:29 by jmeirele         ###   ########.fr        #
>>>>>>> 67bfcd04716ebe35ec69a6b3ff92227defe2b8b9
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
CLIENT_PATH = $(SRC_PATH)/client
UTILS_PATH  = $(SRC_PATH)/utils
<<<<<<< HEAD
=======
RESPONSE_PATH = $(SRC_PATH)/response
>>>>>>> 67bfcd04716ebe35ec69a6b3ff92227defe2b8b9

SRCS        = $(SRC_PATH)/main.cpp \
              $(SERVER_PATH)/Server.cpp \
              $(SERVER_PATH)/ServerConfig.cpp \
              $(CLIENT_PATH)/Client.cpp \
              $(CLIENT_PATH)/ClientManager.cpp \
<<<<<<< HEAD
              $(UTILS_PATH)/Logger.cpp \
=======
              $(RESPONSE_PATH)/Response.cpp \
              $(UTILS_PATH)/Logger.cpp 
>>>>>>> 67bfcd04716ebe35ec69a6b3ff92227defe2b8b9

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
