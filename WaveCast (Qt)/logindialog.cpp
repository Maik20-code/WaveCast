#include "logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("WaveCast - Вход");
    setFixedSize(300, 150);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Ваш никнейм:"));
    nicknameEdit = new QLineEdit;
    layout->addWidget(nicknameEdit);
    loginButton = new QPushButton("Войти в чат");
    layout->addWidget(loginButton);
    errorLabel = new QLabel;
    errorLabel->setStyleSheet("color: red;");
    layout->addWidget(errorLabel);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
}

void LoginDialog::onLoginClicked()
{
    QString nick = nicknameEdit->text().trimmed();
    if (nick.isEmpty()) {
        errorLabel->setText("Введите никнейм");
        return;
    }
    m_nickname = nick;
    accept(); // закрываем диалог с успехом
}

QString LoginDialog::getNickname() const
{
    return m_nickname;
}
