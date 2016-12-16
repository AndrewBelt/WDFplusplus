# WDF++

This project is derived from code written by maxprod from the JUCE forums.
https://forum.juce.com/t/wdf-new-restructuration-project-audio-processor/13104

The file `WDF++.hpp` is MIT-licensed. The other files implementing a Fairchild 670 Limiter can be used as a resource for usage but are not licensed explicitly.

>maxprod
>July 2, 2014
>
>Hi,
>
>I'm back with some new about my WDF++ project that is in stand-by since few months but with enough information to share with those who want to use this project.
>
>The new version correct some little bugs, now the main class is named OnePort and i've add a IdealTransformer and a NewtonRaphson class easy to use to solve implicit equations.
>
>---
>
>The project is based on the DAFX-12 paper of Peter Raffensperger named : Toward a Wave Digital Filter Model of the Fairchild 670 Limiter.
>
>The Fairchild 670 Limiter is a 50's stereo compressor that is know as "the mother of all compressor", a part of the sound of The Beatles. Today Fairchild is dead and the compressor is a high-price vintage rarity.
>
>Peter explore the Wave Digital Filter model to generate a pretty good approximation of the sound of the original compressor.
>
>If you want to read the paper : http://dafx12.york.ac.uk/papers/dafx12_submission_9.pdf
>
>The idea is to write the circuit in WDF++ style and put the processor in a Juce AudioProcessor and by extension, a plugin. With a GUI that is already in progress, i need to work on the Stockli buttons (rotary button) in 3D, because i'm not a expert with Photoshop to generate photo-realistic 2D buttons. The main plate with a vintage style :
>
>![](http://img11.hostingpics.net/pics/787046Wavechild670.png)
>
>---
>
>I repeat that the project is not yet functionnal, but shared only in relation with the new WDF++ project, to help you to understand how to use the classes. I hope to find time to finish a definitive version of the plugin, but not before the end of the year.
>
>In all case, the source code is already a good reference i think, better than the 5 lines of code of the initial diode clipper.
>
>Feel free to use the WDF++ project, it's for the humanity benefice.
>
>.oO Maxprod Oo.