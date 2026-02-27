#include "head.h"
Dlist *head=NULL;
Olist *head1=NULL;
pthread_mutex_t online_mutex = PTHREAD_MUTEX_INITIALIZER;
void *serverdata(void * argc)
{

	struct d data;
	int data_fd=(*(int *)argc);
	//free(*argc);
	free(argc); 
	memset(&data,0,sizeof(data));
	recv(data_fd,&data,sizeof(data),0);
	struct packet server;
	memset(&server,0,sizeof(server));
	if(data.opt==LOGIN) // login
	{
		printf(BOLD_CYAN "Client attempting database login...\n" RESET);
		if(user_check(&head,data.user)==SUCCESS)
		{
			if(pass_check(&head,data.pass)==SUCCESS)
			{
				printf(BOLD_GREEN "Client has successfully logged into the database.\n" RESET);
				online_to_all_clients(head1,data.user);
				insert_at_first_onlinelist(&head1,data.user,data_fd);
				server.type=ERROR_MSG;
				strcpy(server.msg,"SUCCESS");
				send(data_fd,&server,sizeof(server),0);

				print_onlinelist(head1,data.user,&data_fd);
			}
			else
			{
				// printf("Invalid Password to login\n");
				printf(BOLD_RED "Client could not login to the database.\n" RESET);
				server.type=ERROR_MSG;
				strcpy(server.msg,"Invalid Password to login");
				send(data_fd,&server,sizeof(server),0);
			}

		}

		else
		{
			printf(BOLD_RED "Client could not login to the database.\n" RESET);
			// printf("Invalid user name to login\n");
			server.type=ERROR_MSG;
			strcpy(server.msg,"Invalid user name to login");
			send(data_fd,&server,sizeof(server),0);
		}

	}
	else if(data.opt==REGISTER) // register
	{
		printf(BOLD_CYAN "Client is attempting to register data in the database...\n" RESET);
		if(user_check(&head,data.user)!=SUCCESS)
		{
			printf("Client successfully registered to database\n");
			online_to_all_clients(head1,data.user);
			insert_at_first_onlinelist(&head1,data.user,data_fd);
			insert_at_first_datalist(&head,data.user,data.pass);
			loadfile(&head);
			server.type=ERROR_MSG;
			strcpy(server.msg,"SUCCESS");
			send(data_fd,&server,sizeof(server),0);
			print_onlinelist(head1,data.user,&data_fd);
		}
		else
		{
			printf(BOLD_RED "Client could not register in the database.\n" RESET);
			server.type=ERROR_MSG;
			strcpy(server.msg,"User already present,so failed to register");
			send(data_fd,&server,sizeof(server),0);

		}
	}

	while(1)
	{
		int r=recv(data_fd,&server,sizeof(server),0);
		if(r <= 0)
		{
			printf(BOLD_CYAN "Client has disconnected from the server.\n" RESET);

			pthread_mutex_lock(&online_mutex);
			remove_fromonline(&head1,data.user);
			pthread_mutex_unlock(&online_mutex);

			close(data_fd);
			pthread_exit(NULL);
		}
		if(server.type==SINGLECHAT)
		{
			printf(BOLD_CYAN "Client has chosen Single chat mode.\n" RESET);
			char msg[100];
			char send[100];
			char recv[100];
			strcpy(msg,server.msg);
			strcpy(send,data.user);
			strcpy(recv,server.receiver);
			singlechat(head1,msg,send,recv);
			// printf("Client opted for Single chat\n");
			//printf("%s\n",server.msg);
		}
		if(server.type==GROUPCHAT)
		{
			printf(BOLD_CYAN "Client has chosen Group chat mode.\n" RESET);
			char msg[100];
			strcpy(msg,server.msg);
			groupchat(head1,msg,data.user);
		}
		if(server.type==EXIT)
		{
			printf(BOLD_CYAN "Client has chosen to exit the chat.\n" RESET);
			pthread_mutex_lock(&online_mutex);
			offline(head1,data.user);
			remove_fromonline(&head1,data.user);
			pthread_mutex_unlock(&online_mutex);
			close(data_fd);
			pthread_exit(NULL); 
		}

	}
	//close(data_fd);





}
void own_handler(int sig)
{
	serveroffline(head1);
	signal(SIGINT,SIG_DFL);
	kill(getpid(),SIGINT);
}

int main()
{
	signal(SIGPIPE, SIG_IGN);
	loadlist(&head);

	// create socket bind
	//int data_fd;
	pthread_t tid;
	int sockfd=socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)
	{
		printf("Socket not created for server\n");
		return 0;
	}
	struct sockaddr_in serveraddr;

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(SERVER_PORT); 
	serveraddr.sin_addr.s_addr=inet_addr(SERVER_IP_ADDRESS);
	int ret=bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	if(ret==-1)
	{
		perror("bind");
		return 0;
	}
	listen(sockfd,30);
	signal(SIGINT,own_handler);
	usleep(10000);
	while(1)
	{
		printf(BOLD_CYAN "Server waiting for the client...\n" RESET);

		int *data_fd=malloc(sizeof(int));
		//int data_fd;
		struct sockaddr_in clientaddr;
		socklen_t len=sizeof(clientaddr);

		*data_fd=accept(sockfd,(struct sockaddr *)&clientaddr,&len);
		printf(BOLD_GREEN "Server connected with Client.\n" RESET);

		pthread_create(&tid,NULL,serverdata,data_fd);

	}

}
