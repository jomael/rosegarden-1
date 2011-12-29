/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2012 the Rosegarden development team.
 
    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/


#include "CreateOrDeleteDeviceCommand.h"

#include "misc/Debug.h"
#include "misc/Strings.h"
#include "base/Device.h"
#include "base/MidiDevice.h"
#include "base/Studio.h"
#include "sequencer/RosegardenSequencer.h"
#include "gui/studio/DeviceManagerDialog.h"
#include "gui/application/RosegardenMainWindow.h"

#include <QByteArray>
#include <QDataStream>
#include <QString>


namespace Rosegarden
{

CreateOrDeleteDeviceCommand::CreateOrDeleteDeviceCommand(Studio *studio,
                                                         DeviceId id) :
    NamedCommand(getGlobalName(true)),
    m_studio(studio),
    m_deviceId(id),
    m_deviceCreated(true)
{
    Device *device = m_studio->getDevice(m_deviceId);

    if (device) {
        m_name = device->getName();
        m_type = device->getType();
        m_direction = MidiDevice::Play;
        MidiDevice *md = dynamic_cast<MidiDevice *>(device);
        if (md) m_direction = md->getDirection();
        m_connection = qstrtostr(RosegardenSequencer::getInstance()
                                 ->getConnection(md->getId()));
    } else {
        RG_DEBUG << "CreateOrDeleteDeviceCommand: No such device as "
                 << m_deviceId << endl;
    }
}

void
CreateOrDeleteDeviceCommand::execute()
{
    if (!m_deviceCreated) {

        //!DEVPUSH: Not ideal; we probably just want to add it to the studio (and then trigger a re-push) rather than add it twice to studio and sequencer

        // Create

        // don't want to do this again on undo even if it fails -- only on redo
        m_deviceCreated = true;

        m_deviceId = m_studio->getSpareDeviceId(m_baseInstrumentId);

        bool success = RosegardenSequencer::getInstance()->
            addDevice(m_type, m_deviceId, m_baseInstrumentId, m_direction);

        if (!success) {
            SEQMAN_DEBUG << "CreateDeviceCommand::execute - "
                         << "sequencer addDevice failed" << endl;
            return ;
        }

        SEQMAN_DEBUG << "CreateDeviceCommand::execute - "
                     << " added device " << m_deviceId
                     << " with base instrument id " << m_baseInstrumentId
                     << endl;

        RosegardenSequencer::getInstance()->setConnection
            (m_deviceId, strtoqstr(m_connection));

        SEQMAN_DEBUG << "CreateDeviceCommand::execute - "
                     << " reconnected device " << m_deviceId
                     << " to " << m_connection << endl;

        m_studio->addDevice(m_name, m_deviceId, m_baseInstrumentId, m_type);
        Device *device = m_studio->getDevice(m_deviceId);
        if (device) {
            MidiDevice *md = dynamic_cast<MidiDevice *>(device);
            if (md) md->setDirection(m_direction);
        }

        /* update view automatically (without pressing refresh button) */
        DeviceManagerDialog *dmd=RosegardenMainWindow::self()->getDeviceManager();
        if (dmd!=NULL) {
          dmd->slotResyncDevicesReceived();
        }
    } else {

        // Delete

        RosegardenSequencer::getInstance()->removeDevice(m_deviceId);

        SEQMAN_DEBUG << "CreateDeviceCommand::unexecute - "
                     << " removed device " << m_deviceId << endl;

        m_studio->removeDevice(m_deviceId);

        m_deviceId = Device::NO_DEVICE;
        m_deviceCreated = false;
    }
}

}
