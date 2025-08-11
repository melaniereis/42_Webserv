# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/02 13:15:43 by jmeirele          #+#    #+#              #
#    Updated: 2025/08/11 15:24:12 by meferraz         ###   ########.fr        #
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
SERVER_PATH = $(SRC_PATH)/servers
CLIENT_PATH = $(SRC_PATH)/client
CONFIG_PATH  = $(SRC_PATH)/config
UTILS_PATH  = $(SRC_PATH)/utils
HTTP_PATH   = $(SRC_PATH)/http
CGI_PATH = $(SRC_PATH)/cgi
SESSION_PATH = $(SRC_PATH)/session

SRCS        = $(SRC_PATH)/main.cpp \
              $(CONFIG_PATH)/ServerConfig.cpp \
			  $(CONFIG_PATH)/LocationConfig.cpp \
			  $(CONFIG_PATH)/ConfigParser.cpp \
			  $(CONFIG_PATH)/ListenConfig.cpp \
              $(SERVER_PATH)/Server.cpp \
			  $(SERVER_PATH)/WebServer.cpp \
              $(CLIENT_PATH)/Client.cpp \
              $(CLIENT_PATH)/ClientManager.cpp \
              $(HTTP_PATH)/Request.cpp \
              $(HTTP_PATH)/Response.cpp \
              $(HTTP_PATH)/RequestHandler.cpp \
              $(HTTP_PATH)/RequestHandlerUtils.cpp \
              $(HTTP_PATH)/HttpStatus.cpp \
			  $(CGI_PATH)/CgiHandler.cpp \
              $(UTILS_PATH)/Logger.cpp \

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
#                                   TESTING                                    #
#==============================================================================#


test: $(NAME)
	clear ; ./$(NAME) config/valid/miguel.conf

#==============================================================================#
#                                CLEANING RULES                                #
#==============================================================================#

clean:
	$(RM) $(BUILD_PATH) $(GDB_FILE)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
