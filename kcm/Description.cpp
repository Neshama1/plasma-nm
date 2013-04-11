/***************************************************************************
 *   Copyright (C) 2013 by Daniel Nicoletti <dantti12@gmail.com>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#include "Description.h"
#include "ui_Description.h"

#include "AvailableConnectionsModel.h"

#include <uiutils.h>

#include <QStringBuilder>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include <KGlobal>
#include <KLocale>
#include <KDateTime>
#include <KToolInvocation>
#include <KDebug>
#include <KMessageWidget>

#include <QtNetworkManager/manager.h>
#include <QtNetworkManager/ipconfig.h>
#include <QtNetworkManager/settings/connection.h>
#include <QtNetworkManager/settings.h>
#include <QtNetworkManager/activeconnection.h>

typedef QList<QDBusObjectPath> ObjectPathList;
typedef QMap<QString, QString>  StringStringMap;

using namespace NetworkManager;

Description::Description(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Description)
{
    ui->setupUi(this);

    m_availableConnectionsModel = new AvailableConnectionsModel(this);
    ui->connectionCB->setModel(m_availableConnectionsModel);
}

Description::~Description()
{
    delete ui;
}

int Description::innerHeight() const
{
    return ui->tabWidget->currentWidget()->height();
}

void Description::setDevice(const QString &uni)
{
    while (ui->tabWidget->count() > 1) {
        ui->tabWidget->removeTab(1);
    }

    if (m_device && m_device->uni() == uni) {
        return;
    }

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(uni);
    m_device = device;
    if (device) {
        connect(device.data(), SIGNAL(stateChanged(NetworkManager::Device::State,NetworkManager::Device::State,NetworkManager::Device::StateChangeReason)),
                this, SLOT(updateState()));
        updateState();

        m_availableConnectionsModel->setDevice(device);

        connect(device.data(), SIGNAL(activeConnectionChanged()),
                this, SLOT(updateActiveConnection()));
        updateActiveConnection();

        connect(device.data(), SIGNAL(ipV4AddressChanged()),
                this, SLOT(updateIpV4Config()));
        updateIpV4Config();
    }
}

void Description::updateState()
{
    ui->statusL->setText(UiUtils::connectionStateToString(m_device->state()));
    ui->disconnectPB->setEnabled(m_device->state() == Device::Activated);
}

void Description::updateActiveConnection()
{
    kDebug();
    int active = -1;
    if (m_device->activeConnection()) {
        QString activeConnectionPath = m_device->activeConnection()->connection()->path();
        active = ui->connectionCB->findData(activeConnectionPath, AvailableConnectionsModel::RoleConectionPath);
        kDebug() << activeConnectionPath << active;
    }
    ui->connectionCB->setCurrentIndex(active);
}

void Description::updateIpV4Config()
{
    QStringList routers;
    QStringList addresses;
    foreach (const NetworkManager::IpAddress &address, m_device->ipV4Config().addresses()) {
        addresses << address.ip().toString() % QLatin1Char('/') % QString::number(address.prefixLength());
        routers << address.gateway().toString();
    }
    ui->ipv4AddressL->setText(addresses.join(QLatin1String("\n")));


    foreach (const NetworkManager::IpRoute &route, m_device->ipV4Config().routes()) {
        routers << i18n("%1/%2, nexthop %3 metric %4",
                        route.ip().toString(),
                        QString::number(route.prefixLength()),
                        route.nextHop().toString(),
                        QString::number(route.metric()));
    }
    ui->ipv4RouterL->setText(routers.join(QLatin1String("\n")));

    QStringList nameservers;
    foreach (const QHostAddress &nameserver, m_device->ipV4Config().nameservers()) {
        nameservers << nameserver.toString();
    }
    ui->ipv4DnsServerL->setText(nameservers.join(QLatin1String(", ")));

    ui->ipv4SearchDomainsL->setText(m_device->ipV4Config().domains().join(QLatin1String("\n")));
}

void Description::on_disconnectPB_clicked()
{
    kDebug() << m_device;
    if (m_device) {
        kDebug() << m_device->state() << NetworkManager::Device::Activated;
        if (m_device->state() == NetworkManager::Device::Activated) {
            m_device->disconnectInterface();
        } else {
            ui->disconnectPB->setEnabled(false);
        }
    }
}

void Description::on_connectionCB_activated(int index)
{
    kDebug();
    if (m_device) {
        QStandardItem *stdItem = m_availableConnectionsModel->item(index);
        if (stdItem) {
            QString newConnectionPath = stdItem->data(AvailableConnectionsModel::RoleConectionPath).toString();
            QString oldConnectionPath;
            if (m_device->activeConnection()) {
                oldConnectionPath = m_device->activeConnection()->connection()->path();
            }

            if (newConnectionPath != oldConnectionPath) {
                NetworkManager::activateConnection(newConnectionPath, m_device->uni(), QString());
            }
        }
    }
}

#include "Description.moc"
