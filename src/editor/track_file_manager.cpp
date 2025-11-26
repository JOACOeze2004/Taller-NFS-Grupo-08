#include "track_file_manager.h"
#include "track_serializer.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <QDebug>

bool TrackFileManager::saveTrackWithDialog(QWidget* parent,
                                           const RaceTrackData& data,
                                           const QString& currentCity) {
    if (data.checkpoints.empty()) {
        QMessageBox::warning(parent, "Guardar Recorrido",
            "No hay checkpoints para guardar. Agrega al menos un checkpoint.");
        return false;
    }

    bool ok;
    QString trackName = QInputDialog::getText(parent, "Guardar Recorrido",
                                             "Nombre del recorrido:",
                                             QLineEdit::Normal,
                                             "", &ok);
    if (!ok || trackName.isEmpty()) {
        return false;
    }

    QString serverTracksDir = getServerTracksDirectory();
    QDir dir;
    if (!dir.exists(serverTracksDir)) {
        if (!dir.mkpath(serverTracksDir)) {
            QMessageBox::critical(parent, "Error",
                QString("No se pudo crear el directorio: %1").arg(serverTracksDir));
            return false;
        }
    }

    QString filePath = getDefaultSavePath(currentCity, trackName);

    if (QFile::exists(filePath)) {
        auto reply = QMessageBox::question(parent, "Archivo Existente",
            QString("El archivo '%1' ya existe.\n¿Deseas sobrescribirlo?")
                .arg(QFileInfo(filePath).fileName()),
            QMessageBox::Yes | QMessageBox::No);

        if (reply != QMessageBox::Yes) {
            return false;
        }
    }

    RaceTrackData completeData = data;
    completeData.trackName = trackName;
    completeData.author = qgetenv("USER");
    if (completeData.author.isEmpty()) {
        completeData.author = "Jugador";
    }
    completeData.cityId = currentCity;
    completeData.createdDate = QDateTime::currentDateTime().toString(Qt::ISODate);

    if (TrackSerializer::saveToFile(completeData, filePath)) {
        QMessageBox::information(parent, "Guardar Recorrido",
            QString("Recorrido '%1' guardado exitosamente!\n\n"
                    "Checkpoints: %2\n"
                    "Hints: %3\n"
                    "Archivo: %4")
                .arg(trackName)
                .arg(completeData.checkpoints.size())
                .arg(completeData.hints.size())
                .arg(filePath));
        return true;
    }

    QMessageBox::critical(parent, "Error", "No se pudo guardar el recorrido.");
    return false;
}

bool TrackFileManager::loadTrackWithDialog(QWidget* parent,
                                           RaceTrackData& data,
                                           QString& currentCity) {
    QString serverTracksDir = getServerTracksDirectory();

    QString filePath = QFileDialog::getOpenFileName(parent,
        "Cargar Recorrido",
        serverTracksDir,
        "Archivos de Recorrido (*.yaml *.yml)");

    if (filePath.isEmpty()) {
        return false;
    }

    if (!TrackSerializer::loadFromFile(filePath, data)) {
        QMessageBox::critical(parent, "Error", "No se pudo cargar el recorrido.");
        return false;
    }

    if (data.cityId != currentCity) {
        if (confirmCityChange(parent, data.cityId, currentCity)) {
            currentCity = data.cityId;
        }
    }

    QMessageBox::information(parent, "Recorrido Cargado",
        QString("Recorrido '%1' cargado exitosamente!\n\n"
                "Autor: %2\n"
                "Fecha: %3\n"
                "Checkpoints: %4\n"
                "Hints: %5")
            .arg(data.trackName)
            .arg(data.author)
            .arg(data.createdDate)
            .arg(data.checkpoints.size())
            .arg(data.hints.size()));

    return true;
}

QString TrackFileManager::getServerTracksDirectory() {
    QStringList possiblePaths = {
        "../src/server/tracks",
        "../../src/server/tracks",
        "../../../src/server/tracks",
        "src/server/tracks"
    };

    for (const QString& path : possiblePaths) {
        QDir dir(path);
        if (dir.exists()) {
            return dir.absolutePath();
        }
    }
    return "../src/server/tracks";
}

QString TrackFileManager::getDefaultSavePath(const QString& cityId, const QString& trackName) {
    QString serverTracksDir = getServerTracksDirectory();
    QString sanitizedName = QString(trackName).replace(" ", "_");
    return QString("%1/%2_%3.yaml")
        .arg(serverTracksDir)
        .arg(cityId)
        .arg(sanitizedName);
}

bool TrackFileManager::confirmCityChange(QWidget* parent,
                                         const QString& trackCity,
                                         const QString& currentCity) {
    auto reply = QMessageBox::question(parent, "Ciudad Diferente",
        QString("Este recorrido es para '%1' pero estás editando '%2'.\n"
                "¿Cambiar al mapa correcto?")
            .arg(trackCity)
            .arg(currentCity),
        QMessageBox::Yes | QMessageBox::No);

    return reply == QMessageBox::Yes;
}