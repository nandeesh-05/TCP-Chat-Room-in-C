#include "head.h"
extern pthread_mutex_t online_mutex ;
void print_onlinelist(Olist *head,char *user,int *sockfd)
{
	struct packet server;
	memset(&server,0,sizeof(server));
	server.type = ONLINELIST;

	if (head == NULL)
	{
		strcpy(server.msg,"No online users are present");
		send(*sockfd,&server,sizeof(server),0);
		return;
	}

	while (head)
	{
		if(strcmp(head->user,user) != 0)
		{
			memset(&server,0,sizeof(server));
			server.type = ONLINELIST;
			strcpy(server.sender,head->user);

			send(*sockfd,&server,sizeof(server),0);
		}

		head = head->link;   // ALWAYS MOVE POINTER
	}

	// Send END packet
	memset(&server,0,sizeof(server));
	server.type = ONLINELIST;
	strcpy(server.sender,"END");

	send(*sockfd,&server,sizeof(server),0);
}



void print_datalist(Dlist *head)
{
	if (head == NULL)
	{
		printf("Data list is empty\n");
	}
	else
	{
		printf("\n----- Data Base -----\n");

		while (head)		
		{
			printf("User: %s | Pass: %s\n", head->user, head->pass);
			head = head -> link;
		}

		//printf("NULL\n");
	}
}


int insert_at_first_onlinelist(Olist **head, char * user,int sockfd)
{

	// pthread_mutex_lock(&online_mutex);
	Olist *new =malloc(sizeof(Olist));   // create a new node
	new->fd=sockfd;
	strcpy(new->user,user);            // fill the node with data and link
	new->link=NULL;
	if(*head==NULL)
	{
		*head=new;
		return SUCCESS;   // If the List is empty
	}
	else
	{
		// Slist *temp=*head;
		new->link=*head;
		*head=new;            // if the list is not empty
		return SUCCESS;
	}
	//pthread_mutex_unlock(&online_mutex);
}

int insert_at_first_datalist(Dlist **head,char * user,char * pass)
{
	Dlist *new =malloc(sizeof(Dlist));   // create a new node
	strcpy(new->user,user);
	strcpy(new->pass,pass);              // fill the node with data and link
	new->link=NULL;
	if(*head==NULL)
	{
		*head=new;
		return SUCCESS;   // If the List is empty
	}
	else
	{
		// Slist *temp=*head;
		new->link=*head;
		*head=new;            // if the list is not empty
		return SUCCESS;
	}
}

void loadlist(Dlist **head)
{
	FILE *fptr = fopen("data.csv", "r");
	if (fptr == NULL)
	{
		printf("Failed to open the file\n");
		return;
	}

	char user[50];
	char pass[50];

	while (fscanf(fptr, "%[^,],%[^\n]\n", user, pass) !=EOF)
	{
		insert_at_first_datalist(head, user, pass);
	}

	fclose(fptr);
}
int user_check(Dlist ** head,char *user)
{
	if(*head==NULL)
	{
		printf("Data list is empty\n");
		return FAILURE;
	}
	else
	{
		Dlist *temp=*head;
		while(temp!=NULL)
		{
			if(strcmp(temp->user,user)==0)
			{
				return SUCCESS;
			}
			temp=temp->link;

		}
		return FAILURE;
	}
}
int pass_check(Dlist **head,char *pass)
{
	if(*head==NULL)
	{
		printf("Data list is empty\n");
		return FAILURE;
	}
	else
	{
		Dlist *temp=*head;
		while(temp!=NULL)
		{
			if(strcmp(temp->pass,pass)==0)
			{
				return SUCCESS;
			}
			temp=temp->link;

		}
		return FAILURE;
	}

}



void loadfile(Dlist **head)
{
	FILE *fptr = fopen("data.csv", "w");
	if (fptr == NULL)
	{
		printf("Failed to open the file\n");
		return;
	}

	Dlist *temp = *head;

	while (temp != NULL)
	{
		fprintf(fptr, "%s,%s\n", temp->user, temp->pass);
		temp = temp->link;
	}

	fclose(fptr);

}
void singlechat(Olist *head,char *msg,char *sender,char *receiver)
{
	struct packet server;
	memset(&server,0,sizeof(server));
	server.type = SINGLECHAT;
	strcpy(server.msg,msg);
	//pthread_mutex_lock(&online_mutex);
	while (head)
	{
		if(strcmp(head->user,receiver) == 0)
		{
			strcpy(server.sender,sender);
			send(head->fd,&server,sizeof(server),0);
			return;
		}
		head=head->link;
	}
	// pthread_mutex_unlock(&online_mutex);

}
void groupchat(Olist *head,char *msg,char * sender)
{
	struct packet server;
	memset(&server,0,sizeof(server));
	server.type = GROUPCHAT;
	strcpy(server.msg,msg);
	//pthread_mutex_lock(&online_mutex);
	while(head)
	{
		if(strcmp(head->user, sender) != 0)   // Skip sender
		{
			send(head->fd,&server,sizeof(server),0);
		}
		head=head->link;

	}
	//pthread_mutex_unlock(&online_mutex);

}

int remove_fromonline(Olist **head,char *user)
{
	if(*head==NULL)
	{
		return FAILURE ; 
	}
	else
	{
		Olist *temp=*head;
		Olist *prev=NULL;
		while(temp!=NULL)
		{
			if(strcmp(temp ->user,user)==0)
			{
				if(*head==temp)
				{
					*head=temp->link;     // delete first node
				}
				else
				{
					prev->link=temp->link;    // delete middle node
				}
				free(temp);
				return SUCCESS;

			}
			else
			{
				prev=temp;
				temp=temp->link;
			}
		}
		return FAILURE;
	}

}

void offline(Olist *head,char *user)
{
	//pthread_mutex_lock(&online_mutex);
	if (head == NULL )
	{
		return;
	}
	struct packet server;
	memset(&server,0,sizeof(server));
	server.type=ONLINELIST;
	strcpy(server.msg,"offline");
	strcpy(server.sender,user);
	Olist *temp =head;

	while(temp)
	{
		if(strcmp(temp->user,user)!=0)
		{
			send(temp->fd,&server,sizeof(server),0);
		}
		temp=temp->link;
	}
	// pthread_mutex_lock(&online_mutex);
}

void  online_to_all_clients(Olist *head,char *user)
{
	if (head == NULL )
	{
		return;
	}
	struct packet server;
	memset(&server,0,sizeof(server));
	server.type=ONLINELIST;
	strcpy(server.msg,"Online");
	strcpy(server.sender,user);
	Olist *temp =head;

	while(temp)
	{
		if(strcmp(temp->user,user)!=0)
		{
			send(temp->fd,&server,sizeof(server),0);
		}
		temp=temp->link;
	}


}
void serveroffline(Olist *head)
{
	//pthread_mutex_lock(&online_mutex);
	if (head == NULL )
	{
		return;
	}
	struct packet server;
	memset(&server,0,sizeof(server));
	server.type=EXIT;
	//strcpy(server.msg,"offline");
	//strcpy(server.sender,user);
	Olist *temp =head;

	while(temp)
	{


		send(temp->fd,&server,sizeof(server),0);

		temp=temp->link;
	}
	// pthread_mutex_lock(&online_mutex);

}
