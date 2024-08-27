#include "book.h"
#include <iostream>

Book::Book(std::string title, std::string author, std::string isbn, int publication_year,
           int total_copies, int available_copies)
    : title(title), author(author), isbn(isbn), publication_year(publication_year),
      total_copies(total_copies), available_copies(available_copies)
{
}

void Book::display()
{
    std::cout << "Title: " << title << std::endl;
    std::cout << "Author: " << author << std::endl;
    std::cout << "ISBN: " << isbn << std::endl;
    std::cout << "Publication Year: " << publication_year << std::endl;
    std::cout << "Total Copies: " << total_copies << std::endl;
    std::cout << "Available Copies: " << available_copies << std::endl;
}

// Functions to manage borrowers
void Book::addCurrentBorrower(Borrower* borrower)
{
    currentBorrowers.push_back(borrower);
}

void Book::addToBorrowerHistory(Borrower* borrower)
{
    allBorrowers.push_back(borrower);
}

void Book::removeCurrentBorrower(Borrower* borrower)
{
    for (int i = 0; i < currentBorrowers.size(); i++)
    {
        if (currentBorrowers[i] == borrower)
        {
            currentBorrowers.erase(i);
            break;
        }
    }
}