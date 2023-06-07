# Password Manager

This project is aimed at building a minimal console application for managing and storing passwords securely. It allows users to create, modify, and retrieve passwords for different websites or services. The passwords are stored in an encrypted file, protected by a master password chosen by the user.

## Features

#### Choose a source file:
Upon launching the program, the user can either select a file located in the program's folder or provide a direct absolute path to the file. This file will serve as the source file for storing the encrypted passwords.
#### File encryption:
The data in the source file is encrypted using an original and fully understandable encryption method. The master password chosen by the user is an essential part of the encryption and decryption process. The encryption method should be designed in such a way that simple deduction methods or opening the file in a text editor will not be sufficient to decrypt its contents.
#### Timestamp tracking:
Each attempt to decrypt the file will save a timestamp of that operation. To hide this information, it can be broken down into different lines within the file.
