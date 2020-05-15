#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 255
#define PORT 8080
#define BACKLOG 10
#define SA struct sockaddr
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

unsigned int changetime = 1;
int flag = 0, stopprocess = 0;
char *buffer;
pthread_t t1;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

// Function designed for chat between client and server.
void func(int sockfd)
{
    char buff[MAX],time;
    unsigned int *newtime = (unsigned int*)malloc(sizeof(unsigned int));

    // Infinite loop
    for (;;) {
        memset(buff,0, MAX);
        read(sockfd, buff, sizeof(buff));

        switch(buff[0])
        {

        case 'q':
        {
           free(buffer);
           free(newtime);
           pthread_cancel(t1);
           exit(42);
           break;
        }

        case 'c':
        {
           if (buffer == NULL)
           {
              buffer=(char*)malloc(sizeof(char));
              printf("String buffer created\n");
           }

           else
           {
              printf("Buffer was created previously\n");
           }

           break;
        }

        case 's':
        {
           printf("Please key in a string\n");
           read(sockfd, buffer, sizeof(buffer));
           break;

        }

        case 'g':
        printf("%s\n",buffer);
        break;

        case 't':
        {  
            changetime++;
            printf("Time:%is\n",changetime);
            break;
        }

        case 'r':
        {
            if(changetime > 1)
            {
                changetime--;
                printf("Time:%is\n",changetime);             
            }

            else
            {
                printf("Reached the minimum allowed time\n");
            }
            break;  
        }

        case 'f':
        {
           flag = 1;
           break;
        }

        case 'z':
        {
           flag = 0;
           break;
        }
        memset(buff,0, MAX);
        }
    }
}

void *myfunc2(void *ptr) //Thread t2
{

   while(1)
   {
      while(flag == 0)
      {
        sleep(1);
      }

	    while(flag == 1)
	    {
		    if (buffer)
		    {
			    printf("%s\n",buffer);
                pthread_mutex_lock(&m);
			    buffer[0]+=1;
                pthread_mutex_unlock(&m);
			    sleep(changetime);

		    }

	    }
   }
}


// Driver function
int main()
{
    int sockfd, connfd;
    socklen_t len;
    struct sockaddr_in addr,client;

    pthread_create(&t1,NULL,myfunc2,"Thread1");
    // SOCKET
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation FAILED\n");
        handle_error("socket");
    }
    else

    printf("Socket creation SUCCESSFUL\n");
    memset(&addr,0,sizeof(addr));
    //bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    // BIND
    if (bind(sockfd,(SA*)&addr,sizeof(addr)) != 0)
    {
        printf("%s","Bind FAILED\n");
        handle_error("bind");
    }

    else

    printf("%s","Bind SUCCESSFUL\n");

    // LISTEN
    if ((listen(sockfd, BACKLOG)) != 0) {
        printf("Listen FAILED\n");
        handle_error("listen");
    }
    else

    printf("LISTENING\n");
    len = sizeof(client);

    // ACCEPT
    connfd = accept(sockfd,(SA*)&client,&len);
    if (connfd < 0) {
        printf("Client rejected...\n");
        handle_error("accept");
    }
    else

    printf("Client Accepted...\n");

    func(connfd);

}

