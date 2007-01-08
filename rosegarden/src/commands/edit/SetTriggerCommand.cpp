/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
 
    This program is Copyright 2000-2007
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <richard.bown@ferventsoftware.com>
 
    The moral rights of Guillaume Laurent, Chris Cannam, and Richard
    Bown to claim authorship of this work have been asserted.
 
    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/


#include "SetTriggerCommand.h"

#include "base/NotationTypes.h"
#include "base/Selection.h"
#include "base/TriggerSegment.h"
#include "document/BasicSelectionCommand.h"
#include "base/BaseProperties.h"
#include <qstring.h>


namespace Rosegarden
{
using namespace BaseProperties;

void
SetTriggerCommand::modifySegment()
{
    EventSelection::eventcontainer::iterator i;

    for (i = m_selection->getSegmentEvents().begin();
            i != m_selection->getSegmentEvents().end(); ++i) {

        if (!m_notesOnly || (*i)->isa(Note::EventType)) {
            (*i)->set
            <Int>(TRIGGER_SEGMENT_ID, m_triggerSegmentId);
            (*i)->set
            <Bool>(TRIGGER_SEGMENT_RETUNE, m_retune);
            (*i)->set
            <String>(TRIGGER_SEGMENT_ADJUST_TIMES, m_timeAdjust);
            if (m_mark != Marks::NoMark) {
                Marks::addMark(**i, m_mark, true);
            }
        }
    }

    // Update the rec references here, without bothering to do so in unexecute
    // or in ClearTriggersCommand -- because it doesn't matter if a trigger
    // has references to segments that don't actually trigger it, whereas it
    // does matter if it loses a reference to something that does

    TriggerSegmentRec *rec =
        m_selection->getSegment().getComposition()->getTriggerSegmentRec
        (m_triggerSegmentId);

    if (rec)
        rec->updateReferences();
}

}
