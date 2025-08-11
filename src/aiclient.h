#pragma once

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QUrl>

class AIClient : public QObject {
    Q_OBJECT

public:
    explicit AIClient(QObject *parent = nullptr);
    
    void setApiUrl(const QString &url);
    void setApiKey(const QString &key);
    void setModel(const QString &model);
    
    void sendRequest(const QString &systemPrompt, const QString &userPrompt);
    void cancelRequest();

signals:
    void responseReceived(const QString &response);
    void errorOccurred(const QString &error);
    void requestStarted();
    void requestFinished();

private slots:
    void onReplyFinished();
    void onRequestTimeout();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QString buildRequestBody(const QString &systemPrompt, const QString &userPrompt);
    QString extractResponse(const QJsonDocument &doc);
    
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QTimer *m_timeoutTimer;
    
    QString m_apiUrl;
    QString m_apiKey;
    QString m_model;
    
    static const int REQUEST_TIMEOUT_MS = 60000;  // 60秒超时
};
