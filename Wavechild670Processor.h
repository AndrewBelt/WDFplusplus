//==============================================================================
/**
    Wavechild 670
    -------------
    Wave digital filter based emulation of a famous 1950's tube stereo limiter

    WDF++ based source code by Maxime Coorevits (Nord, France) in 2013
    
    Some part are inspired by the Peter Raffensperger project: Wavechild670,
    a command line with python WDF generator that produce C++ code of the circuit. 
    
    Major restructuration:
    ----------------------
        * WDF++ based project (single WDF++.hpp file)
        * full C++, zero-dependencies except JUCE (core API, AudioProcessor).
        * JUCE Plugin wrapper processor (VST, AU ...)
        * Photo-Realistic GUI
    
    Reference:
    ----------
    Toward a Wave Digital Filter Model of the Fairchild 670 Limiter, 
    Raffensperger, P. A., (2012). 
    Proc. of the 15th International Conference on Digital Audio Effects (DAFx-12), 
    York, UK, September 17-21, 2012.
    Note:
    -----
    Fairchild (R) a registered trademark of Avid Technology, Inc., 
    which is in no way associated or affiliated with the author.
    
**/
//==============================================================================
#ifndef __WAVECHILD670PROCESSOR_H_E9BC67EE__
#define __WAVECHILD670PROCESSOR_H_E9BC67EE__
//==============================================================================
#include "../JuceLibraryCode/JuceHeader.h"
//==============================================================================
#include "f670l_StereoProcessor.hpp"
//==============================================================================
class Wavechild670Editor;
//==============================================================================
class Wavechild670Processor : public AudioProcessor
{
    public:
        Wavechild670Processor ();
        ~Wavechild670Processor ();
        //======================================================================
        void prepareToPlay (double sampleRate, int samplesPerBlock);
        void releaseResources ();
        void processBlock (AudioSampleBuffer&amp; buffer, MidiBuffer&amp; midiMessages);
        //======================================================================
        AudioProcessorEditor* createEditor ();
        bool hasEditor () const;
        //======================================================================
        const String getName () const;
        //======================================================================
        int getNumParameters ();
        float getParameter (int index);
        void setParameter (int index, float newValue);
        const String getParameterName (int index);
        const String getParameterText (int index);
        //======================================================================
        const String getInputChannelName (int channelIndex) const;
        const String getOutputChannelName (int channelIndex) const;
        bool isInputChannelStereoPair (int index) const;
        bool isOutputChannelStereoPair (int index) const;
        //======================================================================
        bool acceptsMidi () const;
        bool producesMidi () const;
        bool silenceInProducesSilenceOut () const;
        double getTailLengthSeconds() const;
        //======================================================================
        int getNumPrograms ();
        int getCurrentProgram ();
        void setCurrentProgram (int index);
        const String getProgramName (int index);
        void changeProgramName (int index, const String&amp; newName);
        //======================================================================
        void getStateInformation (MemoryBlock&amp; destData);
        void setStateInformation (const void* data, int sizeInBytes);
        //======================================================================
    private:
        //======================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wavechild670Processor)
        //======================================================================
        ScopedPointer&lt;Wavechild670::StereoProcessor&lt;double&gt;&gt; wc670s; 
        double Fs;
        bool isInit;
        //======================================================================
};
//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Wavechild670Processor();
}
//==============================================================================
#endif  // __WAVECHILD670PROCESSOR_H_E9BC67EE__
//==============================================================================