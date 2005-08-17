// -*- c-basic-offset: 4 -*-

/*
    Rosegarden-4
    A sequencer and musical notation editor.

    This program is Copyright 2000-2005
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <bownie@bownie.com>

    The moral right of the authors to claim authorship of this work
    has been asserted.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _PLUGIN_FACTORY_H_
#define _PLUGIN_FACTORY_H_

#include <qstring.h>
#include <vector>

#include "MappedCommon.h"

namespace Rosegarden
{

class RunnablePluginInstance;
class MappedPluginSlot;

class PluginFactory
{
public:
    static PluginFactory *instance(QString pluginType);
    static PluginFactory *instanceFor(QString identifier);
    static void enumerateAllPlugins(MappedObjectPropertyList &);

    static void setSampleRate(int sampleRate) { m_sampleRate = sampleRate; }

    /**
     * Look up the plugin path and find the plugins in it.  Called 
     * automatically after construction of a factory.
     */
    virtual void discoverPlugins() = 0;

    /**
     * Return a reference to a list of all plugin identifiers that can
     * be created by this factory.
     */
    virtual const std::vector<QString> &getPluginIdentifiers() const = 0;

    /**
     * Append to the given list descriptions of all the available
     * plugins and their ports.  This is in a standard format, see
     * the LADSPA implementation for details.
     */
    virtual void enumeratePlugins(MappedObjectPropertyList &list) = 0;

    /**
     * Populate the given plugin slot with information about its
     * plugin.  This is called from the plugin slot's set method
     * when it's been asked to set its plugin identifier.  This
     * method should also destroy and recreate the plugin slot's
     * port child objects.
     */
    virtual void populatePluginSlot(QString identifier,
				    MappedPluginSlot &slot) = 0;

    /**
     * Instantiate a plugin.
     */
    virtual RunnablePluginInstance *instantiatePlugin(QString identifier,
						      int instrumentId,
						      int position,
						      unsigned int sampleRate,
						      unsigned int blockSize,
						      unsigned int channels) = 0;

protected:
    PluginFactory() { }

    // for call by RunnablePluginInstance dtor
    virtual void releasePlugin(RunnablePluginInstance *, QString identifier) = 0;
    friend class RunnablePluginInstance;

    static int m_sampleRate;
};


}

#endif
