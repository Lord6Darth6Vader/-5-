#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>
#include <string>
#include <utility>

using namespace std;

const int MAX_BOOKS = 100;
const int MAX_READERS = 100;

// Класс книг
class Book {
private:
    int id;
    string title;
    string author;
    int year;
    int borrowerId = 0;

public:
    Book(int startId, const string& startTitle,
         const string& startAuthor, int startYear)
        : id(startId),
          title(startTitle),
          author(startAuthor),
          year(startYear) {}

    int getId() const {
        return id;
    }

    const string& getTitle() const {
        return title;
    }

    const string& getAuthor() const {
        return author;
    }

    int getYear() const {
        return year;
    }

    int getBorrowerId() const {
        return borrowerId;
    }

    bool isBorrowedBy(int readerId) const {
        return readerId > 0 && borrowerId == readerId;
    }

    bool isAvailable() const {
        return borrowerId == 0;
    }

    bool issueToReader(int readerId) {
        if (!isAvailable() || readerId <= 0) {
            return false;
        }

        borrowerId = readerId;
        return true;
    }

    bool returnBook() {
        if (isAvailable()) {
            return false;
        }

        borrowerId = 0;
        return true;
    }

    void updateDetails(const string& newTitle,
                       const string& newAuthor,
                       int newYear) {
        title = newTitle;
        author = newAuthor;
        year = newYear;
    }
};


// Класс читателей
class Reader {
private:
    int id;
    string name;

public:
    Reader(int startId, const string& startName)
        : id(startId), name(startName) {}

    int getId() const {
        return id;
    }

    const string& getName() const {
        return name;
    }

    void setName(const string& newName) {
        name = newName;
    }
};


// Класс библиотеки
class Library {
private:
    unique_ptr<Book> books[MAX_BOOKS];
    unique_ptr<Reader> readers[MAX_READERS];

    int bookCount = 0;
    int readerCount = 0;

public:
    bool addBook(int id, const string& title,
                 const string& author, int year) {
        if (id <= 0 || year <= 0 ||
            title.empty() || author.empty() ||
            bookCount >= MAX_BOOKS) {
            return false;
        }

        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getId() == id) {
                return false;
            }
        }

        books[bookCount] =
            make_unique<Book>(id, title, author, year);

        bookCount++;
        return true;
    }

    bool addReader(int id, const string& name) {
        if (id <= 0 || name.empty() ||
            readerCount >= MAX_READERS) {
            return false;
        }

        for (int i = 0; i < readerCount; i++) {
            if (readers[i]->getId() == id) {
                return false;
            }
        }

        readers[readerCount] =
            make_unique<Reader>(id, name);

        readerCount++;
        return true;
    }

    Book* findBookById(int id) {
        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getId() == id) {
                return books[i].get();
            }
        }

        return nullptr;
    }

    Reader* findReaderById(int id) {
        for (int i = 0; i < readerCount; i++) {
            if (readers[i]->getId() == id) {
                return readers[i].get();
            }
        }

        return nullptr;
    }

    bool editBookById(int id, const string& title,
                      const string& author, int year) {
        Book* book = findBookById(id);

        if (book == nullptr || title.empty() ||
            author.empty() || year <= 0) {
            return false;
        }

        book->updateDetails(title, author, year);
        return true;
    }

    bool editReaderById(int id, const string& name) {
        Reader* reader = findReaderById(id);

        if (reader == nullptr || name.empty()) {
            return false;
        }

        reader->setName(name);
        return true;
    }

    bool issueBookToReader(int bookId, int readerId) {
        Book* book = findBookById(bookId);
        Reader* reader = findReaderById(readerId);

        if (book == nullptr || reader == nullptr) {
            return false;
        }

        return book->issueToReader(readerId);
    }

    bool returnBookById(int bookId) {
        Book* book = findBookById(bookId);

        if (book == nullptr) {
            return false;
        }

        return book->returnBook();
    }

    bool removeBookById(int id) {
        int index = -1;

        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getId() == id) {
                index = i;
                break;
            }
        }

        if (index == -1 || !books[index]->isAvailable()) {
            return false;
        }

        for (int i = index; i < bookCount - 1; i++) {
            books[i] = std::move(books[i + 1]);
        }

        books[bookCount - 1].reset();
        bookCount--;

        return true;
    }

    bool removeReaderById(int id) {
        int index = -1;

        for (int i = 0; i < readerCount; i++) {
            if (readers[i]->getId() == id) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            return false;
        }

        for (int i = 0; i < bookCount; i++) {
            if (books[i]->isBorrowedBy(id)) {
                return false;
            }
        }

        for (int i = index; i < readerCount - 1; i++) {
            readers[i] = std::move(readers[i + 1]);
        }

        readers[readerCount - 1].reset();
        readerCount--;

        return true;
    }

    void listBooks() const {
        if (bookCount == 0) {
            cout << "Библиотека пока не содержит книг.\n";
            return;
        }

        cout << "\n========== СПИСОК КНИГ ==========\n";

        for (int i = 0; i < bookCount; i++) {
            cout << "ID: " << books[i]->getId()
                 << "\nНазвание: " << books[i]->getTitle()
                 << "\nАвтор: " << books[i]->getAuthor()
                 << "\nГод: " << books[i]->getYear()
                 << "\nСтатус: "
                 << (books[i]->isAvailable()
                     ? "Доступна"
                     : "Выдана")
                 << '\n';

            if (!books[i]->isAvailable()) {
                cout << "ID читателя: "
                     << books[i]->getBorrowerId() << '\n';
            }

            cout << "---------------------------------\n";
        }
    }

    void listReaders() const {
        if (readerCount == 0) {
            cout << "Зарегистрированных читателей нет.\n";
            return;
        }

        cout << "\n======= СПИСОК ЧИТАТЕЛЕЙ =======\n";

        for (int i = 0; i < readerCount; i++) {
            cout << "ID: " << readers[i]->getId()
                 << "\nИмя: " << readers[i]->getName()
                 << "\n---------------------------------\n";
        }
    }

    void listBooksByReader(int readerId) const {
        bool readerExists = false;

        for (int i = 0; i < readerCount; i++) {
            if (readers[i]->getId() == readerId) {
                readerExists = true;
                cout << "\nКниги читателя "
                     << readers[i]->getName() << ":\n";
                break;
            }
        }

        if (!readerExists) {
            cout << "Читатель не найден.\n";
            return;
        }

        bool found = false;

        for (int i = 0; i < bookCount; i++) {
            if (books[i]->isBorrowedBy(readerId)) {
                cout << "ID: " << books[i]->getId()
                     << " | " << books[i]->getTitle()
                     << " | " << books[i]->getAuthor()
                     << '\n';

                found = true;
            }
        }

        if (!found) {
            cout << "У читателя нет выданных книг.\n";
        }
    }

    void showStatistics() const {
        int available = 0;
        int borrowed = 0;

        for (int i = 0; i < bookCount; i++) {
            if (books[i]->isAvailable()) {
                available++;
            } else {
                borrowed++;
            }
        }

        cout << "\n========== СТАТИСТИКА ==========\n"
             << "Всего книг: " << bookCount << '\n'
             << "Доступно книг: " << available << '\n'
             << "Выдано книг: " << borrowed << '\n'
             << "Зарегистрировано читателей: "
             << readerCount << '\n'
             << "Свободных мест для книг: "
             << MAX_BOOKS - bookCount << '\n'
             << "Свободных мест для читателей: "
             << MAX_READERS - readerCount << '\n';
    }
};


// Штуки для ввода
int readInt(const string& prompt) {
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        if (cin.eof()) {
            cout << "\nВвод завершён.\n";
            exit(0);
        }

        cout << "Ошибка: введите целое число.\n";

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readText(const string& prompt) {
    string value;

    while (true) {
        cout << prompt;
        getline(cin, value);

        if (cin.eof()) {
            cout << "\nВвод завершён.\n";
            exit(0);
        }

        if (value.find_first_not_of(" \t\r\n") != string::npos) {
            return value;
        }

        cout << "Строка не должна быть пустой.\n";
    }
}


// Функции main
int main() {
    setlocale(LC_ALL, "");

    Library library;

    // Начальные данные
    library.addBook(1, "Преступление и наказание",
                    "Ф. М. Достоевский", 1866);
    library.addBook(2, "Мастер и Маргарита",
                    "М. А. Булгаков", 1967);
    library.addBook(3, "1984", "Джордж Оруэлл", 1949);

    library.addReader(1, "Иван Иванов");
    library.addReader(2, "Анна Петрова");

    int choice;

    do {
        cout << "\n========== БИБЛИОТЕКА ==========\n"
             << "1. Показать все книги\n"
             << "2. Показать всех читателей\n"
             << "3. Добавить книгу\n"
             << "4. Зарегистрировать читателя\n"
             << "5. Редактировать книгу\n"
             << "6. Редактировать читателя\n"
             << "7. Удалить книгу\n"
             << "8. Удалить читателя\n"
             << "9. Выдать книгу читателю\n"
             << "10. Вернуть книгу\n"
             << "11. Книги конкретного читателя\n"
             << "12. Статистика библиотеки\n"
             << "0. Выход\n"
             << "================================\n";

        choice = readInt("Выберите действие: ");

        switch (choice) {
        case 1:
            library.listBooks();
            break;

        case 2:
            library.listReaders();
            break;

        case 3: {
            int id = readInt("ID книги: ");
            string title = readText("Название: ");
            string author = readText("Автор: ");
            int year = readInt("Год издания: ");

            if (library.addBook(id, title, author, year)) {
                cout << "Книга успешно добавлена.\n";
            } else {
                cout << "Не удалось добавить книгу. "
                     << "Проверьте ID, год и лимит записей.\n";
            }
            break;
        }

        case 4: {
            int id = readInt("ID читателя: ");
            string name = readText("Имя читателя: ");

            if (library.addReader(id, name)) {
                cout << "Читатель зарегистрирован.\n";
            } else {
                cout << "Не удалось добавить читателя. "
                     << "Проверьте ID и лимит записей.\n";
            }
            break;
        }

        case 5: {
            int id = readInt("ID редактируемой книги: ");
            string title = readText("Новое название: ");
            string author = readText("Новый автор: ");
            int year = readInt("Новый год издания: ");

            if (library.editBookById(id, title, author, year)) {
                cout << "Данные книги обновлены.\n";
            } else {
                cout << "Не удалось обновить книгу.\n";
            }
            break;
        }

        case 6: {
            int id = readInt("ID читателя: ");
            string name = readText("Новое имя: ");

            if (library.editReaderById(id, name)) {
                cout << "Данные читателя обновлены.\n";
            } else {
                cout << "Не удалось обновить читателя.\n";
            }
            break;
        }

        case 7: {
            int id = readInt("ID удаляемой книги: ");

            if (library.removeBookById(id)) {
                cout << "Книга удалена.\n";
            } else {
                cout << "Не удалось удалить книгу. "
                     << "Возможно, она не найдена или выдана.\n";
            }
            break;
        }

        case 8: {
            int id = readInt("ID удаляемого читателя: ");

            if (library.removeReaderById(id)) {
                cout << "Читатель удалён.\n";
            } else {
                cout << "Не удалось удалить читателя. "
                     << "Проверьте ID и выданные ему книги.\n";
            }
            break;
        }

        case 9: {
            int bookId = readInt("ID книги: ");
            int readerId = readInt("ID читателя: ");

            if (library.issueBookToReader(bookId, readerId)) {
                cout << "Книга успешно выдана.\n";
            } else {
                cout << "Выдача невозможна. "
                     << "Проверьте ID и доступность книги.\n";
            }
            break;
        }

        case 10: {
            int bookId = readInt("ID возвращаемой книги: ");

            if (library.returnBookById(bookId)) {
                cout << "Книга возвращена.\n";
            } else {
                cout << "Не удалось вернуть книгу. "
                     << "Проверьте ID и статус книги.\n";
            }
            break;
        }

        case 11: {
            int readerId = readInt("ID читателя: ");
            library.listBooksByReader(readerId);
            break;
        }

        case 12:
            library.showStatistics();
            break;

        case 0:
            cout << "Завершение работы программы.\n";
            break;

        default:
            cout << "Неизвестная команда. Выберите пункт меню.\n";
            break;
        }

    } while (choice != 0);

    return 0;
}
