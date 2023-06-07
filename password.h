#ifndef PASSWORD_H
#define PASSWORD_H

#include <string>
#include <vector>
#include <fstream>

class Password {
    static std::vector<std::string> CATEGORIES;
    std::string name;
    std::string password;
    std::string category;
    std::string service;
    std::string login;

    static std::string globalPassword;
    static std::string filePath;
    static size_t ID;

public:
    Password();
    Password(std::string name, std::string password, std::string category, std::string service, std::string login);

    friend std::fstream& operator<<(std::fstream& out, const Password& p);
    friend std::ostream& operator<<(std::ostream& out, const Password& p);

    static void setGlobalPassword(std::string p);
    static void setID(size_t a);
    static void setFilePath(std::string f);

    void setName(std::string n);
    void setPassword(std::string p);
    void setCategory(std::string c);
    void setService(std::string s);
    void setLogin(std::string l);

    static std::string getGlobalPassword();
    static void setCategories(std::vector<std::string> categories);
    static std::string getFilePath();
    std::string getName();
    std::string getPassword();
    std::string getCategory();
    std::string getService();
    std::string getLogin();

    bool operator<(const Password& other) const;

    static void getCategoriesList();
    static std::vector<std::string> getCategories();

    static std::string encryptLine(std::string line);
    static void emplaceBackCategory(const std::string& c);
    static void deleteCategory(int a);
};

#endif
