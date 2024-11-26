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
#define FIELD_PIN_NUM 0
#define TryAgain printf("Please try again later!\n")
#define NoRequire printf("\nSorry, your entered balance didn't meet the requirement!\nTry again or press 0 to quit: ")
#define Chatak printf("\n------------------------------------------------------------------------------------------------|\n")
#define WithdrawMsg printf("Withdraw amount (or press 0 to quit): ")

int signup(MYSQL *conn);
int login(MYSQL *conn, char* pin_num);
void check_balance(MYSQL *conn, char* pin_num);
void deposit(MYSQL *conn, char* pin_num);
int withdraw(MYSQL *conn, char* pin_num);
void clear_screen();
int generate_random_pin();
void store_user_pin_and_balance(MYSQL *conn, char username[50], char password[50], int pin_num, int bal);
int atm_menu(MYSQL *conn, char* pin_num); 
int initial_balance();

int initial_balance()
{
    int bal;
    Chatak;
    printf("Deposit atleast Rs.500 to create the account!\nEnter the balance: ");

    while(true)
    {
        scanf("%d", &bal);
        if(bal >= 500)
            break;

        else if(bal == 0)
            break;

        NoRequire;
    }

    return bal;
}

void deposit(MYSQL *conn, char* pin_num)
{
    char query[200], secquery[200];
    sprintf(query, "SELECT Balance FROM User_Balance WHERE Pin_Num = '%s'", pin_num);
    
    if (mysql_query(conn, query))
    {
        TryAgain;
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL)
    {
        printf("Facing some issues while fetching your balance!\n");
        TryAgain;
        return;
    }

    int num_rows = mysql_num_rows(res);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (num_rows > 0)
    {
        int new_bal, old_bal;

        old_bal = atoi(row[0]);

        printf("Enter amount: ");
        scanf("%d", &new_bal);

        sprintf(secquery, "UPDATE User_Balance SET Balance = (%d) WHERE Pin_Num = ('%s')", (new_bal + old_bal), pin_num);

        if (mysql_query(conn, secquery))
        {
            printf("Money couldn't be deposited!\n");
            TryAgain;
        }
        else
        {
            Chatak;
            printf("Money deposited successfully!\nNew balance: %d\n", (new_bal + old_bal));
        }
    }
    
    else
    {
        TryAgain;
    }

    mysql_free_result(res);
}

int withdraw(MYSQL *conn, char* pin_num)
{
    char query[200], secquery[200];
    sprintf(query, "SELECT Balance FROM User_Balance WHERE Pin_Num = '%s'", pin_num);
    
    if (mysql_query(conn, query))
    {
        TryAgain;
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL)
    {
        printf("Facing some issues while fetching your balance!\n");
        TryAgain;
        return 0;
    }

    int num_rows = mysql_num_rows(res);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (num_rows > 0)
    {
        int old_bal, sub_bal;

        old_bal = atoi(row[0]);

        printf("Withdraw Amount: ");

        while(true)
        {
            float temp;
            scanf("%f", &temp);
            if(temp == (int)temp)
            {
                sub_bal = temp;
                if((old_bal - sub_bal) < 500)
                {
                    printf("!INSUFFICIENT BALANCE!\n!!!(NOTE: BALANCE CAN'T BE LESS THAN 500)!!!\n");
                    WithdrawMsg;
                }

                else if(sub_bal == 0)
                    return 0;

                else if((sub_bal < 500) || (sub_bal % 100 != 0))
                {
                    printf("CAN'T WITHDRAW THE AMOUNT ENTERED (MAYBE YOU ENTERED VALUE LESS THAN 500?)\n");
                    WithdrawMsg;
                }

                else
                    break;
            }

            else
            {   printf("Value can't be a decimal!\n");
                WithdrawMsg;
            }
        }

        sprintf(secquery, "UPDATE User_Balance SET Balance = (%d) WHERE Pin_Num = ('%s')", (old_bal - sub_bal), pin_num);

        if (mysql_query(conn, secquery))
        {
            printf("Money couldn't be withdrawn!\n");
            TryAgain;
        }
        else
        {
            Chatak;
            printf("Money withdrawn successfully!\nNew balance: %d\n", (old_bal - sub_bal));
        }
    }
    
    else
    {
        TryAgain;
    }

    mysql_free_result(res);
}

void check_balance(MYSQL *conn, char* pin_num)
{
    char query[200];
    sprintf(query, "SELECT Balance FROM User_Balance WHERE Pin_Num = '%s'", pin_num);
    
    if (mysql_query(conn, query))
    {
        TryAgain;
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL)
    {
        printf("Facing some issues while fetching your data!\n");
        return;
    }

    int num_rows = mysql_num_rows(res);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (num_rows > 0)
    {
        printf("Your balance is: %s\n", row[0]);
    }
    
    else
    {
        TryAgain;
    }

    mysql_free_result(res);
}

void clear_screen()
{
    system("clear");
}

int generate_random_pin()
{
    int min = 1000, max = 9999;
    srand((unsigned int)time(NULL));

    return (rand() % (max - min + 1) + min);
}

void store_user_pin_and_balance(MYSQL *conn, char username[50], char password[50], int pin_num, int bal)
{
    char query[200], secquery[200];
    sprintf(query, "INSERT INTO User_Info (Username, Password, Pin_Num) VALUES ('%s', '%s', %d)", username, password, pin_num);
    sprintf(secquery, "UPDATE User_Balance SET Balance = (%d) WHERE Pin_Num = (%d)", bal, pin_num);
    if (mysql_query(conn, query) || mysql_query(conn, secquery))
    {
        printf("Pin_generation failed: %s\n", mysql_error(conn));
    }
    else
    {
        Chatak;
        printf("Your pin number is: %d\n!!!DON'T SHARE THIS WITH ANYONE!!!\n", pin_num);
    }
}

int signup(MYSQL *conn)
{
    MYSQL_ROW row; // Used for holding the data of a row when such data is fetched using a MySQL query.
    char username[50], password[50];
    int flag = 0;

    Chatak;
    printf("Enter username: ");
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = 0; // Remove trailing newline

    printf("Enter password: ");
    fgets(password, 50, stdin);
    password[strcspn(password, "\n")] = 0;

    // if (mysql_query(conn, query))
    // {
    //     printf("Sign up failed: %s\n", mysql_error(conn));
    // }

    // else
    // {
    //     printf("Sign up successful!\n");
    // }
    
    int re = initial_balance(conn);
    if(re == 0)
        return 0;

    printf("Wait till we create you your pin number!\n");
    int pin_num = generate_random_pin();
    sleep(2);

    // {
    //     /*
    //     fprintf: Used of writing the message to a particular output stream.
    //     stderr: Used for writing the error to error text file. Also, it a dedicated output stream for any errors other than stdout which is a general output stream for printing anything on the terminal.
    //     %s: It takes the error string passed on by mysql_error(conn) which contacts with MySQL's API while trying to execute the query.
    //     */

    //     fprintf(stderr, "%s\n", mysql_error(conn));
    // }
    char query[200];

    sprintf(query, "SELECT Pin_Num FROM User_Info");

    if (mysql_query(conn, query))
    {
        // fprintf(stderr, "%s\n", mysql_error(conn));
        printf("Sorry we can't generate a pin number for you at the moment.\nPlease try again later\n");
    }

    else
    {
        MYSQL_RES *res = mysql_store_result(conn);

        if((row = mysql_fetch_row(res)) != NULL && (row > 0) && (mysql_errno(conn)) == 0)
        {
            do
            {
                mysql_data_seek(res, 0);
                flag = 1;
                int temp;

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
                }
            }

            while (!flag);
            store_user_pin_and_balance(conn,username, password, pin_num, re);
        }

        else if(row == NULL)
            store_user_pin_and_balance(conn,username, password, pin_num, re);

        else 
        {
            printf("Sorry we can't generate a pin number for you at the moment.\nPlease try again later\n");
            mysql_free_result(res);
            return 0;
        }

        mysql_free_result(res); // Freeing the memory taken by result when we used mysql_store_result(conn)
        return 1;
    }
}

int login(MYSQL *conn, char* pin_num)
{
    printf("Pin number: ");
    fgets(pin_num, 5, stdin);
    // pin_num[strcspn(pin_num, "\n")] = 0;

    char query[200];
    sprintf(query, "SELECT * FROM User_Info WHERE Pin_Num='%s'", pin_num);

    if (mysql_query(conn, query))
    {
        printf("Login failed: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL)
    {
        printf("Login failed: %s\n", mysql_error(conn));
        return 0;
    }

    int num_rows = mysql_num_rows(res);
    mysql_free_result(res);

    if (num_rows > 0)
    {
        return 1;
    }
    
    else
    {
        printf("Incorret Pin Number!\n");
        return 0;
    }
}

int atm_menu(MYSQL *conn, char* pin_num)
{
    int choice;
    Chatak;
    printf("1. Check balance\n2. Withdraw\n3. Depoist\nChoose an option: ");
    scanf("%d", &choice);
    getchar();

    Chatak;
    switch (choice)
    {
    case 1:
        check_balance(conn, pin_num);
        break;
    case 2:
        withdraw(conn, pin_num);
        break;
    case 3:
        deposit(conn, pin_num);
        break;
    default:
        printf("Invalid option!\n");
        mysql_close(conn);
        return 1;
    }
}

int main()
{
    MYSQL *conn = mysql_init(NULL);
    char pin_num[5];

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
            int re = login(conn, pin_num);
            if(re == 0)
                break;
            
            atm_menu(conn,pin_num);
            break;
        default:
            printf("Invalid option!\n");
            mysql_close(conn);
            return 1;
        }
    }
}