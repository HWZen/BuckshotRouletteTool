#include "aisettings.h"
#include <QMessageBox>
#include <QSslSocket>
#include <QDebug>

AISettings::AISettings(QWidget *parent)
    : QDialog(parent)
    , m_settings(new QSettings("BuckshotRouletteTool", "AI", this))
{
    setWindowTitle("AI决策建议设置");
    setModal(true);
    setMinimumSize(500, 400);
    
    setupUI();
    loadSettings();
    
    connect(m_okButton, &QPushButton::clicked, this, &AISettings::onAccepted);
    connect(m_cancelButton, &QPushButton::clicked, this, &AISettings::onRejected);
}

void AISettings::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // API设置组
    QGroupBox *apiGroup = new QGroupBox("API设置");
    QGridLayout *apiLayout = new QGridLayout(apiGroup);
    
    // API URL
    apiLayout->addWidget(new QLabel("API URL:"), 0, 0);
    m_apiUrlEdit = new QLineEdit;
    m_apiUrlEdit->setPlaceholderText("https://api.openai.com/v1/chat/completions");
    apiLayout->addWidget(m_apiUrlEdit, 0, 1);
    
    // API Key
    apiLayout->addWidget(new QLabel("API Key:"), 1, 0);
    m_apiKeyEdit = new QLineEdit;
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);  // 密码模式防窥屏
    m_apiKeyEdit->setPlaceholderText("输入您的API密钥");
    apiLayout->addWidget(m_apiKeyEdit, 1, 1);
    
    // Model设置
    apiLayout->addWidget(new QLabel("AI模型:"), 2, 0);
    m_modelEdit = new QLineEdit;
    m_modelEdit->setPlaceholderText("gpt-3.5-turbo");
    apiLayout->addWidget(m_modelEdit, 2, 1);
    
    mainLayout->addWidget(apiGroup);
    
    // 自定义策略组
    QGroupBox *promptGroup = new QGroupBox("自定义策略问题");
    QVBoxLayout *promptLayout = new QVBoxLayout(promptGroup);
    
    QLabel *promptLabel = new QLabel("在默认策略问题基础上，您可以添加额外的自定义策略要求:");
    promptLabel->setWordWrap(true);
    promptLayout->addWidget(promptLabel);
    
    m_customPromptEdit = new QTextEdit;
    m_customPromptEdit->setPlaceholderText("例如：请特别考虑保守策略，优先保证生存...");
    m_customPromptEdit->setMaximumHeight(120);
    promptLayout->addWidget(m_customPromptEdit);
    
    mainLayout->addWidget(promptGroup);
    
    // 说明信息
    QString infoText = "说明：\n"
                      "• API使用OpenAI格式，兼容大多数AI服务提供商\n"
                      "• API密钥将安全存储在本地，不会上传到任何服务器\n"
                      "• 自定义策略将与默认分析一起发送给AI\n";
    
    // 检查SSL支持状态
    if (!QSslSocket::supportsSsl()) {
        infoText += "• ⚠️ 当前系统不支持SSL，建议使用HTTP端点而非HTTPS";
    } else {
        infoText += "• ✅ SSL支持正常，可以使用HTTPS端点";
    }
    
    QLabel *infoLabel = new QLabel(infoText);
    infoLabel->setStyleSheet("QLabel { color: #666; font-size: 11px; }");
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);
    
    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    
    m_testButton = new QPushButton("测试连接");
    m_testButton->setStyleSheet("QPushButton { background-color: #17a2b8; color: white; }");
    buttonLayout->addWidget(m_testButton);
    
    buttonLayout->addStretch();
    
    m_cancelButton = new QPushButton("取消");
    m_okButton = new QPushButton("确定");
    m_okButton->setStyleSheet("QPushButton { background-color: #28a745; color: white; }");
    m_okButton->setDefault(true);
    
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_okButton);
    
    mainLayout->addLayout(buttonLayout);
}

QString AISettings::getApiUrl() const
{
    return m_apiUrlEdit->text().trimmed();
}

QString AISettings::getApiKey() const
{
    return m_apiKeyEdit->text().trimmed();
}

QString AISettings::getModel() const
{
    return m_modelEdit->text().trimmed();
}

QString AISettings::getCustomPrompt() const
{
    return m_customPromptEdit->toPlainText().trimmed();
}

void AISettings::setApiUrl(const QString &url)
{
    m_apiUrlEdit->setText(url);
}

void AISettings::setApiKey(const QString &key)
{
    m_apiKeyEdit->setText(key);
}

void AISettings::setModel(const QString &model)
{
    m_modelEdit->setText(model);
}

void AISettings::setCustomPrompt(const QString &prompt)
{
    m_customPromptEdit->setPlainText(prompt);
}

void AISettings::loadSettings()
{
    QString defaultUrl = "https://api.openai.com/v1/chat/completions";
    QString defaultModel = "gpt-3.5-turbo";
    m_apiUrlEdit->setText(m_settings->value("api_url", defaultUrl).toString());
    m_apiKeyEdit->setText(m_settings->value("api_key", "").toString());
    m_modelEdit->setText(m_settings->value("model", defaultModel).toString());
    m_customPromptEdit->setPlainText(m_settings->value("custom_prompt", "").toString());
}

void AISettings::saveSettings()
{
    m_settings->setValue("api_url", getApiUrl());
    m_settings->setValue("api_key", getApiKey());
    m_settings->setValue("model", getModel());
    m_settings->setValue("custom_prompt", getCustomPrompt());
    m_settings->sync();
}

void AISettings::onAccepted()
{
    // 验证必填字段
    if (getApiUrl().isEmpty()) {
        QMessageBox::warning(this, "设置错误", "请输入API URL");
        return;
    }
    
    if (getApiKey().isEmpty()) {
        QMessageBox::warning(this, "设置错误", "请输入API Key");
        return;
    }
    
    saveSettings();
    accept();
}

void AISettings::onRejected()
{
    reject();
}
