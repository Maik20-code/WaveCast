#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString getNickname() const;

private slots:
    void onLoginClicked();

private:
    QLineEdit *nicknameEdit;
    QPushButton *loginButton;
    QLabel *errorLabel;
    QString m_nickname;
};

#endif // LOGINDIALOG_H
