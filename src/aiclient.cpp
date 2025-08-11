#include "aiclient.h"
#include <QNetworkRequest>
#include <QJsonArray>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QDebug>

AIClient::AIClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
    , m_timeoutTimer(new QTimer(this))
    , m_model("gpt-3.5-turbo")  // 初始化默认模型
{
    
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &AIClient::onRequestTimeout);
    
    // 检查SSL支持
    bool sslSupported = QSslSocket::supportsSsl();
    qDebug() << "SSL Support:" << sslSupported;
    qDebug() << "SSL Library Build Version:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "SSL Library Runtime Version:" << QSslSocket::sslLibraryVersionString();
    
    if (!sslSupported) {
        qDebug() << "Warning: SSL not supported. HTTPS requests may fail.";
        qDebug() << "Consider using HTTP endpoints or install SSL libraries.";
    } else {
        // 只有在SSL支持时才配置SSL
        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
        sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
        QSslConfiguration::setDefaultConfiguration(sslConfig);
    }
}

void AIClient::setApiUrl(const QString &url)
{
    m_apiUrl = url;
}

void AIClient::setApiKey(const QString &key)
{
    m_apiKey = key;
}

void AIClient::setModel(const QString &model)
{
    QString oldModel = m_model;
    m_model = model.isEmpty() ? "gpt-3.5-turbo" : model;
    qDebug() << "=== AIClient Model Changed ===";
    qDebug() << "Old Model:" << oldModel;
    qDebug() << "New Model:" << m_model;
    qDebug() << "Input Model:" << model;
}

void AIClient::sendRequest(const QString &systemPrompt, const QString &userPrompt)
{
    if (m_currentReply) {
        cancelRequest();
    }
    
    qDebug() << "=== AI Client Request Start ===";
    qDebug() << "API URL:" << m_apiUrl;
    qDebug() << "API Key:" << (m_apiKey.isEmpty() ? "Empty" : QString("***...%1").arg(m_apiKey.right(4)));
    
    QUrl url(m_apiUrl);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());
    
    // 只有在SSL支持且是HTTPS时才配置SSL设置
    if (QSslSocket::supportsSsl() && url.scheme().toLower() == "https") {
        QSslConfiguration sslConfig = request.sslConfiguration();
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
        sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
        request.setSslConfiguration(sslConfig);
        qDebug() << "SSL configuration applied for HTTPS request";
    } else if (url.scheme().toLower() == "https") {
        qDebug() << "Warning: HTTPS requested but SSL not supported";
    }
    
    QString requestBody = buildRequestBody(systemPrompt, userPrompt);
    
    qDebug() << "Request Body Length:" << requestBody.length();
    qDebug() << "System Prompt:" << systemPrompt;
    qDebug() << "User Prompt:" << userPrompt;
    qDebug() << "Full Request Body:" << requestBody;
    
    emit requestStarted();
    m_currentReply = m_networkManager->post(request, requestBody.toUtf8());
    
    connect(m_currentReply, &QNetworkReply::finished, this, &AIClient::onReplyFinished);
    
    // 只在SSL支持时连接SSL错误信号
    if (QSslSocket::supportsSsl()) {
        connect(m_currentReply, QOverload<const QList<QSslError> &>::of(&QNetworkReply::sslErrors),
                this, &AIClient::onSslErrors);
    }
    
    // 启动超时计时器
    m_timeoutTimer->start(REQUEST_TIMEOUT_MS);
    qDebug() << "Request sent, waiting for response...";
}

void AIClient::cancelRequest()
{
    if (m_currentReply) {
        m_timeoutTimer->stop();
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        emit requestFinished();
    }
}

void AIClient::onReplyFinished()
{
    m_timeoutTimer->stop();
    
    if (!m_currentReply) {
        return;
    }
    
    qDebug() << "=== AI Client Response Received ===";
    
    QNetworkReply::NetworkError error = m_currentReply->error();
    int httpStatus = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    qDebug() << "HTTP Status Code:" << httpStatus;
    qDebug() << "Network Error Code:" << error;
    
    if (error == QNetworkReply::NoError) {
        QByteArray responseData = m_currentReply->readAll();
        qDebug() << "Response Data Length:" << responseData.length();
        qDebug() << "Raw Response:" << responseData;
        
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
        
        if (parseError.error == QJsonParseError::NoError) {
            qDebug() << "JSON Parse: Success";
            qDebug() << "Parsed JSON:" << doc.toJson(QJsonDocument::Compact);
            
            QString response = extractResponse(doc);
            if (!response.isEmpty()) {
                qDebug() << "Extracted Response:" << response;
                emit responseReceived(response);
            } else {
                qDebug() << "Failed to extract response from JSON";
                emit errorOccurred("AI返回的响应格式不正确");
            }
        } else {
            qDebug() << "JSON Parse Error:" << parseError.errorString();
            qDebug() << "Parse Error Offset:" << parseError.offset;
            emit errorOccurred("解析AI响应时出错: " + parseError.errorString());
        }
    } else {
        // 即使在错误情况下也读取响应数据
        QByteArray responseData = m_currentReply->readAll();
        qDebug() << "Error Response Data Length:" << responseData.length();
        qDebug() << "Raw Error Response:" << responseData;
        
        QString errorString = m_currentReply->errorString();
        qDebug() << "Request Failed - Error String:" << errorString;
        qDebug() << "HTTP Status:" << httpStatus;
        
        // 尝试解析错误响应中的详细信息
        QString detailedError = errorString;
        if (!responseData.isEmpty()) {
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                qDebug() << "Error Response JSON Parse: Success";
                qDebug() << "Error Response JSON:" << doc.toJson(QJsonDocument::Compact);
                
                // 尝试提取API错误信息
                QJsonObject rootObj = doc.object();
                if (rootObj.contains("error")) {
                    QJsonObject errorObj = rootObj["error"].toObject();
                    if (errorObj.contains("message")) {
                        QString apiErrorMsg = errorObj["message"].toString();
                        detailedError += QString(" (API错误: %1)").arg(apiErrorMsg);
                        qDebug() << "Extracted API Error Message:" << apiErrorMsg;
                    }
                }
            } else {
                qDebug() << "Error Response JSON Parse Failed:" << parseError.errorString();
                // 如果不是JSON，直接显示原始错误响应
                QString rawError = QString::fromUtf8(responseData);
                if (!rawError.isEmpty()) {
                    detailedError += QString(" (服务器响应: %1)").arg(rawError.left(200)); // 限制长度
                    qDebug() << "Raw Error Response Text:" << rawError;
                }
            }
        }
        
        if (httpStatus == 401) {
            emit errorOccurred("API密钥无效，请检查设置");
        } else if (httpStatus == 429) {
            emit errorOccurred("API调用频率超限，请稍后再试");
        } else if (httpStatus >= 500) {
            emit errorOccurred("AI服务器错误，请稍后再试");
        } else {
            emit errorOccurred(QString("网络请求失败: %1").arg(detailedError));
        }
    }
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
    emit requestFinished();
    qDebug() << "=== AI Client Request Finished ===";
}

void AIClient::onRequestTimeout()
{
    if (m_currentReply) {
        m_currentReply->abort();
        emit errorOccurred("请求超时，请检查网络连接或稍后再试");
    }
}

void AIClient::onSslErrors(const QList<QSslError> &errors)
{
    qDebug() << "=== SSL Errors Detected ===";
    for (const QSslError &error : errors) {
        qDebug() << "SSL Error:" << error.errorString();
    }
    
    // 暂时忽略SSL错误以解决连接问题
    if (m_currentReply) {
        m_currentReply->ignoreSslErrors();
        qDebug() << "SSL errors ignored to continue connection";
    }
}

QString AIClient::buildRequestBody(const QString &systemPrompt, const QString &userPrompt)
{
    qDebug() << "=== Building Request Body ===";
    
    QJsonObject message1;
    message1["role"] = "system";
    message1["content"] = systemPrompt;
    
    QJsonObject message2;
    message2["role"] = "user";
    message2["content"] = userPrompt;
    
    QJsonArray messages;
    messages.append(message1);
    messages.append(message2);
    
    QJsonObject requestObj;
    QString modelToUse = m_model.isEmpty() ? "gpt-3.5-turbo" : m_model;
    requestObj["model"] = modelToUse;
    requestObj["messages"] = messages;
    requestObj["max_tokens"] = 1000;
    requestObj["temperature"] = 0.7;
    
    qDebug() << "Request Object Fields:";
    qDebug() << "  m_model member variable:" << m_model;
    qDebug() << "  modelToUse (actual):" << modelToUse;
    qDebug() << "  model in JSON:" << requestObj["model"].toString();
    qDebug() << "  max_tokens:" << requestObj["max_tokens"].toInt();
    qDebug() << "  temperature:" << requestObj["temperature"].toDouble();
    qDebug() << "  messages count:" << messages.size();
    
    QJsonDocument doc(requestObj);
    QString result = doc.toJson(QJsonDocument::Compact);
    qDebug() << "Final Request Body Size:" << result.length() << "bytes";
    return result;
}

QString AIClient::extractResponse(const QJsonDocument &doc)
{
    QJsonObject rootObj = doc.object();
    
    if (rootObj.contains("choices")) {
        QJsonArray choices = rootObj["choices"].toArray();
        if (!choices.isEmpty()) {
            QJsonObject firstChoice = choices[0].toObject();
            if (firstChoice.contains("message")) {
                QJsonObject message = firstChoice["message"].toObject();
                return message["content"].toString();
            }
        }
    }
    
    // 尝试其他可能的响应格式
    if (rootObj.contains("content")) {
        return rootObj["content"].toString();
    }
    
    if (rootObj.contains("text")) {
        return rootObj["text"].toString();
    }
    
    return QString();
}
