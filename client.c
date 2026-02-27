#include "head.h"
pthread_mutex_t online_mutex = PTHREAD_MUTEX_INITIALIZER;
int sockfd;
void *clientdata(void * argc)
{

	int sockfd = *(int *)argc;
	free(argc);
	while(1)
	{
		struct packet client;
		memset(&client,0,sizeof(client));
		socklen_t len=sizeof(client);
		int rec=recv(sockfd,&client,len,0);
		if(rec<=0)
		{
			printf(RED "Server disconnected!" RESET "\n");
			close(sockfd);
			pthread_exit(NULL);
		}
		if(client.type == SINGLECHAT || client.type == GROUPCHAT)
		{
			// send the message  through packet
			// recv the message through packet
			if(client.type == SINGLECHAT)
			{
				//pthread_mutex_lock(&online_mutex);
				printf(GREEN "MESSAGE :" CYAN "%s " RESET "from " YELLOW "%s\n" RESET, client.msg, client.sender);
				fflush(stdout);
				//pthread_mutex_unlock(&online_mutex);
			}
			else if(client.type == GROUPCHAT)
			{
				//pthread_mutex_lock(&online_mutex);
				printf(MAGENTA "GROUP :" GREEN "%s\n" RESET, client.msg);
				fflush(stdout);
				//pthread_mutex_unlock(&online_mutex);

			}
		}
		if(client.type == ONLINELIST)
		{
			// printing the online users

			if(strcmp(client.sender,"END")!=0)
			{
               printf(BOLD_CYAN "• %s is online\n" RESET, client.sender);
				fflush(stdout);
				//pthread_mutex_unlock(&online_mutex);
			}
			if(strcmp(client.msg,"offline")==0)
			{
				printf(YELLOW "%s " RESET "has gone offline.\n", client.sender);
				fflush(stdout);
			}
		}
		if(client.type == ERROR_MSG)
		{
			// printing the error mesaages to client
			if(strcmp(client.msg,"SUCCESS")==0)
			{
				printf(BOLD_GREEN "\n✔ VALIDATION SUCCESSFUL\n" RESET);
				fflush(stdout);
			}
			else{
				printf(RED "ERROR : %s\n" RESET, client.msg);
				//printf("Hello\n");
				fflush(stdout);
				kill(getpid(),SIGTERM);
			} 
		}
		if(client.type == EXIT)
		{
			// termination
			// kill(getpid(),SIGTERM);
			// exit(0);
			printf(BOLD_RED "Server crashed!\n" RESET);
			fflush(stdout);
			kill(getpid(),SIGTERM);
		}

	}

	close(sockfd);

}
void ownhandler(int sig)
{
	
	struct packet client;
	memset(&client,0,sizeof(client));
	client.type = EXIT;

	send(sockfd, &client, sizeof(client), 0);
	printf(GREEN "Exiting gracefully...\n" RESET);

	close(sockfd);
	//exit(0);
	signal(SIGINT,SIG_DFL);
	kill(getpid(),SIGINT);
	//break;
}
int main()
{


	int op;
	printf(BOLD_CYAN "\n======= MAIN MENU =======\n" RESET);
    printf(BOLD_YELLOW "1. " GREEN "Login\n" RESET);
    printf(BOLD_YELLOW "2. " GREEN "Register\n" RESET);
    printf(BOLD_CYAN "Select the Option: " RESET);
	scanf("%d",&op);
	struct d data;
	printf(BOLD_CYAN "\nEnter the username: " RESET);
	scanf(" %[^\n]",data.user);
	printf(BOLD_CYAN "Enter the password: " RESET);
	scanf(" %[^\n]",data.pass);
	data.opt=op;
	// create the socket and connect this server
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		printf("Socket not created for client\n");
		return 0;
	}
	/* Populate it with server's address details */
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT); 
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);

	if(connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1)
	{
		perror("connect");
		printf(BOLD_RED "Failed to connect to the server.\n" RESET);
		exit(1);
	}
	else
	{
		printf(BOLD_GREEN "Connection to the server was successful.\n" RESET);
	}
	// send the username and password to server
	send(sockfd,&data,sizeof(data),0);
	pthread_t tid;
	int *p = malloc(sizeof(int));
	*p = sockfd;
	signal(SIGINT,ownhandler);
	pthread_create(&tid,NULL,clientdata,p);
	while(1)
	{

		int val;
		char mess[100];
		struct packet client;
		memset(&client,0,sizeof(client));
		usleep(10000);
		//pthread_mutex_lock(&online_mutex);
		printf(BOLD_CYAN "\n======= CHAT MENU =======\n" RESET);
        printf(BOLD_YELLOW "1. " GREEN "Single chat\n" RESET);
        printf(BOLD_YELLOW "2. " GREEN "Group chat\n" RESET);
        printf(BOLD_YELLOW "3. " RED "Exit\n" RESET);
        printf(BOLD_CYAN "Enter Option: " RESET);
		scanf("%d",&val);

		//pthread_mutex_unlock(&online_mutex);
		if(val==1)
		{
			printf(GREEN "Your message: " RESET);
			scanf(" %[^\n]",mess);
			char name[50];
			client.type=SINGLECHAT;
			strcpy(client.msg,mess);
			printf(BOLD_CYAN "To whom you want to send: " RESET);
			scanf("%s",name);
			strcpy(client.receiver,name);
			send(sockfd,&client,sizeof(client),0);

		}
		if(val==2)
		{
			printf(GREEN "Your message: " RESET);
			scanf(" %[^\n]",mess);
			client.type=GROUPCHAT;
			strcpy(client.msg,mess);
			send(sockfd,&client,sizeof(client),0);
		}
		if(val==3)
		{
			client.type=EXIT;
			send(sockfd,&client,sizeof(client),0);
			exit(0);

		}


	}


}
