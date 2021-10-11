#include <QInputDialog>
#include <QApplication>
#include <iostream>

int main(int ac, char **av)
{
    QApplication app(ac, av);
    bool ok;
    QInputDialog::getText(nullptr, "sudo", "PASSW", QLineEdit::Password, "lala", &ok);
    if (!ok)
        std::cout << "muy grave" << std::endl;
    return app.exec();
}