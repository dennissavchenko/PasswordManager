#include <fstream>
#include <filesystem>
#include <iostream>
#include <regex>
#include <algorithm>
#include <random>
#include <set>
#include "encryptor.h"
#include "password.h"
#include "menu.h"

/**
 * Extracts the information from a line by removing the part before the first space character.
 *
 * @param line The line to extract information from.
 * @return The extracted information.
 */

std::string getInfo(const std::string &line) {
    return line.substr(line.find_first_of(' ') + 1, line.length() - line.find_first_of(' '));
}

/**
 * Decodes passwords from a file.
 *
 * @param path The file path to decode passwords from.
 * @param globalPassword The global password used for decryption.
 * @return A vector of Password objects containing the decoded passwords.
 */

std::vector<Password> decodePasswords(const std::string &path, const std::string &globalPassword) {

    auto fileIn = std::fstream(path);
    auto fileOut = std::fstream(path);

    std::time_t second_from_epoch=std::time(nullptr);
    auto localisation = std::localtime(&second_from_epoch);

    auto correctPassword = true;

    auto timestamp = true;

    std::string text;
    std::string line;

    auto itr = 0;

    std::vector<Password> passwords;
    Password password = Password();

    Password::setGlobalPassword(globalPassword);
    Password::setFilePath(path);
    Password::setID(0);

    while(std::getline(fileIn, line)){
        auto decrypted = decrypt(line, globalPassword);
        if(decrypted.substr(0, 11) ==  "Categories,") {
            std::vector<std::string> categories;
            while (!decrypted.empty()) {
                decrypted = decrypted.substr(decrypted.find_first_of(',') + 1,decrypted.length() - decrypted.find_first_of(','));
                if(decrypted.substr(0, decrypted.find_first_of(',')).size() > 1) categories.emplace_back(decrypted.substr(0, decrypted.find_first_of(',')));
            }
            Password::setCategories(categories);
            text += line;
        }
        else {switch (itr) {
            case 0: {
                if(timestamp) {
                    std::cout << "Last attempt to enter or edit the file was at " << line.substr(0, 2) << ":";
                    decrypted = decrypt(line.substr(2, line.size() - 2), globalPassword);
                }
                if(decrypted.substr(0, 5) == "Name:") {
                    password.setName(getInfo(decrypted));
                } else correctPassword = false;
                if(timestamp) {
                    text += std::to_string(localisation->tm_hour).size() == 1 ? "0" + std::to_string(localisation->tm_hour) : std::to_string(localisation->tm_hour);
                    text += line.substr(2, line.size() - 2) + "\n";
                } else text += line + "\n";
                itr++;
                break;
            }
            case 1: {
                if(timestamp) {
                    std::cout << line.substr(0, 2) << ":";
                    decrypted = decrypt(line.substr(2, line.size() - 2), globalPassword);
                }
                if(correctPassword) password.setPassword(getInfo(decrypted));
                if(timestamp) {
                    text += std::to_string(localisation->tm_min).size() == 1 ? "0" + std::to_string(localisation->tm_min) : std::to_string(localisation->tm_min);
                    text += line.substr(2, line.size() - 2) + "\n";
                } else text += line + "\n";
                itr++;
                break;
            }
            case 2: {
                if(timestamp) {
                    std::cout << line.substr(0, 2) << "\n";
                    decrypted = decrypt(line.substr(2, line.size() - 2), globalPassword);
                }
                if(correctPassword) {
                    password.setCategory(getInfo(decrypted));
                }
                if(timestamp) {
                    text += std::to_string(localisation->tm_sec).size() == 1 ? "0" + std::to_string(localisation->tm_sec) : std::to_string(localisation->tm_sec);
                    text += line.substr(2, line.size() - 2) + "\n";
                } else text += line + "\n";
                timestamp = false;
                itr++;
                break;
            }
            case 3: {
                if(correctPassword) password.setService(getInfo(decrypted));
                text += line + "\n";
                itr++;
                break;
            }
            case 4: {
                if(correctPassword) {
                    password.setLogin(getInfo(decrypted));
                    passwords.push_back(password);
                }
                text += line + "\n";
                itr = 0;
            }
        }}
    }
    fileOut << text;
    if(correctPassword && !passwords.empty()) std::cout << "Decoded successfully!" << "\n";
    else std::cout << "Wrong password or file!" << "\n";
    return passwords;
}

/**
 * Encodes and saves passwords to a file.
 *
 * @param path The file path to save the passwords.
 * @param passwords The vector of passwords to encode and save.
 * @param globalPassword The global password used for encryption.
 */

void encodePasswords(const std::string &path, const std::vector<Password>  &passwords, const std::string &globalPassword) {
    std::ofstream fileClean(path, std::ios::trunc);
    fileClean.close();
    Password::setID(0);
    auto file = std::fstream(path);
    Password::setGlobalPassword(globalPassword);
    for (const auto &password : passwords) {
        file << password;
    }
    std::string line;
    line += "Categories,";
    for(const auto &category : Password::getCategories()) line += category + ",";
    file << Password::encryptLine(line);
}

/**
 * Displays the main menu and handles user input for various operations.
 *
 * @param passwords The vector of Password objects.
 */

void mainMenu(std::vector<Password> passwords);

/**
 * Searches for passwords by name.
 *
 * @param passwords The vector of passwords to search from.
 */

void searchPasswordByName(std::vector<Password> &passwords);

/**
 * Searches for passwords by service.
 *
 * @param passwords The vector of passwords to search from.
 */

void searchPasswordByService(std::vector<Password> &passwords);

/**
 * Searches for passwords by login.
 *
 * @param passwords The vector of passwords to search from.
 */

void searchPasswordByLogin(std::vector<Password> &passwords);

/**
 * Searches for passwords by category.
 *
 * @param passwords The vector of passwords to search from.
 */

void searchPasswordByCategory(std::vector<Password> &passwords);

/**
 * Allows the user to search for passwords based on different criteria.
 *
 * @param passwords The vector of passwords to search.
 */


void searchPasswords(std::vector<Password> &passwords);

/**
 * Searches for passwords by name.
 *
 * @param passwords The vector of passwords to search from.
 */

void searchPasswordByName(std::vector<Password> &passwords) {
    auto a = 0;
    std::cout << "Name: ";
    std::string word;
    std::getline(std::cin, word);
    std::cout << "-- Result of the search ----------------" << "\n";
    for(auto & password : passwords) {
        if(password.getName().substr(0, word.size()) == word) {
            a++;
            std::cout << "---- " << a << " password ------------------------" << "\n";
            std::cout << password;
        }
    }
    std::cout << "-- End of the search -------------------" << "\n";
    if(a == 0) std::cout << "Password with such a name was not found!" << "\n";
    std::cout << "Press '1' to search by name more." << "\n";
    std::cout << "Press any other key to come back to previous menu." << "\n";
    std::cout << "Provide your choice: ";
    auto key = '1';
    std::cin >> key;
    std::cin.ignore();
    if(key == '1') searchPasswordByName(passwords);
    else searchPasswords(passwords);
}

/**
 * Searches for passwords by service.
 *
 * @param passwords The vector of passwords to search from.
 */

void searchPasswordByService(std::vector<Password> &passwords) {
    auto a = 0;
    std::cout << "Service: ";
    std::string word;
    std::cin >> word;
    std::cout << "-- Result of the search ----------------" << "\n";
    for(auto & password : passwords) {
        if(password.getService().substr(0, word.size()) == word) {
            a++;
            std::cout << "---- " << a << " password ------------------------" << "\n";
            std::cout << password;
        }
    }
    std::cout << "-- End of the search -------------------" << "\n";
    if(a == 0) std::cout << "Password with such a service was not found!" << "\n";
    std::cout << "Press '1' to search by service more." << "\n";
    std::cout << "Press any other key to come back to previous menu." << "\n";
    std::cout << "Provide your choice: ";
    auto key = '1';
    std::cin >> key;
    if(key == '1') searchPasswordByName(passwords);
    else searchPasswords(passwords);

}

/**
 * Searches for passwords by login.
 *
 * @param passwords The vector of passwords to search from.
 */

void searchPasswordByLogin(std::vector<Password> &passwords) {
    auto a = 0;
    std::cout << "Login: ";
    std::string word;
    std::cin >> word;
    std::cout << "-- Result of the search ----------------" << "\n";
    for(auto & password : passwords) {
        if(password.getLogin().substr(0, word.size()) == word) {
            a++;
            std::cout << "---- " << a << " password ------------------------" << "\n";
            std::cout << password;
        }
    }
    std::cout << "-- End of the search -------------------" << "\n";
    if(a == 0) std::cout << "Password with such a login was not found!" << "\n";
    std::cout << "Press '1' to search by login more." << "\n";
    std::cout << "Press any other key to come back to previous menu." << "\n";
    std::cout << "Provide your choice: ";
    auto key = '1';
    std::cin >> key;
    if(key == '1') searchPasswordByLogin(passwords);
    else searchPasswords(passwords);
}

/**
 * Searches for passwords by category.
 *
 * @param passwords The vector of passwords to search from.
 */

void searchPasswordByCategory(std::vector<Password> &passwords) {
    auto a = 0;
    Password::getCategoriesList();
    auto number = 1;
    std::cout << "Provide your choice: ";
    std::cin >> number;
    std::cout << "-- Result of the search ----------------" << "\n";
    for(auto & password : passwords) {
        if(password.getCategory() == Password::getCategories()[number - 1]) {
            a++;
            std::cout << "---- " << a << " password ------------------------" << "\n";
            std::cout << password;
        }
    }
    std::cout << "-- End of the search -------------------" << "\n";
    if(a == 0) std::cout << "Password with such a category was not found!" << "\n";
    std::cout << "Press '1' to search by category more." << "\n";
    std::cout << "Press any other key to come back to previous menu." << "\n";
    std::cout << "Provide your choice: ";
    auto key = '1';
    std::cin >> key;
    if(key == '1') searchPasswordByCategory(passwords);
    else searchPasswords(passwords);
}

/**
 * Allows the user to search for passwords based on different criteria.
 *
 * @param passwords The vector of passwords to search.
 */

void searchPasswords(std::vector<Password> &passwords) {
    std::cout << "Press '1' to search password by name." << "\n";
    std::cout << "Press '2' to search password by category." << "\n";
    std::cout << "Press '3' to search password by service." << "\n";
    std::cout << "Press '4' to search password by login." << "\n";
    std::cout << "Press any other key to come back to main manu." << "\n";
    std::cout << "Provide your choice: ";
    auto search = '1';
    std::cin >> search;
    std::cin.ignore();
    switch (search) {
        case '1' : {
            searchPasswordByName(passwords);
            break;
        }
        case '2': {
            searchPasswordByCategory(passwords);
            break;
        }
        case '3': {
            searchPasswordByService(passwords);
            break;
        }
        case '4': {
            searchPasswordByLogin(passwords);
        }
        default: {
            mainMenu(passwords);
        }
    }
}

/**
 * Sorts the passwords vector by category and then by service.
 *
 * @param passwords The vector of passwords to sort.
 */

void sortPasswords(std::vector<Password> &passwords) {
    std::cout << "Passwords were sorted by category and then by service!" << "\n";
    std::sort(passwords.begin(), passwords.end());
    std::cout << "-- Result of the sorting ---------------" << "\n";
    auto a = 0;
    for(auto & password : passwords) {
        a++;
        std::cout << "---- " << a << " password -----------------------" << "\n";
        std::cout << password;
    }
    std::cout << "-- End of the sorting ------------------" << "\n";
    encodePasswords(Password::getFilePath(), passwords, Password::getGlobalPassword());
    mainMenu(passwords);
}

/**
 * Checks if a password meets the safety requirements.
 *
 * @param password The password to check.
 * @return True if the password is safe, false otherwise.
 */

bool isPasswordSafe(const std::string &password) {
    if (password.length() < 8) {
        std::cout << "Password should be at least 8 characters long." << std::endl;
        return false;
    }
    if (!std::regex_search(password, std::regex("[A-Z]"))) {
        std::cout << "Password should contain at least one uppercase letter." << std::endl;
        return false;
    }
    if (!std::regex_search(password, std::regex("[a-z]"))) {
        std::cout << "Password should contain at least one lowercase letter." << std::endl;
        return false;
    }
    if (!std::regex_search(password, std::regex("[0-9]"))) {
        std::cout << "Password should contain at least one digit." << std::endl;
        return false;
    }
    if (!std::regex_search(password, std::regex("[^a-zA-Z0-9]"))) {
        std::cout << "Password should contain at least one special character." << std::endl;
        return false;
    }
    std::cout << "Password is safe." << std::endl;
    return true;
}

/**
 * Generates a random password based on the specified criteria.
 *
 * @param length The length of the password.
 * @param uppercase Flag indicating whether the password should contain uppercase letters.
 * @param lowercase Flag indicating whether the password should contain lowercase letters.
 * @param special Flag indicating whether the password should contain special characters.
 * @return The generated password.
 */

std::string generatePassword(int length, bool uppercase, bool lowercase, bool special) {

    std::string password;
    std::string uppercaseChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string lowercaseChars = "abcdefghijklmnopqrstuvwxyz";
    std::string specialChars = "!@#$%^&*()";
    std::string numbers = "0123456789";

    std::string signs;

    signs += numbers;

    if (uppercase) {
        signs += uppercaseChars;
    }
    if (lowercase) {
        signs += lowercaseChars;
    }
    if (special) {
        signs += specialChars;
    }

    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> range(0, signs.length() - 1);

    for (int i = 0; i < length; ++i) {
        password += signs[range(generator)];
    }

    std::cout << "Password: " << password << "\n";
    return password;
}

/**
 * Retrieves a password from the user, either by creating one or using a generated password.
 *
 * @param passwords The vector of Password objects.
 * @return The retrieved password.
 */

std::string getLocalPassword(std::vector<Password> &passwords) {
    std::cout << "Press '1' to create your own password." << "\n";
    std::cout << "Press any other key to use generated password." << "\n";
    std::cout << "Provide your choice: ";
    std::string buf;
    auto b = '1';
    std::cin >> b;
    if(b == '1') {
        std::cout << "Password: ";
        std::cin >> buf;
        auto accept = false;
        while (!accept) {
            auto duplicate = false;
            for(int i = 0; i < passwords.size() && !duplicate; i++) {
                if(buf == passwords[i].getPassword()) {
                    std::cout << "This password is already used!" << "\n";
                    duplicate = true;
                }
            }
            if(!duplicate) std::cout << "This password was never used!" << "\n";
            if(!duplicate && isPasswordSafe(buf)) accept = true;
            if(!accept) {
                std::cout << "Press '1' if you want to change the password." << "\n";
                std::cout << "Press any other key if you want to continue." << "\n";
                std::cout << "Provide your choice: ";
                auto c = '1';
                std::cin >> c;
                if (c == '1') {
                    std::cout << "Password: ";
                    std::cin >> buf;
                } else accept = true;
            }
        }
        return buf;
    } else {
        auto upper = false;
        auto lower = false;
        auto special = false;
        std::cout << "Enter number of characters of the password: ";
        auto l = 0;
        std::cin >> l;
        while (l <= 0) {
            std::cout << "Length must be bigger than zero! Try again: ";
            std::cin >> l;
        }
        std::cout << "Press '1' if you want password to contain uppercase." << "\n";
        std::cout << "Press any other key if you don't want password to contain uppercase." << "\n";
        std::cout << "Provide your choice: ";
        auto a = '1';
        std::cin >> a;
        if(a == '1') upper = true;
        std::cout << "Press '1' if you want password to contain lowercase." << "\n";
        std::cout << "Press any other key if you don't want password to contain lowercase." << "\n";
        std::cout << "Provide your choice: ";
        a = '1';
        std::cin >> a;
        if(a == '1') lower = true;
        std::cout << "Press '1' if you want password to contain special chars." << "\n";
        std::cout << "Press any other key if you don't want password to contain special chars." << "\n";
        std::cout << "Provide your choice: ";
        a = '1';
        std::cin >> a;
        if(a == '1') special = true;
        return generatePassword(l, upper, lower, special);
    }
}

/**
 * Adds a new password to the vector of Password objects.
 *
 * @param passwords The vector of Password objects.
 */

void addPassword(std::vector<Password> &passwords) {
    Password password;
    std::cout << "Name: ";
    std::string buf;
    std::getline(std::cin, buf);
    password.setName(buf);
    password.setPassword(getLocalPassword(passwords));
    Password::getCategoriesList();
    std::cout << "Enter number of the category from the list: ";
    auto i = 1;
    std::cin >> i;
    while (i <= 0 || i > Password::getCategories().size()) {
        std::cout << "Wrong category number! Try again!";
        std::cin >> i;
    }
    password.setCategory(Password::getCategories()[i - 1]);
    std::cout << "Service: ";
    std::cin >> buf;
    password.setService(buf);
    std::cout << "Login: ";
    std::cin >> buf;
    password.setLogin(buf);
    std::cout << "-- Password summary --------------------" << "\n";
    std::cout << password;
    std::cout << "-- End of the summary ------------------" << "\n";
    std::cout << "Press '1' to save created password." << "\n";
    std::cout << "Press any other key if you do not want to save the password." << "\n";
    std::cout << "Provide your choice: ";
    auto a = '1';
    std::cin >> a;
    if(a == '1') {
        passwords.emplace_back(password);
        encodePasswords(Password::getFilePath(), passwords, Password::getGlobalPassword());
    }
    std::cout << "Press '1' to create one more password!" << "\n";
    std::cout << "Press any other key to come back to main menu." << "\n";
    std::cout << "Provide your choice: ";
    std::cin >> a;
    if(a == '1') addPassword(passwords);
    else mainMenu(passwords);
}

/**
 * Helper function that assists in editing a specific attribute of a Password object.
 *
 * @param passwordCopy The Password object to be edited.
 * @param passwords The vector of Password objects.
 * @return The modified Password object.
 */

Password editHelper(Password &passwordCopy, std::vector<Password> &passwords) {
    Password password = passwordCopy;
    std::vector<std::string> list = {"Name", "Password", "Category", "Service", "Login"};
    std::cout << "What do you want to edit?" << "\n";
    auto b = 1;
    for (const auto &l: list) std::cout << b++ << ". " << l << "\n";
    std::cout << "Provide the number of the attribute you want to change: ";
    std::cin >> b;
    std::cin.ignore();
    while (b <= 0 || b > list.size()) {
        std::cout << "Attribute under such a number doesn't exist! Try again: ";
        std::cin >> b;
        std::cin.ignore();
    }

    if (b != 3 && b != 2) std::cout << list[b - 1] << ": ";
    std::string word;
    if (b == 1) std::getline(std::cin, word);
    else if (b == 2) password.setPassword(getLocalPassword(passwords));
    else if (b == 3) {
        Password::getCategoriesList();
        std::cout << "Enter the number of the category from the list: ";
        auto i = 1;
        std::cin >> i;
        while (i <= 0 || i > Password::getCategories().size()) {
            std::cout << "Wrong category number! Try again: ";
            std::cin >> i;
        }
        password.setCategory(Password::getCategories()[i - 1]);
    } else std::cin >> word;

    switch (b) {
        case 1: {
            password.setName(word);
            break;
        }
        case 4: {
            password.setService(word);
            break;
        }
        case 5:
            password.setLogin(word);
    }

    std::cout << "Press '1' to edit other attributes of the password." << "\n";
    std::cout << "Press any other key to go to the password summary." << "\n";
    std::cout << "Provide your choice: ";
    auto d = '1';
    std::cin >> d;

    if (d == '1') {
        return editHelper(password, passwords); // Update the return value
    } else {
        passwordCopy = password; // Update the passwordCopy in the outer scope
        return password;
    }
}


/**
 * Allows the user to edit a specific password from the vector of Password objects.
 *
 * @param passwords The vector of Password objects.
 */

void editPassword(std::vector<Password> &passwords) {
    std::cout << "-- List of available passwords ---------" << "\n";
    auto a = 1;
    for (auto password: passwords) {
        std::cout << "---- " << a++ << " password -----------------------" << "\n";
        std::cout << password;
    }
    std::cout << "-- End of the list ---------------------" << "\n";
    std::cout << "Provide number of password, you want to change, from the list: ";
    std::cin >> a;
    while (a <= 0 || a > passwords.size()) {
        std::cout << "Password under such a number doesn't exist! Try again: ";
        std::cin >> a;
    }
    std::cout << "-- Password summary --------------------" << "\n";
    std::cout << passwords[a - 1];
    std::cout << "-- End of the summary ------------------" << "\n";
    Password passwordCopy = passwords[a - 1];
    Password password = editHelper(passwordCopy, passwords);
    std::cout << "-- Password summary --------------------" << "\n";
    std::cout << password;
    std::cout << "-- End of the summary ------------------" << "\n";
    std::cout << "Press '1' to save edited password." << "\n";
    std::cout << "Press any other key if you do not want to save the password." << "\n";
    std::cout << "Provide your choice: ";
    auto c = '1';
    std::cin >> c;
    if (c == '1') {
        passwords[a - 1].setName(password.getName());
        passwords[a - 1].setPassword(password.getPassword());
        passwords[a - 1].setCategory(password.getCategory());
        passwords[a - 1].setService(password.getService());
        passwords[a - 1].setLogin(password.getLogin());
        encodePasswords(Password::getFilePath(), passwords, Password::getGlobalPassword());
    }
    std::cout << "Press '1' to edit one more password!" << "\n";
    std::cout << "Press any other key to come back to main menu." << "\n";
    std::cout << "Provide your choice: ";
    std::cin >> c;
    if (c == '1') editPassword(passwords);
    else mainMenu(passwords);
}

/**
 * Deletes one or more passwords from the vector of Password objects.
 *
 * @param passwords The vector of Password objects.
 */

void deletePasswords(std::vector<Password> &passwords) {
    std::cout << "-- List of available passwords ---------" << "\n";
    auto a = 0;
    for(auto & password : passwords) {
        a++;
        std::cout << "---- " << a << " password -----------------------" << "\n";
        std::cout << password;
    }
    std::cout << "-- End of the list ---------------------" << "\n";
    std::cout << "How many passwords from the list do you want to delete: ";
    std::cin >> a;
    while (a <= 0 || a > passwords.size()) {
        std::cout << "Minimum number is 1. Maximum number is " << passwords.size() << "! Try again: ";
        std::cin >> a;
    }
    std::set<int> number;
    for(int i = 0; i < a; i++) {
        std::cout << "Enter number of " << i + 1 <<  " password you want to delete: ";
        auto b = 0;
        std::cin >> b;
        while (b <= 0 || b > passwords.size()) {
            std::cout << "Password under such a number doesn't exist! Try again: ";
            std::cin >> b;
        }
        number.insert(b);
    }
    std::cout << "-- Password(s) summary -----------------" << "\n";
    a = 1;
    for(auto index : number) {
        std::cout << "---- " << a++ << " password -----------------------" << "\n";
        std::cout << passwords[index - 1];
    }
    std::cout << "-- End of the summary ------------------" << "\n";
    std::cout << "Press '1' to confirm deleting the range of password(s)" << "\n";
    std::cout << "Press any other key if you do not want to delete the password(s)." << "\n";
    std::cout << "Provide your choice: ";
    auto c = '1';
    std::cin >> c;
    if(c == '1') {
        auto d = 0;
        for(auto index : number) {
            passwords.erase(passwords.begin() + index - 1 + d--);
        }
        std::cout << "-- List of available passwords ---------" << "\n";
        a = 1;
        for(auto & password : passwords) {
            std::cout << "---- " << a++ << " password -----------------------" << "\n";
            std::cout << password;
        }
        std::cout << "-- End of the list ---------------------" << "\n";
        encodePasswords(Password::getFilePath(), passwords, Password::getGlobalPassword());
    }
    std::cout << "Press '1' to delete more passwords." << "\n";
    std::cout << "Press any other key to come back to previous menu." << "\n";
    std::cout << "Provide your choice: ";
    std::cin >> c;
    if(c == '1') deletePasswords(passwords);
    else mainMenu(passwords);
}

/**
 * Adds a new category to the vector of Password objects and updates the password file.
 *
 * @param passwords The vector of Password objects.
 */

void addCategory(std::vector<Password> &passwords) {
    std::cout << "You have such categories: " << "\n";
    Password::getCategoriesList();
    std::cout << "Type name of the category you want to add: ";
    std::string word;
    std::cin >> word;
    Password::emplaceBackCategory(word);
    encodePasswords(Password::getFilePath(), passwords, Password::getGlobalPassword());
    std::cout << "Updated list of categories: " << "\n";
    Password::getCategoriesList();
    std::cout << "Do you want to create password for the new category?" << "\n";
    std::cout << "Press '1' to create new password." << "\n";
    std::cout << "Press '2' to add new category." << "\n";
    std::cout << "Press any other key to come back to previous menu." << "\n";
    std::cout << "Provide your choice: ";
    auto a = '1';
    std::cin >> a;
    if(a == '1') addPassword(passwords);
    else if(a == '2') addCategory(passwords);
    else mainMenu(passwords);
}

/**
 * Deletes a category and all passwords belonging to that category from the vector of Password objects.
 *
 * @param passwords The vector of Password objects.
 */

void deleteCategory(std::vector<Password> &passwords) {
    std::cout << "You have such categories: " << "\n";
    Password::getCategoriesList();
    std::cout << "Enter number of the category, you want to delete, from the list: ";
    auto a = 0;
    std::cin >> a;
    while (a <= 0 || a > Password::getCategories().size()) {
        std::cout << "You have entered wrong number! Try again: ";
        std::cin >> a;
    }
    std::cout << "Such passwords will be deleted: " << "\n";
    std::cout << "-- Password summary --------------------" << "\n";
    auto b = 1;
    for(auto password : passwords) {
        if(password.getCategory() == Password::getCategories()[a - 1]) {
            std::cout << "---- " << b++ << " password -----------------------" << "\n";
            std::cout << password;
        }
    }
    std::cout << "-- End of the summary ------------------" << "\n";
    std::cout << "Press '1' to confirm deleting the category and belonging passwords." << "\n";
    std::cout << "Press any other key to decline deleting." << "\n";
    std::cout << "Provide your choice: ";
    auto c = '1';
    std::cin >> c;
    if(c == '1') {
        passwords.erase(std::remove_if(passwords.begin(), passwords.end(), [&](Password password) {
            return password.getCategory() == Password::getCategories()[a - 1];
        }), passwords.end());
        Password::deleteCategory(a - 1);
        encodePasswords(Password::getFilePath(), passwords, Password::getGlobalPassword());
    }
    std::cout << "Press '1' to delete one more category." << "\n";
    std::cout << "Press any other key to come back to previous menu." << "\n";
    std::cout << "Provide your choice: ";
    std::cin >> c;
    if(c == '1') deleteCategory(passwords);
    else mainMenu(passwords);
}

/**
 * Checks if a file is empty by opening it and checking if the file stream reaches the end of file.
 *
 * @param filename The name of the file to check.
 * @return True if the file is empty, false otherwise.
 */

bool isFileEmpty(const std::string& filename) {
    std::ifstream file(filename);
    return file.peek() == std::ifstream::traits_type::eof();
}

/**
 * Displays the file menu and handles user input for choosing a file or providing a custom file path.
 *
 * @return A vector of Password objects loaded from the chosen file.
 */

std::vector<Password> fileMenu() {

    std::cout << "Hello! It is your password manager." << "\n";
    std::cout << "Press '1' to choose a file from our directory." << "\n";
    std::cout << "Press any other key to provide your own path to the file." << "\n";
    std::cout << "Provide your choice: ";

    auto choice = '1';

    std::cin >> choice;

    auto path = std::string();

    if(choice == '1') {
        auto itr = 1;
        std::vector<std::string> filePaths;
        std::cout << "Such files have been detected: " << "\n";
        std::filesystem::path home_path = std::filesystem::current_path();
        home_path = home_path.parent_path();
        for (const auto& entry : std::filesystem::directory_iterator(home_path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt" && entry.path().filename() != "CMakeLists.txt") {
                std::cout << itr++ << " - " << entry.path().filename() << "\n";
                filePaths.emplace_back(entry.path().filename());
            }
        }
        std::cout << "Choose file from the list and type its number : ";
        int ind = 0;
        std::cin >> ind;
        while (ind <= 0 || ind > filePaths.size()) {
            std::cout << "There is no such a file in the list! Try again: ";
            std::cin >> ind;
        }
        path = "../" + filePaths[ind - 1];
    }
    else {
        std::cout << "Enter path to the file: ";
        std::cin >> path;
        auto file = std::fstream(path);
        while (file.fail()) {
            std::cerr << "Error: File not found! Try another one: ";
            std::cin >> path;
            file = std::fstream(path);
        }
    }

    if(isFileEmpty(path)) {
        std::cout << "File is empty!" << "\n";
        std::vector<Password> passwords;
        mainMenu(fileMenu());
        return passwords;
    } else {
        std::cout << "Provide password for the file: ";
        auto globalPassword = std::string();
        std::cin >> globalPassword;
        return decodePasswords(path, globalPassword);
    }
}

/**
 * Displays the main menu and handles user input for various operations.
 *
 * @param passwords The vector of Password objects.
 */

void mainMenu(std::vector<Password> passwords) {
    if (!passwords.empty()) {
        std::cout << "Press '1' to search password." << "\n";
        std::cout << "Press '2' to sort passwords." << "\n";
        std::cout << "Press '3' to add password." << "\n";
        std::cout << "Press '4' to edit password." << "\n";
        std::cout << "Press '5' to delete password(s)." << "\n";
        std::cout << "Press '6' to add category." << "\n";
        std::cout << "Press '7' to delete category." << "\n";
        std::cout << "Press any other key to come back to file menu." << "\n";
        std::cout << "Provide your choice: ";
        auto choice = '1';
        std::cin >> choice;
        std::cin.ignore();
        switch (choice) {
            case '1': {
                searchPasswords(passwords);
                break;
            }
            case '2': {
                sortPasswords(passwords);
                break;
            }
            case '3': {
                addPassword(passwords);
                break;
            }
            case '4': {
                editPassword(passwords);
                break;
            }
            case '5': {
                deletePasswords(passwords);
                break;
            }
            case '6': {
                addCategory(passwords);
                break;
            }
            case '7': {
                deleteCategory(passwords);
                break;
            }
            default:
                mainMenu(fileMenu());
        }
    }
}

