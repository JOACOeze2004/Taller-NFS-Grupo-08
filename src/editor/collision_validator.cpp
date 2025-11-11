#include "collision_validator.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <yaml-cpp/yaml.h>

CollisionValidator::CollisionValidator() = default;

bool CollisionValidator::loadCollisionData(const QString& cityId) {
    clear();
    currentCity = cityId;

    QString fileName;
    if (cityId == "liberty_city") {
        fileName = "libertycity.yaml";
    } else if (cityId == "san_andreas") {
        fileName = "sanandreas.yaml";
    } else if (cityId == "vice_city") {
        fileName = "vicecity.yaml";
    } else {
        qWarning() << "Ciudad desconocida:" << cityId;
        return false;
    }

    QString filePath = QString("../src/server/%1").arg(fileName);

    if (!parseCollisionFile(filePath)) {
        qWarning() << "No se pudo cargar el archivo de colisiones para" << cityId;
        return false;
    }
    return true;
}

bool CollisionValidator::parseCollisionFile(const QString& filePath) {
    try {
        YAML::Node map = YAML::LoadFile(filePath.toStdString());

        if (!map["layers"]) {
            qWarning() << "No se encontró la clave 'layers' en el YAML";
            return false;
        }

        for (const auto& layer : map["layers"]) {
            if (!layer["type"] || layer["type"].as<std::string>() != "objectgroup") {
                continue;
            }
            if (!layer["objects"]) {
                continue;
            }

            for (const auto& obj : layer["objects"]) {
                try {
                    auto x = obj["x"].as<float>();
                    auto y = obj["y"].as<float>();
                    auto width = obj["width"].as<float>();
                    auto height = obj["height"].as<float>();

                    CollisionRect rect;
                    rect.rect = QRectF(x, y, width, height);
                    rect.type = "collision";
                    collisionRects.push_back(rect);

                } catch (const YAML::Exception& e) {
                    qWarning() << "Error parseando objeto:" << e.what();
                }
            }
        }

        return !collisionRects.empty();

    } catch (const YAML::Exception& e) {
        qWarning() << "Error de YAML:" << e.what();
        return false;
    } catch (const std::exception& e) {
        qWarning() << "Error general:" << e.what();
        return false;
    }
}

bool CollisionValidator::isPointValid(const QPointF& point) const {
    for (const auto& collision : collisionRects) {
        if (isPointInRect(point, collision.rect)) {
            return false;
        }
    }
    return true;
}

bool CollisionValidator::isPointInRect(const QPointF& point, const QRectF& rect) const {
    return rect.contains(point);
}

CollisionValidator::ValidationResult 
CollisionValidator::validateCheckpoints(const std::vector<QPointF>& checkpoints) const {
    ValidationResult result;
    result.valid = true;
    
    for (size_t i = 0; i < checkpoints.size(); ++i) {
        if (!isPointValid(checkpoints[i])) {
            result.valid = false;
            result.invalidPoint = checkpoints[i];
            result.errorMessage = QString("Checkpoint #%1 está en una zona inválida (colisión)")
                .arg(i + 1);
            return result;
        }
    }
    
    return result;
}

CollisionValidator::ValidationResult 
CollisionValidator::validateHints(const std::vector<QPointF>& hints) const {
    ValidationResult result;
    result.valid = true;
    
    for (size_t i = 0; i < hints.size(); ++i) {
        if (!isPointValid(hints[i])) {
            result.valid = false;
            result.invalidPoint = hints[i];
            result.errorMessage = QString("Hint #%1 está en una zona inválida (colisión)")
                .arg(i + 1);
            return result;
        }
    }
    
    return result;
}

void CollisionValidator::clear() {
    collisionRects.clear();
    currentCity.clear();
}