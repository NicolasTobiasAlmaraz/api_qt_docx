#ifndef REGEXPINPUTDIALOG_H
#define REGEXPINPUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QRegExpValidator>

class RegExpInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegExpInputDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    void setTitle(const QString &title) { setWindowTitle(title); }
    void setLabelText(const QString &label) { m_label->setText(label); }
    void setText(const QString &text);
    void setRegExp(const QRegExp &regExp);

    QString getLabelText() { return m_label->text(); }
    QString getText() { return m_text->text(); }

    static QString getText(QWidget *parent, const QString &title,
                           const QString &label, const QString &text,
                           const QRegExp &regExp, bool *ok, QIcon &icon,
                           Qt::WindowFlags flags=0);
signals:

public slots:
    void checkValid(const QString &m_text);

private:
    QLabel *m_label;
    QLineEdit *m_text;
    QDialogButtonBox *m_buttonBox;
    QRegExp m_regExp;
    QRegExpValidator *m_validator;
};

#endif // REGEXPINPUTDIALOG_H
