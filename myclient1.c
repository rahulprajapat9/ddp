#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr

int main(int argc, char* argv[])
{
        int cliSock,n;
        int i;
	struct sockaddr_in servAddress;
        struct sockaddr_in cliAddress;
        char reply[1024];

        int msgLen;
        char* buffer;
        char* filename;

        if (argc<3)
        {
                printf ("\nIP or PORT or name of file to be sent not entered\n");
                return 1;
        }

	cliSock = socket (AF_INET, SOCK_STREAM, 0);
        if (cliSock==-1)
        {
                printf ("\ncould not create socket\n");
        }

	cliAddress.sin_family = AF_INET;
        cliAddress.sin_port = htons(0);
        cliAddress.sin_addr.s_addr = inet_addr (argv[1]);
        if( bind(cliSock,(struct sockaddr *)&cliAddress , sizeof(cliAddress)) < 0)
        {
                puts("bind failed");
                return 1;
        }
	puts("bind done");
	
	
	servAddress.sin_family = AF_INET;
        servAddress.sin_port = htons(atoi (argv[3]));
        if (inet_pton (AF_INET, argv[2], &servAddress.sin_addr) <=0)
        {
                printf ("\ninet_pton Error occured\n");
                return 1;
        }


	// Connect to remote server
        if ( (connect (cliSock, (struct sockaddr*)&servAddress, sizeof (servAddress))) < 0 )
        {
                puts ("connect error");
                return 1;
        }
	puts ("CONNECTED");


        // Recieve data from the server
        n = recv (cliSock, reply, sizeof (reply) -1, 0);

        if (n>0)
        {
                //printf ("Reply recieved\n");
                //printf ("length of message recieved = %d\n",n);
                reply[n] = 0;
                printf ("String recieved is \n\t\t%s\n", reply);
        }
	else
	{
                puts ("Reception error");
        }

		filename = argv[4];
        FILE *in = fopen (filename, "rb");
        if (in == NULL)
        {
                printf ("error opening file\n");
        }
	int len;
        int sum = 0;

        buffer = calloc(1,sizeof(char));
        while ((len = fread (buffer, sizeof (char), 1, in)) > 0)
        {
                n = send (cliSock, buffer, sizeof(char), 0);
                sum = sum + n;

                //printf("\n%d",len);
        }

	if (sum>0) printf("Total bytes send from %s = %d\n", filename, sum);
	
	close (cliSock);
        fclose (in);
        return 0;
}
