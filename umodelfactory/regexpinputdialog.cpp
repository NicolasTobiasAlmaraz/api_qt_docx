#include "regexpinputdialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QRegExpValidator>
#include <QPushButton>

RegExpInputDialog::RegExpInputDialog(QWidget *parent, Qt::WindowFlags flags) :
    QDialog(parent)
{
    if (flags != 0)
        setWindowFlags(flags);

    m_regExp = QRegExp("*");
    m_regExp.setPatternSyntax(QRegExp::Wildcard);
    m_validator = new QRegExpValidator(m_regExp);

    m_text = new QLineEdit(this);
    QFont fnt = QFont(m_text->font());
    fnt.setPointSize(9);
    m_text->setFont(fnt);
    m_text->setValidator(m_validator);
    connect(m_text, SIGNAL(textEdited(QString)), this, SLOT(checkValid(QString)));

    m_label = new QLabel(this);
    m_label->setFont(fnt);

    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->addButton("Aceptar", QDialogButtonBox::AcceptRole);
    m_buttonBox->addButton("Cancelar", QDialogButtonBox::RejectRole);
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_label);
    layout->addWidget(m_text);
    layout->addWidget(m_buttonBox);

    this->setMinimumSize(350,90);
}

void RegExpInputDialog::setText(const QString &text)
{
    m_text->setText(text);
    m_text->selectAll();
}

void RegExpInputDialog::setRegExp(const QRegExp &regExp)
{
    m_validator->setRegExp(regExp);
    checkValid(m_text->text());
}

void RegExpInputDialog::checkValid(const QString &text)
{

    bool valid = !text.isEmpty();
    m_buttonBox->buttons().first()->setEnabled(valid);
}

QString RegExpInputDialog::getText(QWidget *parent, const QString &title,
                                   const QString &label, const QString &text,
                                   const QRegExp &regExp, bool *ok, QIcon &icon,
                                   Qt::WindowFlags flags)
{
    RegExpInputDialog *dialog = new RegExpInputDialog(parent, flags);
    dialog->setTitle(title);
    dialog->setLabelText(label);
    dialog->setText(text);
    dialog->setRegExp(regExp);
    dialog->setWindowIcon(icon);

    if (dialog->exec() == QDialog::Accepted) {
        *ok = true;
        return dialog->getText();
    } else {
        *ok = false;
        return QString();
    }
}
