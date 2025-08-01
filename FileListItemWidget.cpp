#include "FileListItemWidget.h"
#include <QHBoxLayout>

FileListItemWidget::FileListItemWidget(const QString& fileName, QWidget* parent)
    : QWidget(parent), m_fileName(fileName)
{
    m_checkBox = new QCheckBox(this);
    m_label = new QLabel(fileName, this);
    m_deleteButton = new QPushButton("✖", this);
    m_deleteButton->setFixedWidth(24);
    m_deleteButton->setToolTip("Delete this item");

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(m_checkBox);
    layout->addWidget(m_label);
    layout->addStretch();
    layout->addWidget(m_deleteButton);
    m_checkBox->setMinimumSize(18, 15);

    connect(m_deleteButton, &QPushButton::clicked, this, [this]() {
        emit deleteRequested(m_fileName);
    });
    connect(m_checkBox, &QCheckBox::toggled, this, &FileListItemWidget::checkStateChanged);


}

void FileListItemWidget::setChecked(bool checked) {
    m_checkBox->setChecked(checked);
}

bool FileListItemWidget::isChecked() const {
    return m_checkBox->isChecked();
}