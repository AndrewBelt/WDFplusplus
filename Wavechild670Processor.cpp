//==============================================================================
#include "Wavechild670Processor.h"
#include "Wavechild670Editor.h"
//==============================================================================
Wavechild670Processor::Wavechild670Processor ()
    : wc670s (new Wavechild670::StereoProcessor<double>()),
      isInit (false),
      Fs(0)
{
}
//------------------------------------------------------------------------------
Wavechild670Processor::~Wavechild670Processor ()
{
}
//==============================================================================
void Wavechild670Processor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    if (!isInit || sampleRate != Fs)
    {
        isInit = true;
        wc670s->init (sampleRate);
        Fs = sampleRate;
    }
}
//------------------------------------------------------------------------------
void Wavechild670Processor::releaseResources ()
{
}
//==============================================================================
void Wavechild670Processor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    int ni = getNumInputChannels();
    int no = getNumOutputChannels();
    if (ni == 2)
    {
        float *left = buffer.getSampleData(0, 0);
        float *right = buffer.getSampleData(1, 0);
        int i=0; int max=buffer.getNumSamples();
        for (;i<max;++i)
        {
            wc670s->process (&left[i], &right[i]);
        }
    }
    int i = ni;
    for (; i < no; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
}
//==============================================================================
AudioProcessorEditor* Wavechild670Processor::createEditor()
{
    return new Wavechild670Editor (this);
}
//------------------------------------------------------------------------------
bool Wavechild670Processor::hasEditor() const
{
    return true;
}
//==============================================================================
const String Wavechild670Processor::getName() const
{
    return "Wavechild670";
}
//==============================================================================
int Wavechild670Processor::getNumParameters()
{
    return 11;
}
//------------------------------------------------------------------------------
float Wavechild670Processor::getParameter (int index)
{
    switch (index)
    {
        case  0: return (wc670s->levelA);                   // -20db CCW
        case  1: return (wc670s->thresholdA);               // 0-10 CW
        case  2: return (float(wc670s->tcA) / 10.f);        // 6-switch
        //----------------------------------------------------------------------
        case  3: return (wc670s->levelB);                   // -20db CCW
        case  4: return (wc670s->thresholdB);               // 0-10 CW
        case  5: return (float(wc670s->tcB) / 10.f);        // 6-switch
        //----------------------------------------------------------------------
        case  6: return (wc670s->feedback) ? 1.0f : 0.0f;
        case  7: return (wc670s->midside) ? 1.0f : 0.0f;
        case  8: return (wc670s->linked) ? 1.0f : 0.0f;
        //----------------------------------------------------------------------
        case  9: return (wc670s->gain);
        case 10: return (wc670s->hardclipout) ? 1.0f : 0.0f;
        //----------------------------------------------------------------------
        default: return 0.0f;
    };
}
//------------------------------------------------------------------------------
void Wavechild670Processor::setParameter (int index, float newValue)
{
    switch (index)
    {
        case  0: wc670s->levelA = newValue; break;
        case  1: wc670s->thresholdA = newValue; break;
        case  2: wc670s->parameters (int(newValue * 10.f), wc670s->tcB);
        //----------------------------------------------------------------------
        case  3: wc670s->levelB = newValue; break;
        case  4: wc670s->thresholdB = newValue; break;
        case  5: wc670s->parameters (wc670s->tcA, int(newValue * 10.f));
        //----------------------------------------------------------------------
        case  6: wc670s->feedback = (newValue > 0.5f) ? true : false; break;
        case  7: wc670s->midside = (newValue > 0.5f) ? true : false; break;
        case  8: wc670s->linked = (newValue > 0.5f) ? true : false; break;
        //----------------------------------------------------------------------
        case  9: wc670s->gain = newValue; break;
        case 10: wc670s->hardclipout = (newValue > 0.5f) ? true : false;
        //----------------------------------------------------------------------
        default: break;
    };
}
//------------------------------------------------------------------------------
const String Wavechild670Processor::getParameterName (int index)
{
    switch (index)
    {
        case  0: return "A|Input Gain";
        case  1: return "A|Threshold";
        case  2: return "A|Time Constant";
        //----------------------------------------------------------------------
        case  3: return "B|Input Gain";
        case  4: return "B|Threshold";
        case  5: return "B|Time Constant";
        //----------------------------------------------------------------------
        case  6: return "Feedback Topology";
        case  7: return "Midside Coupling";
        case  8: return "Sidechain Link";
        //----------------------------------------------------------------------
        case  9: return "Output Gain";
        case 10: return "Hardclip Output";
        //----------------------------------------------------------------------
        default: return "Undefined";
    };
}
//------------------------------------------------------------------------------
const String Wavechild670Processor::getParameterText (int index)
{
    switch (index)
    {
        case  0: return "";
        case  1: return "";
        case  2: return String(wc670s->tcA);
        //----------------------------------------------------------------------
        case  3: return "";
        case  4: return "";
        case  5: return String(wc670s->tcB);
        //----------------------------------------------------------------------
        case  6: return (wc670s->hardclipout) ? "ON" : "OFF";
        case  7: return (wc670s->hardclipout) ? "ON" : "OFF";
        case  8: return (wc670s->hardclipout) ? "ON" : "OFF";
        //----------------------------------------------------------------------
        case  9: return ""; // min -50.0 max 10.0 ?
        case 10: return (wc670s->hardclipout) ? "ON" : "OFF";
        //----------------------------------------------------------------------
        default: return "Undefined";
    };
}
//==============================================================================
const String Wavechild670Processor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}
//------------------------------------------------------------------------------
const String Wavechild670Processor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}
//------------------------------------------------------------------------------
bool Wavechild670Processor::isInputChannelStereoPair (int index) const
{
    return true;
}
//------------------------------------------------------------------------------
bool Wavechild670Processor::isOutputChannelStereoPair (int index) const
{
    return true;
}
//==============================================================================
bool Wavechild670Processor::acceptsMidi() const
{
    return false;
}
//------------------------------------------------------------------------------
bool Wavechild670Processor::producesMidi() const
{
    return false;
}
//------------------------------------------------------------------------------
bool Wavechild670Processor::silenceInProducesSilenceOut() const
{
    return true;
}
//==============================================================================
double Wavechild670Processor::getTailLengthSeconds() const
{
    return 0;
}
//==============================================================================
int Wavechild670Processor::getNumPrograms()
{
    return 0;
}
//------------------------------------------------------------------------------
int Wavechild670Processor::getCurrentProgram()
{
    return 0;
}
//------------------------------------------------------------------------------
void Wavechild670Processor::setCurrentProgram (int index)
{
}
//------------------------------------------------------------------------------
const String Wavechild670Processor::getProgramName (int index)
{
    return String::empty;
}
//------------------------------------------------------------------------------
void Wavechild670Processor::changeProgramName (int index, const String& newName)
{
}
//==============================================================================
void Wavechild670Processor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}
//----------------------------------------------------------------------
void Wavechild670Processor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}
//==============================================================================