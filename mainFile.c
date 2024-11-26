#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include <unistd.h>
#include <stdlib.h> //For using atoi i.e., to convert an ASCII character into its respective integer value.
#include <time.h>

#define USERLENGTH 10
#define PASSLENGTH 10
#define MAX_USERS 20
#define HOST "localhost"
#define USER "root"
#define PASS "4713"
#define DB "25_Din_Paisa_Double"
#define FIELD_PIN_NUM 3

void signup(MYSQL *conn);
void login(MYSQL *conn);
int check_username_exists(MYSQL *conn, const char *username);

int count_turn = 0;

void clear_screen()
{
    system("clear");
}

int generate_random_pin()
{
    int min = 1000, max = 9999;
    // srand((unsigned int)time(NULL));

    return (rand() % (max - min + 1) + min);
}

void signup(MYSQL *conn)
{
    MYSQL_ROW row; // Used for holding the data of a row when such data is fetched using a MySQL query.
    char username[50], password[50];
    int flag = 0;

    while (flag == 0)
    {
        printf("Enter username: ");
        fgets(username, 50, stdin);
        username[strcspn(username, "\n")] = 0; // Remove trailing newline

        if (check_username_exists(conn, username))
        {
            printf("Username already exists. Please try a different one.\n");
            sleep(10);
            clear_screen();
        }

        else
            flag = 1;
    }

    printf("Enter password: ");
    fgets(password, 50, stdin);
    password[strcspn(password, "\n")] = 0;

    char query[200];

    // if (mysql_query(conn, query))
    // {
    //     printf("Sign up failed: %s\n", mysql_error(conn));
    // }

    // else
    // {
    //     printf("Sign up successful!\n");
    // }

    printf("Wait till we create you your pin number!\n");
    int pin_num = generate_random_pin();
    sleep(2);

    mysql_query(conn, "SELECT * FROM User_Info");

    // {
    //     /*
    //     fprintf: Used of writing the message to a particular output stream.
    //     stderr: Used for writing the error to error text file. Also, it a dedicated output stream for any errors other than stdout which is a general output stream for printing anything on the terminal.
    //     %s: It takes the error string passed on by mysql_error(conn) which contacts with MySQL's API while trying to execute the query.
    //     */

    //     fprintf(stderr, "%s\n", mysql_error(conn));
    // }

    // sprintf(query, "SELECT * FROM User_Info");

    MYSQL_RES *res = mysql_store_result(conn);

    // if (mysql_query(conn, query))
    // {
    //     fprintf(stderr, "%s\n", mysql_error(conn));
    //     printf("Sorry we can't generate a pin number for you at the moment.\nPlease try again later\n");
    // }

    int temp;
    flag = 0;

    if((row = mysql_fetch_row(res)) != NULL || (mysql_errno(conn)) == 0)
    {
        do
        {
            printf("Working\n");
            mysql_data_seek(res, 0);

            while ((row = mysql_fetch_row(res)) != NULL) /*mysql_fetch_row(res) fetches one row of data from the result at a time. Loop incase of multiple
                                                                 rows.*/
            {
                temp = atoi(row[FIELD_PIN_NUM]);

                if (pin_num == temp)
                {
                    pin_num = generate_random_pin();
                    flag = 0;
                    break;
                }

                else
                    flag = 1;
            }
        }

        while (flag == 0);

        sprintf(query, "INSERT INTO User_Info (Username, Password, Pin_Num) VALUES ('%s', '%s', %d)", username, password, pin_num);
        // sprintf(query, "INSERT INTO User_Info (Pin_Num) VALUES (%d) WHERE Username = ('%s')", pin_num, username);

        if (mysql_query(conn, query))
        {
            printf("Pin_generation failed: %s\n", mysql_error(conn));
        }

        else
        {
            printf("Your pin number is: %d\n!!!DON'T SHARE THIS WITH ANYONE!!!\n", pin_num);
        }
    }

    else 
    {
        printf("Sorry we can't generate a pin number for you at the moment.\nPlease try again later\n");
    }

    mysql_free_result(res); // Freeing the memory taken by result when we used mysql_store_result(conn)
}

void login(MYSQL *conn)
{
    char username[50], password[50];
    printf("Enter username: ");
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Enter password: ");
    fgets(password, 50, stdin);
    password[strcspn(password, "\n")] = 0;

    char query[200];
    sprintf(query, "SELECT * FROM User_Info WHERE Username='%s' AND Password='%s'", username, password);

    if (mysql_query(conn, query))
    {
        printf("Login failed: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL)
    {
        printf("Login failed: %s\n", mysql_error(conn));
        return;
    }

    int num_rows = mysql_num_rows(res);
    mysql_free_result(res);

    if (num_rows > 0)
    {
        printf("Access Granted\n");
    }
    else
    {
        printf("Access Denied\n");
    }
}

int check_username_exists(MYSQL *conn, const char *username)
{
    char query[200];
    sprintf(query, "SELECT * FROM User_Info WHERE Username='%s'", username);

    if (mysql_query(conn, query))
    {
        printf("Error checking username: %s\n", mysql_error(conn));
        return 1;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL)
    {
        printf("Error storing result: %s\n", mysql_error(conn));
        return 1;
    }

    int num_rows = mysql_num_rows(res);
    mysql_free_result(res);

    return num_rows > 0;
}

int main()
{
    MYSQL *conn = mysql_init(NULL);

    if (conn == NULL)
    {
        printf("mysql_init() failed\n");
        return 1;
    }

    if (mysql_real_connect(conn, HOST, USER, PASS, DB, 0, NULL, 0) == NULL)
    {
        printf("mysql_real_connect() failed\n");
        mysql_close(conn);
        return 1;
    }

    else
    {
        int choice;
        printf("1. Sign Up\n2. Login\nChoose an option: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            signup(conn);
            break;
        case 2:
            login(conn);
            break;
        default:
            printf("Invalid option!\n");
            mysql_close(conn);
            return 1;
        }
    }
}