#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>

#define MAXSIZE 6000

int readn(int,char *,int);
int writen(int,char *,int);
void selMode(int);

int mode =20;

void main(int argc,char *argv[])
{
  int sock,newsock,n,addrlen;
  struct sockaddr_in server_addr,client_addr;
  char buffer[256] = {0};
  struct hostent *host;
  

   int ack=-1;
   char *filename;
   FILE *ip;
   long j=0;
   int tempData=0;
   char data[MAXSIZE];
  

  sock = socket(AF_INET,SOCK_STREAM,0) ; //create a socket
  if(sock<0)
  {
    perror("failed to open a socket\n");
    exit(1);
  }

  host = gethostbyname(argv[1]);
  
  server_addr.sin_family = AF_INET;     
  server_addr.sin_port = htons(atoi(argv[2]));   
  server_addr.sin_addr = *((struct in_addr *)host->h_addr);
  //bcopy((char *)host->h_addr,(char *)&server_addr.sin_addr.s_addr,host->h_length);

  if(connect(sock,(struct sockaddr *)(&server_addr),sizeof(server_addr))<0)
  {
    perror("failed to connect\n");
    exit(1);
  }
  bzero(buffer,256);
 
  printf("i am here fghgh %d\n",mode);

/*  switch(mode)
   {
     case 10:
       mode=20;
     break;

     case 20:
       printf("i am here 2\n");
       writen(sock,&mode,sizeof(mode));
       readn(sock,&ack,sizeof(char));
       if(ack>0)
       {
         printf("Enter the name of file to upload: ");
         scanf("%s",filename);
         writen(sock,filename,sizeof(filename));
       }
       else
       {
         printf("Invalid filename\n");
         exit(0);
       }       

       readn(sock,&ack,sizeof(char));
       if(ack>0)
       {
         ip = fopen(filename,"rb");
         if(ip == NULL)
         {
           printf("file open failed\n");
           exit(0);
         }

         while((tempData = fgetc(ip)) != EOF)
         { 
           data[j] = (char)tempData;
           j++;
         }         
         writen(sock,data,j);
       }
       else
       {
         printf("failed to send the file data\n");
         exit(0);
       }

       readn(sock,&ack,sizeof(char));
       if(ack>0)
        printf("File Uploaded Successfully\n");
       else
        printf("File Upload Failed\n");

     break;  
   }*/

  selMode(sock);

  close(sock);
}


void selMode(int sd)
{
   int ack=-1,rem=0,fchar=0;
   char filename[100];
   FILE *ip;
   long j=0;
   unsigned int tempData=0;
   char data[1000];
   
   printf("i am here 1\n");

   switch(mode)
   {
     case 10:
       mode=20;
     break;

     case 20:
       printf("i am here 2\n");
       writen(sd,(char *)&mode,sizeof(mode));
       readn(sd,(char *)&ack,sizeof(ack));
       if(ack>0)
       {
         ack=-1;
         printf("Enter the name of file to upload: ");
         scanf("%s",filename);
         printf("file = %s\n",filename);
         writen(sd,(char *)filename,sizeof(filename));
       }
       else
       {
         printf("Invalid filename\n");
         exit(0);
       }       

       readn(sd,(char *)&ack,sizeof(int));
       if(ack>0)
       {
         ack=-1;
         ip = fopen(filename,"rb");
         if(ip<0)
         {
           printf("file open failed\n");
           exit(0);
         }

         while(1)
         { 
           tempData = fgetc(ip);
           
           if(tempData==EOF)
             break;
           
           //data[j] = (char)tempData;
           //printf("0x%d ",data[j]);
           //usleep(100);
           j++;
         }    
         //fclose(ip); 
         fseek(ip,0,SEEK_SET);

         printf("j=%d\n",j); 
         writen(sd,(char *)&j,sizeof(int));
         readn(sd,(char *)&ack,sizeof(int));
         if(ack>0)
           printf("File size sent\n");
         else
           printf("operation Failed\n");

         rem=0;
         while( (tempData = fgetc(ip)) != EOF)
         {
           data[rem++] = (char)tempData;
           printf("0x%x ",data[rem-1]); 
           j--;
           if(j<1000)
           {
             rem=0;
             writen(sd,(char *)data,j);
           }            
           else if(rem ==1000)
           {
             rem=0;
             writen(sd,(char *)data,1000);
           }
           //printf("0x%d ",data[j]); 
                         
         }

         /*rem=j;
         while(rem>0)
         {
           if(rem>=1000) 
           { 
             writen(sd,(char *)data,1000);
             data+=1000;
           }
           else
           {
             //data+=rem;
             writen(sd,(char *)data,rem);
           }

           rem-=1000;
         }*/
         printf("file written successfully\n");
       }
       else
       {
         printf("failed to send the file data\n");
         exit(0);
       }

       readn(sd,(char *)&ack,sizeof(int));
       if(ack>0)
        printf("File Uploaded Successfully\n");
       else
        printf("File Upload Failed\n");

     break;

     case 30:
     break;

     case 40:
     break;
     
   }
}

int readImage(char *data,char *filename)
{
   FILE *ip;
   long j=0;
   int tempData=0;

   ip = fopen(filename,"rb");
   if(ip == NULL)
   {
     printf("file open failed\n");
   }

   while((tempData = fgetc(ip)) != EOF)
   { 
     data[j] = (char)tempData;
     j++;
   } 
   
   return j; 
}

int readn(int sd,char *ptr,int size)

{         int no_left,no_read;
          no_left = size;
          while (no_left > 0) 
                     { no_read = read(sd,ptr,no_left);
                       if(no_read <0)  return(no_read);
                       if (no_read == 0) break;
                       no_left -= no_read;
                       ptr += no_read;
                     }
          return(size - no_left);
}


int writen(int sd,char *ptr,int size)
{         int no_left,no_written;
          no_left = size;
          while (no_left > 0) 
                     { no_written = write(sd,ptr,no_left);
                       if(no_written <=0)  return(no_written);
                       no_left -= no_written;
                       ptr += no_written;
                     }
          return(size - no_left);
}
