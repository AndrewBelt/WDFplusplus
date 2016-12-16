//==============================================================================
/**
    WDF++ "A easy-to-use collection of C++ classes for Wave Digital Filter WDF"
    ----------------------------------------------------------------------------
    Author : Maxime Coorevits (Nord, France).
    ----------------------------------------------------------------------------
    Official project location: no hub yet. 
    ----------------------------------------------------------------------------
    License: MIT License (http://www.opensource.org/licenses/mit-license.php)
    Copyright (c) 2013 by Maxime Coorevits.
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
    ----------------------------------------------------------------------------
*/
//==============================================================================
#ifndef __WDF_DEFINITION_HPP_870F9F26__
#define __WDF_DEFINITION_HPP_870F9F26__
//==============================================================================
#include "../JuceLibraryCode/JuceHeader.h"
//==============================================================================
namespace WDF {
//==============================================================================
// ** 1-PORT ** (base class for every WDF classes)
//==============================================================================
template &lt;typename T&gt;
class OnePort
{
    public:
        OnePort (T R, String n = String::empty)
            : name (n), Rp (R), a (0), b (0), port(this) {}
        //----------------------------------------------------------------------
        virtual String name () const { return name.isEmpty() ? label() : _name; }
        virtual String label () const = 0;
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) = 0;
        //----------------------------------------------------------------------
        virtual inline T reflected () = 0;
        //----------------------------------------------------------------------
        virtual T R () { return port-&gt;Rp; }       // Port resistance
        virtual T G () { return 1.0 / port-&gt;Rp; } // Port conductance (inv.Rp)
        //----------------------------------------------------------------------
        virtual void connect (OnePort&lt;T&gt;* other)
        {
            port = other; 
            other-&gt;port = this;
        }
        //----------------------------------------------------------------------
        T voltage () // v
        { 
            return (port-&gt;a + port-&gt;b) / 2.0; 
        }
        //----------------------------------------------------------------------
        T current () // i
        {
            return (port-&gt;a - port-&gt;b) / (port-&gt;Rp + port-&gt;Rp);
        } 
        //----------------------------------------------------------------------
    protected:
        //----------------------------------------------------------------------
        String _name; // Port name
        T Rp; // Port resistance
        //----------------------------------------------------------------------
        T a; // incident wave (incoming wave)
        T b; // reflected wave (outgoing wave)
        //----------------------------------------------------------------------
        OnePort&lt;T&gt;* port; // internal pointer (used for direct connect form)
        //----------------------------------------------------------------------
};
//==============================================================================
// ** 2-PORT **
//==============================================================================
template &lt;typename T&gt;
class TwoPort : public OnePort&lt;T&gt; // parent
{
    public:
        OnePort&lt;T&gt;* child;
        //----------------------------------------------------------------------
        TwoPort (String name = String::empty)
            : OnePort (1.0, name) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "2P"; }
        //----------------------------------------------------------------------
        virtual void connectParent (OnePort&lt;T&gt;* parent)
        {
            OnePort::connect (parent);
        }
        //----------------------------------------------------------------------
        virtual void connectChild (OnePort&lt;T&gt;* port) = 0;
        //----------------------------------------------------------------------
        virtual void connect (OnePort&lt;T&gt;* p, OnePort&lt;T&gt;* c)
        {
            OnePort::connect (p);
            connectChild (c);
        }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            child.port-&gt;a = child.port-&gt;reflected ();
            computeParentB ();
            return port-&gt;b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            port-&gt;a = wave; 
            computeChildB ();
            child.port-&gt;incident (child.port-&gt;b);
        }
        //----------------------------------------------------------------------
        virtual inline void computeChildB () = 0;
        virtual inline void computeParentB () = 0;
        //----------------------------------------------------------------------
};
//==============================================================================
// ** 3-PORT **
//==============================================================================
template &lt;typename T&gt;
class ThreePort : public OnePort&lt;T&gt; // adapted
{
    public:
        OnePort&lt;T&gt; *left, *right; 
        //----------------------------------------------------------------------
        ThreePort (String name = String::empty)
            : left (nullptr), right (nullptr), OnePort (1.0, name) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "3P"; }
        //----------------------------------------------------------------------
        virtual void connect (OnePort&lt;T&gt;* left, OnePort&lt;T&gt;* right) = 0;
        //----------------------------------------------------------------------
        virtual inline T reflected () = 0;
        virtual inline void incident (T wave) = 0;
        //----------------------------------------------------------------------
};
//==============================================================================
// ** SERIE **
//==============================================================================
template &lt;typename T&gt;
class Serie : public ThreePort&lt;T&gt;
{
    public:
        Serie (String name = "--")
            : ThreePort (name)
        {}
        //----------------------------------------------------------------------
        virtual String label () const { return "--"; }
        //----------------------------------------------------------------------
        virtual void connect (OnePort&lt;T&gt;* l, OnePort&lt;T&gt;* r)
        {
            left = l; right = r; 
            port-&gt;Rp = (left-&gt;R() + right-&gt;R());
        }
        //----------------------------------------------------------------------
        virtual inline T reflected () 
        { 
            port-&gt;b = -(left-&gt;port-&gt;reflected() 
                     + right-&gt;port-&gt;reflected()); 
            return port-&gt;b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) 
        {
            register T lrW = (wave + left-&gt;port-&gt;b + right-&gt;port-&gt;b);
             left-&gt;port-&gt;incident ( left-&gt;port-&gt;b - ( left-&gt;R()/port-&gt;R()) * lrW);
            right-&gt;port-&gt;incident (right-&gt;port-&gt;b - (right-&gt;R()/port-&gt;R()) * lrW);
            port-&gt;a = wave;
        }
        //----------------------------------------------------------------------
};
//==============================================================================
// ** PARALLEL **
//==============================================================================
template &lt;typename T&gt;
class Parallel : public ThreePort&lt;T&gt;
{
    public:
        Parallel (String name = "||")
            : ThreePort (name)
        {}
        //----------------------------------------------------------------------
        virtual String label () const { return "||"; }
        //----------------------------------------------------------------------
        virtual void connect (OnePort&lt;T&gt;* l, OnePort&lt;T&gt;* r)
        {
            left = l; right = r; 
            port-&gt;Rp = (left-&gt;R() * right-&gt;R()) 
                     / (left-&gt;R() + right-&gt;R());
        }
        //----------------------------------------------------------------------
        virtual inline T reflected () 
        { 
            register T lrG = left-&gt;G() + right-&gt;G();
            port-&gt;b = ( left-&gt;G()/lrG) *  left-&gt;port-&gt;reflected() + 
                      (right-&gt;G()/lrG) * right-&gt;port-&gt;reflected();
            return port-&gt;b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) 
        {
            register T lrG = left-&gt;G() + right-&gt;G();
            register T lrW = (wave + left-&gt;port-&gt;b + right-&gt;port-&gt;b);
             left-&gt;port-&gt;incident ( left-&gt;port-&gt;b - ( left-&gt;G()/lrG) * lrW);
            right-&gt;port-&gt;incident (right-&gt;port-&gt;b - (right-&gt;G()/lrG) * lrW);
            port-&gt;a = wave;
        }
        //----------------------------------------------------------------------
};
//==============================================================================
// ** RESISTOR **
//==============================================================================
template &lt;typename T&gt;
class Resistor : public OnePort&lt;T&gt;
{
    public:
        Resistor (T R, String name = String::empty)
            : OnePort (R, name) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "R"; }
        //----------------------------------------------------------------------
        virtual inline T reflected () 
        {
            port-&gt;b = 0; return port-&gt;b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) 
        {
            port-&gt;a = wave;
        }
        //----------------------------------------------------------------------
};
//==============================================================================
// ** CAPACITOR **
//==============================================================================
template &lt;typename T&gt;
class Capacitor : public OnePort&lt;T&gt;
{
    public:
        Capacitor (T C, T Fs, String name = String::empty)
            : OnePort (Fs/2.0*C, name), state (0) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "C"; }
        //----------------------------------------------------------------------
        virtual inline T reflected () 
        {
            port-&gt;b = state; return port-&gt;b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) 
        {
            port-&gt;a = wave; state = port-&gt;a;
        }
        //----------------------------------------------------------------------
    private:
        T state; 
        //----------------------------------------------------------------------
};
//==============================================================================
// ** INDUCTOR **
//==============================================================================
template &lt;typename T&gt;
class Inductor : public OnePort&lt;T&gt;
{
    public:
        Inductor (T L, T Fs, String name = String::empty)
            : OnePort (2.0*L/Fs, name), state (0) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "L"; }
        //----------------------------------------------------------------------
        virtual inline T reflected () 
        {
            port-&gt;b = -state; return port-&gt;b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) 
        {
            port-&gt;a = wave; state = port-&gt;a;
        }
        //----------------------------------------------------------------------
    private:
        T state; 
        //----------------------------------------------------------------------
};
//==============================================================================
// ** OPEN CIRCUIT **
//==============================================================================
template &lt;typename T&gt;
class OpenCircuit : public OnePort&lt;T&gt;
{
    public:
        OpenCircuit (T R, String name = String::empty)
            : OnePort (R, name) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "Oc"; }
        //----------------------------------------------------------------------
        virtual inline T reflected () 
        {
            port-&gt;b = port-&gt;a; return port-&gt;b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) 
        {
            port-&gt;a = wave;
        }
        //----------------------------------------------------------------------
};
//==============================================================================
// ** SHORT CIRCUIT **
//==============================================================================
template &lt;typename T&gt;
class ShortCircuit : public OnePort&lt;T&gt;
{
    public:
        ShortCircuit (T R, String name = String::empty)
            : OnePort (R, name) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "Sc"; }
        //----------------------------------------------------------------------
        virtual inline T reflected () 
        {
            port-&gt;b = -port-&gt;a; return port-&gt;b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) 
        {
            port-&gt;a = wave; 
        }
        //----------------------------------------------------------------------
};
//==============================================================================
// ** VOLTAGE SOURCE **
//==============================================================================
template &lt;typename T&gt;
class VoltageSource : public OnePort&lt;T&gt;
{
    public:
        VoltageSource (T V, T R, String name = String::empty)
            : OnePort (R, name), Vs (V) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "Vs"; }
        //----------------------------------------------------------------------
        virtual inline T reflected () 
        {
            port-&gt;b = -port-&gt;a + 2.0 * Vs; return port-&gt;b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) 
        {
            port-&gt;a = wave;
        }
        //----------------------------------------------------------------------
    private:
        T Vs; 
        //----------------------------------------------------------------------
};
//==============================================================================
// ** CURRENT SOURCE **
//==============================================================================
template &lt;typename T&gt;
class CurrentSource : public OnePort&lt;T&gt;
{
    public:
        CurrentSource (T I, T R, String name = String::empty)
            : OnePort (R, name), Is (I) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "Is"; }
        //----------------------------------------------------------------------
        virtual inline T reflected () 
        {
            port-&gt;b = port-&gt;a + 2.0 * R() * Is; return port-&gt;b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) 
        {
            port-&gt;a = wave;
        }
        //----------------------------------------------------------------------
    private:
        T Is; 
        //----------------------------------------------------------------------
};
//==============================================================================
// ** IDEAL TRANSFORMER **
//==============================================================================
template &lt;typename T&gt;
class IdealTransformer : public TwoPort&lt;T&gt;
{
    public:
        IdealTransformer (T ratio, String name = String::empty)
            : TwoPort (1.0, name), N (ratio) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "][" }
        //----------------------------------------------------------------------
        virtual void connectChild (OnePort&lt;T&gt;* port)
        {
            T Rs = port.R();
            port-&gt;Rp = Rs / (n*n);
            child.Rp = Rs; 
            TwoPort::connectChild (port);
        }
        //----------------------------------------------------------------------
        virtual inline void computeChildB ()
        {
            child.port-&gt;b = port-&gt;a * (1.0/N);
        }
        //----------------------------------------------------------------------
        virtual inline void computeParentB ()
        {
            port-&gt;b = child.port-&gt;a * N;
        }
        //----------------------------------------------------------------------
    private:
        T N; 
        //----------------------------------------------------------------------
};
//==============================================================================
/**
    EXTRA TEMPLATES
    ---------------
    
    Not direct WDF classes but interfaces for your non-linear blackboxes. 
    
**/
//==============================================================================
// ** Newton/Raphson ** (implicit equation solver)
//==============================================================================
template &lt;typename T&gt;
class NewtonRaphson
{
    public:
        NewtonRaphson (T guess = 100.0) : xguess (guess) {}
        //----------------------------------------------------------------------
        inline T solve (int max_iter = 100, T epsilon = 1e-9)
        {
            T x = xguess;
            T err = 1e6;
            int iteration = 0;
            while (fabs(err) / fabs(x) &gt; epsilon)
            {
                xguess = iterate (x);
                err = x - xguess;
                x = xguess;
                if (iteration &gt; max_iter) break;
                ++iteration;
            }
            return x;
        }
        //----------------------------------------------------------------------
        inline T iterate (T x, T dx = 1e-6)
        {
            T F = evaluate (x);
            T xNew = x - dx*F / (evaluate (x + dx) - F);
            return xNew;
        }
        //----------------------------------------------------------------------
        virtual inline T evaluate (T x) = 0; // declare your implicit equation
        //----------------------------------------------------------------------
    private : 
        T xguess;
        //----------------------------------------------------------------------
};
//==============================================================================
} // namespace WDF
//==============================================================================
#endif  // __WDF_DEFINITION_HPP_870F9F26__
//==============================================================================