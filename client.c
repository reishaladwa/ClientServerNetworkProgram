#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

//functions
char **getCommands(char *line);
int getNumberOfTokens(char *line);
char **splitDate(char *orig);
char *getDomainName(char *dest, char const *input);

int main(int argc, char const *argv[])
{
    int port = atoi(argv[2]);
    const char *domainName = argv[1];
    char host[256];
    domainName = getDomainName(host, argv[1]);

    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, domainName, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if ((client_fd = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    //sending hello back and forth testing
    // send(sock, hello, strlen(hello), 0);
    // printf("Hello message sent\n");
    // valread = read(sock, buffer, 1024);
    // printf("%s\n", buffer);

    while (1)
    {
        char str[80];
        printf("%s", "> ");
        if (!fgets(str, sizeof str, stdin))
            continue;

        //parsing input
        int numTk = getNumberOfTokens(str);
        char **tkns = {NULL};
        tkns = getCommands(str);

        if (numTk == 1)
        {
            char *input_1 = tkns[0];
            if (strcmp(input_1, "quit") == 0)
            {
                break;
            }
            else
            {
                printf("Invalid Syntax\n");
            }
        }
        else if (numTk == 2)
        {
            char *command = tkns[0];
            char *date = tkns[1];
            if (strcmp(command, "PricesOnDate") == 0)
            {

                sprintf(command, "%s %s", command, date); //concatenate the strings with a space in between
                send(sock, command, strlen(command), 0);  //send the command
                memset(buffer, 0, sizeof(buffer));        //reset buffer
                valread = read(sock, buffer, 1024);       //read answer into buffer
                printf("%s\n", buffer);                   //print the answer (buffer)
            }
            else
            {
                printf("Invalid Syntax\n");
            }
        }

        else if (numTk == 5)
        {
            char *command = tkns[0];
            char *input2 = tkns[1];
            char *input3 = tkns[2];
            char *input4 = tkns[3];
            char *input5 = tkns[4];

            if (strcmp(command, "MaxPossible") == 0 &&
                (strcmp(input2, "profit") == 0 || strcmp(input2, "loss") == 0) &&
                (strcmp(input3, "MRNA") == 0 || strcmp(input3, "PFE") == 0))
            {
                char combined[100];
                sprintf(combined, "%s %s %s %s %s", command, input2, input3, input4, input5);
                send(sock, combined, strlen(combined), 0);
                memset(buffer, 0, sizeof(buffer));
                valread = read(sock, buffer, 1024);
                printf("%s\n", buffer);
            }
            else
            {
                printf("Invalid Syntax\n");
            }
        }

        else
        {
            printf("Invalid Syntax\n");
        }
    }

    close(client_fd);
    return 0;
}

char *getDomainName(char *dest, char const *input)
{
    struct addrinfo hints;
    struct addrinfo *res, *tmp;
    char host[256];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;

    int ret = getaddrinfo(input, NULL, &hints, &res);
    if (ret != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(EXIT_FAILURE);
    }

    for (tmp = res; tmp != NULL; tmp = tmp->ai_next)
    {
        getnameinfo(tmp->ai_addr, tmp->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
    }
    strcpy(dest, host);
    freeaddrinfo(res);
    return (dest);
}

char **splitDate(char *orig)
{
    char **sub_str = malloc(10 * sizeof(char *));
    char line[80];
    strcpy(line, orig);
    int numTokens = 0;
    char *token;
    token = strtok(line, "-\n");
    while (token != NULL)
    {
        // printf(" %s\n", token);
        sub_str[numTokens] = malloc(20 * sizeof(char));
        strcpy(sub_str[numTokens], token);
        numTokens = numTokens + 1;
        token = strtok(NULL, "-\n");
    }
    return sub_str;
}

// parse input into an array of strings where each string is a command separated by a space
char **getCommands(char *orig)
{
    char **sub_str = malloc(10 * sizeof(char *));
    for (int i = 0; i < 10; i++)
    {
        sub_str[i] = malloc(20 * sizeof(char));
    }

    char line[80];
    strcpy(line, orig);

    int numTokens = 0;
    char *token = strtok(line, " \n");
    while (token != NULL)
    {
        strcpy(sub_str[numTokens], token);
        numTokens++;
        token = strtok(NULL, " \n");
    }

    return sub_str;
}

//count the number of tokens (commands) in a string
int getNumberOfTokens(char *orig)
{
    char line[80];
    strcpy(line, orig);

    int numTokens = 0;
    char *token = strtok(line, " \n");
    while (token != NULL)
    {
        numTokens++;
        token = strtok(NULL, " \n");
    }

    return numTokens;
}
