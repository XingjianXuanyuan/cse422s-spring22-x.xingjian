/* client.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "/tmp/server"

#define handle_error(msg, fd) \
        do { perror(msg); close(fd); exit(EXIT_FAILURE); } while(0)

#define MAX_BUF_LEN 100

#define FALSE 0

const char* sig = "quit";

int main(int argc, char* argv[])
{
    int client_fd = -1, rc = -1;
    unsigned int val = 0;
    char str[MAX_BUF_LEN];
    struct sockaddr_un server_addr;
    FILE* fp = NULL;

    if (argc > 2) {
        fprintf(stdout, "Wrong number of arguments!\n");
	    return EXIT_FAILURE;
    }

    do
    {

        client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (client_fd < 0) {
            handle_error("socket()", client_fd);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SERVER_PATH);

        rc = connect(client_fd, (struct sockaddr *)&server_addr, SUN_LEN(&server_addr));
        if (rc < 0) {
            handle_error("connect()", client_fd);
        }

	    while (1) {

		    fp = fdopen(dup(client_fd), "w");
            if (!fp) {
            	handle_error("fdopen()", client_fd);
            }

            while (fscanf(stdin, " %u", &val) > 0) {
                if (fprintf(fp, "%u ", val) > 0) {
                    fprintf(stdout, "Successfully sent %u.\n", val);
                }
            }
            fscanf(stdin, " %s", str);
		    if (strcmp(str, sig) == 0) {
			    fclose(fp);
			    break;
		    }

		    fclose(fp);
	    }

    } while (FALSE);

    if (client_fd != -1) {
        close(client_fd);
    }

    return EXIT_SUCCESS;
}
