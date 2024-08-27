#include "borrower.h"
#include <iostream>

Borrower::Borrower(string name, string id) : name(name), id(id) {}

void Borrower::listBooks() {
    std::cout << "Books borrowed by " << name << " (" << id << "):" << std::endl;
    if (books_borrowed.empty()) {
        std::cout << "No books borrowed." << std::endl;
    } else {
        for (int i = 0; i < books_borrowed.size(); i++) {
            Book* book = books_borrowed[i];
            std::cout << i + 1 << ". " << book->title << " by " << book->author << std::endl;
        }
    }
}
void Borrower::removeBook(Book* book) { //function to remove a borrowed book
    for (int i = 0; i < books_borrowed.size(); i++) {
        if (books_borrowed[i] == book) {
            books_borrowed.erase(i);
            break;
        }
    }
}
