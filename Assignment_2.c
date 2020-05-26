/*

Created by: Irfan
Function:

Upon process running, create 1 thread, thread function as describe below:

Main thread:

Accept user command from TCP client

Command supported:
q : Quit current program (whole program)
c : create a string buffer
s : insert user key in string into buffer (created by c)
g : print string buffer (user key in-in string using command s)
t : Increment time by 1 second for Thread 1
r : Decrement time by 1 second for Thread 1 (min 1s)
f : flag HIGH
z : flag LOW

Thread 1:

Perform the following function periodically (time control by ‘s’). Default to every seconds
Increase the first character of string buffer : e.g. a → b, or A→ B


Instructions:

Connect to TCP server by using a TCP client. You will then be allowed to execute the following functions from the client.

   1. Create string buffer
   2. Input string
   3. Print out buffer (Optional)
   4. Update time for Thread 2 (Optional)
   5. Set flag to HIGH to begin increment of string
*/

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define MAX 255
#define MAX_INBUF 4
#define MAXMSG 31
#define PORT 8080
#define BACKLOG 10
#define SA struct sockaddr
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

unsigned int changetime = 1;
int flag = 0, stopprocess = 0, filedes[2];
char *buffer, *msg1 = "Your request has been rejected",*msg2 = "Your request has been accepted";
pthread_t t1;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void handle_sigint(int sig) 
{ 
    printf("Please exit by sending command 'q' to server\n");
} 

// Function designed for chat between client and server.
void func(int sockfd)
{
    char buff[MAX];
    int inbuf;
    unsigned int *newtime = (unsigned int*)malloc(sizeof(unsigned int));
    FILE *fd;

    signal(SIGINT, handle_sigint);

    read(filedes[0],&inbuf,MAX_INBUF);

    if(inbuf == 0)   
    {
        write(sockfd, msg1,MAXMSG);
        exit(EXIT_SUCCESS);
    }
    
    else
  
    write(sockfd, msg2,MAXMSG);

    // Infinite loop
    for (;;) {
        memset(buff,0, MAX);
        read(sockfd, buff, sizeof(buff));

        pthread_mutex_lock(&m);
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
            pthread_mutex_lock(&m);
            read(sockfd, buffer, sizeof(buffer));
            pthread_mutex_unlock(&m);
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

        case 'y':
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

        case 'w':
        {
            fd = fopen("/home/irfan-khaidhir/Documents/Assignment2.txt","w");

            if (fd == NULL)
            {
                printf("Error while attempting to write\n");
            }

            else
            {
                //printf("Enter num: ");
                //scanf("%s",&num);
                fprintf(fd,"%s",buffer);
                fclose(fd);
            }
        }

        case 'r':
        {
            if ((fd = fopen("/home/irfan-khaidhir/Documents/Assignment2.txt","r")) == NULL)
            {
                printf("Error! opening file");
            }

            else
            {
                fscanf(fd,"%s", buffer);
                fclose(fd);                 
            }
        }
        }
        pthread_mutex_unlock(&m);
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
                    pthread_mutex_lock(&m);
                    printf("%s\n",buffer);
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
    int sockfd, connfd, *input = (int*)malloc(sizeof(int));
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
    printf("Allow communication?\n\n1. Yes\n2. No\n");
    pipe(filedes);
    scanf("%i",input);

    write(filedes[1],input,MAX_INBUF);
    free(input);
    func(connfd);

}