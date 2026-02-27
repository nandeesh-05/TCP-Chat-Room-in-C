#ifndef HEAD_H
#define HEAD_H


#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include<signal.h>

#define LOGIN 1
#define REGISTER 2
#define SINGLECHAT 3
#define GROUPCHAT 4
#define ERROR_MSG 5
#define ONLINELIST 6
#define EXIT 7

#define SERVER_IP_ADDRESS 	"127.0.0.1"
#define SERVER_PORT       	8149

struct d
{
    char user[50];
    char pass[50];
    int opt;

};

struct packet
{
    char sender[50];
    char receiver[50];
    char msg[100];
    int type;
};



/* =========================
   ANSI Color Code Macros
   ========================= */

#define RESET       "\033[0m"

#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"

/* Bold */
#define BOLD_BLACK      "\033[1;30m"
#define BOLD_RED        "\033[1;31m"
#define BOLD_GREEN      "\033[1;32m"
#define BOLD_YELLOW     "\033[1;33m"
#define BOLD_BLUE       "\033[1;34m"
#define BOLD_MAGENTA    "\033[1;35m"
#define BOLD_CYAN       "\033[1;36m"
#define BOLD_WHITE      "\033[1;37m"

/* Background Colors */
#define BG_BLACK    "\033[40m"
#define BG_RED      "\033[41m"
#define BG_GREEN    "\033[42m"
#define BG_YELLOW   "\033[43m"
#define BG_BLUE     "\033[44m"
#define BG_MAGENTA  "\033[45m"
#define BG_CYAN     "\033[46m"
#define BG_WHITE    "\033[47m"



















#endif