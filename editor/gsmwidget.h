/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GSM_WIDGET_H
#define GSM_WIDGET_H

#include <QtGui/QWidget>

#include <QtNetworkManager/settings/setting.h>

#include "settingwidget.h"

namespace Ui
{
class GsmWidget;
}

class GsmWidget : public SettingWidget
{
    Q_OBJECT
public:
    GsmWidget(NetworkManager::Settings::Setting *setting = 0, QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~GsmWidget();

    void loadConfig(NetworkManager::Settings::Setting *setting);

    QVariantMap setting() const;

private slots:
    void showPasswords(bool show);

private:
    Ui::GsmWidget * m_ui;
};

#endif // GSM_WIDGET_H