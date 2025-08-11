#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSettings>

class AISettings : public QDialog {
    Q_OBJECT

public:
    explicit AISettings(QWidget *parent = nullptr);
    
    QString getApiUrl() const;
    QString getApiKey() const;
    QString getModel() const;
    QString getCustomPrompt() const;
    
    void setApiUrl(const QString &url);
    void setApiKey(const QString &key);
    void setModel(const QString &model);
    void setCustomPrompt(const QString &prompt);
    
    void loadSettings();
    void saveSettings();

private slots:
    void onAccepted();
    void onRejected();

private:
    void setupUI();
    
    QLineEdit *m_apiUrlEdit;
    QLineEdit *m_apiKeyEdit;
    QLineEdit *m_modelEdit;
    QTextEdit *m_customPromptEdit;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_testButton;
    
    QSettings *m_settings;
};
