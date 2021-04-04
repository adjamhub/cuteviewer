/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef SEARCHBAR_H
#define SEARCHBAR_H


#include <QWidget>

class QCheckBox;
class QLabel;
class QLineEdit;


class SearchBar : public QWidget
{
    Q_OBJECT

public:
    explicit SearchBar(QWidget *parent);

    void setText(const QString& text);
    QString text();

    bool caseChecked();

Q_SIGNALS:
    void search(const QString &search,
                bool forward = true,
                bool casesensitive = false);

public Q_SLOTS:
    void searchMessage(const QString & msg);

private Q_SLOTS:
    void findBackward();
    void findForward();

private:
    QLineEdit* _findLineEdit;

    QCheckBox* _caseCheckBox;
    QLabel* _notFoundLabel;
};

#endif // SEARCHBAR_H
