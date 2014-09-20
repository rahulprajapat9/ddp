#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define msgLen 100;

struct timespec diff(struct timespec start, struct timespec end);

struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}



int main(int argc, char* argv[])
{
	if (argc<3)
	{
		printf ("\nPort or output file name not entered\n");
		return 1;
	}
	char *filename;
	int listenSock, acceptSock;
	struct sockaddr_in servAddress, cliAddress;	
	char *msg;
	int i,n;	
	
	filename = argv[2];
	char* buffer;
	

	listenSock = socket (AF_INET, SOCK_STREAM, 0);
	if (listenSock==-1)
	{
		printf ("\ncould not create socket\n");
	}
	servAddress.sin_family = AF_INET;
	servAddress.sin_addr.s_addr = INADDR_ANY; //inet_addr ("10.5.131.53");
	servAddress.sin_port = htons(atoi(argv[1]));

	//Bind
	if( bind(listenSock,(struct sockaddr *)&servAddress , sizeof(servAddress)) < 0)
	{
    		puts("bind failed");
		return 1;
	}
	puts("bind done");

	//Listen
	listen (listenSock, 3);

	struct timespec startTime, endTime;	
	double runtime;	
	unsigned long long int sum;	

	// Accept
	while (1)
	{

		puts ("Waiting for incomming connections......");
		acceptSock = accept (listenSock, (struct sockaddr*)NULL, NULL);
		puts ("Connection Accepted\n\n\n");
		
		msg = "\n\n\t\tMr. Client, you are ready to go\n\n";
		send (acceptSock, msg, strlen(msg), 0);
		
		FILE *out = fopen (filename, "w"); 
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);
		
		buffer = calloc(1,sizeof(char));
		sum = 0;
		while (1)
		{
			n = recv (acceptSock, buffer, sizeof (char), 0);
			if (n>0)
			{
				fwrite (buffer, sizeof (char), 1, out);
				sum = sum +n;
			}
			else
			{
				break;
			}
		}
		fclose (out);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime);		
		printf ("\n\nTotal bytes written in %s = %llu\n\n",filename, sum);
		runtime = (diff(startTime,endTime).tv_sec + (long double)diff(startTime,endTime).tv_nsec/1000000000)*1000;
		if (sum>0) printf ("Message recieved in time = %f ms\n\n\n", runtime);

		if (acceptSock < 0)
		{
			perror ("accept failed");
			return 1;
		}		

		sleep(1);
		
	}

	close (listenSock);
	close (acceptSock);
	return 0;
}



