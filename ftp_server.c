#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/resource.h>
#include <strings.h>
#include <string.h>
#include <dirent.h>

//#define portno 1234
#define MAXSIZE 6000

int readn(int,char *,int);
int writen(int,char *,int);
void selMode(int);

int mode = 10;
//char newFile[100] = "/home/jaymit/ping/";

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
   //char data[MAXSIZE];


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
   
    pid = fork();
    if(pid == 0)
    {
      while(1){
      selMode(newsd);
      }
      close(newsd);
    }
    /*readn(newsd,(char *)&mode,sizeof(mode));
    
    printf("i am here rerer %d\n",mode);*/
    //while(1){
    //}
  } 

  /******************Close the Socket***********************************/
  close(sds);
}

void selMode(int sd)
{
   int ack=-1,rem=0,th=1000,fchar=0,count = 0;
   char filename[100], newFile[100] = "/home/jaymit/ping/";
   char list[100];
   long j=0;
   char data[1000];
   char name[256][256];
   FILE *op;
   FILE *ip;
   char uname[100];
   char password[100];
   char valid1[5][100] = {"jaymit", "akshar", "shrads", "prachi", "navdeep"};
   char valid2[5][100] = {"123456", "234567", "345678", "456789", "567890"};
   DIR *dp;
   struct dirent *ep;

   //printf("i am here 1\n");
   readn(sd,(char *)&mode,sizeof(mode));
   printf("Mode: %d\n", mode);

   switch(mode)
   {
     case 10:	
       ack=mode;
       //write ack
       writen(sd,(char *)&ack,sizeof(ack));
       read(sd,(char *)uname,sizeof(uname));
       read(sd,(char *)password,sizeof(password));
       printf("%s\n", uname);
       printf("%s\n", password);
       int a;
       for (a = 0; a < 5; a++)
       	 if (strcmp (uname,valid1[a]) == 0){
       	   if (strcmp (password,valid2[a]) == 0)
             {
                write(sd, "Kudos!", sizeof("Kudos!"));
             }
           else{
             write(sd, "Fail!", sizeof("Fail!"));
        	}
      } 
     break;

     case 20:    
       ack=mode;
       //write ack
       writen(sd,(char *)&ack,sizeof(ack));
	// recieve and generate ack for filename
       fchar = readn(sd,filename,100);
       printf("filename=%s\n",filename);
       ack = fchar;
       writen(sd,(char *)&ack,sizeof(ack));
	
	// read size and send ack
       readn(sd,(char *)&j,sizeof(int));
       writen(sd,(char *)&ack,sizeof(ack));
        
        // open and write file
       rem=j;
       strcat(newFile,filename);
       op=fopen(newFile,"wb");
       while(rem>0)
       {
         if(rem>=1000)
         {
           fchar = read(sd,&data[0],1000);
           writen(sd,(char *)&ack,sizeof(int));
         }
         else
         {
           fchar = read(sd,&data[0],rem);
           writen(sd,(char *)&ack,sizeof(int)); 
         }
         //printf("fchar=%d rem=%d\n ",fchar,rem);
         
         j=fchar;
         while(fchar>0)
         { 
           fputc(data[j-fchar],op);
           //printf("0x%x ",data[j-fchar]);
           data[j-fchar]=0;
           fchar--;
         }
         rem-=1000;
       }
         
       //printf("char 2=%i\n",fchar);
       ack = fchar;
       //printf("char 3=%i\n",ack);
       printf("File recvd successfully\n");
       writen(sd,(char *)&ack,sizeof(ack));

       fclose(op);
       printf("here...\n");

     break;

     case 30:
       ack=mode;
       //write ack
       writen(sd,(char *)&ack,sizeof(ack));
       // recieve and generate ack for filename
       fchar = readn(sd,filename,100);
       printf("filename=%s\n",filename);
       ack = fchar;
       writen(sd,(char *)&ack,sizeof(ack));
       readFileSize(&j,filename);

       printf("j=%d\n",j); 
       //write size to server and read ack
       fchar = writen(sd,(char *)&j,sizeof(int));
       ack = fchar;
       writen(sd,(char *)&ack,sizeof(ack));
       //read file data
       readFileData(data,filename,j,sd);
       printf("file written successfully\n");   
     break;

     case 40:
       ack=mode;
       //write ack
       writen(sd,(char *)&ack,sizeof(ack));
       int index = 0;
       dp = opendir("/home/jaymit/ping/");
       if (dp)
       {
         while ((ep = readdir(dp)) != NULL)
         {
          //printf("%s\n", ep->d_name);
          strcpy(name[count],ep->d_name);
          count++;
         }

        closedir(dp);
        }
	writen( sd, (char *)&count, sizeof(int));
	printf("%d\n",count);
	
        while( count > 0 )
        {
          strcpy(list,name[index]);
          write(sd, (char *)list, sizeof(list));
          index++;
          count--;
          //sleep(1);
        }  
        break;
     
   }
}

void readFileSize(int *j,char *filename)
{
   FILE *ip;
   
   unsigned int tempData=0;   
   char newFile[100] = "/home/jaymit/ping/";
   strcat(newFile,filename);
   
   ip = fopen(newFile,"rb");
   
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
     (*j)+=1;
   }  
   fseek(ip,0,SEEK_SET);  
}

void readFileData(char data[],char *filename,int j,int sd)
{  
   FILE *ip;
   int k=0,ack=-1;
   unsigned int tempData=0; 
   int rem=0;  
   char newFile[100] = "/home/jaymit/ping/";
   strcat(newFile,filename);
   
   ip = fopen(newFile,"rb");
   if(ip<0)
   {
     printf("file open failed\n");
     exit(0);
   }
   while( ((tempData = fgetc(ip)) != EOF) && j>0)
   {
     data[rem++] = (char)tempData;
     //printf("0x%x,",data[rem-1]);
 
     if(j<1000)
     {
      j--;
      k++;
      if(j<=0)
        lastBlock(data,j,k,sd);      
     }   
     else if(rem>=1000)
     {
      rem=0;
      dataBlocks(data,&j,sd);     
     }              
   }
}

void dataBlocks(char data[],int *j,int sd)
{
   int fchar=0,ack=-1;
   fchar = writen(sd,(char *)data,1000);
   readn(sd,(char *)&ack,sizeof(int));
   printf("fchar = %d\n",fchar);
   bzero(data,1000);
   *j-=1000;  
}

void lastBlock(char data[],int j,int k,int sd)
{
  int fchar=0,i=0,ack=-1;

  /*for(i=0;i<k;i++)
    printf("0x%x,",data[i]);*/
  printf("\n");
  fchar = writen(sd,(char *)&data[0],k);
  readn(sd,(char *)&ack,sizeof(int));
  printf("fchar = %d ",fchar);
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
