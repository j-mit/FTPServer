#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <libgen.h>

//#define MAXSIZE 6000

int readn(int,char *,int);
int writen(int,char *,int);
void selMode(int);

int mode =10;

void main(int argc,char *argv[])
{
  int sock,newsock,n,addrlen;
  struct sockaddr_in server_addr,client_addr;
  char buffer[256] = {0};
  struct hostent *host;
  

   /*int ack=-1;
   char *filename;
   FILE *ip;
   long j=0;
   int tempData=0;
   //char data[MAXSIZE];*/
  
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
 
  //printf("i am here fghgh %d\n",mode);

  selMode(sock);

  close(sock);
}


void selMode(int sd)
{
   int ack=-1,fchar=0,rem=0,th=1000;
   char filename[100],dFile[100] = "/home/jaymit/Down/";
   char directory[100];
   int count;
   char *file;
   char dirc[100];
   FILE *ip;
   FILE *op;
   int j=0;
   char data[1000];
   int i=0;
   char uname[100];
   char password[100];
   char valid[100];
   
   //printf("i am here 1\n");

   while(1)
   {
   switch(mode)
   {
     case 10:
       //printf("i am here again\n");
       writen(sd,(char *)&mode,sizeof(mode));
       readn(sd,(char *)&ack,sizeof(ack));
       printf("Please Enter Username:");
       scanf("%s", uname);
       writen(sd,(char *)&uname,sizeof(uname));
       printf("Please Enter Password:");
       scanf("%s", password);
       writen(sd,(char *)&password,sizeof(password));
       read(sd, (char *)&valid, sizeof(valid));
       printf("\n%s\n", valid);
       if(strcmp(valid , "Fail!") == 0){
       printf("Incorrect username or password. Please Try Again\n");
       selMode(sd);
       }else{
      //SSH Validation
      mode = selFromMenu();//call this function at the end
      }
     break;

     case 20:
//Upload File to the Server
       printf("i am here 2\n");
       writen(sd,(char *)&mode,sizeof(mode));
       readn(sd,(char *)&ack,sizeof(ack));
//ack and send file name
       chkAck(&ack,"Read Mode");
       getFilename(filename);
       //dirc = strdup(filename);
       file = basename(filename);
       strcpy(dirc, file);    
       writen(sd,(char *)&dirc,sizeof(dirc));
       printf("file = %s\n",file);
// read ack and read file size
       readn(sd,(char *)&ack,sizeof(int));
       chkAck(&ack,"Send Filename");
       readFileSize(&j,filename);

       printf("j=%d\n",j); 
//write size to server and read ack
       writen(sd,(char *)&j,sizeof(int));
       readn(sd,(char *)&ack,sizeof(int));
       chkAck(&ack,"Send FileSize");
// read file data
       printf("value of j %d\n",j);

       readFileData(data,filename,j,sd);
       printf("file written successfully\n");
// read ack 
       readn(sd,(char *)&ack,sizeof(int));
       chkAck(&ack,"file upload");

      mode = selFromMenu();
     break;

     case 30:
   //Download File to the Client
       writen(sd,(char *)&mode,sizeof(mode));
       readn(sd,(char *)&ack,sizeof(ack));

       chkAck(&ack,"Write Mode");
       getFilename(filename);
       writen(sd,(char *)filename,sizeof(filename));
       printf("file = %s\n",filename);
       
       readn(sd,(char *)&ack,sizeof(int));
       chkAck(&ack,"Send Filename");
       
       readn(sd,(char *)&j,sizeof(int));
       readn(sd,(char *)&ack,sizeof(int));
       chkAck(&ack,"Get FileSize");
     
       
       // open and write file
       rem=j;
       strcat(dFile,filename);
       op=fopen(dFile,"wb");
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
      //Download file from the server
      // mode = selFromMenu();//call this function at the end
     break;

     case 40:
      
      //View the server directory
       writen(sd,(char *)&mode,sizeof(mode));
       readn(sd,(char *)&ack,sizeof(ack));

       chkAck(&ack,"Check Mode");
       readn(sd, (char *)&count, sizeof(int));
       printf("Count : %d files\n", count);
       int l=1;
       while(count > 0)
       {
         readn(sd,(char *)directory,sizeof(directory));
         printf("\t%d. %s\n", l, directory);
         count--;
         l++;
       }
      mode = selFromMenu();//call this function at the end
     break;
     
   }
  }
}

int selFromMenu()
{
  int input=0; 
 
  printf("\n\t-------How do you wish to use the FTP server--------\n\n");
  printf("\tPress 1 to Upload a File to the server\n\n");
  printf("\tPress 2 to Download a File from the server\n\n");
  printf("\tPress 3 to View the server directory\n\n");
  
  printf("Enter your option here(by default view): ");
  scanf("%d",&input);

  if(input == 1)
  {
    mode = 20;
    printf("File Upload mode selected\n");
  }
  else if(input == 2)
  {
    mode = 30;
    printf("File Download mode selected\n");
  }
  else if(input == 3)
  {
    mode = 40;
    printf("View Directory Mode mode selected\n");
  }
  else
  {printf("Wrong input\n");
   selFromMenu();}

  return mode;
}

void chkAck(int *ack,char *name)
{
   if(*ack>=0)
   {
     *ack=-1;
     printf("Operation %s successfull\n\n",name);
    }
   else
   {
    printf("Operation %s Failed\n\n",name);
    exit(1);
   }  
}

void getFilename(char *filename)
{ 
  
  printf("Enter the name of file to upload: ");
  scanf("%s",filename);
}

void readFileSize(int *j,char *filename)
{
   FILE *ip;
   unsigned int tempData=0;   

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

   ip = fopen(filename,"rb");
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
   chkAck(&ack,"Write Block");
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
  chkAck(&ack," ----Write Final Block---- ");
  printf("fchar = %d ",fchar);
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
