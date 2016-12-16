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
#ifndef __F670L_LEVELTIMECONSTANT_HPP_E666EC81__
#define __F670L_LEVELTIMECONSTANT_HPP_E666EC81__
//==============================================================================
#include "WDF++.hpp"
//==============================================================================
namespace Wavechild670 {
//==============================================================================
//----------------------------------------------------------------------
// Level Time Constant 6-way switch parameters (time from 10dB limiting)
//----------------------------------------------------------------------
static const double ltc[6][6] =
{
    //------------------------------------------------------------------
    // CT    CU        CV        RT        RU       RV   | Release Time
    //------------------------------------------------------------------
    { 2e-6, 8e-6, 20e-6,  51.9e3,  10e9,  10e9 }, // 0.3s
    { 2e-6, 8e-6, 20e-6, 149.9e3,  10e9,  10e9 }, // 0.8s
    { 4e-6, 8e-6, 20e-6,   220e3,  10e9,  10e9 }, // 2.0s
    { 8e-6, 8e-6, 20e-6,   220e3,  10e9,  10e9 }, // 5.0s
    { 4e-6, 8e-6, 20e-6,   220e3, 100e3,  10e9 }, // 2.0s / 10.0s
    { 2e-6, 8e-6, 20e-6,   220e3, 100e3, 100e3 }  // 0.3s / 5.0s / 25.0s
    //------------------------------------------------------------------
};
//==============================================================================
template <typename T>
class LevelTimeConstant
{
    public:
        LevelTimeConstant (T Fs)
            : //----------------------------------------------------------------
              R1 (220e3, "RT"),
              R2 (  1e9, "RU"),
              R3 (  1e9, "RV"),
              //----------------------------------------------------------------
              C1 ( 2e-6, Fs, "CT"),
              C2 ( 8e-6, Fs, "CU"),
              C3 (20e-6, Fs, "CV"),
              //----------------------------------------------------------------
        {
            wiring ();
        }
        //----------------------------------------------------------------------
        void parameters (T Fs, const int index)
        {
            jassert(index >= 0 && index < 6);
            update (Fs, ltc[index][0], ltc[index][1], ltc[index][2],
                        ltc[index][3], ltc[index][4], ltc[index][5]);
        }
        //----------------------------------------------------------------------
        T process (T Iin) // Iin == current (current law apply)
        {
            root.incident (root.reflected() - (2.0*(Iin * root.R())));
            return C1.voltage();
        }
        //----------------------------------------------------------------------
    protected:
        WDF::Resistor<T>    R1, R2, R3;
        WDF::Capacitor<T>   C1, C2, C3;
        //----------------------------------------------------------------------
        WDF::Serie<T>       serie_A;
        WDF::Serie<T>       serie_B;
        WDF::Parallel<T>    paral_A;
        WDF::Parallel<T>    paral_B;
        WDF::Parallel<T>    root;
        //----------------------------------------------------------------------
        /**
                --------------------------
                |       |    |     |     |
                |       |    |     R2    R3
              root      R1   C1    |     |
                |       |    |     C2    C3
                |       |    |     |     |
                --------------------------
        **/
        //----------------------------------------------------------------------
        inline void wiring ()
        {
            paral_A.connect (&R1,       &C1);
            serie_A.connect (&R2,       &C2);
            serie_B.connect (&R3,       &C3);
            paral_B.connect (&serie_A,  &serie_B);
               root.connect (&paral_A,  &paral_B);
        }
        //----------------------------------------------------------------------
        void update (T Fs, T CT = 2e-6,  T CU = 8e-6, T CV = 20e-6,
                           T RT = 220e3, T RU = 1e9,  T RV = 1e9)
        {
            T hFs = FS*.5;
            //------------------------------------------------------------------
            C1.Rp = hFs*CT;
            C2.Rp = hFs*CU;
            C3.Rp = hFs*CV;
            //------------------------------------------------------------------
            R1.Rp = RT;
            R2.Rp = RU;
            R3.Rp = RV;
            //------------------------------------------------------------------
            wiring ();
        }
        //----------------------------------------------------------------------
};
//==============================================================================
} // namespace Wavechild670
//==============================================================================
#endif  // __F670L_LEVELTIMECONSTANT_HPP_E666EC81__
//==============================================================================