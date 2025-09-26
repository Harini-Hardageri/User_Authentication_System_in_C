#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
char getch()
{
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

#define MAX_USERS 125
#define CREDENTIAL_LENGTH 15

typedef struct
{
    char username[CREDENTIAL_LENGTH];
    char password[CREDENTIAL_LENGTH];
    char secretQuestion[50];
    char secretAnswer[CREDENTIAL_LENGTH];
} User;

int user_count = 0;
int isLoggedIn = 0;
User currentUser;

// Function prototypes
void initUserCount();
void registerUser(User newUser);
int isValidUsername(const char *username);
int isValidPassword(const char *password);
int isUsernameExists(char *username);
int authenticateUser(char *username, char *password);
void getMaskedPassword(char *password);
void resetPassword(char *username);
void logoutUser();
void deleteAccount(char *username, char *password);

int main()
{
    int choice;
    User newUser;
    char username[CREDENTIAL_LENGTH];
    char password[CREDENTIAL_LENGTH];

    initUserCount(); // load existing users

    do
    {
        printf("\n***** Welcome to User Authentication System *****\n");
        printf("1. Register new Account\n");
        printf("2. Login into your Account\n");
        printf("3. Forgot Password? Reset it\n");
        printf("4. Logout from your Account\n");
        printf("5. Delete your Account\n");
        printf("6. Exit Program\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input.\n");
            while (getchar() != '\n')
                ;
            continue;
        }
        getchar(); // clear newline

        switch (choice)
        {
        case 1:
            printf("\nRegistration of a new user\n");
            printf("Enter username: ");
            scanf("%14s", newUser.username);
            printf("Enter password: ");
            getMaskedPassword(newUser.password);

            if (isUsernameExists(newUser.username))
            {
                printf("Username already exists. Choose a different username.\n");
                break;
            }
            registerUser(newUser);
            break;

        case 2:
            if (isLoggedIn)
            {
                printf("You are already logged in as %s.\n", currentUser.username);
                break;
            }
            printf("Enter username: ");
            scanf("%14s", username);
            printf("Enter password: ");
            getMaskedPassword(password);

            if (authenticateUser(username, password))
                printf("Login successful!\n");
            else
                printf("Login failed. Invalid username or password.\n");
            break;

        case 3:
            printf("Enter your username: ");
            scanf("%14s", username);
            resetPassword(username);
            break;

        case 4:
            logoutUser();
            break;

        case 5:
            if (isLoggedIn)
                deleteAccount(currentUser.username, currentUser.password);
            else
            {
                printf("Enter username to delete: ");
                scanf("%14s", username);
                printf("Enter password: ");
                getMaskedPassword(password);
                deleteAccount(username, password);
            }
            break;

        case 6:
            printf("Exiting Program...\n");
            break;

        default:
            printf("Invalid choice! Please enter again.\n");
        }
    } while (choice != 6);

    return 0;
}

void initUserCount()
{
    FILE *fp = fopen("users_auth.txt", "r");
    if (!fp)
    {
        user_count = 0;
        return;
    }

    char line[256];
    User temp;
    user_count = 0;

    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%14[^\t]\t%14[^\t]\t%49[^\t]\t%14[^\n]",
                   temp.username, temp.password, temp.secretQuestion, temp.secretAnswer) == 4)
        {
            user_count++;
        }
    }
    fclose(fp);
}

int isValidUsername(const char *username)
{
    int len = strlen(username);
    if (len < 3 || len > 15)
    {
        printf("Username must be 3-15 chars.\n");
        return 0;
    }
    if (isdigit(username[0]))
    {
        printf("Username must not start with digit.\n");
        return 0;
    }
    for (int i = 0; i < len; i++)
    {
        char c = username[i];
        if (!isalnum(c) && c != '_' && c != '.')
        {
            printf("Invalid character in username.\n");
            return 0;
        }
        if (isspace(c))
        {
            printf("Username cannot contain spaces.\n");
            return 0;
        }
    }
    return 1;
}

int isValidPassword(const char *password)
{
    int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0;
    int len = strlen(password);
    if (len < 8)
    {
        printf("Password too short.\n");
        return 0;
    }
    for (int i = 0; i < len; i++)
    {
        if (isupper(password[i]))
            hasUpper = 1;
        else if (islower(password[i]))
            hasLower = 1;
        else if (isdigit(password[i]))
            hasDigit = 1;
        else if (strchr("!@#$^&*()-_=+[]{};:'\",.<>?/\\|", password[i]))
            hasSpecial = 1;
        else if (isspace(password[i]))
        {
            printf("Password cannot contain spaces.\n");
            return 0;
        }
    }
    if (!hasUpper)
    {
        printf("Password must have uppercase.\n");
        return 0;
    }
    if (!hasLower)
    {
        printf("Password must have lowercase.\n");
        return 0;
    }
    if (!hasDigit)
    {
        printf("Password must have digit.\n");
        return 0;
    }
    if (!hasSpecial)
    {
        printf("Password must have special char.\n");
        return 0;
    }
    return 1;
}

void getMaskedPassword(char *password)
{
    int i = 0;
    char c;
    while (1)
    {
        c = getch();
        if (c == '\r' || c == '\n')
            break;
        else if (c == '\b' || c == 127)
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
            }
        }
        else if (i < CREDENTIAL_LENGTH - 1)
        {
            password[i++] = c;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}

void registerUser(User newUser)
{
    if (!isValidUsername(newUser.username))
    {
        printf("Invalid username.\n");
        return;
    }
    if (!isValidPassword(newUser.password))
    {
        printf("Invalid password.\n");
        return;
    }
    printf("Set secret question: ");
    getchar();
    fgets(newUser.secretQuestion, sizeof(newUser.secretQuestion), stdin);
    newUser.secretQuestion[strcspn(newUser.secretQuestion, "\n")] = 0;
    if (strlen(newUser.secretQuestion) == 0)
    {
        printf("Secret question cannot be empty.\n");
        return;
    }
    printf("Set answer to secret question: ");
    scanf("%14s", newUser.secretAnswer);
    if (strlen(newUser.secretAnswer) == 0)
    {
        printf("Secret answer cannot be empty.\n");
        return;
    }
    if (user_count >= MAX_USERS)
    {
        printf("Maximum users reached.\n");
        return;
    }

    FILE *fp = fopen("users_auth.txt", "a+");
    if (!fp)
    {
        printf("Error opening file.\n");
        return;
    }
    fprintf(fp, "%s\t%s\t%s\t%s\n", newUser.username, newUser.password, newUser.secretQuestion, newUser.secretAnswer);
    fclose(fp);
    user_count++;
    printf("Registration successful!\n");
}

int isUsernameExists(char *username)
{
    FILE *fp = fopen("users_auth.txt", "r");
    if (!fp)
        return 0;
    char line[256];
    User u;
    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%14[^\t]\t%14[^\t]\t%49[^\t]\t%14[^\n]", u.username, u.password, u.secretQuestion, u.secretAnswer) == 4)
        {
            if (strcmp(u.username, username) == 0)
            {
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);
    return 0;
}

int authenticateUser(char *username, char *password)
{
    FILE *fp = fopen("users_auth.txt", "r");
    if (!fp)
        return 0;
    char line[256];
    User u;
    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%14[^\t]\t%14[^\t]\t%49[^\t]\t%14[^\n]", u.username, u.password, u.secretQuestion, u.secretAnswer) == 4)
        {
            if (strcmp(u.username, username) == 0 && strcmp(u.password, password) == 0)
            {
                fclose(fp);
                isLoggedIn = 1;
                currentUser = u;
                return 1;
            }
        }
    }
    fclose(fp);
    return 0;
}

void resetPassword(char *username) {
    FILE *fp = fopen("users_auth.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        printf("User database not found.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    char line[256];
    User u;
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%14[^\t]\t%14[^\t]\t%49[^\t]\t%14[^\n]",
                   u.username, u.password, u.secretQuestion, u.secretAnswer) == 4) {

            if (strcmp(u.username, username) == 0) {
                found = 1;
                printf("Answer your secret question:\n%s\n", u.secretQuestion);

                getchar(); // clear newline
                char answer[CREDENTIAL_LENGTH];
                fgets(answer, sizeof(answer), stdin);
                answer[strcspn(answer, "\n")] = 0; // remove newline

                if (strcmp(answer, u.secretAnswer) == 0) {
                    char newPassword[CREDENTIAL_LENGTH];
                    printf("Enter your new password: ");
                    getMaskedPassword(newPassword);

                    if (isValidPassword(newPassword)) {
                        strcpy(u.password, newPassword); // only update if valid
                        printf("Password reset successful!\n");
                    } else {
                        printf("Invalid password. Reset aborted. Original password retained.\n");
                        // do not change u.password
                    }
                } else {
                    printf("Incorrect answer. Cannot reset password.\n");
                    // do not change u.password
                }
            }

            // Write user line to temp file
            fprintf(temp, "%s\t%s\t%s\t%s\n", u.username, u.password, u.secretQuestion, u.secretAnswer);
        }
    }

    fclose(fp);
    fclose(temp);

    if (!found) {
        printf("Username not found.\n");
        remove("temp.txt");
        return;
    }

    remove("users_auth.txt");
    rename("temp.txt", "users_auth.txt");
}

void logoutUser()
{
    if (isLoggedIn)
    {
        char tmp[CREDENTIAL_LENGTH];
        strcpy(tmp, currentUser.username);
        isLoggedIn = 0;
        memset(&currentUser, 0, sizeof(User));
        printf("%s, logged out successfully.\n", tmp);
    }
    else
        printf("Not logged in.\n");
}

void deleteAccount(char *username, char *password)
{
    char confirm;
    printf("Are you sure you want to delete your account? (y/n): ");
    scanf(" %c", &confirm);
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("Account deletion canceled.\n");
        return;
    }

    FILE *fp = fopen("users_auth.txt", "r");
    if (!fp)
    {
        printf("No user database found.\n");
        return;
    }
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        printf("Error creating temp file.\n");
        fclose(fp);
        return;
    }

    char line[256];
    User u;
    int found = 0;
    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%14[^\t]\t%14[^\t]\t%49[^\t]\t%14[^\n]", u.username, u.password, u.secretQuestion, u.secretAnswer) == 4)
        {
            if (strcmp(u.username, username) == 0 && strcmp(u.password, password) == 0)
            {
                found = 1;
                printf("Account deleted successfully.\n");
                continue;
            }
            fprintf(temp, "%s\t%s\t%s\t%s\n", u.username, u.password, u.secretQuestion, u.secretAnswer);
        }
    }

    fclose(fp);
    fclose(temp);
    if (!found)
    {
        printf("Invalid credentials.\n");
        remove("temp.txt");
        return;
    }
    remove("users_auth.txt");
    rename("temp.txt", "users_auth.txt");
    user_count--;
    if (isLoggedIn && strcmp(currentUser.username, username) == 0)
        logoutUser();
}