/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "searchbar.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


SearchBar::SearchBar(QWidget *parent)
    : QWidget(parent)
    , _findLineEdit( new QLineEdit(this) )
    , _caseCheckBox( new QCheckBox( tr("Match Case") , this) )
    , _notFoundLabel( new QLabel(this) )
{
    connect(_findLineEdit, &QLineEdit::returnPressed, this, &SearchBar::findForward);

    auto label = new QLabel( tr("Search for:"), this);
    label->setMinimumWidth(100);

    _findLineEdit->setMinimumWidth(250);
    _findLineEdit->setMaximumWidth(250);

    auto prevButton = new QPushButton( tr("Previous") , this);
    prevButton->setMinimumWidth(120);
    prevButton->setShortcut(QKeySequence::FindPrevious);
    connect(prevButton, &QPushButton::clicked, this, &SearchBar::findBackward);

    auto nextButton = new QPushButton( tr("Next"), this);
    nextButton->setShortcut(QKeySequence::FindNext);
    nextButton->setMinimumWidth(120);
    connect(nextButton, &QPushButton::clicked, this, &SearchBar::findForward);

    // The UI
    auto layout = new QHBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (label);
    layout->addWidget (_findLineEdit);
    layout->addWidget (nextButton);
    layout->addWidget (prevButton);
    layout->addWidget (_caseCheckBox);
    layout->addStretch();
    layout->addWidget (_notFoundLabel);
    layout->addStretch();

    setLayout (layout);

    setFocusProxy(_findLineEdit);

    // tab order
    setTabOrder(_findLineEdit, nextButton);
    setTabOrder(nextButton, prevButton);
    setTabOrder(prevButton,_caseCheckBox);
}


void SearchBar::setText(const QString& text)
{
    _findLineEdit->setText(text);
}


QString SearchBar::text()
{
    return _findLineEdit->text();
}


bool SearchBar::caseChecked()
{
    return _caseCheckBox->isChecked();
}


void SearchBar::findBackward()
{
    _notFoundLabel->clear();

    QString str = _findLineEdit->text();
    bool caseSensitive = _caseCheckBox->isChecked();
    Q_EMIT search(str, false, caseSensitive);
}


void SearchBar::findForward()
{
    _notFoundLabel->clear();

    QString str = _findLineEdit->text();
    bool caseSensitive = _caseCheckBox->isChecked();
    Q_EMIT search(str, true, caseSensitive);
}


void SearchBar::searchMessage(const QString &msg)
{
    QString text = QLatin1String("<b>") + msg + QLatin1String("</b>");
    _notFoundLabel->setText(text);
}
