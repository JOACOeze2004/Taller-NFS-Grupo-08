#include <QApplication>
#include "city_selec.h"
#include "editor_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CitySelection dialog;

    if (dialog.exec() == QDialog::Accepted) {
        QString selectedCity = dialog.getSelectedCity();

        if (!selectedCity.isEmpty()) {
            EditorWindow window(selectedCity);
            window.show();

            return app.exec();
        }
    }

    return 0;
}