#include<iostream>

using namespace std;
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include<time.h>
#define portno 5000
#include "common.hpp"
#include<wait.h>
#include <fcntl.h>



void process_GET_CWD(int newsockfd);
void process_GET_DETAILS(int newsockfd);
void process_CHDIR(int newsockfd);
void process_GET(int newsockfd);
void process_PUT(int newsockfd);
void process_GET_CRC(int newsockfd);
void process_GET_SIZE(int newsockfd);

int getCRC(int fd, int start, int length)
{
	return 0;
}

int main (int argc,char *argv[])
{
	int sockfd,newsockfd;
	unsigned int length;
	struct sockaddr_in serv_addr;
	char filename[256];
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Socket creation error\n");
		exit(1);
	}
	
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	serv_addr.sin_port=htons(portno);

	if(bind(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))<0)
	{
		printf("Binding error\n");
		exit(1);
	}

	length = sizeof(serv_addr);
	listen(sockfd,5);

	if (fork()) exit(0); // Parent terminates

	while (1)
	{
		newsockfd=accept(sockfd,(struct sockaddr *) &serv_addr,&length);
		if(newsockfd<0)
		{
			printf("accepting error\n");
			exit(1);
		}

		if (fork())
		{
			close(newsockfd);
			continue;
		}
		else
		{
			close(sockfd);
			break;
		}
	}


	int type;
	while (read(newsockfd,&type,sizeof(type)) > 0)
	{
		type = ntohl(type);
printf("SERVER TYPE=%d\n",type);
		switch(type)
		{
		case GET_CWD:
			process_GET_CWD(newsockfd);
			break;
		case FETCH_CONTENTS:
			process_GET_DETAILS(newsockfd);
			break;
		case CHDIR:
			process_CHDIR(newsockfd);
			break;
		case GET   :
			process_GET(newsockfd);
			break;
		case PUT :
			process_PUT(newsockfd);
			break;
		case GET_CRC   :
			process_GET_CRC(newsockfd);
			break;
		case GET_SIZE   :
			process_GET_SIZE(newsockfd);
			break;

		}
	}
}

void process_GET_CWD(int newsockfd)
{
	int x=htonl(CWD);
	char arr[256];
	getcwd(arr,256);
	write(newsockfd,&x,sizeof(x));
	write(newsockfd,arr,256);
	printf("%s",arr);
}

void process_GET_DETAILS(int newsockfd)
{
    FILE *fp = popen("ls -a","r");
    int b;
    struct stat st;
	struct details d;

	while (fgets(d.filename,256,fp))
    {
        d.filename[strlen(d.filename)-1] = '\0';

        lstat(d.filename,&st);
        if(S_ISDIR(st.st_mode))
        strcpy(d.type,"Directory File");

 		if(S_ISCHR(st.st_mode))
        strcpy(d.type,"Chararcter Device File");

        if(S_ISBLK(st.st_mode))
        strcpy(d.type,"Block Device File");

        if(S_ISREG(st.st_mode))
        strcpy(d.type,"Regular File");

        if(S_ISLNK(st.st_mode))
        strcpy(d.type,"Symbolic Link File");

        if(S_ISFIFO(st.st_mode))
        strcpy(d.type,"FIFO File");
        
		strcpy(d.perm,"");

        if(st.st_mode & S_IRUSR)
        strcat(d.perm,"r");
        else
        strcat(d.perm,"-");

        if(st.st_mode & S_IWUSR)
        strcat(d.perm,"w");
        else
        strcat(d.perm,"-");

        if(st.st_mode & S_IXUSR)
		 strcat(d.perm,"x");
        else
        strcat(d.perm,"-");

        if(st.st_mode & S_IRGRP)
        strcat(d.perm,"r");
        else
        strcat(d.perm,"-");

        if(st.st_mode & S_IWGRP)
        strcat(d.perm,"w");
        else
        strcat(d.perm,"-");

        if(st.st_mode & S_IXGRP)
        strcat(d.perm,"x");
        else
        strcat(d.perm,"-");

        if(st.st_mode & S_IROTH)
        strcat(d.perm,"r");
        else
        strcat(d.perm,"-");

        if(st.st_mode & S_IWOTH)
        strcat(d.perm,"w");
        else
        strcat(d.perm,"-");
		if(st.st_mode & S_IXOTH)
        strcat(d.perm,"x");
        else
        strcat(d.perm,"-");

        sprintf(d.size,"%d",st.st_size);
        sprintf(d.owner,"%d",st.st_uid);
        sprintf(d.group,"%d",st.st_gid);

        strcpy(d.time,ctime(&st.st_mtime));
        d.time[strlen(d.time)-1] = '\0';

        int y=FILE_DETAILS;
		y=htonl(FILE_DETAILS);
    	write(newsockfd,&y,sizeof(y));
    	write(newsockfd,&d,sizeof(d));

		read(newsockfd, &d, sizeof(d));        
    }
    pclose(fp);

    int a=END;
    a=htonl(END);
    write(newsockfd,&a,sizeof(a));
}

void process_CHDIR(int newsockfd)
{
	char filename[256];
	read(newsockfd,filename,sizeof(filename));
	cout << "Changing to directory: " << filename << endl;
	chdir(filename);
}
	
void process_GET(int newsockfd)
{
	int n=0;
	int fd,length,startPosition,reply,packetsize;
	char filename[256],data[PACKET_SIZE];
	
	read(newsockfd,filename,sizeof(filename));
    read(newsockfd,&startPosition,sizeof(startPosition));
    read(newsockfd,&length,sizeof(length));

    startPosition=ntohl(startPosition);
    length=ntohl(length);

    fd = open(filename,O_RDONLY);
    if (fd==-1) perror("File open");

    if(fd!=-1)
    {
    	lseek(fd,startPosition,SEEK_SET);

	    packetsize = sizeof(data);
    	if(length<packetsize)
	    packetsize = length;

	    while((n = read(fd,data,packetsize))>0)
    	{
    		reply=htonl(DATA);
	        write(newsockfd,&reply,sizeof(reply));
    	    n=htonl(n);
	        write(newsockfd,&n,sizeof(n));
    	    write(newsockfd,data,ntohl(n));
        	length-=ntohl(n);
			packetsize = sizeof(data);
    	    if(length<packetsize)
        		packetsize = length;
	        read(newsockfd,&reply,sizeof(reply));
    	}
		close(fd);
	}
	reply=htonl(DATA);
    write(newsockfd,&reply,sizeof(reply));
	n=htonl(n);
    write(newsockfd,&n,sizeof(n));
}                            

void process_PUT(int newsockfd)
{
	int startPosition,fd,reply,n;
	char filename[256],data[1000];
	read(newsockfd,filename,sizeof(filename));
	read(newsockfd,&startPosition,sizeof(startPosition));
	startPosition=ntohl(startPosition);
	fd = open(filename,O_WRONLY|O_CREAT,0644);
                                       
	lseek(fd,startPosition,SEEK_SET);
	while(1)
	{
		read(newsockfd,&reply,sizeof(reply));
		if (ntohl(reply)!=DATA)
		{
			printf("could not get the DATA packet\n");
			break;
		}
		read(newsockfd,&n,sizeof(n));
		n=ntohl(n);
		if(n==0) break;
		read(newsockfd,data,n);
		if (fd != -1) write(fd,data,n);
		int ack=htonl(ACK);
		write(newsockfd,&ack,sizeof(ack));
	}
	if (fd != -1) close(fd);	
}

void process_GET_CRC(int newsockfd)
{
	int startPosition,length,crc,fd,reply;
	char filename[256];
	read(newsockfd, filename, 256);
    read(newsockfd, &startPosition, sizeof(startPosition));
    read(newsockfd, &length, sizeof(length));

    startPosition = ntohl(startPosition);
    length = ntohl(length);

    fd = open(filename,O_RDONLY);
    if (fd == -1) crc = 0;
    else
    {
    	crc = getCRC(fd, startPosition, length);
        close(fd);
    }

    reply = htonl(CRC);
    write(newsockfd, &reply, sizeof(reply));	
    crc = htonl(crc);
    write(newsockfd, &crc, sizeof(crc));
                            
}

void process_GET_SIZE(int newsockfd)
{
	int reply,size;
	struct stat s;
	char filename[256];
	read(newsockfd,filename,256);
    reply=htonl(SIZE);
    write(newsockfd,&reply,sizeof(reply));
    if(stat(filename,&s)<0)
    {
    	printf("Stat error in server\n");
        
    }
    size=htonl(s.st_size);
    write(newsockfd,&size,sizeof(size));
}                            


