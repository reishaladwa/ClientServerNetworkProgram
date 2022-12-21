#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct Stock
{
    char date[503][11];
    float close[503];
};

struct Date
{
    int m;
    int d;
    char *y;
};

struct Stock mrna;
struct Stock pfe;

//functions
void readFile(char *file1, struct Stock *mystruct);
char *getAnswer(char *buffer);
char **getCommands(char *line);
int getNumberOfTokens(char *line);
char *getPrices(char *date);
char *maxPossible(char **tkns);
char **splitDate(char *orig);
float MaxProfit(float numbers[], int left, int right, int len);
float MaxLoss(float numbers[], int left, int right, int len);
struct Date reformat(char *date);

int main(int argc, char *argv[])
{
    readFile(argv[1], &pfe);
    readFile(argv[2], &mrna);

    int port = atoi(argv[3]);
    int server_fd, new_socket, valread;
    int opt = 1;
    char buffer[1024] = {0};
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", "server started");

    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        while (1)
        {                                             //keep reading until control c
            memset(buffer, 0, sizeof(buffer));        //reset buffer
            valread = read(new_socket, buffer, 1024); //read command from client into buffer
            if (valread == 0 || valread == -1)
            {
                break;
            }
            printf("%s\n", buffer); //print command (buffer)
            char *myAnswer = getAnswer(buffer);
            send(new_socket, myAnswer, strlen(myAnswer), 0); //send answer
        }
        // back to accepting connections
    }

    close(new_socket);
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

char *getAnswer(char *buffer)
{
    int numTk = getNumberOfTokens(buffer);
    char **tkns = {NULL};
    tkns = getCommands(buffer);
    char *input_1 = tkns[0];

    if (strcmp(input_1, "PricesOnDate") == 0)
    {
        char *date = tkns[1];
        return getPrices(date);
    }
    else
    {
        return maxPossible(tkns);
    }
}

struct Date reformat(char *date)
{
    struct Date reformatted_date;

    char *year;
    char *month;
    char *day;
    char **tkns = {NULL};
    tkns = splitDate(date);
    year = tkns[0];
    month = tkns[1];
    day = tkns[2];
    int newmonth = atoi(month);
    int newday = atoi(day);

    reformatted_date.m = newmonth;
    reformatted_date.d = newday;
    reformatted_date.y = year;

    return reformatted_date;
}

char *getPrices(char *date)
{
    //reformatting date
    char str[100];
    struct Date newdate = reformat(date);
    sprintf(str, "%d/%d/%s", newdate.m, newdate.d, newdate.y);

    //getting answer
    float pfeStock = 0;
    float mrnaStock = 0;
    for (int i = 0; i < 503; i++)
    {
        if (strcmp(pfe.date[i], str) == 0)
        {
            pfeStock = pfe.close[i];
        }
        if (strcmp(mrna.date[i], str) == 0)
        {
            mrnaStock = mrna.close[i];
        }
    }
    static char result[1024] = {0};

    sprintf(result, "PFE: %0.2f | MRNA: %0.2f", pfeStock, mrnaStock);
    return (result);
}
char *maxPossible(char **tkns)
{
    char *profitOrLoss = tkns[1];
    char *dataSet = tkns[2];
    char *startDate = tkns[3];
    char *endDate = tkns[4];

    struct Date newStartDate = reformat(startDate);

    char start[100];
    sprintf(start, "%d/%d/%s", newStartDate.m, newStartDate.d, newStartDate.y);

    struct Date newEndDate = reformat(endDate);

    char end[100];
    sprintf(end, "%d/%d/%s", newEndDate.m, newEndDate.d, newEndDate.y);

    //making the array of all of the prices
    float profit;
    if (strcmp(dataSet, "PFE") == 0)
    {
        int startindex;
        int endindex;
        for (int i = 1; i < 503; i++)
        {
            if (strcmp(pfe.date[i], start) == 0)
            {
                startindex = i;
            }
            if (strcmp(pfe.date[i], end) == 0)
            {
                endindex = i;
            }
        }
        int len = endindex - startindex + 1;
        float numbers[len];
        for (int i = 0; i < len; i++)
        {
            numbers[i] = pfe.close[startindex + i];
        }
        int left = 0;
        int right = -1;
        if (strcmp(profitOrLoss, "profit") == 0)
        {
            profit = MaxProfit(numbers, left, right, len);
        }
        else
        {
            profit = (-1) * MaxLoss(numbers, left, right, len);
        }
    }

    else
    {
        int startindex;
        int endindex;
        for (int i = 1; i < 503; i++)
        {
            if (strcmp(mrna.date[i], start) == 0)
            {
                startindex = i;
            }
            if (strcmp(mrna.date[i], end) == 0)
            {
                endindex = i;
            }
        }
        int len = endindex - startindex + 1;
        float numbers[len];
        for (int i = 0; i < len; i++)
        {
            numbers[i] = mrna.close[startindex + i];
        }
        int left = 0;
        int right = -1;

        if (strcmp(profitOrLoss, "profit") == 0)
        {
            profit = MaxProfit(numbers, left, right, len);
        }
        else
        {
            profit = (-1) * MaxLoss(numbers, left, right, len);
        }
    }

    //convert float to char*
    char res[50];
    sprintf(res, "%0.2f", profit);

    char *pointer = malloc(sizeof(res));
    strcpy(pointer, res);

    return pointer;
}

float MaxProfit(float numbers[], int left, int right, int len)
{

    int indexOfLowest = 0;
    float lowest = numbers[0];
    float answer = 0;
    int buyDay = 0;
    int sellDay = 0;

    for (int j = 0; j < len; j++)
    {
        if (numbers[j] <= lowest)
        {
            indexOfLowest = j;
            lowest = numbers[j];
        }
        if (answer <= numbers[j] - lowest)
        {
            answer = numbers[j] - lowest;
            buyDay = indexOfLowest;
            sellDay = j;
        }
    }
    return answer;
}

float MaxLoss(float numbers[], int left, int right, int len)
{
    int indexOfLowest = 0;
    float lowest = numbers[0];
    float answer = 0;
    int buyDay = 0;
    int sellDay = 0;

    for (int j = 0; j < len; j++)
    {
        if (numbers[j] >= lowest)
        {
            indexOfLowest = j;
            lowest = numbers[j];
        }
        if (answer >= numbers[j] - lowest)
        {
            answer = numbers[j] - lowest;
            buyDay = indexOfLowest;
            sellDay = j;
        }
    }
    return answer;
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

void readFile(char *file1, struct Stock *mystruct)
{
    FILE *fd1 = fopen(file1, "r");

    char buffer[1024];
    int row = 0;
    int column = 0;
    int i = 0;

    while (fgets(buffer, 1024, fd1))
    {
        column = 0;

        row++;
        if (row == 1)
        {
            continue;
        }

        char *value = strtok(buffer, ",");
        while (value)
        {
            if (column == 0)
            {
                strcpy(mystruct->date[row - 2], value);
            }
            if (column == 4)
            {
                mystruct->close[row - 2] = atof(value);
            }

            value = strtok(NULL, ",");
            column++;
        }
    }
    fclose(fd1);
}