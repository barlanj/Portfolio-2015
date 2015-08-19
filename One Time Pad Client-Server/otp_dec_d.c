/********************************************************************
 * Author: barlanj
 * Description:
 *      This is a server program for decrypting a plain text.
 *
 * Sources Used:
 *      http://www.tutorialspoint.com/unix_sockets/socket_server_example.htm
 *************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>

#define BUFFERSIZE 1024
#define MODULO(a,b) ((a>=0)?(a%b):((b+a)%b)) //to handle negative modulo calc

void doprocessing( int sock );
char * decrypt_msg(char message[], char key[]);
void getMessage(char buffer[], int sock);
void sendMessage(char message[], int sock);
void handshake(char buffer[], int sock);
char * getFile(int sock, int length);

int main( int argc, char *argv[] )
{

    // check for port argument
    if (argc < 2)
    {
        printf("usage: %s port#\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //variable declarations
    int sockfd, newsockfd, portno, clilen;
    char buffer[BUFFERSIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int  n, pid;

    /* First call to socket() function */
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 )
    {
        perror( "ERROR opening socket" );
        exit( 1 );
    }

    /* Initialize socket structure */
    bzero( ( char * ) &serv_addr, sizeof( serv_addr ) );
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );

    /* Now bind the host address using bind() call.*/
    if( bind( sockfd, ( struct sockaddr * ) &serv_addr, sizeof( serv_addr ) ) < 0 )
    {
        perror( "ERROR on binding" );
        exit( 1 );
    }

    /* Now start listening for the clients, here
    * process will go in sleep mode and will wait
    * for the incoming connection
    */
    listen( sockfd, 5 );
    clilen = sizeof( cli_addr );

    while( 1 )
    {
        newsockfd = accept( sockfd, ( struct sockaddr * ) &cli_addr, &clilen );

        if( newsockfd < 0 )
        {
            perror( "ERROR on accept" );
            exit( 1 );
        }

        /* Create child process */
        pid = fork();

        if( pid < 0 )
        {
            perror( "ERROR on fork" );
            exit( 1 );
        }

        if( pid == 0 )
        {
            /* This is the client process */
            close( sockfd );
            doprocessing( newsockfd );
            exit( 0 );
        }
        else
        {
            close( newsockfd );
        }
    } /* end of while */
}

void doprocessing( int sock )
{
    int n;
    char buffer[BUFFERSIZE];
    char plaintext[BUFFERSIZE];
    char *cipher_msg;
    char *key;
    char *plain;

    //check to make sure client is correct
    handshake(buffer, sock);

    //get the plain text
    getMessage(buffer, sock);
    sendMessage("plain size received", sock);
    plain = getFile(sock, atoi(buffer));
    sendMessage("plain file received", sock);

    //get the key text size
    getMessage(buffer, sock);
    sendMessage("key size received", sock);
    key = getFile(sock, atoi(buffer));

    //encrypt the message and send it back to client
    cipher_msg = decrypt_msg(plain, key);
    sendMessage(cipher_msg, sock);

    free(plain);
    free(key);
    free(cipher_msg);
}

void handshake(char buffer[], int sock)
{
    getMessage(buffer, sock);
    if( strcmp( buffer, "otp_dec" ) != 0 )
    {
        sendMessage("Access Denied.", sock);
        exit( 2 );
    }
    else
    {
        sendMessage("otp_dec_d", sock);
    }
}

void getMessage(char buffer[], int sock)
{
    bzero( buffer, BUFFERSIZE );

    int n = read( sock, buffer, BUFFERSIZE );
    if( n < 0 ){
        perror( "ERROR reading from socket" );
        exit( 1 );
    }
//    printf("Received: %s\n", buffer);
}

void sendMessage(char message[], int sock)
{
    int n = write( sock, message, strlen(message) );
    if( n < 0 )
    {
        perror( "ERROR writing to socket" );
        exit( 1 );
    }
//    printf("Sent: %s\n", message);
}

char * getFile(int sock, int length)
{
    char* str;
    int nBytesRead, i;

    str = (char*)malloc(length);

    for(i = 0; i < length; i += nBytesRead)
    {
        nBytesRead = read(sock, str + i, length - 1);
        if(nBytesRead < 0)
        {
            perror( "ERROR reading from socket" );
            exit( 1 );
        }
    }

//    printf("File Got: %s\n", str);
    return str;
}

char * decrypt_msg(char message[], char key[])
{
    int msg_len = strlen(message);

    char *cipherText;
    cipherText = (char *)malloc(sizeof(char) * msg_len);

    int i, k;
    for(i=0; i < msg_len; i++) {

        if(isalpha(message[i]) && isupper(message[i]) && isalpha(key[i])) {
            k = key[i] - 'A';
            cipherText[i] = MODULO((message[i] - 65 - k),26) + 65;
        }
        else
        {
            cipherText[i] = message[i];
        }

    }
    return cipherText;
}
