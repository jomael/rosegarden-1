/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2009 the Rosegarden development team.
 
    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/


#include <Q3Canvas>
#include <Q3CanvasPixmap>

// #include <kglobal.h>
// #include <kmainwindow.h>
// #include <kstandardshortcut.h>
// #include <kstandardaction.h>
// #include <QDir>

#include "TriggerSegmentManager.h"
#include "TriggerManagerItem.h"

#include "base/BaseProperties.h"
#include "misc/Debug.h"
#include "misc/Strings.h"
#include "base/Clipboard.h"
#include "base/Composition.h"
#include "base/CompositionTimeSliceAdapter.h"
#include "base/RealTime.h"
#include "base/Segment.h"
#include "base/TriggerSegment.h"
#include "commands/segment/AddTriggerSegmentCommand.h"
#include "commands/segment/DeleteTriggerSegmentCommand.h"
#include "commands/segment/PasteToTriggerSegmentCommand.h"
#include "document/CommandHistory.h"
#include "document/RosegardenGUIDoc.h"
#include "document/ConfigGroups.h"
#include "gui/dialogs/TimeDialog.h"
#include "gui/general/MidiPitchLabel.h"
#include "gui/general/IconLoader.h"
#include "gui/kdeext/KTmpStatusMsg.h"
#include "document/Command.h"

#include <QLayout>
#include <QApplication>
#include <QAction>
#include <QTreeWidget>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>
#include <QDialog>
#include <QFrame>
#include <QIcon>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>
#include <QToolTip>
#include <QWidget>
#include <QVBoxLayout>


namespace Rosegarden
{

TriggerSegmentManager::TriggerSegmentManager(QWidget *parent,
        RosegardenGUIDoc *doc):
        QMainWindow(parent),	//, "triggereditordialog"),
        m_doc(doc),
        m_modified(false)
{
	
	this->setObjectName( "triggereditordialog" );
	
    QWidget *mainFrame = new QWidget(this);
    QVBoxLayout *mainFrameLayout = new QVBoxLayout;
    setCentralWidget(mainFrame);

    setCaption(QObject::tr("Manage Triggered Segments"));

    m_listView = new QTreeWidget( mainFrame );
    mainFrameLayout->addWidget(m_listView);
	
	QStringList sl;
	sl 		<< "Index"
			<< QObject::tr("ID")
			<< QObject::tr("Label")
			<< QObject::tr("Duration")
			<< QObject::tr("Base pitch")
			<< QObject::tr("Base velocity")
			<< QObject::tr("Triggers");
	
	m_listView->setColumnCount( 7 );
	m_listView->setHeaderLabels( sl );
	
	/*
	m_listView->addColumn("Index");
    m_listView->addColumn(QObject::tr("ID"));
    m_listView->addColumn(QObject::tr("Label"));
    m_listView->addColumn(QObject::tr("Duration"));
    m_listView->addColumn(QObject::tr("Base pitch"));
    m_listView->addColumn(QObject::tr("Base velocity"));
    m_listView->addColumn(QObject::tr("Triggers"));
	*/
	
    // Align centrally
//     for (int i = 0; i < 2; ++i)
//         m_listView->setColumnAlignment(i, Qt::AlignHCenter);		//&&& note:
	
	// NOTE: use QTreeWidgetItem::setTextAlignment( int column, Qt::AlignHCenter ) instead
	//
	

    QFrame *btnBox = new QFrame( mainFrame );
    mainFrameLayout->addWidget(btnBox);
    mainFrame->setLayout(mainFrameLayout);

    btnBox->setSizePolicy(
        QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

    QHBoxLayout* layout = new QHBoxLayout(btnBox, 4, 10);

    m_addButton = new QPushButton(QObject::tr("Add"), btnBox);
    m_deleteButton = new QPushButton(QObject::tr("Delete"), btnBox);
    m_deleteAllButton = new QPushButton(QObject::tr("Delete All"), btnBox);

    m_closeButton = new QPushButton(QObject::tr("Close"), btnBox);

    m_addButton->setToolTip(QObject::tr("Add a Triggered Segment"));

    m_deleteButton->setToolTip(QObject::tr("Delete a Triggered Segment"));

    m_deleteAllButton->setToolTip(QObject::tr("Delete All Triggered Segments"));

    m_closeButton->setToolTip(QObject::tr("Close the Triggered Segment Manager"));

    layout->addStretch(10);
    layout->addWidget(m_addButton);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_deleteAllButton);
    layout->addSpacing(30);

    layout->addWidget(m_closeButton);
    layout->addSpacing(5);

    connect(m_addButton, SIGNAL(released()),
            SLOT(slotAdd()));

    connect(m_deleteButton, SIGNAL(released()),
            SLOT(slotDelete()));

    connect(m_closeButton, SIGNAL(released()),
            SLOT(slotClose()));

    connect(m_deleteAllButton, SIGNAL(released()),
            SLOT(slotDeleteAll()));

    setupActions();

//     CommandHistory::getInstance()->attachView(actionCollection());	//&&&
	
    connect(CommandHistory::getInstance(), SIGNAL(commandExecuted()),
            this, SLOT(slotUpdate()));

    connect(m_listView, SIGNAL(doubleClicked(QTreeWidgetItem *)),
            SLOT(slotEdit(QTreeWidgetItem *)));

    connect(m_listView, SIGNAL(pressed(QTreeWidgetItem *)),
            this, SLOT(slotItemClicked(QTreeWidgetItem *)));

    // Highlight all columns - enable extended selection mode
    //
    m_listView->setAllColumnsShowFocus(true);
//     m_listView->setSelectionMode(QTreeWidget::Extended);
	m_listView->setSelectionMode( QAbstractItemView::ExtendedSelection );
// 	m_listView->setSelectionBehavior( QAbstractItemView::SelectRows );
	
//     m_listView->setItemsRenameable(true);	//&&&

    initDialog();

//     setAutoSaveSettings(TriggerManagerConfigGroup, true);	//&&&

    m_shortcuts = new QShortcut(this);
}

TriggerSegmentManager::~TriggerSegmentManager()
{
    RG_DEBUG << "TriggerSegmentManager::~TriggerSegmentManager" << endl;

//     m_listView->saveLayout(TriggerManagerConfigGroup);	//&&&

//     if (m_doc)
//         CommandHistory::getInstance()->detachView(actionCollection());
}

void
TriggerSegmentManager::initDialog()
{
    RG_DEBUG << "TriggerSegmentManager::initDialog" << endl;
    slotUpdate();
}

void
TriggerSegmentManager::slotUpdate()
{
    RG_DEBUG << "TriggerSegmentManager::slotUpdate" << endl;

    TriggerManagerItem *item;

    m_listView->clear();

    Composition &comp = m_doc->getComposition();

    const Composition::triggersegmentcontainer &triggers =
        comp.getTriggerSegments();

    Composition::triggersegmentcontainerconstiterator it;

    QSettings settings;

    settings.beginGroup( TriggerManagerConfigGroup );

    int timeMode = settings.value("timemode", 0).toInt() ;

    int i = 0;

    for (it = triggers.begin(); it != triggers.end(); ++it) {

        // duration is as of first usage, or 0

        int uses = 0;
        timeT first = 0;
        std::set
            <int> tracks;

        CompositionTimeSliceAdapter tsa(&m_doc->getComposition());
        for (CompositionTimeSliceAdapter::iterator ci = tsa.begin();
                ci != tsa.end(); ++ci) {
            if ((*ci)->has(BaseProperties::TRIGGER_SEGMENT_ID) &&
                    (*ci)->get
                    <Int>(BaseProperties::TRIGGER_SEGMENT_ID) == (long)(*it)->getId()) {
                ++uses;
                if (tracks.empty()) {
                    first = (*ci)->getAbsoluteTime();
                }
                tracks.insert(ci.getTrack());
            }
        }

        timeT duration =
            (*it)->getSegment()->getEndMarkerTime() -
            (*it)->getSegment()->getStartTime();

        QString timeString = makeDurationString
                             (first, duration, timeMode);

        QString label = strtoqstr((*it)->getSegment()->getLabel());
        if (label == "")
            label = QObject::tr("<no label>");

        QString used = QObject::tr("%1 on %n track(s)", "",
                            tracks.size()).arg(uses);

        QString pitch = QString("%1 (%2)")
                        .arg(MidiPitchLabel((*it)->getBasePitch()).getQString())
                        .arg((*it)->getBasePitch());

        QString velocity = QString("%1").arg((*it)->getBaseVelocity());

        item = new TriggerManagerItem(
               			m_listView, 
				  			QStringList() 
							<< QString("%1").arg(i + 1)
							<< QString("%1").arg((*it)->getId())
							<< label << timeString << pitch << velocity << used );

        item->setRawDuration(duration);
        item->setId((*it)->getId());
        item->setUsage(uses);
        item->setPitch((*it)->getBasePitch());

        m_listView->addTopLevelItem(item);
        ++i;
    }

    if (m_listView->topLevelItemCount() == 0) {
        QTreeWidgetItem *item =
            new TriggerManagerItem(m_listView, QStringList() << QObject::tr("<none>") );
        m_listView->addTopLevelItem(item);

//         m_listView->setSelectionMode(QTreeWidget::NoSelection);
		m_listView->setSelectionMode( QAbstractItemView::NoSelection );
    } else {
//         m_listView->setSelectionMode(QTreeWidget::Extended);
		m_listView->setSelectionMode( QAbstractItemView::ExtendedSelection );

    }

    settings.endGroup();
}

void
TriggerSegmentManager::slotDeleteAll()
{
	if (QMessageBox::warning(this, "", QObject::tr("This will remove all triggered segments from the whole composition.  Are you sure?"), QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Cancel ) != QMessageBox::Yes )
        return ;

    RG_DEBUG << "TriggerSegmentManager::slotDeleteAll" << endl;
    MacroCommand *command = new MacroCommand(QObject::tr("Remove all triggered segments"));

// 	QTreeWidgetItem *it = m_listView->firstChild();
	QTreeWidgetItem *it = m_listView->topLevelItem(0);

    do {

        TriggerManagerItem *item =
            dynamic_cast<TriggerManagerItem*>(it);

        if (!item)
            continue;

        DeleteTriggerSegmentCommand *c =
            new DeleteTriggerSegmentCommand(m_doc,
                                            item->getId());
        command->addCommand(c);

	} while ( (it = m_listView->itemBelow( it )) );

    addCommandToHistory(command);
}

void
TriggerSegmentManager::slotAdd()
{
    TimeDialog dialog(this, QObject::tr("Trigger Segment Duration"),
                      &m_doc->getComposition(),
                      0, 3840, false);

    if (dialog.exec() == QDialog::Accepted) {
        addCommandToHistory(new AddTriggerSegmentCommand
                            (m_doc, dialog.getTime(), 64));
    }
}

void
TriggerSegmentManager::slotDelete()
{
    RG_DEBUG << "TriggerSegmentManager::slotDelete" << endl;

    TriggerManagerItem *item =
        dynamic_cast<TriggerManagerItem*>( m_listView->currentItem() );

    if (!item)
        return ;

    if (item->getUsage() > 0) {
        if (QMessageBox::warning(this, "", QObject::tr("This triggered segment is used %1 time(s) in the current composition.  Are you sure you want to remove it?", "", item->getUsage()),
										QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel
		   					) != QMessageBox::Yes )
            return ;
    }

    DeleteTriggerSegmentCommand *command =
        new DeleteTriggerSegmentCommand(m_doc, item->getId());

    addCommandToHistory(command);
}

void
TriggerSegmentManager::slotPasteAsNew()
{
    Clipboard *clipboard = m_doc->getClipboard();

    if (clipboard->isEmpty()) {
        QMessageBox::information(this, "", QObject::tr("Clipboard is empty"));
        return ;
    }

    addCommandToHistory(new PasteToTriggerSegmentCommand
                        (&m_doc->getComposition(),
                         clipboard,
                         "",
                         -1));
}

void
TriggerSegmentManager::slotClose()
{
    RG_DEBUG << "TriggerSegmentManager::slotClose" << endl;

//     if (m_doc)
//         CommandHistory::getInstance()->detachView(actionCollection());	//&&&
    m_doc = 0;

    close();
}

void
TriggerSegmentManager::setupActions()
{
//     KAction* close = KStandardAction::close(this,
//                                        SLOT(slotClose()),
//                                        actionCollection());
    createAction( "file_close", SLOT(slotClose()) );

    //!!! will not work right -- need to do this after createGUI call or just use hardcoded QObject::tr("Close")
    m_closeButton->setText( findAction("file_close")->text() );

    connect(m_closeButton, SIGNAL(released()), this, SLOT(slotClose()));

    createAction("paste_to_trigger_segment", SLOT(slotPasteAsNew()));

    QSettings settings;
    settings.beginGroup( TriggerManagerConfigGroup );

    int timeMode = settings.value("timemode", 0).toInt() ;
    
    QAction *a;
    a = createAction("time_musical", SLOT(slotMusicalTime()));
    if (timeMode == 0) { a->setCheckable(true); a->setChecked(true); }

    a = createAction("time_real", SLOT(slotRealTime()));
    if (timeMode == 1) { a->setCheckable(true); a->setChecked(true); }

    a = createAction("time_raw", SLOT(slotRawTime()));
    if (timeMode == 2) { a->setCheckable(true); a->setChecked(true); }

    createGUI("triggermanager.rc"); //@@@ JAS orig. 0

    settings.endGroup();
}

void
TriggerSegmentManager::addCommandToHistory(Command *command)
{
    CommandHistory::getInstance()->addCommand(command);
    setModified(false);
}

void
TriggerSegmentManager::setModified(bool modified)
{
    RG_DEBUG << "TriggerSegmentManager::setModified(" << modified << ")" << endl;

    m_modified = modified;
}

void
TriggerSegmentManager::checkModified()
{
    RG_DEBUG << "TriggerSegmentManager::checkModified(" << m_modified << ")"
    << endl;

}

void
TriggerSegmentManager::slotEdit(QTreeWidgetItem *i)
{
    RG_DEBUG << "TriggerSegmentManager::slotEdit" << endl;

    TriggerManagerItem *item =
        dynamic_cast<TriggerManagerItem*>(i);

    if (!item)
        return ;

    TriggerSegmentId id = item->getId();

    RG_DEBUG << "id is " << id << endl;

    emit editTriggerSegment(id);
}

void
TriggerSegmentManager::closeEvent(QCloseEvent *e)
{
    emit closing();
	close();
//     KMainWindow::closeEvent(e);
}

void
TriggerSegmentManager::setDocument(RosegardenGUIDoc *doc)
{
    // reset our pointers
    m_doc = doc;
    m_modified = false;

    slotUpdate();
}

void
TriggerSegmentManager::slotItemClicked(QTreeWidgetItem *item)
{
    RG_DEBUG << "TriggerSegmentManager::slotItemClicked" << endl;
}

QString
TriggerSegmentManager::makeDurationString(timeT time,
        timeT duration, int timeMode)
{
    //!!! duplication with EventView::makeDurationString -- merge somewhere?

    switch (timeMode) {

    case 0:  // musical time
        {
            int bar, beat, fraction, remainder;
            m_doc->getComposition().getMusicalTimeForDuration
            (time, duration, bar, beat, fraction, remainder);
            return QString("%1%2%3-%4%5-%6%7-%8%9   ")
                   .arg(bar / 100)
                   .arg((bar % 100) / 10)
                   .arg(bar % 10)
                   .arg(beat / 10)
                   .arg(beat % 10)
                   .arg(fraction / 10)
                   .arg(fraction % 10)
                   .arg(remainder / 10)
                   .arg(remainder % 10);
        }

    case 1:  // real time
        {
            RealTime rt =
                m_doc->getComposition().getRealTimeDifference
                (time, time + duration);
            //	return QString("%1  ").arg(rt.toString().c_str());
            return QString("%1  ").arg(rt.toText().c_str());
        }

    default:
        return QString("%1  ").arg(duration);
    }
}

void
TriggerSegmentManager::slotMusicalTime()
{
    QSettings settings;
    settings.beginGroup( TriggerManagerConfigGroup );

    settings.setValue("timemode", 0);
    slotUpdate();

    settings.endGroup();
}

void
TriggerSegmentManager::slotRealTime()
{
    QSettings settings;
    settings.beginGroup( TriggerManagerConfigGroup );

    settings.setValue("timemode", 1);
    slotUpdate();

    settings.endGroup();
}

void
TriggerSegmentManager::slotRawTime()
{
    QSettings settings;
    settings.beginGroup( TriggerManagerConfigGroup );

    settings.setValue("timemode", 2);
    slotUpdate();

    settings.endGroup();
}

}
#include "TriggerSegmentManager.moc"
