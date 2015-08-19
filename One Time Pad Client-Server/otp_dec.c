/********************************************************************
 * Author: barlanj
 * Description:
 *      This is a client program for decrypting a cipher text to its plaintext form.
 *
 * Sources Used:
 *      http://www.tutorialspoint.com/unix_sockets/socket_client_example.htm
 *************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>

#define BUFFERSIZE 1024

void getMessage(char buffer[], int sock);
void sendMessage(char message[], int sock);
void loadfile(char path[], char loadbuffer[], int socketfd);
void validate_files(char plaintext[], char key[]);
int _validate(char *path);

int main(int argc, char *argv[])
{
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   char buffer[BUFFERSIZE];

   if (argc < 4) {
      fprintf(stderr,"usage %s plaintext key port#\n", argv[0]);
      exit(0);
   }

   //validate the plaintext and key files
   validate_files(argv[1], argv[2]);

   /* Create a socket point */
   portno = atoi(argv[3]);
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0)
   {
      perror("ERROR opening socket");
      exit(1);
   }
   server = gethostbyname("localhost");

   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }

   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);

   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR connecting");
      exit(1);
   }

   // handshake with server
    bzero(buffer, BUFFERSIZE);
    write(sockfd, "otp_dec", sizeof("otp_dec"));
    read(sockfd, buffer, sizeof(buffer));
    if(strcmp("otp_dec_d", buffer) != 0)
    {
        printf("Unable to find otp_dec_d \n");
        exit(2);
    }

   //load the plaintext to buffer
   bzero(buffer, BUFFERSIZE);
   loadfile(argv[1], buffer, sockfd);
   getMessage(buffer, sockfd);

    /* Now send the key */
   bzero(buffer, BUFFERSIZE);
   loadfile(argv[2], buffer, sockfd);

   /* Now read server response */
   bzero(buffer, BUFFERSIZE);
   getMessage(buffer, sockfd);
   printf( "%s", buffer );

   return 0;
}

void validate_files(char plaintext[], char key[])
{
    // validate plaintext and key
    int bool_plaintext = _validate(plaintext);
    if(bool_plaintext == 0)
    {
        printf("plaintext contains invalid characters.");
        exit(1);
    }

    int bool_key = _validate(key);
    if(bool_plaintext == 0)
    {
        printf("key has invalid characters.");
        exit(1);
    }


    if(bool_plaintext > bool_key)
    {
        printf("Error: key length insufficient");
        exit(1);
    }
}

int _validate(char *path)
{
    int length = 0;
    FILE* file = fopen(path, "r");
    char line[256];

    // check the message
    while (fgets(line, sizeof(line), file)) {
        int i = 0;
        for(i =0; i < strlen(line); i++)
        {
            if(line[i] == ' ' || (line[i] >= 65 && line[i] <= 90))
                length++;
            else
            {
                if(line[i] != 10)
                {
                    return 0;
                }

            }
        }
    }
    fclose(file);

    return length; //0 means validation failed
}

void getMessage(char buffer[], int sock)
{
    bzero( buffer, BUFFERSIZE );

    int n = read( sock, buffer, BUFFERSIZE );
    if( n < 0 ){
        perror( "ERROR reading from socket" );
        exit( 1 );
    }

}

void sendMessage(char message[], int sock)
{
    int n = write( sock, message, strlen(message) );
    if( n < 0 )
    {
        perror( "ERROR writing to socket" );
        exit( 1 );
    }
}

void loadfile(char path[], char loadbuffer[], int socketfd)
{
    int infile;
    int length;
    int slength;
    char buffer[BUFFERSIZE];
    int nBytesRead, i, nBytesWrite;

    infile = open(path, O_RDONLY);

    // get the number of bytes
    length = lseek(infile, 0, SEEK_END);
    lseek(infile, 0, SEEK_SET);

    //send the size
    slength = sprintf(buffer, "%d", length);
    write(socketfd, buffer, slength);

    getMessage(buffer, socketfd);

    //send the file
    while(1)
    {
        nBytesRead = read(infile, buffer, sizeof(buffer));
        if(nBytesRead == 0)
        {
            close(infile);
            break;
        }

        for(i = 0; i < nBytesRead; i+=nBytesWrite)
        {
            nBytesWrite = write(socketfd, buffer + i, nBytesRead - i);
            if(nBytesWrite < 0)
            {
                perror( "ERROR writing to socket" );
                exit( 1 );
            }
        }
    }
}


