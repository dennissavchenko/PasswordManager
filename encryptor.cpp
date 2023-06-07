#include <string>
#include "encryptor.h"

/**
 * Moves the characters in the given phrase one position to the right.
 *
 * @param phrase The phrase to be moved.
 * @return The modified phrase after moving the characters to the right.
 */

std::string move_right(std::string &phrase) {

    auto holder = phrase[phrase.size() - 1];

    for (int i = phrase.size() - 1; i > 0; i--) {
        phrase[i] = phrase[i - 1];
    }

    phrase[0] = holder;

    return phrase;
}

/**
 * Moves the characters in the given phrase one position to the left.
 *
 * @param phrase The phrase to be moved.
 * @return The modified phrase after moving the characters to the left.
 */

std::string move_left(std::string &phrase) {

    auto holder = phrase[0];

    for (int i = 0; i < phrase.size() - 1; i++) {
        phrase[i] = phrase[i + 1];
    }

    phrase[phrase.size() - 1] = holder;

    return phrase;

}

/**
 * Encrypts the given phrase using the provided password.
 *
 * @param phrase The phrase to encrypt.
 * @param password The password used for encryption.
 * @return The encrypted phrase.
 */

std::string encrypt(std::string phrase, const std::string &password) {

    std::string bigKey;

    for (auto symbol: password) {
        bigKey += std::to_string((int) symbol);
    }

    std::string encrypt;

    for (auto charKey: bigKey) {

        encrypt = "";

        int key = charKey - '0';

        if (key >= phrase.size()) key -= phrase.size();

        if (key == 0) phrase = move_right(phrase);
        else if (key == 1) phrase = move_left(phrase);
        else {
            for (int i = 0; i < key; i++) {
                for (int j = i; j < phrase.size(); j += key) {
                    encrypt += phrase[j];
                }
            }

            phrase = encrypt;

        }

    }

    encrypt = "";

        for (int i = 0; i < phrase.size(); i++) {
            int digit = phrase[i];
            digit += bigKey[0] - '0';
            char moved = digit;
            encrypt += moved;
        }

    phrase = encrypt;

    return phrase;

}

/**
 * Decrypts the given phrase using the provided password.
 *
 * @param phrase The phrase to decrypt.
 * @param password The password used for decryption.
 * @return The decrypted phrase.
 */

std::string decrypt(std::string phrase, const std::string &password) {

    std::string bigKey;

    for(auto symbol : password) {
        bigKey += std::to_string((int)symbol);
    }

    std::string decrypt;

    decrypt = "";

    for(int i = 0; i < phrase.size(); i++) {
            int digit = phrase[i];
            digit -= bigKey[0] - '0';
            char moved = digit;
            decrypt += moved;
    }

    phrase = decrypt;

    for(int i = bigKey.length() - 1; i >= 0; i--) {

        int key = bigKey[i] - '0';

        if(key >= phrase.size()) key -= phrase.size();

        int itr = 0;

        decrypt = phrase;

        if(key == 0) phrase = move_left(phrase);
        else if(key == 1) phrase = move_right(phrase);
        else {

            for (int i = 0; i < key; i++) {
                for (int j = i; j < phrase.size(); j += key) {
                    decrypt[j] = phrase[itr++];
                }
            }

            phrase = decrypt;

        }

    }

    return phrase;

}