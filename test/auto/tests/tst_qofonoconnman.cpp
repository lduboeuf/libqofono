/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Alexander Kanavin <alex.kanavin@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QtTest/QtTest>

#include "qofonoconnectioncontext.h"
#include "qofonoconnectionmanager.h"

class TestOfonoConnMan : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoConnectionManager(this);
        m->setModemPath("/phonesim");

        QTRY_VERIFY(m->isValid());
        QTRY_VERIFY(m->powered());
    }

    void testOfonoConnMan()
    {
        m->setRoamingAllowed(true);
        QTRY_COMPARE(m->roamingAllowed(), true);
        Q_FOREACH (QString context, m->contexts()) {
            m->removeContext(context);
        }
        QTRY_VERIFY(m->contexts().isEmpty());

        QSignalSpy roam(m,SIGNAL(roamingAllowedChanged(bool)));
        QSignalSpy pow(m, SIGNAL(poweredChanged(bool)));
        QSignalSpy add(m, SIGNAL(contextAdded(QString)));
        QSignalSpy rem(m, SIGNAL(contextRemoved(QString)));

        m->setPowered(false);
        QTRY_COMPARE(pow.count(), 1);
        QCOMPARE(pow.takeFirst().at(0).toBool(), false);
        QCOMPARE(m->powered(), false);
        m->setPowered(true);
        QTRY_COMPARE(pow.count(), 1);
        QCOMPARE(pow.takeFirst().at(0).toBool(), true);
        QCOMPARE(m->powered(), true);

        m->setRoamingAllowed(false);
        QTRY_COMPARE(roam.count(), 1);
        QCOMPARE(roam.takeFirst().at(0).toBool(), false);
        QCOMPARE(m->roamingAllowed(), false);
        m->setRoamingAllowed(true);
        QTRY_COMPARE(roam.count(), 1);
        QCOMPARE(roam.takeFirst().at(0).toBool(), true);
        QCOMPARE(m->roamingAllowed(), true);

        m->addContext(QString("internet"));
        QTRY_COMPARE(add.count(), 1);

        QCOMPARE(m->powered(),true);
        QCOMPARE(m->attached(),true);
        QCOMPARE(m->suspended(),false);
        QCOMPARE(m->roamingAllowed(),true);

        QString path = add.takeFirst().at(0).toString();
        m->removeContext(path);
        QTRY_COMPARE(rem.count(), 1);
        QCOMPARE(rem.takeFirst().at(0).toString(), path);

        m->addContext(QString("internet"));
        QTRY_COMPARE(add.count(), 1);
        path = add.takeFirst().at(0).toString();
        QOfonoConnectionContext* contextInternet = new QOfonoConnectionContext(this);
        contextInternet->setContextPath(path);
        m->addContext(QString("mms"));
        QTRY_COMPARE(add.count(), 1);
        path = add.takeFirst().at(0).toString();
        QOfonoConnectionContext* contextMms = new QOfonoConnectionContext(this);
        contextMms->setContextPath(path);

        contextInternet->setActive(true);
        contextMms->setActive(true);
        QTRY_VERIFY(contextInternet->active() && contextMms->active());

        m->deactivateAll();
        QTRY_VERIFY(!contextInternet->active());
        QTRY_VERIFY(!contextMms->active());

        QCOMPARE(rem.count(), 0);
    }

    void testResetContexts()
    {
        Q_FOREACH (QString context, m->contexts()) {
            m->removeContext(context);
        }
        QTRY_VERIFY(m->contexts().isEmpty());

        QSignalSpy pow(m, SIGNAL(poweredChanged(bool)));
        QSignalSpy add(m, SIGNAL(contextAdded(QString)));
        QSignalSpy rem(m, SIGNAL(contextRemoved(QString)));

        // Add new context.
        m->addContext(QString("internet"));
        QTRY_COMPARE(add.count(), 1);
        QString path = add.takeFirst().at(0).toString();
        QOfonoConnectionContext* contextInternet = new QOfonoConnectionContext(this);
        contextInternet->setContextPath(path);

        // We activate context to make sure connman is deactivating on reset.
        contextInternet->setActive(true);
        QTRY_VERIFY(contextInternet->active());

        // Connman needs to be powered down before calling reset.
        m->setPowered(false);
        QTRY_COMPARE(pow.count(), 1);
        QCOMPARE(pow.takeFirst().at(0).toBool(), false);
        QCOMPARE(m->powered(), false);

        m->resetContexts();
        QTRY_COMPARE(rem.count(), 1);
    }

    void cleanupTestCase()
    {
        QSignalSpy pow(m, SIGNAL(poweredChanged(bool)));
        m->setPowered(true);
        QTRY_COMPARE(pow.count(), 1);
        QCOMPARE(pow.takeFirst().at(0).toBool(), true);
        QCOMPARE(m->powered(), true);
    }

private:
    QOfonoConnectionManager *m;
};

QTEST_MAIN(TestOfonoConnMan)
#include "tst_qofonoconnman.moc"
