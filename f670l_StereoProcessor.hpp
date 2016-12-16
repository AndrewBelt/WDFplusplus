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
#ifndef __F670L_STEREO_PROCESSOR_HPP_9D267F6C__
#define __F670L_STEREO_PROCESSOR_HPP_9D267F6C__
//==============================================================================
#include "f670l_SignalAmplifier.hpp"
#include "f670l_LevelTimeConstant.hpp"
#include "f670l_SidechainAmplifier.hpp"
//==============================================================================
namespace Wavechild670 {
//==============================================================================
#define SQRT_2 sqrt(2.0)
//==============================================================================
template <typename T>
class StereoProcessor
{
    public:
        StereoProcessor ()
            : Fs (44100.0),       gain (1.0),
              //-------------------------
                   A (0.0),          B (0.0),
                capA (0.0),       capB (0.0),
              levelA (1.0),     levelB (1.0),
          thresholdA (1.0), thresholdB (1.0),
                 tcA (2),          tcB (2),
              //-------------------------
              hardclipout (true),
                 feedback (false),
                  midside (false),
                   linked (true)
        {}
        //----------------------------------------------------------------------
        void init (T sampleRate)
        {
            Fs = sampleRate;
            //------------------------------------------------------------------
            signalAmpA = new SignalAmplifier<double> (Fs);
            signalAmpB = new SignalAmplifier<double> (Fs);
            //------------------------------------------------------------------
            sidechainAmpA = new SidechainAmplifier<double> (Fs);
            sidechainAmpB = new SidechainAmplifier<double> (Fs);
            //------------------------------------------------------------------
            timeConstantA = new LevelTimeConstant<double> (Fs);
            timeConstantB = new LevelTimeConstant<double> (Fs);
            //------------------------------------------------------------------
            timeConstantA->parameters (Fs, tcA);
            timeConstantB->parameters (Fs, tcB);
            //------------------------------------------------------------------
            capA = A = 0.0;
            capB = B = 0.0;
            //------------------------------------------------------------------
            warmup ();
        }
        //----------------------------------------------------------------------
        void parameters (const int tA, const int tB)
        {
            tcA = tA; timeConstantA->parameters (Fs, tcA);
            tcB = tB; timeConstantB->parameters (Fs, tcB);
        }
        //----------------------------------------------------------------------
        inline T sidechain (T VscA, T VscB)
        {
            T IscA = sidechainAmpA->process (VscA, capA);
            T IscB = sidechainAmpB->process (VscB, capB);

            if (linked)
            {
                T IscT = (IscA + IscB) * 0.5;
                T Ax = timeConstantA->process (IscT);
                T Bx = timeConstantB->process (IscT);
                capA =
                capB = (Ax + Bx) * 0.5;
            }
            else
            {
                capA = timeConstantA->process (IscA);
                capB = timeConstantA->process (IscB);
            }
        }
        //----------------------------------------------------------------------
        inline T hardclip (T x, T min, T max) { return (x < min) ? min
                                                     : (x > max) ? max
                                                     :             x; }
        //----------------------------------------------------------------------
        inline void process (float *left, float *right)
        {
            A = (midside) ? (T)((left[0]  + left[0] ) / SQRT_2) : left[0];
            B = (midside) ? (T)((right[0] - right[0]) / SQRT_2) : right[0];

            A *= levelA;
            B *= levelB;

            if (!feedback) sidechain (A, B);

            A = signalAmpA->process (A, capA);
            B = signalAmpB->process (B, capB);

            if ( feedback) sidechain (A, B);

            A = (midside) ? (A + B) / SQRT_2 : A;
            B = (midside) ? (A - B) / SQRT_2 : B;

            A *= gain;
            B *= gain;

            A = (hardclipout) ? hardclip(A, -1.0, 1.0) : A;
            B = (hardclipout) ? hardclip(B, -1.0, 1.0) : B;

            left[0]  = (float)A;
            right[0] = (float)B;
        }
        //----------------------------------------------------------------------
        void warmup (T timeInSec = 0.5)
        {
            long i, samples = (long) (timeInSec*Fs)/2;
            i = 0; for (; i < samples; ++i) { signalAmpA->process (0.0, capA);
                                              signalAmpB->process (0.0, capB); }
            i = 0; for (; i < samples; ++i) { T VscA = signalAmpA->process (0.0, capA);
                                              T VscB = signalAmpB->process (0.0, capB);
                                              sidechain (VscA, VscB); }
        }
        //----------------------------------------------------------------------
        T Fs; // samplerate
        //----------------------------------------------------------------------
        int tcA, tcB;
        bool hardclipout, midside, linked, feedback;
        T A, B, capA, capB, levelA, levelB, thresholdA, thresholdB, gain;
        //----------------------------------------------------------------------
        ScopedPointer<SignalAmplifier<T>>    signalAmpA;
        ScopedPointer<SignalAmplifier<T>>    signalAmpB;
        //----------------------------------------------------------------------
        ScopedPointer<LevelTimeConstant<T>>  timeConstantA;
        ScopedPointer<LevelTimeConstant<T>>  timeConstantB;
        //----------------------------------------------------------------------
        ScopedPointer<SidechainAmplifier<T>> sidechainAmpA;
        ScopedPointer<SidechainAmplifier<T>> sidechainAmpB;
        //----------------------------------------------------------------------
};
//==============================================================================
#undef SQRT_2
//==============================================================================
} // namespace Wavechild670
//==============================================================================
#endif  // __F670L_STEREO_PROCESSOR_HPP_9D267F6C__
//==============================================================================