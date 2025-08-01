#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QString>

class FileListItemWidget : public QWidget {
    Q_OBJECT
public:
    FileListItemWidget(const QString& fileName, QWidget* parent = nullptr);

    QString fileName() const { return m_fileName; }
    void setChecked(bool checked);
    bool isChecked() const;

signals:
    void deleteRequested(const QString& fileName);
    void checkStateChanged(bool checked);

private:
    QString m_fileName;
    QLabel* m_label;
    QPushButton* m_deleteButton;
    QCheckBox* m_checkBox;
};