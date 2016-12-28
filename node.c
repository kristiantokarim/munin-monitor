#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <sys/sysinfo.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>
#define BUFSIZE 1024


void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
}

   
void error(char* msg)
{
perror(msg);
exit(0);
}
  
int main(int argc,char* argv[])
{
pid_t pid;
struct sockaddr_in addr_in,cli_addr,serv_addr;
struct hostent* host;
int sockfd,newsockfd;
   
if(argc<2)
error("./proxy <port_no>");
  

   
bzero((char*)&serv_addr,sizeof(serv_addr));
bzero((char*)&cli_addr, sizeof(cli_addr));
   
serv_addr.sin_family=AF_INET;
serv_addr.sin_port=htons(atoi(argv[1]));
serv_addr.sin_addr.s_addr=INADDR_ANY;
   
  
sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if(sockfd<0)
error("Problem in initializing socket");
   
if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
error("Error on binding");
  
  
listen(sockfd,50);
int clilen=sizeof(cli_addr);
  
 
 
accepting:
 
newsockfd=accept(sockfd,(struct sockaddr*)&cli_addr,&clilen);
   
if(newsockfd<0)
error("Problem in accepting connection");
  
pid=fork();
if(pid==0)
{
  int n;

	char hostname[BUFSIZE];
	memset(hostname,0,BUFSIZE);
			gethostname(hostname, BUFSIZE);
  char welcomemsg[BUFSIZE] = "#munin node at ";
  strcat(welcomemsg,hostname);
  strcat(welcomemsg, "\n");
  
	memset(hostname,0,BUFSIZE);
  n = write(newsockfd, welcomemsg, strlen(welcomemsg));
  if (n < 0) {
     error("ERROR writing to socket");
  }
  memset(welcomemsg, 0, BUFSIZE);
  while(1)
  {
	struct sockaddr_in host_addr;
    int flag=0,newsockfd1,port=0,i,sockfd1;
    char buf[BUFSIZE];
    char senddata[BUFSIZE];
    char* temp=NULL;
    memset(buf,0,BUFSIZE);
    recv(newsockfd,buf,500,0);
    if(strncmp(buf,"cap multigraph dirtyconfig",26) == 0){
        memset(senddata, 0, BUFSIZE);
        strcpy(senddata,"cap multigraph dirtyconfig\n");
        printf("receive : %s\n%s",buf,senddata);
        send(newsockfd,senddata,strlen(senddata),0);
        memset(senddata, 0, BUFSIZE);
        memset(buf,0,BUFSIZE);
     }
     else if (strncmp(buf,"nodes",strlen("nodes")) == 0){
        memset(senddata, 0, BUFSIZE);	
		memset(hostname,0,BUFSIZE);
			gethostname(hostname, BUFSIZE);
		strcpy(senddata,hostname);	
		memset(hostname,0,BUFSIZE);
        strcat(senddata,"\n.\n");
        printf("receive : %s\n%s",buf,senddata);
        send(newsockfd,senddata,strlen(senddata),0);
        memset(senddata, 0, BUFSIZE);
        memset(buf,0,BUFSIZE);
	 }
	 else if (strncmp(buf, "quit",strlen("quit")) == 0) {
		 printf("receive : %s\n",buf);
		 memset(buf,0,BUFSIZE);
        return(0);
     }
     else if (strncmp(buf, "version",strlen("version")) == 0) {
		memset(senddata, 0, BUFSIZE);
        strcpy(senddata,"lovely node on ");
		memset(hostname,0,BUFSIZE);
			gethostname(hostname, BUFSIZE);
		strcat(senddata,hostname);
		memset(hostname,0,BUFSIZE);
		strcat(senddata," version: 8.48\n");
        printf("receive : %s\n%s",buf,senddata);
        send(newsockfd,senddata,strlen(senddata),0);
        memset(senddata, 0, BUFSIZE);
        memset(buf,0,BUFSIZE);
	 }
	 else if (strncmp(buf,"config memory",strlen("config memory"))==0){
         struct sysinfo info;
	sysinfo(&info);
        memset(senddata, 0, BUFSIZE);
        char tmp[BUFSIZE],tmp1[BUFSIZE];
        memset(tmp, 0, BUFSIZE);
		sprintf(tmp,"%lu",info.totalram<<10);
		strcpy(senddata,"graph_args --base 1024 -l 0 --upper-limit ");
		strcat(senddata,tmp);
        strcat(senddata,"\ngraph_vlabel Bytes\ngraph_title Memory usage\ngraph_category system\ngraph_info This graph shows this machine memory.\ngraph_order used free\nused.label used\nused.draw STACK\nused.info Used memory.\nfree.label free\nfree.draw STACK\nfree.info Free memory.\n.\n");
        printf("receive : %s\n%s",buf,senddata);
        send(newsockfd,senddata,strlen(senddata),0);
        memset(senddata, 0, BUFSIZE);
        memset(buf,0,BUFSIZE);
     }
     else if (strncmp(buf,"fetch memory",strlen("fetch memory"))==0) {
        struct sysinfo info;
	sysinfo(&info);
	long unsigned fr = info.freeram; 
	memset(senddata, 0, BUFSIZE);
        strcpy(senddata,"used.value ");
	char buffx[BUFSIZE];
        FILE * fp = popen("free -b","r");
        fgets(buffx,1024, fp);
        memset(buffx, 0, BUFSIZE);
        fgets(buffx,1024, fp);
        int indexbuf = 0 ;
        while(buffx[indexbuf] != ' '){
          indexbuf++;
        }
        while(buffx[indexbuf] == ' '){
          indexbuf++;
        }
        while(buffx[indexbuf] != ' '){
          indexbuf++;
        }
        while(buffx[indexbuf] == ' '){
          indexbuf++;
        }
        int i =0;
	char x[BUFSIZE];
	memset(x, 0, BUFSIZE);
        while(buffx[indexbuf] != ' '){
          x[i] = buffx[indexbuf];
          i++;
          indexbuf++;
        }
        strcat(senddata,x);
	memset(x, 0, BUFSIZE);
        strcat(senddata,"\nfree.value ");
	while(buffx[indexbuf] == ' '){
          indexbuf++;
        }
	memset(x, 0, BUFSIZE);
	i=0;
	while(buffx[indexbuf] != ' '){
          x[i] = buffx[indexbuf];
          i++;
          indexbuf++;
        }
	strcat(senddata,x);
	memset(x, 0, BUFSIZE);
        strcat(senddata,"\n.\n");
        printf("receive : %s\n%s",buf,senddata);
	send(newsockfd,senddata,strlen(senddata),0);
	memset(senddata, 0, BUFSIZE);
	memset(x, 0, BUFSIZE);
	i =0;
	indexbuf = 0;
        

     }
     else  {
		 char listhost[BUFSIZE];
		 memset(listhost,0,BUFSIZE);
		 strcpy(listhost,"list ");
		 memset(hostname,0,BUFSIZE);
			gethostname(hostname, BUFSIZE);
			strcat(listhost, hostname);
			memset(hostname,0,BUFSIZE);
		 if(strncmp(buf,listhost,strlen(listhost))==0){
			memset(senddata, 0, BUFSIZE);
			memset(listhost,0,BUFSIZE);
			strcpy(senddata,"memory\n");
			printf("receive : %s\n%s",buf,senddata);
			send(newsockfd,senddata,strlen(senddata),0);
			memset(senddata, 0, BUFSIZE);
			memset(buf,0,BUFSIZE);
		 }else {
			memset(senddata, 0, BUFSIZE);
			strcpy(senddata,"# Unknown command. Try cap, list, nodes, config, fetch, version or quit\n");
			printf("receive : %s\n%s",buf,senddata);
			send(newsockfd,senddata,strlen(senddata),0);
			memset(buf,0,BUFSIZE);
			memset(senddata, 0, BUFSIZE);
			
		 }
	 }
    close(sockfd1);
    }
    close(newsockfd);
    close(sockfd);
    _exit(0);
}
else
{
close(newsockfd);
goto accepting;
}
return 0;
}
