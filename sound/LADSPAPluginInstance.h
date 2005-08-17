// -*- c-indentation-style:"stroustrup" c-basic-offset: 4 -*-

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

#include <vector>
#include <set>
#include <qstring.h>
#include "Instrument.h"

#ifndef _LADSPAPLUGININSTANCE_H_
#define _LADSPAPLUGININSTANCE_H_

#ifdef HAVE_LADSPA

#include <ladspa.h>
#include "RunnablePluginInstance.h"

namespace Rosegarden
{

// LADSPA plugin instance.  LADSPA is a variable block size API, but
// for one reason and another it's more convenient to use a fixed
// block size in this wrapper.
//
class LADSPAPluginInstance : public RunnablePluginInstance
{
public:
    virtual ~LADSPAPluginInstance();

    virtual bool isOK() const { return m_instanceHandles.size() != 0; }

    Rosegarden::InstrumentId getInstrument() const { return m_instrument; }
    virtual QString getIdentifier() const { return m_identifier; }
    int getPosition() const { return m_position; }

    virtual void run(const RealTime &rt);

    virtual void setPortValue(unsigned int portNumber, float value);
    virtual float getPortValue(unsigned int portNumber);

    virtual size_t getBufferSize() { return m_blockSize; }
    virtual size_t getAudioInputCount() { return m_instanceCount * m_audioPortsIn.size(); }
    virtual size_t getAudioOutputCount() { return m_instanceCount * m_audioPortsOut.size(); }
    virtual sample_t **getAudioInputBuffers() { return m_inputBuffers; }
    virtual sample_t **getAudioOutputBuffers() { return m_outputBuffers; }

    virtual bool isBypassed() const { return m_bypassed; }
    virtual void setBypassed(bool bypassed) { m_bypassed = bypassed; }

    virtual size_t getLatency();

    virtual void silence();
    virtual void setIdealChannelCount(size_t channels); // may re-instantiate

protected:
    // To be constructed only by LADSPAPluginFactory
    friend class LADSPAPluginFactory;

    // Constructor that creates the buffers internally
    // 
    LADSPAPluginInstance(PluginFactory *factory,
			 Rosegarden::InstrumentId instrument,
			 QString identifier,
                         int position,
			 unsigned long sampleRate,
			 size_t blockSize,
			 int idealChannelCount,
                         const LADSPA_Descriptor* descriptor);

    // Constructor that uses shared buffers
    // 
    LADSPAPluginInstance(PluginFactory *factory,
			 Rosegarden::InstrumentId instrument,
			 QString identifier,
                         int position,
			 unsigned long sampleRate,
			 size_t blockSize,
			 sample_t **inputBuffers,
			 sample_t **outputBuffers,
                         const LADSPA_Descriptor* descriptor);

    void init(int idealChannelCount = 0);
    void instantiate(unsigned long sampleRate);
    void cleanup();
    void activate();
    void deactivate();

    // Connection of data (and behind the scenes control) ports
    //
    void connectPorts();
    
    Rosegarden::InstrumentId   m_instrument;
    int                        m_position;
    std::vector<LADSPA_Handle> m_instanceHandles;
    size_t                     m_instanceCount;
    const LADSPA_Descriptor   *m_descriptor;

    std::vector<std::pair<unsigned long, LADSPA_Data*> > m_controlPortsIn;
    std::vector<std::pair<unsigned long, LADSPA_Data*> > m_controlPortsOut;

    std::vector<int>          m_audioPortsIn;
    std::vector<int>          m_audioPortsOut;

    size_t                    m_blockSize;
    sample_t                **m_inputBuffers;
    sample_t                **m_outputBuffers;
    bool                      m_ownBuffers;
    size_t                    m_sampleRate;
    float                    *m_latencyPort;
    bool                      m_run;
    
    bool                      m_bypassed;
};

}

#endif // HAVE_LADSPA

#endif // _LADSPAPLUGININSTANCE_H_

