#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/resource.h>
#include <strings.h>

//#define portno 1234
#define MAXSIZE 6000

int readn(int,char *,int);
int writen(int,char *,int);
void selMode(int);

int mode = 10;

void main(int argc,char *argv[])
{
  int sds,newsd,n,addrlen,portno,i,pid;
  long sum=0;
  char buffer[256] = {0};
  struct sockaddr_in server_addr,client_addr;

   int ack=-1;
   char *filename;
   int fchar;
   long j=0;
   char data[MAXSIZE];


  /*************************Create a Socket****************************/
  sds = socket(AF_INET,SOCK_STREAM,0) ;
  printf("socket value = %d\n",sds);
  if(sds<0)
  {
    perror("failed to open a socket\n");
    exit(1);
  }

  /********************Server Address Structure for bind***************/
  //bzero();
  portno = atoi(argv[1]);
  printf("port = %d\n",portno);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portno);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  /*********************binding ipaddr and portno***********************/
  if(bind(sds,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) 
  { 
    perror("Failed to bind\n");
    exit(1);
  }
  
  /**********************Server in Listening mode***********************/
  listen(sds,5);

  while(1)
  {
   /*******************accept from client********************************/
    addrlen = sizeof(client_addr);
    newsd = accept(sds,(struct sockaddr *)&client_addr,&addrlen);
    printf("newsd = %d\n",newsd);
    if(newsd < 0) 
    {
      perror("Failed to accept from client\n");
      exit(1);
    }
   
    /*pid = fork();
    if(pid == 0)
    {
      
    }*/
    /*readn(newsd,(char *)&mode,sizeof(mode));
    
    printf("i am here rerer %d\n",mode);

   /*switch(mode)
   {
     case 10:
       mode=20;
     break;

     case 20:    
       ack=mode;
       writen(newsd,(char *)&ack,sizeof(ack));
       fchar = readn(newsd,filename,100);
       ack = fchar;
       writen(newsd,(char *)&ack,sizeof(ack));
       fchar = readn(newsd,data,MAXSIZE);
       ack = fchar;
       writen(newsd,(char *)&ack,sizeof(ack));

       while(j<=fchar)
       { 
         printf("0x%x ",data[j]);
         j++;
       } 

     break;
    }*/


    selMode(newsd);
  } 

  /******************Close the Socket***********************************/
  close(sds);
}

void selMode(int sd)
{
   int ack=-1,rem=0,th=1000;
   char filename[100];
   int fchar;
   long j=0;
   char data[1000];
   FILE *op;

   printf("i am here 1\n");
   readn(sd,(char *)&mode,sizeof(mode));
   printf("i am here 2\n");

   switch(mode)
   {
     case 10:
       mode=20;
     break;

     case 20:    
       ack=mode;
       writen(sd,(char *)&ack,sizeof(ack));

       fchar = readn(sd,filename,100);
       printf("filename=%s\n",filename);
       ack = fchar;
       writen(sd,(char *)&ack,sizeof(ack));

       readn(sd,(char *)&j,sizeof(int));
       writen(sd,(char *)&ack,sizeof(ack));
        
       rem=j;
       op=fopen("something.jpg","wb");
       while(rem>0)
       {
         if(rem>=1000)
           fchar = read(sd,data,1000);
         else
           fchar = read(sd,data,rem); 
         printf("fchar=%d rem=%d\n ",fchar,rem);
         
         j=fchar;
         while(fchar>0)
         { 
           fputc(data[j-fchar],op);
           printf("0x%x ",data[j-fchar]);
           data[j-fchar]=0;
           fchar--;
         }
         rem-=1000;
         /*if(rem>=1000)  
           data+=1000;
         else
           data+=rem;*/
       }
         
       printf("char 2=%i\n",fchar);
       ack = fchar;
       printf("char 3=%i\n",ack);
       writen(sd,(char *)&ack,sizeof(ack));

       /*op=fopen("something.jpg","wb");
       /*while(j>0)
       { 
         //printf("0x%x ",data[j]);
         fputc(data[fchar-j],op);
         j--;
       }*/
       fclose(op);
       printf("here...\n");

     break;

     case 30:
     break;

     case 40:
     break;
     
   }
}


int readn(int sd,char *ptr,int size)
{         int no_left,no_read;
          no_left = size;
          while (no_left > 0) 
                     { no_read = read(sd,ptr,no_left);
                       //printf("no read=%d\n",no_read);
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
