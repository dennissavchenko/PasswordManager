#include "password.h"
#include "encryptor.h"
#include <iostream>
#include <ctime>
#include <utility>

size_t Password::ID{0};
std::vector<std::string> Password::CATEGORIES{"Personal Account", "Work Account", "Financial Account", "Online Shopping", "Entertainment", "Health and Fitness", "Social Media"};
std::string Password::globalPassword{};
std::string Password::filePath{};

Password::Password() : name() {}

Password::Password(std::string name, std::string password, std::string category, std::string service, std::string login) : name(std::move(name)), password(std::move(password)), category(std::move(category)), service(std::move(service)), login(std::move(login)) {}

/**
 * Overloaded insertion operator to write a Password object to an fstream.
 *
 * @param out The output fstream.
 * @param p The Password object to write.
 * @return The output fstream after writing the Password object.
 */

std::fstream &operator<<(std::fstream &out, const Password &p) {

    std::time_t second_from_epoch = std::time(nullptr);
    auto localisation = std::localtime(&second_from_epoch);

    if (Password::ID++ == 0) {
        out << (std::to_string(localisation->tm_hour).size() == 1 ? "0" + std::to_string(localisation->tm_hour) : std::to_string(localisation->tm_hour)) << Password::encryptLine("Name: " + p.name) << "\n";
        out << (std::to_string(localisation->tm_min).size() == 1 ? "0" + std::to_string(localisation->tm_min) : std::to_string(localisation->tm_min)) << Password::encryptLine("Password: " + p.password) << "\n";
        out << (std::to_string(localisation->tm_sec).size() == 1 ? "0" + std::to_string(localisation->tm_sec) : std::to_string(localisation->tm_sec)) << Password::encryptLine("Category: " + p.category) << "\n";
    } else {
        out << Password::encryptLine("Name: " + p.name) << "\n";
        out << Password::encryptLine("Password: " + p.password) << "\n";
        out << Password::encryptLine("Category: " + p.category) << "\n";
    }
    out << Password::encryptLine("Service: " + p.service) << "\n";
    out << Password::encryptLine("Login: " + p.login) << "\n";

    return out;
}

/**
 * Overloaded insertion operator to write a Password object to an ostream.
 *
 * @param out The output ostream.
 * @param p The Password object to write.
 * @return The output ostream after writing the Password object.
 */

std::ostream &operator<<(std::ostream &out, const Password &p) {
    out << "Name: " << p.name << "\n";
    out << "Password: " << p.password << "\n";
    out << "Category: " << p.category << "\n";
    out << "Service: " << p.service << "\n";
    out << "Login: " << p.login << "\n";
    return out;
}

/**
 * Sets the global password for encryption.
 *
 * @param p The global password to set.
 */
void Password::setGlobalPassword(std::string p) {
    globalPassword = std::move(p);
}

/**
 * Sets the categories for password organization.
 *
 * @param categories The vector of categories to set.
 */
void Password::setCategories(std::vector<std::string> categories) {
    CATEGORIES = std::move(categories);
}

/**
 * Adds a new category to the list of categories.
 *
 * @param c The category to add.
 */
void Password::emplaceBackCategory(const std::string &c) {
    CATEGORIES.emplace_back(c);
}

/**
 * Deletes a category at the specified index.
 *
 * @param a The index of the category to delete.
 */
void Password::deleteCategory(int a) {
    CATEGORIES.erase(CATEGORIES.begin() + a);
}

/**
 * Sets the ID of the Password object.
 *
 * @param a The ID value to set.
 */
void Password::setID(size_t a) {
    ID = a;
}

/**
 * Sets the file path of the Password object.
 *
 * @param f The file path to set.
 */
void Password::setFilePath(std::string f) {
    filePath = std::move(f);
}

/**
 * Sets the name of the Password object.
 *
 * @param n The name to set.
 */
void Password::setName(std::string n) {
    this->name = std::move(n);
}

/**
 * Sets the password of the Password object.
 *
 * @param p The password to set.
 */
void Password::setPassword(std::string p) {
    this->password = std::move(p);
}

/**
 * Sets the category of the Password object.
 *
 * @param c The category to set.
 */
void Password::setCategory(std::string c) {
    this->category = std::move(c);
}

/**
 * Sets the service of the Password object.
 *
 * @param s The service to set.
 */
void Password::setService(std::string s) {
    this->service = std::move(s);
}

/**
 * Sets the login of the Password object.
 *
 * @param l The login to set.
 */
void Password::setLogin(std::string l) {
    this->login = std::move(l);
}

/**
 * Returns the global password.
 *
 * @return The global password.
 */
std::string Password::getGlobalPassword() {
    return globalPassword;
}

/**
 * Returns the file path.
 *
 * @return The file path.
 */
std::string Password::getFilePath() {
    return filePath;
}

/**
 * Returns the name.
 *
 * @return The name.
 */
std::string Password::getName() {
    return name;
}

/**
 * Returns the password.
 *
 * @return The password.
 */
std::string Password::getPassword() {
    return password;
}

/**
 * Returns the category.
 *
 * @return The category.
 */
std::string Password::getCategory() {
    return category;
}

/**
 * Returns the service.
 *
 * @return The service.
 */
std::string Password::getService() {
    return service;
}

/**
 * Returns the login.
 *
 * @return The login.
 */
std::string Password::getLogin() {
    return login;
}

/**
 * Comparison operator for sorting Password objects.
 *
 * @param other The Password object to compare to.
 * @return True if this object is less than the other object, false otherwise.
 */
bool Password::operator<(const Password& other) const {
    if (category == other.category)
        return service < other.service;
    else
        return category < other.category;
}

/**
 * Prints the list of categories.
 */
void Password::getCategoriesList() {
    for (auto i = 0; i < CATEGORIES.size(); i++) {
        std::cout << (i + 1) << ". " << CATEGORIES[i] << "\n";
    }
}

/**
 * Returns the vector of categories.
 *
 * @return The vector of categories.
 */
std::vector<std::string> Password::getCategories() {
    return CATEGORIES;
}

/**
 * Encrypts a line using the global password.
 * If the global password is empty, an error message is displayed.
 *
 * @param line The line to encrypt.
 * @return The encrypted line.
 */
std::string Password::encryptLine(std::string line) {
    if (globalPassword.empty()) std::cout << "You have to provide a password for encryption!" << "\n";
    else return encrypt(std::string(std::move(line)), globalPassword);
}

