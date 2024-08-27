#include "lcms.h"
#include <iostream>
#include <fstream>
#include <sstream>

LCMS::LCMS(string name) {
    libTree = new Tree(name);  // Create a new library tree with the given name
}

LCMS::~LCMS() {
    delete libTree;  // Delete the library tree
    for (int i = 0; i < borrowers.size(); i++) {
        delete borrowers[i];  // Delete each borrower object
    }
}

int LCMS::import(string path) {
    std::ifstream file(path);  
    if (!file) {
        std::cout << "Failed to open the file." << std::endl;
        return 0;
    }

    std::string line;
    std::getline(file, line);  //skip the header line

    int count = 0;
    while (std::getline(file, line)) {  //read each line from the file
        std::istringstream iss(line);
        std::string field;
        MyVector<string> fields;

        while (iss.good()) {  //Parse fields separated by commas, handling commas within double quotes 
            std::getline(iss, field, ',');
            if (field.front() == '"') {
                while (field.back() != '"' && iss.good()) { //If the field starts with a double quote, this loop continues reading the next fields until the closing double quote is found or the end of the line is reached. 
                    std::string next_field;
                    std::getline(iss, next_field, ',');
                    field += ',' + next_field;
                }
                field = field.substr(1, field.length() - 2);  //remove the surrounding double quotes
            }
            fields.push_back(field);  //add the field to the vector
        }

        if (fields.size() >= 7) {
            std::string title = fields[0];
            std::string author = fields[1];
            std::string isbn = fields[2];
            int publicationYear = std::stoi(fields[3]);  //convert to int
            std::string category = fields[4];
            int totalCopies = std::stoi(fields[5]);  //convert to int
            int availableCopies = std::stoi(fields[6]);  //convert to int

            Node* node = libTree->getRoot();
            Book* existingBook = libTree->findBook(node, title); //to check if book already exists in library

            if (existingBook == nullptr) {
                Book* book = new Book(title, author, isbn, publicationYear, totalCopies, availableCopies); //create the book
                Node* node = libTree->createNode(category);
                node->books.push_back(book);
                libTree->updateBookCount(node, 1); //update
                count++; 
             } 
                //else {
            //     std::cout << "Book with title \"" << title << "\" already exists." << std::endl;
            // }
        }
    }

    file.close();  
    std::cout << count << " records have been imported." << std::endl;

    return count;
}

void LCMS::exportData(string path) {
    std::ofstream file(path);  
    if (!file) {
        std::cout << "Failed to create the file." << std::endl;
        return;
    }

    file << "Title,Author,ISBN,Publication Year,Category,Total Copies,Available Copies" << std::endl;  //qrite the header line
    int count = exportDataHelper(libTree->getRoot(), file);  //Recursively export data starting from the root node
    file.close();  // Close the file

    std::cout << "Exported " << count << " books to " << path << std::endl;
}

int LCMS::exportDataHelper(Node* node, std::ofstream& file) {
    int count = 0;
    if (node == nullptr) {
        return count;
    }

    for (int i = 0; i < node->books.size(); i++) {  //iterate over the books in the current node
        Book* book = node->books[i];

        file << "\"" << book->title << "\",";  //rite the book details to the file
        file << "\"" << book->author << "\","; //this ensures that if any of these values contain commas, the commas are taken as part of the value rather than as field separators.
        file << "\"" << book->isbn << "\",";
        file << book->publication_year << ",";
        file << "\"" << node->getCategory(node) << "\",";
        file << book->total_copies << ",";
        file << book->available_copies;
        file << std::endl;
        count++;  //increment the count of exported books
    }

    for (int i = 0; i < node->children.size(); i++) {  //recursively export data for each child node
        count += exportDataHelper(node->children[i], file);
    }

    return count;
}


void LCMS::findAll(string category) {
    Node* node = libTree->getNode(category);  //find the node corresponding to the specified category
    if (node == nullptr) {
        std::cout << "Category not found." << std::endl;
        return;
    }

    std::cout << "Books in category: " << category << std::endl;
    libTree->printAll(node);  //print all books in the category
}

void LCMS::findBook(string bookTitle) {
    Node* node = libTree->getRoot();  //start searching from the root node
    Book* book = libTree->findBook(node, bookTitle);  //Find the book with the specified title
    if (book == nullptr) {
        std::cout << "Book not found." << std::endl;
        return;
    }

    std::cout << "Book details:" << std::endl;
    book->display();  // display details of the book
}

void LCMS::addBook() {
    std::string title, author, isbn, category;
    int publicationYear, totalCopies, availableCopies;

    std::cout << "Enter book details:" << std::endl;
    std::cout << "Title: ";
    std::getline(std::cin, title);  
    std::cout << "Author: ";
    std::getline(std::cin, author);  
    std::cout << "ISBN: ";
    std::getline(std::cin, isbn);  
    std::cout << "Publication Year: ";
    std::cin >> publicationYear;  
    std::cin.ignore(); //to discard any remaining characters
    std::cout << "Category: ";
    std::getline(std::cin, category);  
    std::cout << "Total Copies: ";
    std::cin >> totalCopies;  
    std::cout << "Available Copies: ";
    std::cin >> availableCopies;  

    Node* node = libTree->getRoot();
    Book* existingBook = libTree->findBook(node, title); //check if book already exists in library

    if (existingBook == nullptr) {
        Book* book = new Book(title, author, isbn, publicationYear, totalCopies, availableCopies);  
        Node* node = libTree->getNode(category);  
        if (node == nullptr) {
            node = libTree->createNode(category);  //Create a new node for the category if it doesn't exist
        }
        node->books.push_back(book);  //Add the book to the node
        libTree->updateBookCount(node, 1);  //Update the book count in the library tree

        std::cout << "Book added successfully." << std::endl;
    } else {
        std::cout << "Book with title \"" << title << "\" already exists. Book not added." << std::endl;
    }

}

void LCMS::editBook(string bookTitle) {
    Node* node = libTree->getRoot();  
    Book* book = libTree->findBook(node, bookTitle);  //Find the book with the specified title
    if (book == nullptr) {
        std::cout << "Book not found." << std::endl;
        return;
    }

    int choice;
    do {
        std::cout << "Book details:" << std::endl;
        std::cout << "1. Title: " << book->title << std::endl;
        std::cout << "2. Author: " << book->author << std::endl;
        std::cout << "3. ISBN: " << book->isbn << std::endl;
        std::cout << "4. Publication Year: " << book->publication_year << std::endl;
        std::cout << "5. Total Copies: " << book->total_copies << std::endl;
        std::cout << "6. Available Copies: " << book->available_copies << std::endl;
        std::cout << "7. Exit" << std::endl;

        std::cout << "Enter the number of the field you want to edit: ";
        std::cin >> choice;  //Read the user's choice
        std::cin.ignore();

        switch (choice) {
            case 1: {
                std::string title;
                std::cout << "Enter the new title: ";
                std::getline(std::cin, title);  // Read new title from user input
                book->title = title;  // Update
                break;
            }
            case 2: {
                std::string author;
                std::cout << "Enter the new author: ";
                std::getline(std::cin, author);  // Read new author from user input
                book->author = author;  // Update 
                break;
            }
            case 3: {
                std::string isbn;
                std::cout << "Enter the new ISBN: ";
                std::getline(std::cin, isbn);  // Read new ISBN from user input
                book->isbn = isbn;  // Update 
                break;
            }
            case 4: {
                int publicationYear;
                std::cout << "Enter the new publication year: ";
                std::cin >> publicationYear;  // Read  new publication year from user input
                std::cin.ignore();
                book->publication_year = publicationYear;  // Update 
                break;
            }
            case 5: {
                int totalCopies;
                std::cout << "Enter the new total copies: ";
                std::cin >> totalCopies;  // Read new total copies from user input
                std::cin.ignore();
                book->total_copies = totalCopies;  // Update 
                break;
            }
            case 6: {
                int availableCopies;
                std::cout << "Enter the new available copies: ";
                std::cin >> availableCopies;  // Read new available copies from user input
                std::cin.ignore();
                book->available_copies = availableCopies;  // Update 
                break;
            }
            case 7:
                std::cout << "Exiting edit mode." << std::endl;
                break;
            default:
                std::cout << "Invalid choice." << std::endl;
        }

        std::cout << std::endl;
    } while (choice != 7);

    std::cout << "Book edited successfully." << std::endl;
}

void LCMS::borrowBook(string bookTitle) {
    Node* node = libTree->getRoot();  
    Book* book = libTree->findBook(node, bookTitle);  
    if (book == nullptr) {
        std::cout << "Book not found." << std::endl;
        return;
    }

    if (book->available_copies == 0) {
        std::cout << "No copies available for borrowing." << std::endl;
        return;
    }

    std::string borrowerName, borrowerId;
    std::cout << "Enter borrower name: ";
    std::getline(std::cin, borrowerName);  //read name 
    std::cout << "Enter borrower ID: ";
    std::getline(std::cin, borrowerId);  //read ID 

    Borrower* borrower = nullptr;
    for (int i = 0; i < borrowers.size(); i++) {
        if (borrowers[i]->name == borrowerName && borrowers[i]->id == borrowerId) {
            borrower = borrowers[i];  //find borrower with the specified name and ID
            break;
        }
    }

    if (borrower == nullptr) {
        borrower = new Borrower(borrowerName, borrowerId);  //create a new borrower if not found
        borrowers.push_back(borrower);  //add  new borrower to the list of borrowers
    }

    book->addCurrentBorrower(borrower);  //add to the book's current borrowers
    book->addToBorrowerHistory(borrower);  // Add to the book's borrower history
    book->available_copies--;  //Decrement available copies of the book
    borrower->books_borrowed.push_back(book);  // Add book to the borrower's borrowed books

    std::cout << "Book borrowed successfully." << std::endl;
}

void LCMS::returnBook(string bookTitle) {
    Node* node = libTree->getRoot();
    Book* book = libTree->findBook(node, bookTitle);
    if (book == nullptr) {
        std::cout << "Book not found." << std::endl;
        return;
    }

    std::string borrowerName, borrowerId;  //same as beforee
    std::cout << "Enter borrower name: ";
    std::getline(std::cin, borrowerName);
    std::cout << "Enter borrower ID: ";
    std::getline(std::cin, borrowerId);

    Borrower* borrower = nullptr;
    for (int i = 0; i < borrowers.size(); i++) {
        if (borrowers[i]->name == borrowerName && borrowers[i]->id == borrowerId) {
            borrower = borrowers[i];
            break;
        }
    }

    if (borrower == nullptr) {
        std::cout << "Borrower not found." << std::endl;
        return;
    }

    book->removeCurrentBorrower(borrower); //Remove the borrower from the book's current borrowers
    book->available_copies++;  //Increment the available copies of the book
    borrower->removeBook(book);  //Remove the book from the borrower's borrowed books

    std::cout << "Book returned successfully." << std::endl;
}

void LCMS::listCurrentBorrowers(string bookTitle) {
    Node* node = libTree->getRoot();  
    Book* book = libTree->findBook(node, bookTitle); 
    if (book == nullptr) {
        std::cout << "Book not found." << std::endl;
        return;
    }

    std::cout << "Current borrowers of the book:" << std::endl;
    for (int i = 0; i < book->currentBorrowers.size(); i++) {
        std::cout << book->currentBorrowers[i]->name << " (" << book->currentBorrowers[i]->id << ")" << std::endl;  // Print the current borrowers' names and IDs
    }
}

void LCMS::listAllBorrowers(string bookTitle) {
    Node* node = libTree->getRoot();  
    Book* book = libTree->findBook(node, bookTitle);  
    if (book == nullptr) {
        std::cout << "Book not found." << std::endl;
        return;
    }

    std::cout << "All borrowers of the book:" << std::endl;
    for (int i = 0; i < book->allBorrowers.size(); i++) {
        std::cout << book->allBorrowers[i]->name << " (" << book->allBorrowers[i]->id << ")" << std::endl;  // Print all borrowers' names and IDs
    }
}

void LCMS::listBooks(string borrower_name_id) {
    std::istringstream iss(borrower_name_id);
    std::string borrowerName, borrowerId;
    std::getline(iss, borrowerName, ',');  //Extract borrower's name 
    std::getline(iss, borrowerId);  // Extract borrower's ID 

    Borrower* borrower = nullptr;
    for (int i = 0; i < borrowers.size(); i++) {
        if (borrowers[i]->name == borrowerName && borrowers[i]->id == borrowerId) {
            borrower = borrowers[i];  
            break;
        }
    }

    if (borrower == nullptr) {
        std::cout << "Borrower not found." << std::endl;
        return;
    }

    borrower->listBooks();  //Call the borrower's listBooks function to display their borrowed books
}

void LCMS::removeBook(string bookTitle) {
    Node* node = libTree->getRoot();  
    Book* book = libTree->findBook(node, bookTitle);  
    if (book == nullptr) {
        std::cout << "Book not found." << std::endl;
        return;
    }

    char confirmation;
    std::cout << "Are you sure you want to remove the book " << bookTitle << "? (y/n): ";
    std::cin >> confirmation;  //reed the user's confirmation
    std::cin.ignore();

    if (confirmation == 'y' || confirmation == 'Y') {
        if (libTree->removeBook(node, bookTitle)) {  //Remove the book from the library tree
            std::cout << "Book removed successfully." << std::endl;
        } else {
            std::cout << "Failed to remove the book." << std::endl;
        }
    } else {
        std::cout << "Book removal canceled." << std::endl;
    }
}

void LCMS::addCategory(string category) {
    libTree->createNode(category);  //create a new node for the category in the library tree
    std::cout << "Category added successfully." << std::endl;
}

void LCMS::findCategory(string category) {
    Node* node = libTree->getNode(category);  //Find the node corresponding to the specified category
    if (node == nullptr) {
        std::cout << "Category not found." << std::endl;
        return;
    }

    std::cout << category << " was found in the catalog." << std::endl;
}

void LCMS::removeCategory(string category) {
    Node* node = libTree->getNode(category);
    if (node == nullptr) {
        std::cout << "Category not found." << std::endl;
        return;
    }

    // Update the book count of parent nodes
    int bookCountDiff = node->bookCount;
    Node* parentNode = node->parent;
    while (parentNode != nullptr) {
        parentNode->bookCount -= bookCountDiff;
        parentNode = parentNode->parent;
    }

    Node* parent = node->parent; // Get the parent node of the category node
    if (parent != nullptr) {
        for (int i = 0; i < parent->children.size(); i++) {
            if (parent->children[i] == node) {
                parent->children.erase(i); //Remove the category node from its parent's children
                break;
            }
        }
    } else {
        delete libTree; //If the category node has no parent, delete the entire library tree
        libTree = nullptr;
    }

    delete node; //Delete the category node
    std::cout << "Category removed successfully." << std::endl;
}

void LCMS::editCategory(string category) {
    Node* node = libTree->getNode(category);  
    if (node == nullptr) {
        std::cout << "Category not found." << std::endl;
        return;
    }

    std::string newCategory;
    std::cout << "Enter new category name: ";
    std::getline(std::cin, newCategory);  // Read new category name 

    node->name = newCategory;  //pdate the category name
    std::cout << "Category edited successfully." << std::endl;
}