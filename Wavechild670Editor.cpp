//==============================================================================
#include "Wavechild670Editor.h"
#include "Wavechild670Processor.h"
//==============================================================================
Wavechild670Editor::Wavechild670Editor (Wavechild670Processor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      processor (ownerFilter)
{
    setSize (400, 300);
}
//------------------------------------------------------------------------------
Wavechild670Editor::~Wavechild670Editor()
{
}
//------------------------------------------------------------------------------
void Wavechild670Editor::resized ()
{
}
//------------------------------------------------------------------------------
void Wavechild670Editor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}
//==============================================================================