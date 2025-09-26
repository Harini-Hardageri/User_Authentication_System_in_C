# User Authentication System in C

A simple **User Authentication System implemented in C** with the following:

- User registration with username, password, secret question, and answer.
- Login and logout functionality.
- Password reset using secret question and answer.
- Delete user account functionality.
- Input validation and password masking.
- Tab-separated `.txt` file storage (`users_auth.txt`).

## Features

1. **Registration**
   * Username validation (3–15 characters, cannot start with a digit, only letters/digits/`_`/`.`).
   * Password validation (minimum 8 characters, must include uppercase, lowercase, digit, special character, no spaces).
   * Secret question and answer setup for password recovery.

2. **Login**
   * Authenticate users with username and password.
   * Maintains global state for logged-in user.

3. **Password Reset**
   * User can reset their password if they answer the secret question correctly.
   * Invalid passwords during reset are not accepted.

4. **Delete Account**
   * Delete account with confirmation.
   * Removes username, password, secret question, and answer from storage.

5. **Data Storage**
   * Users are stored in a tab-separated `.txt` file (`users_auth.txt`):
     ```
     username    password    secretQuestion    secretAnswer
     ```

6. **Security**
   * Masked password input using `getch()`.
   * Prevents buffer overflow.
   * Keeps original password intact if reset fails.

## Notes
- Maximum supported users: 125
- Password and secret answer length: 14 characters maximum
- Secret questions can be up to 49 characters
- Compatible with both Windows (conio.h) and Linux/Unix (termios.h)

## Compliation and Execution
- Compile the source code in your C compiler (GCC) and generate the executable file using gcc user_auth_reg_login.c -o user_auth_reg_login.exe command.
- Run the source code using user_auth_reg_login.exe command.

## User Authentication Output in C
![User_Authentication_Output_1](https://github.com/user-attachments/assets/d7e93bd2-8873-4952-948c-c3c42b2e1a6d)
<br>

![User_Authentication_Output_2](https://github.com/user-attachments/assets/8895d959-83f0-49d3-8d63-cccc7fd4306d)
<br>

![User_Authentication_Output_2](https://github.com/user-attachments/assets/184d5aa7-b639-4c72-b6f3-11d9295a4563)
