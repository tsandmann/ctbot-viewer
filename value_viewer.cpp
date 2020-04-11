#include <QQmlContext>

#include "value_viewer.h"


ValueViewer::ValueViewer(QQmlApplicationEngine* p_engine) : p_engine_ { p_engine } {
    model_.setList(&list_);
}

void ValueViewer::update_map() {
    for (int i {}; i < model_.rowCount(); ++i) {
        const auto& e { model_.data(model_.index(i, 0), ValueModel::Name) };
        map_[e.toString()] = model_.index(i, 0);
    }
}

void ValueViewer::register_model(const QString& modelname) {
    p_engine_->rootContext()->setContextProperty(modelname, &model_);
}
