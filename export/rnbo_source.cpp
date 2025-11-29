/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#include "RNBO_Common.h"
#include "RNBO_AudioSignal.h"

namespace RNBO {


#define trunc(x) ((Int)(x))

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

class rnbomatic : public PatcherInterfaceImpl {
public:

rnbomatic()
{
}

~rnbomatic()
{
}

rnbomatic* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
}

template <typename T> void listquicksort(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template <typename T> Int listpartition(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template <typename T> void listswapelements(T& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

inline number cubicinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = z - y - w + x;
    number f1 = w - x - f0;
    number f2 = y - w;
    number f3 = x;
    return f0 * a * a2 + f1 * a2 + f2 * a + f3;
}

inline number splineinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = -0.5 * w + 1.5 * x - 1.5 * y + 0.5 * z;
    number f1 = w - 2.5 * x + 2 * y - 0.5 * z;
    number f2 = -0.5 * w + 0.5 * y;
    return f0 * a * a2 + f1 * a2 + f2 * a + x;
}

inline number cosT8(number r) {
    number t84 = 56.0;
    number t83 = 1680.0;
    number t82 = 20160.0;
    number t81 = 2.4801587302e-05;
    number t73 = 42.0;
    number t72 = 840.0;
    number t71 = 1.9841269841e-04;

    if (r < 0.785398163397448309615660845819875721 && r > -0.785398163397448309615660845819875721) {
        number rr = r * r;
        return 1.0 - rr * t81 * (t82 - rr * (t83 - rr * (t84 - rr)));
    } else if (r > 0.0) {
        r -= 1.57079632679489661923132169163975144;
        number rr = r * r;
        return -r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    } else {
        r += 1.57079632679489661923132169163975144;
        number rr = r * r;
        return r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    }
}

inline number cosineinterp(number frac, number x, number y) {
    number a2 = (1.0 - this->cosT8(frac * 3.14159265358979323846)) / (number)2.0;
    return x * (1.0 - a2) + y * a2;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

number samplerate() {
    return this->sr;
}

Index vectorsize() {
    return this->vs;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

inline number safediv(number num, number denom) {
    return (denom == 0.0 ? 0.0 : num / denom);
}

number safepow(number base, number exponent) {
    return fixnan(rnbo_pow(base, exponent));
}

number scale(
    number x,
    number lowin,
    number hiin,
    number lowout,
    number highout,
    number pow
) {
    auto inscale = this->safediv(1., hiin - lowin);
    number outdiff = highout - lowout;
    number value = (x - lowin) * inscale;

    if (pow != 1) {
        if (value > 0)
            value = this->safepow(value, pow);
        else
            value = -this->safepow(-value, pow);
    }

    value = value * outdiff + lowout;
    return value;
}

number __wrapped_op_clamp(number in1, number in2, number in3) {
    return (in1 > in3 ? in3 : (in1 < in2 ? in2 : in1));
}

number wrap(number x, number low, number high) {
    number lo;
    number hi;

    if (low == high)
        return low;

    if (low > high) {
        hi = low;
        lo = high;
    } else {
        lo = low;
        hi = high;
    }

    number range = hi - lo;

    if (x >= lo && x < hi)
        return x;

    if (range <= 0.000000001)
        return lo;

    long numWraps = (long)(trunc((x - lo) / range));
    numWraps = numWraps - ((x < lo ? 1 : 0));
    number result = x - range * numWraps;

    if (result >= hi)
        return result - range;
    else
        return result;
}

inline number safemod(number f, number m) {
    if (m != 0) {
        Int f_trunc = (Int)(trunc(f));
        Int m_trunc = (Int)(trunc(m));

        if (f == f_trunc && m == m_trunc) {
            f = f_trunc % m_trunc;
        } else {
            if (m < 0) {
                m = -m;
            }

            if (f >= m) {
                if (f >= m * 2.0) {
                    number d = f / m;
                    Int i = (Int)(trunc(d));
                    d = d - i;
                    f = d * m;
                } else {
                    f -= m;
                }
            } else if (f <= -m) {
                if (f <= -m * 2.0) {
                    number d = f / m;
                    Int i = (Int)(trunc(d));
                    d = d - i;
                    f = d * m;
                } else {
                    f += m;
                }
            }
        }
    } else {
        f = 0.0;
    }

    return f;
}

template <typename T> inline array<SampleValue, 1 + 1> peek_default(T& buffer, SampleValue sampleIndex, Index channelOffset) {
    number bufferSize = buffer->getSize();
    const Index bufferChannels = (const Index)(buffer->getChannels());
    constexpr int ___N2 = 1 + 1;
    array<SampleValue, ___N2> out = FIXEDSIZEARRAYINIT(1 + 1);

    if (bufferSize == 0 || (5 == 5 && (sampleIndex < 0 || sampleIndex >= bufferSize))) {
        return out;
    } else {
        for (Index channel = 0; channel < 1; channel++) {
            Index channelIndex = (Index)(channel + channelOffset);

            {
                if (channelIndex >= bufferChannels || channelIndex < 0) {
                    out[(Index)channel] = 0;
                    continue;
                }
            }

            SampleValue bufferreadsample_result;

            {
                auto& __buffer = buffer;

                if (sampleIndex < 0.0) {
                    bufferreadsample_result = 0.0;
                }

                SampleIndex index1 = (SampleIndex)(trunc(sampleIndex));

                {
                    {
                        {
                            {
                                bufferreadsample_result = __buffer->getSample(channelIndex, index1);
                            }
                        }
                    }
                }
            }

            out[(Index)channel] = bufferreadsample_result;
        }

        out[1] = sampleIndex;
        return out;
    }
}

template <typename T> inline void poke_default(
    T& buffer,
    SampleValue value,
    SampleValue sampleIndex,
    int channel,
    number overdub
) {
    number bufferSize = buffer->getSize();
    const Index bufferChannels = (const Index)(buffer->getChannels());

    if (bufferSize > 0 && (5 != 5 || (sampleIndex >= 0 && sampleIndex < bufferSize)) && (5 != 5 || (channel >= 0 && channel < bufferChannels))) {
        if (overdub != 0) {
            number currentValue = buffer->getSample(channel, sampleIndex);

            {
                value = value * (1. - overdub) + currentValue * overdub;
            }
        }

        buffer->setSample(channel, sampleIndex, value);
        buffer->setTouched(true);
    }
}

template <typename T> inline number dim(T& buffer) {
    return buffer->getSize();
}

number hann(number x) {
    x = (x > 1 ? 1 : (x < 0 ? 0 : x));
    return 0.5 * (1 - rnbo_cos(6.28318530717958647692 * x));
}

MillisecondTime currenttime() {
    return this->_currentTime;
}

number tempo() {
    return this->getTopLevelPatcher()->globaltransport_getTempo(this->currenttime());
}

number mstobeats(number ms) {
    return ms * this->tempo() * 0.008 / (number)480;
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 0;
}

void processMidiEvent(MillisecondTime , int , ConstByteArray , Index ) {}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);
    const SampleValue * in2 = (numInputs >= 2 && inputs[1] ? inputs[1] : this->zeroBuffer);
    this->dspexpr_02_perform(in1, in2, this->signals[0], n);

    this->fftstream_tilde_01_perform(
        this->signals[0],
        this->fftstream_tilde_01_imagIn,
        this->signals[1],
        this->signals[2],
        this->signals[3],
        n
    );

    this->gen_02_perform(
        this->signals[1],
        this->signals[2],
        this->signals[3],
        this->gen_02_amp15,
        this->gen_02_amp14,
        this->gen_02_amp13,
        this->gen_02_amp12,
        this->gen_02_amp11,
        this->gen_02_amp10,
        this->gen_02_amp9,
        this->gen_02_amp8,
        this->gen_02_amp7,
        this->gen_02_amp6,
        this->gen_02_amp5,
        this->gen_02_amp4,
        this->gen_02_amp3,
        this->gen_02_amp2,
        this->gen_02_amp1,
        this->gen_02_stereoMode,
        this->gen_02_modPanFreq,
        this->gen_02_modAmp,
        this->gen_02_modFreq,
        this->gen_02_freqScale,
        this->gen_02_tuning,
        this->gen_02_equalTemperament,
        this->gen_02_equalTemperamentMode,
        this->gen_02_ampSmooth,
        this->gen_02_freqSmooth,
        this->gen_02_binSmooth,
        this->signals[0],
        this->signals[4],
        n
    );

    this->gen_01_perform(
        this->signals[0],
        this->signals[4],
        this->gen_01_mix,
        this->gen_01_delayCom,
        this->gen_01_delayAll,
        this->signals[3],
        this->signals[2],
        n
    );

    this->dspexpr_01_perform(in1, this->signals[3], this->dspexpr_01_in3, out1, n);
    this->dspexpr_03_perform(in2, this->signals[2], this->dspexpr_03_in3, out2, n);
    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        Index i;

        for (i = 0; i < 5; i++) {
            this->signals[i] = resizeSignal(this->signals[i], this->maxvs, maxBlockSize);
        }

        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->fftstream_tilde_01_dspsetup(forceDSPSetup);
    this->gen_02_dspsetup(forceDSPSetup);
    this->gen_01_dspsetup(forceDSPSetup);
    this->data_01_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

void setProbingTarget(MessageTag id) {
    switch (id) {
    default:
        {
        this->setProbingIndex(-1);
        break;
        }
    }
}

void setProbingIndex(ProbingIndex ) {}

Index getProbingChannels(MessageTag outletId) const {
    RNBO_UNUSED(outletId);
    return 0;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->gen_01_del_9_bufferobj);
        break;
        }
    case 1:
        {
        return addressOf(this->gen_01_del_8_bufferobj);
        break;
        }
    case 2:
        {
        return addressOf(this->gen_01_del_7_bufferobj);
        break;
        }
    case 3:
        {
        return addressOf(this->gen_01_del1_bufferobj);
        break;
        }
    case 4:
        {
        return addressOf(this->gen_01_del_6_bufferobj);
        break;
        }
    case 5:
        {
        return addressOf(this->gen_01_del_5_bufferobj);
        break;
        }
    case 6:
        {
        return addressOf(this->gen_01_del_4_bufferobj);
        break;
        }
    case 7:
        {
        return addressOf(this->gen_01_del_3_bufferobj);
        break;
        }
    case 8:
        {
        return addressOf(this->gen_01_del_2_bufferobj);
        break;
        }
    case 9:
        {
        return addressOf(this->gen_01_del_1_bufferobj);
        break;
        }
    case 10:
        {
        return addressOf(this->gen_01_del3_bufferobj);
        break;
        }
    case 11:
        {
        return addressOf(this->gen_01_del2_bufferobj);
        break;
        }
    case 12:
        {
        return addressOf(this->smoothedData);
        break;
        }
    case 13:
        {
        return addressOf(this->ExDetectionData);
        break;
        }
    case 14:
        {
        return addressOf(this->phaseData);
        break;
        }
    case 15:
        {
        return addressOf(this->lastIndex);
        break;
        }
    case 16:
        {
        return addressOf(this->lastRadius);
        break;
        }
    case 17:
        {
        return addressOf(this->currentIndex);
        break;
        }
    case 18:
        {
        return addressOf(this->currentRadius);
        break;
        }
    case 19:
        {
        return addressOf(this->prev);
        break;
        }
    case 20:
        {
        return addressOf(this->gen_02_modPanDel_bufferobj);
        break;
        }
    case 21:
        {
        return addressOf(this->gen_02_modFreqDel_bufferobj);
        break;
        }
    case 22:
        {
        return addressOf(this->gen_02_delay15_bufferobj);
        break;
        }
    case 23:
        {
        return addressOf(this->gen_02_delay14_bufferobj);
        break;
        }
    case 24:
        {
        return addressOf(this->gen_02_delay13_bufferobj);
        break;
        }
    case 25:
        {
        return addressOf(this->gen_02_delay12_bufferobj);
        break;
        }
    case 26:
        {
        return addressOf(this->gen_02_delay11_bufferobj);
        break;
        }
    case 27:
        {
        return addressOf(this->gen_02_delay10_bufferobj);
        break;
        }
    case 28:
        {
        return addressOf(this->gen_02_delay9_bufferobj);
        break;
        }
    case 29:
        {
        return addressOf(this->gen_02_delay8_bufferobj);
        break;
        }
    case 30:
        {
        return addressOf(this->gen_02_delay7_bufferobj);
        break;
        }
    case 31:
        {
        return addressOf(this->gen_02_delay6_bufferobj);
        break;
        }
    case 32:
        {
        return addressOf(this->gen_02_delay5_bufferobj);
        break;
        }
    case 33:
        {
        return addressOf(this->gen_02_delay4_bufferobj);
        break;
        }
    case 34:
        {
        return addressOf(this->gen_02_delay3_bufferobj);
        break;
        }
    case 35:
        {
        return addressOf(this->gen_02_delay2_bufferobj);
        break;
        }
    case 36:
        {
        return addressOf(this->gen_02_delay1_bufferobj);
        break;
        }
    case 37:
        {
        return addressOf(this->RNBODefaultSinus);
        break;
        }
    case 38:
        {
        return addressOf(this->RNBODefaultFftWindow);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 39;
}

void fillRNBODefaultSinus(DataRef& ref) {
    Float64BufferRef buffer;
    buffer = new Float64Buffer(ref);
    number bufsize = buffer->getSize();

    for (Index i = 0; i < bufsize; i++) {
        buffer[i] = rnbo_cos(i * 3.14159265358979323846 * 2. / bufsize);
    }
}

void fillDataRef(DataRefIndex index, DataRef& ref) {
    switch (index) {
    case 37:
        {
        this->fillRNBODefaultSinus(ref);
        break;
        }
    }
}

void zeroDataRef(DataRef& ref) {
    ref->setZero();
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time);

    if (index == 0) {
        this->gen_01_del_9_buffer = new Float64Buffer(this->gen_01_del_9_bufferobj);
    }

    if (index == 1) {
        this->gen_01_del_8_buffer = new Float64Buffer(this->gen_01_del_8_bufferobj);
    }

    if (index == 2) {
        this->gen_01_del_7_buffer = new Float64Buffer(this->gen_01_del_7_bufferobj);
    }

    if (index == 3) {
        this->gen_01_del1_buffer = new Float64Buffer(this->gen_01_del1_bufferobj);
    }

    if (index == 4) {
        this->gen_01_del_6_buffer = new Float64Buffer(this->gen_01_del_6_bufferobj);
    }

    if (index == 5) {
        this->gen_01_del_5_buffer = new Float64Buffer(this->gen_01_del_5_bufferobj);
    }

    if (index == 6) {
        this->gen_01_del_4_buffer = new Float64Buffer(this->gen_01_del_4_bufferobj);
    }

    if (index == 7) {
        this->gen_01_del_3_buffer = new Float64Buffer(this->gen_01_del_3_bufferobj);
    }

    if (index == 8) {
        this->gen_01_del_2_buffer = new Float64Buffer(this->gen_01_del_2_bufferobj);
    }

    if (index == 9) {
        this->gen_01_del_1_buffer = new Float64Buffer(this->gen_01_del_1_bufferobj);
    }

    if (index == 10) {
        this->gen_01_del3_buffer = new Float64Buffer(this->gen_01_del3_bufferobj);
    }

    if (index == 11) {
        this->gen_01_del2_buffer = new Float64Buffer(this->gen_01_del2_bufferobj);
    }

    if (index == 12) {
        this->gen_02_smoothedData = new Float64Buffer(this->smoothedData);
    }

    if (index == 13) {
        this->gen_02_detectionData = new Float32Buffer(this->ExDetectionData);
        this->data_01_buffer = new Float32Buffer(this->ExDetectionData);
        this->data_01_bufferUpdated();
    }

    if (index == 14) {
        this->gen_02_phaseData = new Float64Buffer(this->phaseData);
    }

    if (index == 15) {
        this->gen_02_lastIndex = new Float64Buffer(this->lastIndex);
    }

    if (index == 16) {
        this->gen_02_lastRadius = new Float64Buffer(this->lastRadius);
    }

    if (index == 17) {
        this->gen_02_currentIndex = new Float64Buffer(this->currentIndex);
    }

    if (index == 18) {
        this->gen_02_currentRadius = new Float64Buffer(this->currentRadius);
    }

    if (index == 19) {
        this->gen_02_prev = new Float64Buffer(this->prev);
        this->gen_02_prev_exprdata_buffer = new Float64Buffer(this->prev);
    }

    if (index == 20) {
        this->gen_02_modPanDel_buffer = new Float64Buffer(this->gen_02_modPanDel_bufferobj);
    }

    if (index == 21) {
        this->gen_02_modFreqDel_buffer = new Float64Buffer(this->gen_02_modFreqDel_bufferobj);
    }

    if (index == 22) {
        this->gen_02_delay15_buffer = new Float64Buffer(this->gen_02_delay15_bufferobj);
    }

    if (index == 23) {
        this->gen_02_delay14_buffer = new Float64Buffer(this->gen_02_delay14_bufferobj);
    }

    if (index == 24) {
        this->gen_02_delay13_buffer = new Float64Buffer(this->gen_02_delay13_bufferobj);
    }

    if (index == 25) {
        this->gen_02_delay12_buffer = new Float64Buffer(this->gen_02_delay12_bufferobj);
    }

    if (index == 26) {
        this->gen_02_delay11_buffer = new Float64Buffer(this->gen_02_delay11_bufferobj);
    }

    if (index == 27) {
        this->gen_02_delay10_buffer = new Float64Buffer(this->gen_02_delay10_bufferobj);
    }

    if (index == 28) {
        this->gen_02_delay9_buffer = new Float64Buffer(this->gen_02_delay9_bufferobj);
    }

    if (index == 29) {
        this->gen_02_delay8_buffer = new Float64Buffer(this->gen_02_delay8_bufferobj);
    }

    if (index == 30) {
        this->gen_02_delay7_buffer = new Float64Buffer(this->gen_02_delay7_bufferobj);
    }

    if (index == 31) {
        this->gen_02_delay6_buffer = new Float64Buffer(this->gen_02_delay6_bufferobj);
    }

    if (index == 32) {
        this->gen_02_delay5_buffer = new Float64Buffer(this->gen_02_delay5_bufferobj);
    }

    if (index == 33) {
        this->gen_02_delay4_buffer = new Float64Buffer(this->gen_02_delay4_bufferobj);
    }

    if (index == 34) {
        this->gen_02_delay3_buffer = new Float64Buffer(this->gen_02_delay3_bufferobj);
    }

    if (index == 35) {
        this->gen_02_delay2_buffer = new Float64Buffer(this->gen_02_delay2_bufferobj);
    }

    if (index == 36) {
        this->gen_02_delay1_buffer = new Float64Buffer(this->gen_02_delay1_bufferobj);
    }

    if (index == 37) {
        this->gen_02_cycle_50_buffer = new Float64Buffer(this->RNBODefaultSinus);
        this->gen_02_cycle_165_buffer = new Float64Buffer(this->RNBODefaultSinus);
    }

    if (index == 38) {
        this->fftstream_tilde_01_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    }
}

void initialize() {
    this->gen_01_del_9_bufferobj = initDataRef("gen_01_del_9_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_8_bufferobj = initDataRef("gen_01_del_8_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_7_bufferobj = initDataRef("gen_01_del_7_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del1_bufferobj = initDataRef("gen_01_del1_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_6_bufferobj = initDataRef("gen_01_del_6_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_5_bufferobj = initDataRef("gen_01_del_5_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_4_bufferobj = initDataRef("gen_01_del_4_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_3_bufferobj = initDataRef("gen_01_del_3_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_2_bufferobj = initDataRef("gen_01_del_2_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del_1_bufferobj = initDataRef("gen_01_del_1_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del3_bufferobj = initDataRef("gen_01_del3_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del2_bufferobj = initDataRef("gen_01_del2_bufferobj", true, nullptr, "buffer~");
    this->smoothedData = initDataRef("smoothedData", true, nullptr, "buffer~");
    this->ExDetectionData = initDataRef("ExDetectionData", false, nullptr, "buffer~");
    this->phaseData = initDataRef("phaseData", true, nullptr, "buffer~");
    this->lastIndex = initDataRef("lastIndex", true, nullptr, "buffer~");
    this->lastRadius = initDataRef("lastRadius", true, nullptr, "buffer~");
    this->currentIndex = initDataRef("currentIndex", true, nullptr, "buffer~");
    this->currentRadius = initDataRef("currentRadius", true, nullptr, "buffer~");
    this->prev = initDataRef("prev", true, nullptr, "buffer~");
    this->gen_02_modPanDel_bufferobj = initDataRef("gen_02_modPanDel_bufferobj", true, nullptr, "buffer~");
    this->gen_02_modFreqDel_bufferobj = initDataRef("gen_02_modFreqDel_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay15_bufferobj = initDataRef("gen_02_delay15_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay14_bufferobj = initDataRef("gen_02_delay14_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay13_bufferobj = initDataRef("gen_02_delay13_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay12_bufferobj = initDataRef("gen_02_delay12_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay11_bufferobj = initDataRef("gen_02_delay11_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay10_bufferobj = initDataRef("gen_02_delay10_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay9_bufferobj = initDataRef("gen_02_delay9_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay8_bufferobj = initDataRef("gen_02_delay8_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay7_bufferobj = initDataRef("gen_02_delay7_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay6_bufferobj = initDataRef("gen_02_delay6_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay5_bufferobj = initDataRef("gen_02_delay5_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay4_bufferobj = initDataRef("gen_02_delay4_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay3_bufferobj = initDataRef("gen_02_delay3_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay2_bufferobj = initDataRef("gen_02_delay2_bufferobj", true, nullptr, "buffer~");
    this->gen_02_delay1_bufferobj = initDataRef("gen_02_delay1_bufferobj", true, nullptr, "buffer~");
    this->RNBODefaultSinus = initDataRef("RNBODefaultSinus", true, nullptr, "buffer~");
    this->RNBODefaultFftWindow = initDataRef("RNBODefaultFftWindow", false, nullptr, "buffer~");
    this->assign_defaults();
    this->setState();
    this->gen_01_del_9_bufferobj->setIndex(0);
    this->gen_01_del_9_buffer = new Float64Buffer(this->gen_01_del_9_bufferobj);
    this->gen_01_del_8_bufferobj->setIndex(1);
    this->gen_01_del_8_buffer = new Float64Buffer(this->gen_01_del_8_bufferobj);
    this->gen_01_del_7_bufferobj->setIndex(2);
    this->gen_01_del_7_buffer = new Float64Buffer(this->gen_01_del_7_bufferobj);
    this->gen_01_del1_bufferobj->setIndex(3);
    this->gen_01_del1_buffer = new Float64Buffer(this->gen_01_del1_bufferobj);
    this->gen_01_del_6_bufferobj->setIndex(4);
    this->gen_01_del_6_buffer = new Float64Buffer(this->gen_01_del_6_bufferobj);
    this->gen_01_del_5_bufferobj->setIndex(5);
    this->gen_01_del_5_buffer = new Float64Buffer(this->gen_01_del_5_bufferobj);
    this->gen_01_del_4_bufferobj->setIndex(6);
    this->gen_01_del_4_buffer = new Float64Buffer(this->gen_01_del_4_bufferobj);
    this->gen_01_del_3_bufferobj->setIndex(7);
    this->gen_01_del_3_buffer = new Float64Buffer(this->gen_01_del_3_bufferobj);
    this->gen_01_del_2_bufferobj->setIndex(8);
    this->gen_01_del_2_buffer = new Float64Buffer(this->gen_01_del_2_bufferobj);
    this->gen_01_del_1_bufferobj->setIndex(9);
    this->gen_01_del_1_buffer = new Float64Buffer(this->gen_01_del_1_bufferobj);
    this->gen_01_del3_bufferobj->setIndex(10);
    this->gen_01_del3_buffer = new Float64Buffer(this->gen_01_del3_bufferobj);
    this->gen_01_del2_bufferobj->setIndex(11);
    this->gen_01_del2_buffer = new Float64Buffer(this->gen_01_del2_bufferobj);
    this->smoothedData->setIndex(12);
    this->gen_02_smoothedData = new Float64Buffer(this->smoothedData);
    this->ExDetectionData->setIndex(13);
    this->gen_02_detectionData = new Float32Buffer(this->ExDetectionData);
    this->data_01_buffer = new Float32Buffer(this->ExDetectionData);
    this->phaseData->setIndex(14);
    this->gen_02_phaseData = new Float64Buffer(this->phaseData);
    this->lastIndex->setIndex(15);
    this->gen_02_lastIndex = new Float64Buffer(this->lastIndex);
    this->lastRadius->setIndex(16);
    this->gen_02_lastRadius = new Float64Buffer(this->lastRadius);
    this->currentIndex->setIndex(17);
    this->gen_02_currentIndex = new Float64Buffer(this->currentIndex);
    this->currentRadius->setIndex(18);
    this->gen_02_currentRadius = new Float64Buffer(this->currentRadius);
    this->prev->setIndex(19);
    this->gen_02_prev = new Float64Buffer(this->prev);
    this->gen_02_prev_exprdata_buffer = new Float64Buffer(this->prev);
    this->gen_02_modPanDel_bufferobj->setIndex(20);
    this->gen_02_modPanDel_buffer = new Float64Buffer(this->gen_02_modPanDel_bufferobj);
    this->gen_02_modFreqDel_bufferobj->setIndex(21);
    this->gen_02_modFreqDel_buffer = new Float64Buffer(this->gen_02_modFreqDel_bufferobj);
    this->gen_02_delay15_bufferobj->setIndex(22);
    this->gen_02_delay15_buffer = new Float64Buffer(this->gen_02_delay15_bufferobj);
    this->gen_02_delay14_bufferobj->setIndex(23);
    this->gen_02_delay14_buffer = new Float64Buffer(this->gen_02_delay14_bufferobj);
    this->gen_02_delay13_bufferobj->setIndex(24);
    this->gen_02_delay13_buffer = new Float64Buffer(this->gen_02_delay13_bufferobj);
    this->gen_02_delay12_bufferobj->setIndex(25);
    this->gen_02_delay12_buffer = new Float64Buffer(this->gen_02_delay12_bufferobj);
    this->gen_02_delay11_bufferobj->setIndex(26);
    this->gen_02_delay11_buffer = new Float64Buffer(this->gen_02_delay11_bufferobj);
    this->gen_02_delay10_bufferobj->setIndex(27);
    this->gen_02_delay10_buffer = new Float64Buffer(this->gen_02_delay10_bufferobj);
    this->gen_02_delay9_bufferobj->setIndex(28);
    this->gen_02_delay9_buffer = new Float64Buffer(this->gen_02_delay9_bufferobj);
    this->gen_02_delay8_bufferobj->setIndex(29);
    this->gen_02_delay8_buffer = new Float64Buffer(this->gen_02_delay8_bufferobj);
    this->gen_02_delay7_bufferobj->setIndex(30);
    this->gen_02_delay7_buffer = new Float64Buffer(this->gen_02_delay7_bufferobj);
    this->gen_02_delay6_bufferobj->setIndex(31);
    this->gen_02_delay6_buffer = new Float64Buffer(this->gen_02_delay6_bufferobj);
    this->gen_02_delay5_bufferobj->setIndex(32);
    this->gen_02_delay5_buffer = new Float64Buffer(this->gen_02_delay5_bufferobj);
    this->gen_02_delay4_bufferobj->setIndex(33);
    this->gen_02_delay4_buffer = new Float64Buffer(this->gen_02_delay4_bufferobj);
    this->gen_02_delay3_bufferobj->setIndex(34);
    this->gen_02_delay3_buffer = new Float64Buffer(this->gen_02_delay3_bufferobj);
    this->gen_02_delay2_bufferobj->setIndex(35);
    this->gen_02_delay2_buffer = new Float64Buffer(this->gen_02_delay2_bufferobj);
    this->gen_02_delay1_bufferobj->setIndex(36);
    this->gen_02_delay1_buffer = new Float64Buffer(this->gen_02_delay1_bufferobj);
    this->RNBODefaultSinus->setIndex(37);
    this->gen_02_cycle_50_buffer = new Float64Buffer(this->RNBODefaultSinus);
    this->gen_02_cycle_165_buffer = new Float64Buffer(this->RNBODefaultSinus);
    this->RNBODefaultFftWindow->setIndex(38);
    this->fftstream_tilde_01_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "modFreq"));
    this->param_02_getPresetValue(getSubState(preset, "freqScale"));
    this->param_03_getPresetValue(getSubState(preset, "mix"));
    this->param_04_getPresetValue(getSubState(preset, "delayAll"));
    this->param_05_getPresetValue(getSubState(preset, "delayCom"));
    this->param_06_getPresetValue(getSubState(preset, "dryWet"));
    this->param_07_getPresetValue(getSubState(preset, "stereoMode"));
    this->param_08_getPresetValue(getSubState(preset, "modPanFreq"));
    this->param_09_getPresetValue(getSubState(preset, "freqSmooth"));
    this->param_10_getPresetValue(getSubState(preset, "ampSmooth"));
    this->param_11_getPresetValue(getSubState(preset, "binSmooth"));
    this->param_12_getPresetValue(getSubState(preset, "modAmp"));
    this->param_13_getPresetValue(getSubState(preset, "tuning"));
    this->param_14_getPresetValue(getSubState(preset, "equalTemperament"));
    this->param_15_getPresetValue(getSubState(preset, "equalTemperamentMode"));
    this->param_16_getPresetValue(getSubState(preset, "amp5"));
    this->param_17_getPresetValue(getSubState(preset, "amp4"));
    this->param_18_getPresetValue(getSubState(preset, "amp3"));
    this->param_19_getPresetValue(getSubState(preset, "amp2"));
    this->param_20_getPresetValue(getSubState(preset, "amp1"));
    this->param_21_getPresetValue(getSubState(preset, "amp10"));
    this->param_22_getPresetValue(getSubState(preset, "amp9"));
    this->param_23_getPresetValue(getSubState(preset, "amp8"));
    this->param_24_getPresetValue(getSubState(preset, "amp7"));
    this->param_25_getPresetValue(getSubState(preset, "amp6"));
    this->param_26_getPresetValue(getSubState(preset, "amp15"));
    this->param_27_getPresetValue(getSubState(preset, "amp14"));
    this->param_28_getPresetValue(getSubState(preset, "amp13"));
    this->param_29_getPresetValue(getSubState(preset, "amp12"));
    this->param_30_getPresetValue(getSubState(preset, "amp11"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "modFreq"));
    this->param_02_setPresetValue(getSubState(preset, "freqScale"));
    this->param_03_setPresetValue(getSubState(preset, "mix"));
    this->param_04_setPresetValue(getSubState(preset, "delayAll"));
    this->param_05_setPresetValue(getSubState(preset, "delayCom"));
    this->param_06_setPresetValue(getSubState(preset, "dryWet"));
    this->param_07_setPresetValue(getSubState(preset, "stereoMode"));
    this->param_08_setPresetValue(getSubState(preset, "modPanFreq"));
    this->param_09_setPresetValue(getSubState(preset, "freqSmooth"));
    this->param_10_setPresetValue(getSubState(preset, "ampSmooth"));
    this->param_11_setPresetValue(getSubState(preset, "binSmooth"));
    this->param_12_setPresetValue(getSubState(preset, "modAmp"));
    this->param_13_setPresetValue(getSubState(preset, "tuning"));
    this->param_14_setPresetValue(getSubState(preset, "equalTemperament"));
    this->param_15_setPresetValue(getSubState(preset, "equalTemperamentMode"));
    this->param_16_setPresetValue(getSubState(preset, "amp5"));
    this->param_17_setPresetValue(getSubState(preset, "amp4"));
    this->param_18_setPresetValue(getSubState(preset, "amp3"));
    this->param_19_setPresetValue(getSubState(preset, "amp2"));
    this->param_20_setPresetValue(getSubState(preset, "amp1"));
    this->param_21_setPresetValue(getSubState(preset, "amp10"));
    this->param_22_setPresetValue(getSubState(preset, "amp9"));
    this->param_23_setPresetValue(getSubState(preset, "amp8"));
    this->param_24_setPresetValue(getSubState(preset, "amp7"));
    this->param_25_setPresetValue(getSubState(preset, "amp6"));
    this->param_26_setPresetValue(getSubState(preset, "amp15"));
    this->param_27_setPresetValue(getSubState(preset, "amp14"));
    this->param_28_setPresetValue(getSubState(preset, "amp13"));
    this->param_29_setPresetValue(getSubState(preset, "amp12"));
    this->param_30_setPresetValue(getSubState(preset, "amp11"));
}

void processTempoEvent(MillisecondTime time, Tempo tempo) {
    this->updateTime(time);

    if (this->globaltransport_setTempo(this->_currentTime, tempo, false))
        {}
}

void processTransportEvent(MillisecondTime time, TransportState state) {
    this->updateTime(time);

    if (this->globaltransport_setState(this->_currentTime, state, false))
        {}
}

void processBeatTimeEvent(MillisecondTime time, BeatTime beattime) {
    this->updateTime(time);

    if (this->globaltransport_setBeatTime(this->_currentTime, beattime, false))
        {}
}

void onSampleRateChanged(double ) {}

void processTimeSignatureEvent(MillisecondTime time, int numerator, int denominator) {
    this->updateTime(time);

    if (this->globaltransport_setTimeSignature(this->_currentTime, numerator, denominator, false))
        {}
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    case 3:
        {
        this->param_04_value_set(v);
        break;
        }
    case 4:
        {
        this->param_05_value_set(v);
        break;
        }
    case 5:
        {
        this->param_06_value_set(v);
        break;
        }
    case 6:
        {
        this->param_07_value_set(v);
        break;
        }
    case 7:
        {
        this->param_08_value_set(v);
        break;
        }
    case 8:
        {
        this->param_09_value_set(v);
        break;
        }
    case 9:
        {
        this->param_10_value_set(v);
        break;
        }
    case 10:
        {
        this->param_11_value_set(v);
        break;
        }
    case 11:
        {
        this->param_12_value_set(v);
        break;
        }
    case 12:
        {
        this->param_13_value_set(v);
        break;
        }
    case 13:
        {
        this->param_14_value_set(v);
        break;
        }
    case 14:
        {
        this->param_15_value_set(v);
        break;
        }
    case 15:
        {
        this->param_16_value_set(v);
        break;
        }
    case 16:
        {
        this->param_17_value_set(v);
        break;
        }
    case 17:
        {
        this->param_18_value_set(v);
        break;
        }
    case 18:
        {
        this->param_19_value_set(v);
        break;
        }
    case 19:
        {
        this->param_20_value_set(v);
        break;
        }
    case 20:
        {
        this->param_21_value_set(v);
        break;
        }
    case 21:
        {
        this->param_22_value_set(v);
        break;
        }
    case 22:
        {
        this->param_23_value_set(v);
        break;
        }
    case 23:
        {
        this->param_24_value_set(v);
        break;
        }
    case 24:
        {
        this->param_25_value_set(v);
        break;
        }
    case 25:
        {
        this->param_26_value_set(v);
        break;
        }
    case 26:
        {
        this->param_27_value_set(v);
        break;
        }
    case 27:
        {
        this->param_28_value_set(v);
        break;
        }
    case 28:
        {
        this->param_29_value_set(v);
        break;
        }
    case 29:
        {
        this->param_30_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processParameterBangEvent(ParameterIndex index, MillisecondTime time) {
    this->setParameterValue(index, this->getParameterValue(index), time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    case 3:
        {
        return this->param_04_value;
        }
    case 4:
        {
        return this->param_05_value;
        }
    case 5:
        {
        return this->param_06_value;
        }
    case 6:
        {
        return this->param_07_value;
        }
    case 7:
        {
        return this->param_08_value;
        }
    case 8:
        {
        return this->param_09_value;
        }
    case 9:
        {
        return this->param_10_value;
        }
    case 10:
        {
        return this->param_11_value;
        }
    case 11:
        {
        return this->param_12_value;
        }
    case 12:
        {
        return this->param_13_value;
        }
    case 13:
        {
        return this->param_14_value;
        }
    case 14:
        {
        return this->param_15_value;
        }
    case 15:
        {
        return this->param_16_value;
        }
    case 16:
        {
        return this->param_17_value;
        }
    case 17:
        {
        return this->param_18_value;
        }
    case 18:
        {
        return this->param_19_value;
        }
    case 19:
        {
        return this->param_20_value;
        }
    case 20:
        {
        return this->param_21_value;
        }
    case 21:
        {
        return this->param_22_value;
        }
    case 22:
        {
        return this->param_23_value;
        }
    case 23:
        {
        return this->param_24_value;
        }
    case 24:
        {
        return this->param_25_value;
        }
    case 25:
        {
        return this->param_26_value;
        }
    case 26:
        {
        return this->param_27_value;
        }
    case 27:
        {
        return this->param_28_value;
        }
    case 28:
        {
        return this->param_29_value;
        }
    case 29:
        {
        return this->param_30_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 30;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "modFreq";
        }
    case 1:
        {
        return "freqScale";
        }
    case 2:
        {
        return "mix";
        }
    case 3:
        {
        return "delayAll";
        }
    case 4:
        {
        return "delayCom";
        }
    case 5:
        {
        return "dryWet";
        }
    case 6:
        {
        return "stereoMode";
        }
    case 7:
        {
        return "modPanFreq";
        }
    case 8:
        {
        return "freqSmooth";
        }
    case 9:
        {
        return "ampSmooth";
        }
    case 10:
        {
        return "binSmooth";
        }
    case 11:
        {
        return "modAmp";
        }
    case 12:
        {
        return "tuning";
        }
    case 13:
        {
        return "equalTemperament";
        }
    case 14:
        {
        return "equalTemperamentMode";
        }
    case 15:
        {
        return "amp5";
        }
    case 16:
        {
        return "amp4";
        }
    case 17:
        {
        return "amp3";
        }
    case 18:
        {
        return "amp2";
        }
    case 19:
        {
        return "amp1";
        }
    case 20:
        {
        return "amp10";
        }
    case 21:
        {
        return "amp9";
        }
    case 22:
        {
        return "amp8";
        }
    case 23:
        {
        return "amp7";
        }
    case 24:
        {
        return "amp6";
        }
    case 25:
        {
        return "amp15";
        }
    case 26:
        {
        return "amp14";
        }
    case 27:
        {
        return "amp13";
        }
    case 28:
        {
        return "amp12";
        }
    case 29:
        {
        return "amp11";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "modFreq";
        }
    case 1:
        {
        return "freqScale";
        }
    case 2:
        {
        return "mix";
        }
    case 3:
        {
        return "delayAll";
        }
    case 4:
        {
        return "delayCom";
        }
    case 5:
        {
        return "dryWet";
        }
    case 6:
        {
        return "stereoMode";
        }
    case 7:
        {
        return "modPanFreq";
        }
    case 8:
        {
        return "freqSmooth";
        }
    case 9:
        {
        return "ampSmooth";
        }
    case 10:
        {
        return "binSmooth";
        }
    case 11:
        {
        return "modAmp";
        }
    case 12:
        {
        return "tuning";
        }
    case 13:
        {
        return "equalTemperament";
        }
    case 14:
        {
        return "equalTemperamentMode";
        }
    case 15:
        {
        return "amp5";
        }
    case 16:
        {
        return "amp4";
        }
    case 17:
        {
        return "amp3";
        }
    case 18:
        {
        return "amp2";
        }
    case 19:
        {
        return "amp1";
        }
    case 20:
        {
        return "amp10";
        }
    case 21:
        {
        return "amp9";
        }
    case 22:
        {
        return "amp8";
        }
    case 23:
        {
        return "amp7";
        }
    case 24:
        {
        return "amp6";
        }
    case 25:
        {
        return "amp15";
        }
    case 26:
        {
        return "amp14";
        }
    case 27:
        {
        return "amp13";
        }
    case 28:
        {
        return "amp12";
        }
    case 29:
        {
        return "amp11";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0.1;
            info->max = 10;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0.1;
            info->max = 5;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 100;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 3:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 5;
            info->min = 0;
            info->max = 1000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 4:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 39.85;
            info->min = 0;
            info->max = 1000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 5:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 1;
            info->max = 100;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 6:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 7:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0.1;
            info->max = 10;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 8:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.99;
            info->min = 0;
            info->max = 0.999;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 9:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.99;
            info->min = 0;
            info->max = 0.999;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 10:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.5;
            info->min = 0;
            info->max = 0.999;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 11:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 10;
            info->min = 0;
            info->max = 200;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 12:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 440;
            info->min = 220;
            info->max = 880;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 13:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 12;
            info->min = 2;
            info->max = 53;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 14:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 15:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 16:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 17:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 18:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 19:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 20:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 21:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 22:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 23:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 24:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 25:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 26:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 27:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 28:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 29:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 6:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - 0) / (1 - 0);
            return normalizedValue;
        }
        }
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 100 ? 100 : value));
            ParameterValue normalizedValue = (value - 0) / (100 - 0);
            return normalizedValue;
        }
        }
    case 11:
        {
        {
            value = (value < 0 ? 0 : (value > 200 ? 200 : value));
            ParameterValue normalizedValue = (value - 0) / (200 - 0);
            return normalizedValue;
        }
        }
    case 3:
    case 4:
        {
        {
            value = (value < 0 ? 0 : (value > 1000 ? 1000 : value));
            ParameterValue normalizedValue = (value - 0) / (1000 - 0);
            return normalizedValue;
        }
        }
    case 8:
    case 9:
    case 10:
        {
        {
            value = (value < 0 ? 0 : (value > 0.999 ? 0.999 : value));
            ParameterValue normalizedValue = (value - 0) / (0.999 - 0);
            return normalizedValue;
        }
        }
    case 5:
        {
        {
            value = (value < 1 ? 1 : (value > 100 ? 100 : value));
            ParameterValue normalizedValue = (value - 1) / (100 - 1);
            return normalizedValue;
        }
        }
    case 13:
        {
        {
            value = (value < 2 ? 2 : (value > 53 ? 53 : value));
            ParameterValue normalizedValue = (value - 2) / (53 - 2);
            return normalizedValue;
        }
        }
    case 12:
        {
        {
            value = (value < 220 ? 220 : (value > 880 ? 880 : value));
            ParameterValue normalizedValue = (value - 220) / (880 - 220);
            return normalizedValue;
        }
        }
    case 1:
        {
        {
            value = (value < 0.1 ? 0.1 : (value > 5 ? 5 : value));
            ParameterValue normalizedValue = (value - 0.1) / (5 - 0.1);
            return normalizedValue;
        }
        }
    case 0:
    case 7:
        {
        {
            value = (value < 0.1 ? 0.1 : (value > 10 ? 10 : value));
            ParameterValue normalizedValue = (value - 0.1) / (10 - 0.1);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 6:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (1 - 0);
            }
        }
        }
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (100 - 0);
            }
        }
        }
    case 11:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (200 - 0);
            }
        }
        }
    case 3:
    case 4:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (1000 - 0);
            }
        }
        }
    case 8:
    case 9:
    case 10:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (0.999 - 0);
            }
        }
        }
    case 5:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 1 + value * (100 - 1);
            }
        }
        }
    case 13:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 2 + value * (53 - 2);
            }
        }
        }
    case 12:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 220 + value * (880 - 220);
            }
        }
        }
    case 1:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0.1 + value * (5 - 0.1);
            }
        }
        }
    case 0:
    case 7:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0.1 + value * (10 - 0.1);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    case 3:
        {
        return this->param_04_value_constrain(value);
        }
    case 4:
        {
        return this->param_05_value_constrain(value);
        }
    case 5:
        {
        return this->param_06_value_constrain(value);
        }
    case 6:
        {
        return this->param_07_value_constrain(value);
        }
    case 7:
        {
        return this->param_08_value_constrain(value);
        }
    case 8:
        {
        return this->param_09_value_constrain(value);
        }
    case 9:
        {
        return this->param_10_value_constrain(value);
        }
    case 10:
        {
        return this->param_11_value_constrain(value);
        }
    case 11:
        {
        return this->param_12_value_constrain(value);
        }
    case 12:
        {
        return this->param_13_value_constrain(value);
        }
    case 13:
        {
        return this->param_14_value_constrain(value);
        }
    case 14:
        {
        return this->param_15_value_constrain(value);
        }
    case 15:
        {
        return this->param_16_value_constrain(value);
        }
    case 16:
        {
        return this->param_17_value_constrain(value);
        }
    case 17:
        {
        return this->param_18_value_constrain(value);
        }
    case 18:
        {
        return this->param_19_value_constrain(value);
        }
    case 19:
        {
        return this->param_20_value_constrain(value);
        }
    case 20:
        {
        return this->param_21_value_constrain(value);
        }
    case 21:
        {
        return this->param_22_value_constrain(value);
        }
    case 22:
        {
        return this->param_23_value_constrain(value);
        }
    case 23:
        {
        return this->param_24_value_constrain(value);
        }
    case 24:
        {
        return this->param_25_value_constrain(value);
        }
    case 25:
        {
        return this->param_26_value_constrain(value);
        }
    case 26:
        {
        return this->param_27_value_constrain(value);
        }
    case 27:
        {
        return this->param_28_value_constrain(value);
        }
    case 28:
        {
        return this->param_29_value_constrain(value);
        }
    case 29:
        {
        return this->param_30_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterBang(this->paramInitIndices[i], 0);
    }
}

void processClockEvent(MillisecondTime , ClockId , bool , ParameterValue ) {}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time);
    this->processOutletAtCurrentTime(sender, index, value);
}

void processNumMessage(MessageTag , MessageTag , MillisecondTime , number ) {}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {

    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->gen_02_modFreq_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->gen_02_freqScale_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        this->getEngine()->presetTouched();
        this->param_03_lastValue = this->param_03_value;
    }

    this->gen_01_mix_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        this->getEngine()->presetTouched();
        this->param_04_lastValue = this->param_04_value;
    }

    this->gen_01_delayAll_set(v);
}

void param_05_value_set(number v) {
    v = this->param_05_value_constrain(v);
    this->param_05_value = v;
    this->sendParameter(4, false);

    if (this->param_05_value != this->param_05_lastValue) {
        this->getEngine()->presetTouched();
        this->param_05_lastValue = this->param_05_value;
    }

    this->gen_01_delayCom_set(v);
}

void param_06_value_set(number v) {
    v = this->param_06_value_constrain(v);
    this->param_06_value = v;
    this->sendParameter(5, false);

    if (this->param_06_value != this->param_06_lastValue) {
        this->getEngine()->presetTouched();
        this->param_06_lastValue = this->param_06_value;
    }

    this->expr_01_in1_set(v);
}

void param_07_value_set(number v) {
    v = this->param_07_value_constrain(v);
    this->param_07_value = v;
    this->sendParameter(6, false);

    if (this->param_07_value != this->param_07_lastValue) {
        this->getEngine()->presetTouched();
        this->param_07_lastValue = this->param_07_value;
    }

    this->gen_02_stereoMode_set(v);
}

void param_08_value_set(number v) {
    v = this->param_08_value_constrain(v);
    this->param_08_value = v;
    this->sendParameter(7, false);

    if (this->param_08_value != this->param_08_lastValue) {
        this->getEngine()->presetTouched();
        this->param_08_lastValue = this->param_08_value;
    }

    this->gen_02_modPanFreq_set(v);
}

void param_09_value_set(number v) {
    v = this->param_09_value_constrain(v);
    this->param_09_value = v;
    this->sendParameter(8, false);

    if (this->param_09_value != this->param_09_lastValue) {
        this->getEngine()->presetTouched();
        this->param_09_lastValue = this->param_09_value;
    }

    this->gen_02_freqSmooth_set(v);
}

void param_10_value_set(number v) {
    v = this->param_10_value_constrain(v);
    this->param_10_value = v;
    this->sendParameter(9, false);

    if (this->param_10_value != this->param_10_lastValue) {
        this->getEngine()->presetTouched();
        this->param_10_lastValue = this->param_10_value;
    }

    this->gen_02_ampSmooth_set(v);
}

void param_11_value_set(number v) {
    v = this->param_11_value_constrain(v);
    this->param_11_value = v;
    this->sendParameter(10, false);

    if (this->param_11_value != this->param_11_lastValue) {
        this->getEngine()->presetTouched();
        this->param_11_lastValue = this->param_11_value;
    }

    this->gen_02_binSmooth_set(v);
}

void param_12_value_set(number v) {
    v = this->param_12_value_constrain(v);
    this->param_12_value = v;
    this->sendParameter(11, false);

    if (this->param_12_value != this->param_12_lastValue) {
        this->getEngine()->presetTouched();
        this->param_12_lastValue = this->param_12_value;
    }

    this->gen_02_modAmp_set(v);
}

void param_13_value_set(number v) {
    v = this->param_13_value_constrain(v);
    this->param_13_value = v;
    this->sendParameter(12, false);

    if (this->param_13_value != this->param_13_lastValue) {
        this->getEngine()->presetTouched();
        this->param_13_lastValue = this->param_13_value;
    }

    this->gen_02_tuning_set(v);
}

void param_14_value_set(number v) {
    v = this->param_14_value_constrain(v);
    this->param_14_value = v;
    this->sendParameter(13, false);

    if (this->param_14_value != this->param_14_lastValue) {
        this->getEngine()->presetTouched();
        this->param_14_lastValue = this->param_14_value;
    }

    this->gen_02_equalTemperament_set(v);
}

void param_15_value_set(number v) {
    v = this->param_15_value_constrain(v);
    this->param_15_value = v;
    this->sendParameter(14, false);

    if (this->param_15_value != this->param_15_lastValue) {
        this->getEngine()->presetTouched();
        this->param_15_lastValue = this->param_15_value;
    }

    this->gen_02_equalTemperamentMode_set(v);
}

void param_16_value_set(number v) {
    v = this->param_16_value_constrain(v);
    this->param_16_value = v;
    this->sendParameter(15, false);

    if (this->param_16_value != this->param_16_lastValue) {
        this->getEngine()->presetTouched();
        this->param_16_lastValue = this->param_16_value;
    }

    this->gen_02_amp5_set(v);
}

void param_17_value_set(number v) {
    v = this->param_17_value_constrain(v);
    this->param_17_value = v;
    this->sendParameter(16, false);

    if (this->param_17_value != this->param_17_lastValue) {
        this->getEngine()->presetTouched();
        this->param_17_lastValue = this->param_17_value;
    }

    this->gen_02_amp4_set(v);
}

void param_18_value_set(number v) {
    v = this->param_18_value_constrain(v);
    this->param_18_value = v;
    this->sendParameter(17, false);

    if (this->param_18_value != this->param_18_lastValue) {
        this->getEngine()->presetTouched();
        this->param_18_lastValue = this->param_18_value;
    }

    this->gen_02_amp3_set(v);
}

void param_19_value_set(number v) {
    v = this->param_19_value_constrain(v);
    this->param_19_value = v;
    this->sendParameter(18, false);

    if (this->param_19_value != this->param_19_lastValue) {
        this->getEngine()->presetTouched();
        this->param_19_lastValue = this->param_19_value;
    }

    this->gen_02_amp2_set(v);
}

void param_20_value_set(number v) {
    v = this->param_20_value_constrain(v);
    this->param_20_value = v;
    this->sendParameter(19, false);

    if (this->param_20_value != this->param_20_lastValue) {
        this->getEngine()->presetTouched();
        this->param_20_lastValue = this->param_20_value;
    }

    this->gen_02_amp1_set(v);
}

void param_21_value_set(number v) {
    v = this->param_21_value_constrain(v);
    this->param_21_value = v;
    this->sendParameter(20, false);

    if (this->param_21_value != this->param_21_lastValue) {
        this->getEngine()->presetTouched();
        this->param_21_lastValue = this->param_21_value;
    }

    this->gen_02_amp10_set(v);
}

void param_22_value_set(number v) {
    v = this->param_22_value_constrain(v);
    this->param_22_value = v;
    this->sendParameter(21, false);

    if (this->param_22_value != this->param_22_lastValue) {
        this->getEngine()->presetTouched();
        this->param_22_lastValue = this->param_22_value;
    }

    this->gen_02_amp9_set(v);
}

void param_23_value_set(number v) {
    v = this->param_23_value_constrain(v);
    this->param_23_value = v;
    this->sendParameter(22, false);

    if (this->param_23_value != this->param_23_lastValue) {
        this->getEngine()->presetTouched();
        this->param_23_lastValue = this->param_23_value;
    }

    this->gen_02_amp8_set(v);
}

void param_24_value_set(number v) {
    v = this->param_24_value_constrain(v);
    this->param_24_value = v;
    this->sendParameter(23, false);

    if (this->param_24_value != this->param_24_lastValue) {
        this->getEngine()->presetTouched();
        this->param_24_lastValue = this->param_24_value;
    }

    this->gen_02_amp7_set(v);
}

void param_25_value_set(number v) {
    v = this->param_25_value_constrain(v);
    this->param_25_value = v;
    this->sendParameter(24, false);

    if (this->param_25_value != this->param_25_lastValue) {
        this->getEngine()->presetTouched();
        this->param_25_lastValue = this->param_25_value;
    }

    this->gen_02_amp6_set(v);
}

void param_26_value_set(number v) {
    v = this->param_26_value_constrain(v);
    this->param_26_value = v;
    this->sendParameter(25, false);

    if (this->param_26_value != this->param_26_lastValue) {
        this->getEngine()->presetTouched();
        this->param_26_lastValue = this->param_26_value;
    }

    this->gen_02_amp15_set(v);
}

void param_27_value_set(number v) {
    v = this->param_27_value_constrain(v);
    this->param_27_value = v;
    this->sendParameter(26, false);

    if (this->param_27_value != this->param_27_lastValue) {
        this->getEngine()->presetTouched();
        this->param_27_lastValue = this->param_27_value;
    }

    this->gen_02_amp14_set(v);
}

void param_28_value_set(number v) {
    v = this->param_28_value_constrain(v);
    this->param_28_value = v;
    this->sendParameter(27, false);

    if (this->param_28_value != this->param_28_lastValue) {
        this->getEngine()->presetTouched();
        this->param_28_lastValue = this->param_28_value;
    }

    this->gen_02_amp13_set(v);
}

void param_29_value_set(number v) {
    v = this->param_29_value_constrain(v);
    this->param_29_value = v;
    this->sendParameter(28, false);

    if (this->param_29_value != this->param_29_lastValue) {
        this->getEngine()->presetTouched();
        this->param_29_lastValue = this->param_29_value;
    }

    this->gen_02_amp12_set(v);
}

void param_30_value_set(number v) {
    v = this->param_30_value_constrain(v);
    this->param_30_value = v;
    this->sendParameter(29, false);

    if (this->param_30_value != this->param_30_lastValue) {
        this->getEngine()->presetTouched();
        this->param_30_lastValue = this->param_30_value;
    }

    this->gen_02_amp11_set(v);
}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

Index getNumInputChannels() const {
    return 2;
}

Index getNumOutputChannels() const {
    return 2;
}

void allocateDataRefs() {
    this->gen_02_smoothedData->requestSize(15, 2);
    this->gen_02_smoothedData->setSampleRate(this->sr);
    this->data_01_buffer->requestSize(15, 2);
    this->data_01_buffer->setSampleRate(this->sr);
    this->gen_02_phaseData->requestSize(15, 2);
    this->gen_02_phaseData->setSampleRate(this->sr);
    this->gen_02_lastIndex->requestSize(15, 1);
    this->gen_02_lastIndex->setSampleRate(this->sr);
    this->gen_02_lastRadius->requestSize(15, 1);
    this->gen_02_lastRadius->setSampleRate(this->sr);
    this->gen_02_currentIndex->requestSize(15, 1);
    this->gen_02_currentIndex->setSampleRate(this->sr);
    this->gen_02_currentRadius->requestSize(15, 1);
    this->gen_02_currentRadius->setSampleRate(this->sr);
    this->gen_02_cycle_50_buffer->requestSize(16384, 1);
    this->gen_02_cycle_50_buffer->setSampleRate(this->sr);
    this->gen_02_cycle_165_buffer->requestSize(16384, 1);
    this->gen_02_cycle_165_buffer->setSampleRate(this->sr);
    this->gen_01_del_9_buffer = this->gen_01_del_9_buffer->allocateIfNeeded();

    if (this->gen_01_del_9_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_9_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_9_bufferobj);

        this->getEngine()->sendDataRefUpdated(0);
    }

    this->gen_01_del_8_buffer = this->gen_01_del_8_buffer->allocateIfNeeded();

    if (this->gen_01_del_8_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_8_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_8_bufferobj);

        this->getEngine()->sendDataRefUpdated(1);
    }

    this->gen_01_del_7_buffer = this->gen_01_del_7_buffer->allocateIfNeeded();

    if (this->gen_01_del_7_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_7_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_7_bufferobj);

        this->getEngine()->sendDataRefUpdated(2);
    }

    this->gen_01_del1_buffer = this->gen_01_del1_buffer->allocateIfNeeded();

    if (this->gen_01_del1_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del1_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del1_bufferobj);

        this->getEngine()->sendDataRefUpdated(3);
    }

    this->gen_01_del_6_buffer = this->gen_01_del_6_buffer->allocateIfNeeded();

    if (this->gen_01_del_6_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_6_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_6_bufferobj);

        this->getEngine()->sendDataRefUpdated(4);
    }

    this->gen_01_del_5_buffer = this->gen_01_del_5_buffer->allocateIfNeeded();

    if (this->gen_01_del_5_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_5_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_5_bufferobj);

        this->getEngine()->sendDataRefUpdated(5);
    }

    this->gen_01_del_4_buffer = this->gen_01_del_4_buffer->allocateIfNeeded();

    if (this->gen_01_del_4_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_4_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_4_bufferobj);

        this->getEngine()->sendDataRefUpdated(6);
    }

    this->gen_01_del_3_buffer = this->gen_01_del_3_buffer->allocateIfNeeded();

    if (this->gen_01_del_3_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_3_bufferobj);

        this->getEngine()->sendDataRefUpdated(7);
    }

    this->gen_01_del_2_buffer = this->gen_01_del_2_buffer->allocateIfNeeded();

    if (this->gen_01_del_2_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_2_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_2_bufferobj);

        this->getEngine()->sendDataRefUpdated(8);
    }

    this->gen_01_del_1_buffer = this->gen_01_del_1_buffer->allocateIfNeeded();

    if (this->gen_01_del_1_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del_1_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del_1_bufferobj);

        this->getEngine()->sendDataRefUpdated(9);
    }

    this->gen_01_del3_buffer = this->gen_01_del3_buffer->allocateIfNeeded();

    if (this->gen_01_del3_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del3_bufferobj);

        this->getEngine()->sendDataRefUpdated(10);
    }

    this->gen_01_del2_buffer = this->gen_01_del2_buffer->allocateIfNeeded();

    if (this->gen_01_del2_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del2_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del2_bufferobj);

        this->getEngine()->sendDataRefUpdated(11);
    }

    this->gen_02_smoothedData = this->gen_02_smoothedData->allocateIfNeeded();

    if (this->smoothedData->hasRequestedSize()) {
        if (this->smoothedData->wantsFill())
            this->zeroDataRef(this->smoothedData);

        this->getEngine()->sendDataRefUpdated(12);
    }

    this->gen_02_detectionData = this->gen_02_detectionData->allocateIfNeeded();
    this->data_01_buffer = this->data_01_buffer->allocateIfNeeded();

    if (this->ExDetectionData->hasRequestedSize()) {
        if (this->ExDetectionData->wantsFill())
            this->zeroDataRef(this->ExDetectionData);

        this->getEngine()->sendDataRefUpdated(13);
    }

    this->gen_02_phaseData = this->gen_02_phaseData->allocateIfNeeded();

    if (this->phaseData->hasRequestedSize()) {
        if (this->phaseData->wantsFill())
            this->zeroDataRef(this->phaseData);

        this->getEngine()->sendDataRefUpdated(14);
    }

    this->gen_02_lastIndex = this->gen_02_lastIndex->allocateIfNeeded();

    if (this->lastIndex->hasRequestedSize()) {
        if (this->lastIndex->wantsFill())
            this->zeroDataRef(this->lastIndex);

        this->getEngine()->sendDataRefUpdated(15);
    }

    this->gen_02_lastRadius = this->gen_02_lastRadius->allocateIfNeeded();

    if (this->lastRadius->hasRequestedSize()) {
        if (this->lastRadius->wantsFill())
            this->zeroDataRef(this->lastRadius);

        this->getEngine()->sendDataRefUpdated(16);
    }

    this->gen_02_currentIndex = this->gen_02_currentIndex->allocateIfNeeded();

    if (this->currentIndex->hasRequestedSize()) {
        if (this->currentIndex->wantsFill())
            this->zeroDataRef(this->currentIndex);

        this->getEngine()->sendDataRefUpdated(17);
    }

    this->gen_02_currentRadius = this->gen_02_currentRadius->allocateIfNeeded();

    if (this->currentRadius->hasRequestedSize()) {
        if (this->currentRadius->wantsFill())
            this->zeroDataRef(this->currentRadius);

        this->getEngine()->sendDataRefUpdated(18);
    }

    this->gen_02_prev = this->gen_02_prev->allocateIfNeeded();
    this->gen_02_prev_exprdata_buffer = this->gen_02_prev_exprdata_buffer->allocateIfNeeded();

    if (this->prev->hasRequestedSize()) {
        if (this->prev->wantsFill())
            this->zeroDataRef(this->prev);

        this->getEngine()->sendDataRefUpdated(19);
    }

    this->gen_02_modPanDel_buffer = this->gen_02_modPanDel_buffer->allocateIfNeeded();

    if (this->gen_02_modPanDel_bufferobj->hasRequestedSize()) {
        if (this->gen_02_modPanDel_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_modPanDel_bufferobj);

        this->getEngine()->sendDataRefUpdated(20);
    }

    this->gen_02_modFreqDel_buffer = this->gen_02_modFreqDel_buffer->allocateIfNeeded();

    if (this->gen_02_modFreqDel_bufferobj->hasRequestedSize()) {
        if (this->gen_02_modFreqDel_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_modFreqDel_bufferobj);

        this->getEngine()->sendDataRefUpdated(21);
    }

    this->gen_02_delay15_buffer = this->gen_02_delay15_buffer->allocateIfNeeded();

    if (this->gen_02_delay15_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay15_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay15_bufferobj);

        this->getEngine()->sendDataRefUpdated(22);
    }

    this->gen_02_delay14_buffer = this->gen_02_delay14_buffer->allocateIfNeeded();

    if (this->gen_02_delay14_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay14_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay14_bufferobj);

        this->getEngine()->sendDataRefUpdated(23);
    }

    this->gen_02_delay13_buffer = this->gen_02_delay13_buffer->allocateIfNeeded();

    if (this->gen_02_delay13_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay13_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay13_bufferobj);

        this->getEngine()->sendDataRefUpdated(24);
    }

    this->gen_02_delay12_buffer = this->gen_02_delay12_buffer->allocateIfNeeded();

    if (this->gen_02_delay12_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay12_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay12_bufferobj);

        this->getEngine()->sendDataRefUpdated(25);
    }

    this->gen_02_delay11_buffer = this->gen_02_delay11_buffer->allocateIfNeeded();

    if (this->gen_02_delay11_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay11_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay11_bufferobj);

        this->getEngine()->sendDataRefUpdated(26);
    }

    this->gen_02_delay10_buffer = this->gen_02_delay10_buffer->allocateIfNeeded();

    if (this->gen_02_delay10_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay10_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay10_bufferobj);

        this->getEngine()->sendDataRefUpdated(27);
    }

    this->gen_02_delay9_buffer = this->gen_02_delay9_buffer->allocateIfNeeded();

    if (this->gen_02_delay9_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay9_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay9_bufferobj);

        this->getEngine()->sendDataRefUpdated(28);
    }

    this->gen_02_delay8_buffer = this->gen_02_delay8_buffer->allocateIfNeeded();

    if (this->gen_02_delay8_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay8_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay8_bufferobj);

        this->getEngine()->sendDataRefUpdated(29);
    }

    this->gen_02_delay7_buffer = this->gen_02_delay7_buffer->allocateIfNeeded();

    if (this->gen_02_delay7_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay7_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay7_bufferobj);

        this->getEngine()->sendDataRefUpdated(30);
    }

    this->gen_02_delay6_buffer = this->gen_02_delay6_buffer->allocateIfNeeded();

    if (this->gen_02_delay6_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay6_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay6_bufferobj);

        this->getEngine()->sendDataRefUpdated(31);
    }

    this->gen_02_delay5_buffer = this->gen_02_delay5_buffer->allocateIfNeeded();

    if (this->gen_02_delay5_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay5_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay5_bufferobj);

        this->getEngine()->sendDataRefUpdated(32);
    }

    this->gen_02_delay4_buffer = this->gen_02_delay4_buffer->allocateIfNeeded();

    if (this->gen_02_delay4_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay4_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay4_bufferobj);

        this->getEngine()->sendDataRefUpdated(33);
    }

    this->gen_02_delay3_buffer = this->gen_02_delay3_buffer->allocateIfNeeded();

    if (this->gen_02_delay3_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay3_bufferobj);

        this->getEngine()->sendDataRefUpdated(34);
    }

    this->gen_02_delay2_buffer = this->gen_02_delay2_buffer->allocateIfNeeded();

    if (this->gen_02_delay2_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay2_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay2_bufferobj);

        this->getEngine()->sendDataRefUpdated(35);
    }

    this->gen_02_delay1_buffer = this->gen_02_delay1_buffer->allocateIfNeeded();

    if (this->gen_02_delay1_bufferobj->hasRequestedSize()) {
        if (this->gen_02_delay1_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_02_delay1_bufferobj);

        this->getEngine()->sendDataRefUpdated(36);
    }

    this->gen_02_cycle_50_buffer = this->gen_02_cycle_50_buffer->allocateIfNeeded();
    this->gen_02_cycle_165_buffer = this->gen_02_cycle_165_buffer->allocateIfNeeded();

    if (this->RNBODefaultSinus->hasRequestedSize()) {
        if (this->RNBODefaultSinus->wantsFill())
            this->fillRNBODefaultSinus(this->RNBODefaultSinus);

        this->getEngine()->sendDataRefUpdated(37);
    }

    this->fftstream_tilde_01_win_buf = this->fftstream_tilde_01_win_buf->allocateIfNeeded();

    if (this->RNBODefaultFftWindow->hasRequestedSize()) {
        if (this->RNBODefaultFftWindow->wantsFill())
            this->zeroDataRef(this->RNBODefaultFftWindow);

        this->getEngine()->sendDataRefUpdated(38);
    }
}

void initializeObjects() {
    this->gen_01_del_9_init();
    this->gen_01_del_8_init();
    this->gen_01_del_7_init();
    this->gen_01_del1_init();
    this->gen_01_del_6_init();
    this->gen_01_del_5_init();
    this->gen_01_del_4_init();
    this->gen_01_del_3_init();
    this->gen_01_del_2_init();
    this->gen_01_del_1_init();
    this->gen_01_del3_init();
    this->gen_01_del2_init();
    this->gen_02_prev_exprdata_init();
    this->gen_02_modPanDel_init();
    this->gen_02_modFreqDel_init();
    this->gen_02_delay15_init();
    this->gen_02_delay14_init();
    this->gen_02_delay13_init();
    this->gen_02_delay12_init();
    this->gen_02_delay11_init();
    this->gen_02_delay10_init();
    this->gen_02_delay9_init();
    this->gen_02_delay8_init();
    this->gen_02_delay7_init();
    this->gen_02_delay6_init();
    this->gen_02_delay5_init();
    this->gen_02_delay4_init();
    this->gen_02_delay3_init();
    this->gen_02_delay2_init();
    this->gen_02_delay1_init();
    this->data_01_init();
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    {
        this->scheduleParamInit(3, 0);
    }

    {
        this->scheduleParamInit(4, 0);
    }

    {
        this->scheduleParamInit(5, 0);
    }

    {
        this->scheduleParamInit(6, 0);
    }

    {
        this->scheduleParamInit(7, 0);
    }

    {
        this->scheduleParamInit(8, 0);
    }

    {
        this->scheduleParamInit(9, 0);
    }

    {
        this->scheduleParamInit(10, 0);
    }

    {
        this->scheduleParamInit(11, 0);
    }

    {
        this->scheduleParamInit(12, 0);
    }

    {
        this->scheduleParamInit(13, 0);
    }

    {
        this->scheduleParamInit(14, 0);
    }

    {
        this->scheduleParamInit(15, 0);
    }

    {
        this->scheduleParamInit(16, 0);
    }

    {
        this->scheduleParamInit(17, 0);
    }

    {
        this->scheduleParamInit(18, 0);
    }

    {
        this->scheduleParamInit(19, 0);
    }

    {
        this->scheduleParamInit(20, 0);
    }

    {
        this->scheduleParamInit(21, 0);
    }

    {
        this->scheduleParamInit(22, 0);
    }

    {
        this->scheduleParamInit(23, 0);
    }

    {
        this->scheduleParamInit(24, 0);
    }

    {
        this->scheduleParamInit(25, 0);
    }

    {
        this->scheduleParamInit(26, 0);
    }

    {
        this->scheduleParamInit(27, 0);
    }

    {
        this->scheduleParamInit(28, 0);
    }

    {
        this->scheduleParamInit(29, 0);
    }

    this->processParamInitEvents();
}

static number param_01_value_constrain(number v) {
    v = (v > 10 ? 10 : (v < 0.1 ? 0.1 : v));
    return v;
}

void gen_02_modFreq_set(number v) {
    this->gen_02_modFreq = v;
}

static number param_02_value_constrain(number v) {
    v = (v > 5 ? 5 : (v < 0.1 ? 0.1 : v));
    return v;
}

void gen_02_freqScale_set(number v) {
    this->gen_02_freqScale = v;
}

static number param_03_value_constrain(number v) {
    v = (v > 100 ? 100 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_mix_set(number v) {
    this->gen_01_mix = v;
}

static number param_04_value_constrain(number v) {
    v = (v > 1000 ? 1000 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_delayAll_set(number v) {
    this->gen_01_delayAll = v;
}

static number param_05_value_constrain(number v) {
    v = (v > 1000 ? 1000 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_delayCom_set(number v) {
    this->gen_01_delayCom = v;
}

static number param_06_value_constrain(number v) {
    v = (v > 100 ? 100 : (v < 1 ? 1 : v));
    return v;
}

void dspexpr_03_in3_set(number v) {
    this->dspexpr_03_in3 = v;
}

void dspexpr_01_in3_set(number v) {
    this->dspexpr_01_in3 = v;
}

void expr_01_out1_set(number v) {
    this->expr_01_out1 = v;
    this->dspexpr_03_in3_set(this->expr_01_out1);
    this->dspexpr_01_in3_set(this->expr_01_out1);
}

void expr_01_in1_set(number in1) {
    this->expr_01_in1 = in1;

    this->expr_01_out1_set(
        (this->expr_01_in2 == 0 ? 0 : (this->expr_01_in2 == 0. ? 0. : this->expr_01_in1 / this->expr_01_in2))
    );//#map:/_obj-21:1
}

static number param_07_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_stereoMode_set(number v) {
    this->gen_02_stereoMode = v;
}

static number param_08_value_constrain(number v) {
    v = (v > 10 ? 10 : (v < 0.1 ? 0.1 : v));
    return v;
}

void gen_02_modPanFreq_set(number v) {
    this->gen_02_modPanFreq = v;
}

static number param_09_value_constrain(number v) {
    v = (v > 0.999 ? 0.999 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_freqSmooth_set(number v) {
    this->gen_02_freqSmooth = v;
}

static number param_10_value_constrain(number v) {
    v = (v > 0.999 ? 0.999 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_ampSmooth_set(number v) {
    this->gen_02_ampSmooth = v;
}

static number param_11_value_constrain(number v) {
    v = (v > 0.999 ? 0.999 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_binSmooth_set(number v) {
    this->gen_02_binSmooth = v;
}

static number param_12_value_constrain(number v) {
    v = (v > 200 ? 200 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_modAmp_set(number v) {
    this->gen_02_modAmp = v;
}

static number param_13_value_constrain(number v) {
    v = (v > 880 ? 880 : (v < 220 ? 220 : v));
    return v;
}

void gen_02_tuning_set(number v) {
    this->gen_02_tuning = v;
}

static number param_14_value_constrain(number v) {
    v = (v > 53 ? 53 : (v < 2 ? 2 : v));
    return v;
}

void gen_02_equalTemperament_set(number v) {
    this->gen_02_equalTemperament = v;
}

static number param_15_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_equalTemperamentMode_set(number v) {
    this->gen_02_equalTemperamentMode = v;
}

static number param_16_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp5_set(number v) {
    this->gen_02_amp5 = v;
}

static number param_17_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp4_set(number v) {
    this->gen_02_amp4 = v;
}

static number param_18_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp3_set(number v) {
    this->gen_02_amp3 = v;
}

static number param_19_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp2_set(number v) {
    this->gen_02_amp2 = v;
}

static number param_20_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp1_set(number v) {
    this->gen_02_amp1 = v;
}

static number param_21_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp10_set(number v) {
    this->gen_02_amp10 = v;
}

static number param_22_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp9_set(number v) {
    this->gen_02_amp9 = v;
}

static number param_23_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp8_set(number v) {
    this->gen_02_amp8 = v;
}

static number param_24_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp7_set(number v) {
    this->gen_02_amp7 = v;
}

static number param_25_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp6_set(number v) {
    this->gen_02_amp6 = v;
}

static number param_26_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp15_set(number v) {
    this->gen_02_amp15 = v;
}

static number param_27_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp14_set(number v) {
    this->gen_02_amp14 = v;
}

static number param_28_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp13_set(number v) {
    this->gen_02_amp13 = v;
}

static number param_29_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp12_set(number v) {
    this->gen_02_amp12 = v;
}

static number param_30_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_amp11_set(number v) {
    this->gen_02_amp11 = v;
}

void dspexpr_02_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] + in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void fftstream_tilde_01_perform(
    const Sample * realIn,
    number imagIn,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    Index n
) {
    RNBO_UNUSED(imagIn);
    auto __fftstream_tilde_01_datapos = this->fftstream_tilde_01_datapos;
    number invFftSize = (number)1 / (number)1024;
    Index i0;

    for (i0 = 0; i0 < n; i0++) {
        SampleIndex i = (SampleIndex)(__fftstream_tilde_01_datapos);
        array<number, 3> out = {0, 0, i};
        number windowFactor = 1;
        windowFactor = this->hann(i * invFftSize);

        if (__fftstream_tilde_01_datapos < 1024) {
            this->fftstream_tilde_01_inWorkspace[(Index)(2 * i)] = windowFactor * realIn[(Index)i0];
            out[0] = this->fftstream_tilde_01_outWorkspace[(Index)(2 * i)];
            this->fftstream_tilde_01_inWorkspace[(Index)(2 * i + 1)] = windowFactor * 0;
            out[1] = this->fftstream_tilde_01_outWorkspace[(Index)(2 * i + 1)];

            if (i == 1024 - 1) {
                this->fftstream_tilde_01_fft_next(this->fftstream_tilde_01_inWorkspace, 1024);

                for (SampleIndex j = 0; j < 1024 * 2; j++) {
                    this->fftstream_tilde_01_outWorkspace[(Index)j] = this->fftstream_tilde_01_inWorkspace[(Index)j];
                }
            }

            __fftstream_tilde_01_datapos = (__fftstream_tilde_01_datapos + 1) % 1024;
        }

        out1[(Index)i0] = out[0];
        out2[(Index)i0] = out[1];
        out3[(Index)i0] = out[2];
    }

    this->fftstream_tilde_01_datapos = __fftstream_tilde_01_datapos;
}

void gen_02_perform(
    const Sample * in1,
    const Sample * in2,
    const Sample * in3,
    number amp15,
    number amp14,
    number amp13,
    number amp12,
    number amp11,
    number amp10,
    number amp9,
    number amp8,
    number amp7,
    number amp6,
    number amp5,
    number amp4,
    number amp3,
    number amp2,
    number amp1,
    number stereoMode,
    number modPanFreq,
    number modAmp,
    number modFreq,
    number freqScale,
    number tuning,
    number equalTemperament,
    number equalTemperamentMode,
    number ampSmooth,
    number freqSmooth,
    number binSmooth,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    Index i0;

    for (i0 = 0; i0 < n; i0++) {
        number radius = 0;
        number angle = 0;
        radius = (in1[(Index)i0] * in1[(Index)i0] + in2[(Index)i0] * in2[(Index)i0] <= 0 ? 0 : rnbo_sqrt(in1[(Index)i0] * in1[(Index)i0] + in2[(Index)i0] * in2[(Index)i0])), angle = rnbo_atan2(in2[(Index)i0], in1[(Index)i0]);
        number oldRadius = 0;
        auto result_0 = this->peek_default(this->gen_02_prev, in3[(Index)i0], 0);
        oldRadius = result_0[0];
        number radius_1_1 = radius + ((binSmooth <= 0 ? 0 : rnbo_sqrt(binSmooth))) * (oldRadius - radius);
        this->poke_default(this->gen_02_prev, radius_1_1, in3[(Index)i0], 0, 0);
        number oldPhase = 0;
        auto result_2 = this->peek_default(this->gen_02_prev, in3[(Index)i0], 1);
        oldPhase = result_2[0];
        angle = angle + ((binSmooth <= 0 ? 0 : rnbo_sqrt(binSmooth))) * (oldPhase - angle);
        this->poke_default(this->gen_02_prev, angle, in3[(Index)i0], 1, 0);
        number peek1 = 0;
        auto result_3 = this->peek_default(this->gen_02_currentRadius, 0, 0);
        peek1 = result_3[0];
        number peek2 = 0;
        auto result_4 = this->peek_default(this->gen_02_currentRadius, 1, 0);
        peek2 = result_4[0];
        number peek3 = 0;
        auto result_5 = this->peek_default(this->gen_02_currentRadius, 2, 0);
        peek3 = result_5[0];
        number peek4 = 0;
        auto result_6 = this->peek_default(this->gen_02_currentRadius, 3, 0);
        peek4 = result_6[0];
        number peek5 = 0;
        auto result_7 = this->peek_default(this->gen_02_currentRadius, 4, 0);
        peek5 = result_7[0];
        number peek6 = 0;
        auto result_8 = this->peek_default(this->gen_02_currentRadius, 5, 0);
        peek6 = result_8[0];
        number peek7 = 0;
        auto result_9 = this->peek_default(this->gen_02_currentRadius, 6, 0);
        peek7 = result_9[0];
        number peek8 = 0;
        auto result_10 = this->peek_default(this->gen_02_currentRadius, 7, 0);
        peek8 = result_10[0];
        number peek9 = 0;
        auto result_11 = this->peek_default(this->gen_02_currentRadius, 8, 0);
        peek9 = result_11[0];
        number peek10 = 0;
        auto result_12 = this->peek_default(this->gen_02_currentRadius, 9, 0);
        peek10 = result_12[0];
        number peek11 = 0;
        auto result_13 = this->peek_default(this->gen_02_currentRadius, 10, 0);
        peek11 = result_13[0];
        number peek12 = 0;
        auto result_14 = this->peek_default(this->gen_02_currentRadius, 11, 0);
        peek12 = result_14[0];
        number peek13 = 0;
        auto result_15 = this->peek_default(this->gen_02_currentRadius, 12, 0);
        peek13 = result_15[0];
        number peek14 = 0;
        auto result_16 = this->peek_default(this->gen_02_currentRadius, 13, 0);
        peek14 = result_16[0];
        number peek15 = 0;
        auto result_17 = this->peek_default(this->gen_02_currentRadius, 14, 0);
        peek15 = result_17[0];

        if (radius_1_1 > peek1) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 0, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 0, 0, 0);
            }
        } else if (radius_1_1 > peek2) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 1, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 1, 0, 0);
            }
        } else if (radius_1_1 > peek3) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 2, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 2, 0, 0);
            }
        } else if (radius_1_1 > peek4) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 3, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 3, 0, 0);
            }
        } else if (radius_1_1 > peek5) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 4, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 4, 0, 0);
            }
        } else if (radius_1_1 > peek6) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 5, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 5, 0, 0);
            }
        } else if (radius_1_1 > peek7) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 6, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 6, 0, 0);
            }
        } else if (radius_1_1 > peek8) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 7, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 7, 0, 0);
            }
        } else if (radius_1_1 > peek9) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 8, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 8, 0, 0);
            }
        } else if (radius_1_1 > peek10) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 9, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 9, 0, 0);
            }
        } else if (radius_1_1 > peek11) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 10, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 10, 0, 0);
            }
        } else if (radius_1_1 > peek12) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 11, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 11, 0, 0);
            }
        } else if (radius_1_1 > peek13) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 12, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 12, 0, 0);
            }
        } else if (radius_1_1 > peek14) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 13, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 13, 0, 0);
            }
        } else if (radius_1_1 > peek15) {
            if (in3[(Index)i0] < 512) {
                this->poke_default(this->gen_02_currentRadius, radius_1_1, 14, 0, 0);
                this->poke_default(this->gen_02_currentIndex, in3[(Index)i0], 14, 0, 0);
            }
        }

        if (in3[(Index)i0] == 1024 - 1) {
            for (number i = 0; i < this->dim(this->gen_02_currentRadius); i = i + 1) {
                number peek_2 = 0;
                number peek_3 = 0;
                auto result_18 = this->peek_default(this->gen_02_currentRadius, i, 0);
                peek_3 = result_18[1];
                peek_2 = result_18[0];
                this->poke_default(this->gen_02_lastRadius, peek_2, i, 0, 0);
                number peek_4 = 0;
                number peek_5 = 0;
                auto result_19 = this->peek_default(this->gen_02_currentIndex, i, 0);
                peek_5 = result_19[1];
                peek_4 = result_19[0];
                this->poke_default(this->gen_02_lastIndex, peek_4, i, 0, 0);
                this->poke_default(this->gen_02_currentRadius, 0, i, 0, 0);
                this->poke_default(this->gen_02_currentIndex, 0, i, 0, 0);
            }
        }

        for (number i = 0; i < this->dim(this->gen_02_currentRadius); i = i + 1) {
            number peek_6 = 0;
            number peek_7 = 0;
            auto result_20 = this->peek_default(this->gen_02_lastIndex, i, 0);
            peek_7 = result_20[1];
            peek_6 = result_20[0];

            this->poke_default(
                this->gen_02_phaseData,
                this->gen_02_latch_21_next(angle, peek_6 == in3[(Index)i0]),
                i,
                0,
                0
            );
        }

        this->poke_default(this->gen_02_phaseData, this->gen_02_delay1_read(1024, 5), 0, 1, 0);
        number delayedPhase1 = 0;
        auto result_22 = this->peek_default(this->gen_02_phaseData, 0, 0);
        delayedPhase1 = result_22[0];
        this->gen_02_delay1_write(delayedPhase1);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay2_read(1024, 5), 1, 1, 0);
        number delayedPhase2 = 0;
        auto result_23 = this->peek_default(this->gen_02_phaseData, 1, 0);
        delayedPhase2 = result_23[0];
        this->gen_02_delay2_write(delayedPhase2);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay3_read(1024, 5), 2, 1, 0);
        number delayedPhase3 = 0;
        auto result_24 = this->peek_default(this->gen_02_phaseData, 2, 0);
        delayedPhase3 = result_24[0];
        this->gen_02_delay3_write(delayedPhase3);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay4_read(1024, 5), 3, 1, 0);
        number delayedPhase4 = 0;
        auto result_25 = this->peek_default(this->gen_02_phaseData, 3, 0);
        delayedPhase4 = result_25[0];
        this->gen_02_delay4_write(delayedPhase4);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay5_read(1024, 5), 4, 1, 0);
        number delayedPhase5 = 0;
        auto result_26 = this->peek_default(this->gen_02_phaseData, 4, 0);
        delayedPhase5 = result_26[0];
        this->gen_02_delay5_write(delayedPhase5);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay6_read(1024, 5), 5, 1, 0);
        number delayedPhase6 = 0;
        auto result_27 = this->peek_default(this->gen_02_phaseData, 5, 0);
        delayedPhase6 = result_27[0];
        this->gen_02_delay6_write(delayedPhase6);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay7_read(1024, 5), 6, 1, 0);
        number delayedPhase7 = 0;
        auto result_28 = this->peek_default(this->gen_02_phaseData, 6, 0);
        delayedPhase7 = result_28[0];
        this->gen_02_delay7_write(delayedPhase7);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay8_read(1024, 5), 7, 1, 0);
        number delayedPhase8 = 0;
        auto result_29 = this->peek_default(this->gen_02_phaseData, 7, 0);
        delayedPhase8 = result_29[0];
        this->gen_02_delay8_write(delayedPhase8);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay9_read(1024, 5), 8, 1, 0);
        number delayedPhase9 = 0;
        auto result_30 = this->peek_default(this->gen_02_phaseData, 8, 0);
        delayedPhase9 = result_30[0];
        this->gen_02_delay9_write(delayedPhase9);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay10_read(1024, 5), 9, 1, 0);
        number delayedPhase10 = 0;
        auto result_31 = this->peek_default(this->gen_02_phaseData, 9, 0);
        delayedPhase10 = result_31[0];
        this->gen_02_delay10_write(delayedPhase10);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay11_read(1024, 5), 10, 1, 0);
        number delayedPhase11 = 0;
        auto result_32 = this->peek_default(this->gen_02_phaseData, 10, 0);
        delayedPhase11 = result_32[0];
        this->gen_02_delay11_write(delayedPhase11);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay12_read(1024, 5), 11, 1, 0);
        number delayedPhase12 = 0;
        auto result_33 = this->peek_default(this->gen_02_phaseData, 11, 0);
        delayedPhase12 = result_33[0];
        this->gen_02_delay12_write(delayedPhase12);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay13_read(1024, 5), 12, 1, 0);
        number delayedPhase13 = 0;
        auto result_34 = this->peek_default(this->gen_02_phaseData, 12, 0);
        delayedPhase13 = result_34[0];
        this->gen_02_delay13_write(delayedPhase13);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay14_read(1024, 5), 13, 1, 0);
        number delayedPhase14 = 0;
        auto result_35 = this->peek_default(this->gen_02_phaseData, 13, 0);
        delayedPhase14 = result_35[0];
        this->gen_02_delay14_write(delayedPhase14);
        this->poke_default(this->gen_02_phaseData, this->gen_02_delay15_read(1024, 5), 14, 1, 0);
        number delayedPhase15 = 0;
        auto result_36 = this->peek_default(this->gen_02_phaseData, 14, 0);
        delayedPhase15 = result_36[0];
        this->gen_02_delay15_write(delayedPhase15);

        for (number i = 0; i < this->dim(this->gen_02_currentRadius); i = i + 1) {
            number peek_8 = 0;
            number peek_9 = 0;
            auto result_37 = this->peek_default(this->gen_02_phaseData, i, 0);
            peek_9 = result_37[1];
            peek_8 = result_37[0];
            number peek_10 = 0;
            number peek_11 = 0;
            auto result_38 = this->peek_default(this->gen_02_phaseData, i, 1);
            peek_11 = result_38[1];
            peek_10 = result_38[0];
            number phaseDiff_39 = this->wrap(peek_8 - peek_10, -3.14159265358979323846, 3.14159265358979323846);
            number peek_12 = 0;
            number peek_13 = 0;
            auto result_40 = this->peek_default(this->gen_02_lastIndex, i, 0);
            peek_13 = result_40[1];
            peek_12 = result_40[0];
            number binFreq_41 = this->samplerate() / (number)1024 * peek_12;
            number freqDiff_42 = phaseDiff_39 * (this->samplerate() / (number)1024 / 6.28318530717958647692);
            this->poke_default(this->gen_02_detectionData, binFreq_41 + freqDiff_42, i, 0, 0);
            number peek_14 = 0;
            number peek_15 = 0;
            auto result_43 = this->peek_default(this->gen_02_lastRadius, i, 0);
            peek_15 = result_43[1];
            peek_14 = result_43[0];
            this->poke_default(this->gen_02_detectionData, peek_14 / (number)512, i, 1, 0);
        }

        for (number i = 0; i < this->dim(this->gen_02_currentRadius); i = i + 1) {
            number smoothedFreq = 0;
            auto result_44 = this->peek_default(this->gen_02_detectionData, i, 0);
            smoothedFreq = result_44[0];
            number oldDetectionFreq = 0;
            auto result_45 = this->peek_default(this->gen_02_smoothedData, i, 0);
            oldDetectionFreq = result_45[0];
            number smoothedFreq_16_46 = smoothedFreq + ((freqSmooth <= 0 ? 0 : rnbo_sqrt(freqSmooth))) * (oldDetectionFreq - smoothedFreq);
            this->poke_default(this->gen_02_smoothedData, smoothedFreq_16_46, i, 0, 0);
            this->poke_default(this->gen_02_detectionData, smoothedFreq_16_46, i, 0, 0);
            number smoothedAmp = 0;
            auto result_47 = this->peek_default(this->gen_02_detectionData, i, 1);
            smoothedAmp = result_47[0];
            number oldDetectionAmp = 0;
            auto result_48 = this->peek_default(this->gen_02_smoothedData, i, 1);
            oldDetectionAmp = result_48[0];
            number smoothedAmp_17_49 = smoothedAmp + ((ampSmooth <= 0 ? 0 : rnbo_sqrt(ampSmooth))) * (oldDetectionAmp - smoothedAmp);
            this->poke_default(this->gen_02_smoothedData, smoothedAmp_17_49, i, 1, 0);
            this->poke_default(this->gen_02_detectionData, smoothedAmp_17_49, i, 1, 0);
        }

        number cycle_18 = 0;
        number cycle_19 = 0;
        array<number, 2> result_51 = this->gen_02_cycle_50_next(modFreq, 0);
        cycle_19 = result_51[1];
        cycle_18 = result_51[0];
        this->gen_02_modFreqDel_write(cycle_18);

        number modFreqVal1_52 = this->gen_02_modFreqDel_read(
            ((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.066666666666667,
            0
        );

        number modFreqVal2_53 = this->gen_02_modFreqDel_read(
            ((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.13333333333333,
            0
        );

        number modFreqVal3_54 = this->gen_02_modFreqDel_read(((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.2, 0);

        number modFreqVal4_55 = this->gen_02_modFreqDel_read(
            ((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.26666666666667,
            0
        );

        number modFreqVal5_56 = this->gen_02_modFreqDel_read(
            ((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.33333333333333,
            0
        );

        number modFreqVal6_57 = this->gen_02_modFreqDel_read(((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.4, 0);

        number modFreqVal7_58 = this->gen_02_modFreqDel_read(
            ((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.46666666666667,
            0
        );

        number modFreqVal8_59 = this->gen_02_modFreqDel_read(
            ((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.53333333333333,
            0
        );

        number modFreqVal9_60 = this->gen_02_modFreqDel_read(((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.6, 0);

        number modFreqVal10_61 = this->gen_02_modFreqDel_read(
            ((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.66666666666667,
            0
        );

        number modFreqVal11_62 = this->gen_02_modFreqDel_read(
            ((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.73333333333333,
            0
        );

        number modFreqVal12_63 = this->gen_02_modFreqDel_read(((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.8, 0);

        number modFreqVal13_64 = this->gen_02_modFreqDel_read(
            ((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.86666666666667,
            0
        );

        number modFreqVal14_65 = this->gen_02_modFreqDel_read(
            ((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 0.93333333333333,
            0
        );

        number modFreqVal15_66 = this->gen_02_modFreqDel_read(((modFreq == 0. ? 0. : this->samplerate() / modFreq)) * 1, 0);

        for (number i = 0; i < this->dim(this->gen_02_currentRadius); i = i + 1) {
            if (equalTemperamentMode == 1) {
                number peek_20 = 0;
                number peek_21 = 0;
                auto result_67 = this->peek_default(this->gen_02_detectionData, i, 0);
                peek_21 = result_67[1];
                peek_20 = result_67[0];
                number calEqualTemperamentPitch_68 = 0;

                {
                    auto F_71 = tuning;
                    auto N_70 = equalTemperament;
                    auto input_69 = peek_20 * freqScale;

                    number n_72 = rnbo_fround(
                        N_70 * ((((F_71 == 0. ? 0. : input_69 / F_71)) <= 0 ? 0 : rnbo_log2((F_71 == 0. ? 0. : input_69 / F_71)))) * 1 / (number)1
                    ) * 1;

                    number mappedFreq_73 = F_71 * fixnan(rnbo_pow(2, (N_70 == 0. ? 0. : n_72 / N_70)));
                    calEqualTemperamentPitch_68 = mappedFreq_73;
                }

                this->poke_default(this->gen_02_detectionData, calEqualTemperamentPitch_68, i, 0, 0);
            } else {
                number peek_22 = 0;
                number peek_23 = 0;
                auto result_74 = this->peek_default(this->gen_02_detectionData, i, 0);
                peek_23 = result_74[1];
                peek_22 = result_74[0];
                this->poke_default(this->gen_02_detectionData, peek_22 * freqScale, i, 0, 0);
            }
        }

        number peek_24 = 0;
        number peek_25 = 0;
        auto result_75 = this->peek_default(this->gen_02_detectionData, 0, 0);
        peek_25 = result_75[1];
        peek_24 = result_75[0];
        this->poke_default(this->gen_02_detectionData, peek_24 + modFreqVal1_52 * modAmp, 0, 0, 0);
        number peek_26 = 0;
        number peek_27 = 0;
        auto result_76 = this->peek_default(this->gen_02_detectionData, 0, 1);
        peek_27 = result_76[1];
        peek_26 = result_76[0];
        this->poke_default(this->gen_02_detectionData, peek_26 * amp1, 0, 1, 0);
        number applyFreq1 = 0;
        auto result_77 = this->peek_default(this->gen_02_detectionData, 0, 0);
        applyFreq1 = result_77[0];
        number peek_28 = 0;
        number peek_29 = 0;
        auto result_78 = this->peek_default(this->gen_02_detectionData, 0, 1);
        peek_29 = result_78[1];
        peek_28 = result_78[0];

        number ampedWave1_80 = rnbo_sin(
            this->gen_02_phasor_79_next(applyFreq1, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_28;

        number peek_30 = 0;
        number peek_31 = 0;
        auto result_81 = this->peek_default(this->gen_02_detectionData, 1, 0);
        peek_31 = result_81[1];
        peek_30 = result_81[0];
        this->poke_default(this->gen_02_detectionData, peek_30 + modFreqVal2_53 * modAmp, 1, 0, 0);
        number peek_32 = 0;
        number peek_33 = 0;
        auto result_82 = this->peek_default(this->gen_02_detectionData, 1, 1);
        peek_33 = result_82[1];
        peek_32 = result_82[0];
        this->poke_default(this->gen_02_detectionData, peek_32 * amp2, 1, 1, 0);
        number applyFreq2 = 0;
        auto result_83 = this->peek_default(this->gen_02_detectionData, 1, 0);
        applyFreq2 = result_83[0];
        number peek_34 = 0;
        number peek_35 = 0;
        auto result_84 = this->peek_default(this->gen_02_detectionData, 1, 1);
        peek_35 = result_84[1];
        peek_34 = result_84[0];

        number ampedWave2_86 = rnbo_sin(
            this->gen_02_phasor_85_next(applyFreq2, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_34;

        number peek_36 = 0;
        number peek_37 = 0;
        auto result_87 = this->peek_default(this->gen_02_detectionData, 2, 0);
        peek_37 = result_87[1];
        peek_36 = result_87[0];
        this->poke_default(this->gen_02_detectionData, peek_36 + modFreqVal3_54 * modAmp, 2, 0, 0);
        number peek_38 = 0;
        number peek_39 = 0;
        auto result_88 = this->peek_default(this->gen_02_detectionData, 2, 1);
        peek_39 = result_88[1];
        peek_38 = result_88[0];
        this->poke_default(this->gen_02_detectionData, peek_38 * amp3, 2, 1, 0);
        number applyFreq3 = 0;
        auto result_89 = this->peek_default(this->gen_02_detectionData, 2, 0);
        applyFreq3 = result_89[0];
        number peek_40 = 0;
        number peek_41 = 0;
        auto result_90 = this->peek_default(this->gen_02_detectionData, 2, 1);
        peek_41 = result_90[1];
        peek_40 = result_90[0];

        number ampedWave3_92 = rnbo_sin(
            this->gen_02_phasor_91_next(applyFreq3, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_40;

        number peek_42 = 0;
        number peek_43 = 0;
        auto result_93 = this->peek_default(this->gen_02_detectionData, 3, 0);
        peek_43 = result_93[1];
        peek_42 = result_93[0];
        this->poke_default(this->gen_02_detectionData, peek_42 + modFreqVal4_55 * modAmp, 3, 0, 0);
        number peek_44 = 0;
        number peek_45 = 0;
        auto result_94 = this->peek_default(this->gen_02_detectionData, 3, 1);
        peek_45 = result_94[1];
        peek_44 = result_94[0];
        this->poke_default(this->gen_02_detectionData, peek_44 * amp4, 3, 1, 0);
        number applyFreq4 = 0;
        auto result_95 = this->peek_default(this->gen_02_detectionData, 3, 0);
        applyFreq4 = result_95[0];
        number peek_46 = 0;
        number peek_47 = 0;
        auto result_96 = this->peek_default(this->gen_02_detectionData, 3, 1);
        peek_47 = result_96[1];
        peek_46 = result_96[0];

        number ampedWave4_98 = rnbo_sin(
            this->gen_02_phasor_97_next(applyFreq4, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_46;

        number peek_48 = 0;
        number peek_49 = 0;
        auto result_99 = this->peek_default(this->gen_02_detectionData, 4, 0);
        peek_49 = result_99[1];
        peek_48 = result_99[0];
        this->poke_default(this->gen_02_detectionData, peek_48 + modFreqVal5_56 * modAmp, 4, 0, 0);
        number peek_50 = 0;
        number peek_51 = 0;
        auto result_100 = this->peek_default(this->gen_02_detectionData, 4, 1);
        peek_51 = result_100[1];
        peek_50 = result_100[0];
        this->poke_default(this->gen_02_detectionData, peek_50 * amp5, 4, 1, 0);
        number applyFreq5 = 0;
        auto result_101 = this->peek_default(this->gen_02_detectionData, 4, 0);
        applyFreq5 = result_101[0];
        number peek_52 = 0;
        number peek_53 = 0;
        auto result_102 = this->peek_default(this->gen_02_detectionData, 4, 1);
        peek_53 = result_102[1];
        peek_52 = result_102[0];

        number ampedWave5_104 = rnbo_sin(
            this->gen_02_phasor_103_next(applyFreq5, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_52;

        number peek_54 = 0;
        number peek_55 = 0;
        auto result_105 = this->peek_default(this->gen_02_detectionData, 5, 0);
        peek_55 = result_105[1];
        peek_54 = result_105[0];
        this->poke_default(this->gen_02_detectionData, peek_54 + modFreqVal6_57 * modAmp, 5, 0, 0);
        number peek_56 = 0;
        number peek_57 = 0;
        auto result_106 = this->peek_default(this->gen_02_detectionData, 5, 1);
        peek_57 = result_106[1];
        peek_56 = result_106[0];
        this->poke_default(this->gen_02_detectionData, peek_56 * amp6, 5, 1, 0);
        number applyFreq6 = 0;
        auto result_107 = this->peek_default(this->gen_02_detectionData, 5, 0);
        applyFreq6 = result_107[0];
        number peek_58 = 0;
        number peek_59 = 0;
        auto result_108 = this->peek_default(this->gen_02_detectionData, 5, 1);
        peek_59 = result_108[1];
        peek_58 = result_108[0];

        number ampedWave6_110 = rnbo_sin(
            this->gen_02_phasor_109_next(applyFreq6, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_58;

        number peek_60 = 0;
        number peek_61 = 0;
        auto result_111 = this->peek_default(this->gen_02_detectionData, 6, 0);
        peek_61 = result_111[1];
        peek_60 = result_111[0];
        this->poke_default(this->gen_02_detectionData, peek_60 + modFreqVal7_58 * modAmp, 6, 0, 0);
        number peek_62 = 0;
        number peek_63 = 0;
        auto result_112 = this->peek_default(this->gen_02_detectionData, 6, 1);
        peek_63 = result_112[1];
        peek_62 = result_112[0];
        this->poke_default(this->gen_02_detectionData, peek_62 * amp7, 6, 1, 0);
        number applyFreq7 = 0;
        auto result_113 = this->peek_default(this->gen_02_detectionData, 6, 0);
        applyFreq7 = result_113[0];
        number peek_64 = 0;
        number peek_65 = 0;
        auto result_114 = this->peek_default(this->gen_02_detectionData, 6, 1);
        peek_65 = result_114[1];
        peek_64 = result_114[0];

        number ampedWave7_116 = rnbo_sin(
            this->gen_02_phasor_115_next(applyFreq7, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_64;

        number peek_66 = 0;
        number peek_67 = 0;
        auto result_117 = this->peek_default(this->gen_02_detectionData, 7, 0);
        peek_67 = result_117[1];
        peek_66 = result_117[0];
        this->poke_default(this->gen_02_detectionData, peek_66 + modFreqVal8_59 * modAmp, 7, 0, 0);
        number peek_68 = 0;
        number peek_69 = 0;
        auto result_118 = this->peek_default(this->gen_02_detectionData, 7, 1);
        peek_69 = result_118[1];
        peek_68 = result_118[0];
        this->poke_default(this->gen_02_detectionData, peek_68 * amp8, 7, 1, 0);
        number applyFreq8 = 0;
        auto result_119 = this->peek_default(this->gen_02_detectionData, 7, 0);
        applyFreq8 = result_119[0];
        number peek_70 = 0;
        number peek_71 = 0;
        auto result_120 = this->peek_default(this->gen_02_detectionData, 7, 1);
        peek_71 = result_120[1];
        peek_70 = result_120[0];

        number ampedWave8_122 = rnbo_sin(
            this->gen_02_phasor_121_next(applyFreq8, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_70;

        number peek_72 = 0;
        number peek_73 = 0;
        auto result_123 = this->peek_default(this->gen_02_detectionData, 8, 0);
        peek_73 = result_123[1];
        peek_72 = result_123[0];
        this->poke_default(this->gen_02_detectionData, peek_72 + modFreqVal9_60 * modAmp, 8, 0, 0);
        number peek_74 = 0;
        number peek_75 = 0;
        auto result_124 = this->peek_default(this->gen_02_detectionData, 8, 1);
        peek_75 = result_124[1];
        peek_74 = result_124[0];
        this->poke_default(this->gen_02_detectionData, peek_74 * amp9, 8, 1, 0);
        number applyFreq9 = 0;
        auto result_125 = this->peek_default(this->gen_02_detectionData, 8, 0);
        applyFreq9 = result_125[0];
        number peek_76 = 0;
        number peek_77 = 0;
        auto result_126 = this->peek_default(this->gen_02_detectionData, 8, 1);
        peek_77 = result_126[1];
        peek_76 = result_126[0];

        number ampedWave9_128 = rnbo_sin(
            this->gen_02_phasor_127_next(applyFreq9, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_76;

        number peek_78 = 0;
        number peek_79 = 0;
        auto result_129 = this->peek_default(this->gen_02_detectionData, 9, 0);
        peek_79 = result_129[1];
        peek_78 = result_129[0];
        this->poke_default(this->gen_02_detectionData, peek_78 + modFreqVal10_61 * modAmp, 9, 0, 0);
        number peek_80 = 0;
        number peek_81 = 0;
        auto result_130 = this->peek_default(this->gen_02_detectionData, 9, 1);
        peek_81 = result_130[1];
        peek_80 = result_130[0];
        this->poke_default(this->gen_02_detectionData, peek_80 * amp10, 9, 1, 0);
        number applyFreq10 = 0;
        auto result_131 = this->peek_default(this->gen_02_detectionData, 9, 0);
        applyFreq10 = result_131[0];
        number peek_82 = 0;
        number peek_83 = 0;
        auto result_132 = this->peek_default(this->gen_02_detectionData, 9, 1);
        peek_83 = result_132[1];
        peek_82 = result_132[0];

        number ampedWave10_134 = rnbo_sin(
            this->gen_02_phasor_133_next(applyFreq10, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_82;

        number peek_84 = 0;
        number peek_85 = 0;
        auto result_135 = this->peek_default(this->gen_02_detectionData, 10, 0);
        peek_85 = result_135[1];
        peek_84 = result_135[0];
        this->poke_default(this->gen_02_detectionData, peek_84 + modFreqVal11_62 * modAmp, 10, 0, 0);
        number peek_86 = 0;
        number peek_87 = 0;
        auto result_136 = this->peek_default(this->gen_02_detectionData, 10, 1);
        peek_87 = result_136[1];
        peek_86 = result_136[0];
        this->poke_default(this->gen_02_detectionData, peek_86 * amp11, 10, 1, 0);
        number applyFreq11 = 0;
        auto result_137 = this->peek_default(this->gen_02_detectionData, 10, 0);
        applyFreq11 = result_137[0];
        number peek_88 = 0;
        number peek_89 = 0;
        auto result_138 = this->peek_default(this->gen_02_detectionData, 10, 1);
        peek_89 = result_138[1];
        peek_88 = result_138[0];

        number ampedWave11_140 = rnbo_sin(
            this->gen_02_phasor_139_next(applyFreq11, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_88;

        number peek_90 = 0;
        number peek_91 = 0;
        auto result_141 = this->peek_default(this->gen_02_detectionData, 11, 0);
        peek_91 = result_141[1];
        peek_90 = result_141[0];
        this->poke_default(this->gen_02_detectionData, peek_90 + modFreqVal12_63 * modAmp, 11, 0, 0);
        number peek_92 = 0;
        number peek_93 = 0;
        auto result_142 = this->peek_default(this->gen_02_detectionData, 11, 1);
        peek_93 = result_142[1];
        peek_92 = result_142[0];
        this->poke_default(this->gen_02_detectionData, peek_92 * amp12, 11, 1, 0);
        number applyFreq12 = 0;
        auto result_143 = this->peek_default(this->gen_02_detectionData, 11, 0);
        applyFreq12 = result_143[0];
        number peek_94 = 0;
        number peek_95 = 0;
        auto result_144 = this->peek_default(this->gen_02_detectionData, 11, 1);
        peek_95 = result_144[1];
        peek_94 = result_144[0];

        number ampedWave12_146 = rnbo_sin(
            this->gen_02_phasor_145_next(applyFreq12, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_94;

        number peek_96 = 0;
        number peek_97 = 0;
        auto result_147 = this->peek_default(this->gen_02_detectionData, 12, 0);
        peek_97 = result_147[1];
        peek_96 = result_147[0];
        this->poke_default(this->gen_02_detectionData, peek_96 + modFreqVal13_64 * modAmp, 12, 0, 0);
        number peek_98 = 0;
        number peek_99 = 0;
        auto result_148 = this->peek_default(this->gen_02_detectionData, 12, 1);
        peek_99 = result_148[1];
        peek_98 = result_148[0];
        this->poke_default(this->gen_02_detectionData, peek_98 * amp13, 12, 1, 0);
        number applyFreq13 = 0;
        auto result_149 = this->peek_default(this->gen_02_detectionData, 12, 0);
        applyFreq13 = result_149[0];
        number peek_100 = 0;
        number peek_101 = 0;
        auto result_150 = this->peek_default(this->gen_02_detectionData, 12, 1);
        peek_101 = result_150[1];
        peek_100 = result_150[0];

        number ampedWave13_152 = rnbo_sin(
            this->gen_02_phasor_151_next(applyFreq13, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_100;

        number peek_102 = 0;
        number peek_103 = 0;
        auto result_153 = this->peek_default(this->gen_02_detectionData, 13, 0);
        peek_103 = result_153[1];
        peek_102 = result_153[0];
        this->poke_default(this->gen_02_detectionData, peek_102 + modFreqVal14_65 * modAmp, 13, 0, 0);
        number peek_104 = 0;
        number peek_105 = 0;
        auto result_154 = this->peek_default(this->gen_02_detectionData, 13, 1);
        peek_105 = result_154[1];
        peek_104 = result_154[0];
        this->poke_default(this->gen_02_detectionData, peek_104 * amp14, 13, 1, 0);
        number applyFreq14 = 0;
        auto result_155 = this->peek_default(this->gen_02_detectionData, 13, 0);
        applyFreq14 = result_155[0];
        number peek_106 = 0;
        number peek_107 = 0;
        auto result_156 = this->peek_default(this->gen_02_detectionData, 13, 1);
        peek_107 = result_156[1];
        peek_106 = result_156[0];

        number ampedWave14_158 = rnbo_sin(
            this->gen_02_phasor_157_next(applyFreq14, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_106;

        number peek_108 = 0;
        number peek_109 = 0;
        auto result_159 = this->peek_default(this->gen_02_detectionData, 14, 0);
        peek_109 = result_159[1];
        peek_108 = result_159[0];
        this->poke_default(this->gen_02_detectionData, peek_108 + modFreqVal15_66 * modAmp, 14, 0, 0);
        number peek_110 = 0;
        number peek_111 = 0;
        auto result_160 = this->peek_default(this->gen_02_detectionData, 14, 1);
        peek_111 = result_160[1];
        peek_110 = result_160[0];
        this->poke_default(this->gen_02_detectionData, peek_110 * amp15, 14, 1, 0);
        number applyFreq15 = 0;
        auto result_161 = this->peek_default(this->gen_02_detectionData, 14, 0);
        applyFreq15 = result_161[0];
        number peek_112 = 0;
        number peek_113 = 0;
        auto result_162 = this->peek_default(this->gen_02_detectionData, 14, 1);
        peek_113 = result_162[1];
        peek_112 = result_162[0];

        number ampedWave15_164 = rnbo_sin(
            this->gen_02_phasor_163_next(applyFreq15, 0) * 6.28318530717958647692 - 3.14159265358979323846
        ) * peek_112;

        number cycle_114 = 0;
        number cycle_115 = 0;
        array<number, 2> result_166 = this->gen_02_cycle_165_next(modPanFreq, 0);
        cycle_115 = result_166[1];
        cycle_114 = result_166[0];
        this->gen_02_modPanDel_write(cycle_114);

        number modPanVal1_167 = this->gen_02_modPanDel_read(
            ((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.066666666666667,
            0
        );

        number stereoL1_168 = ampedWave1_80 * ((modPanVal1_167 + 1) / (number)2);
        number stereoR1_169 = ampedWave1_80 * ((-modPanVal1_167 + 1) / (number)2);

        number modPanVal2_170 = this->gen_02_modPanDel_read(
            ((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.13333333333333,
            0
        );

        number stereoL2_171 = ampedWave2_86 * ((modPanVal2_170 + 1) / (number)2);
        number stereoR2_172 = ampedWave2_86 * ((-modPanVal2_170 + 1) / (number)2);
        number modPanVal3_173 = this->gen_02_modPanDel_read(((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.2, 0);
        number stereoL3_174 = ampedWave3_92 * ((modPanVal3_173 + 1) / (number)2);
        number stereoR3_175 = ampedWave3_92 * ((-modPanVal3_173 + 1) / (number)2);

        number modPanVal4_176 = this->gen_02_modPanDel_read(
            ((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.26666666666667,
            0
        );

        number stereoL4_177 = ampedWave4_98 * ((modPanVal4_176 + 1) / (number)2);
        number stereoR4_178 = ampedWave4_98 * ((-modPanVal4_176 + 1) / (number)2);

        number modPanVal5_179 = this->gen_02_modPanDel_read(
            ((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.33333333333333,
            0
        );

        number stereoL5_180 = ampedWave5_104 * ((modPanVal5_179 + 1) / (number)2);
        number stereoR5_181 = ampedWave5_104 * ((-modPanVal5_179 + 1) / (number)2);
        number modPanVal6_182 = this->gen_02_modPanDel_read(((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.4, 0);
        number stereoL6_183 = ampedWave6_110 * ((modPanVal6_182 + 1) / (number)2);
        number stereoR6_184 = ampedWave6_110 * ((-modPanVal6_182 + 1) / (number)2);

        number modPanVal7_185 = this->gen_02_modPanDel_read(
            ((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.46666666666667,
            0
        );

        number stereoL7_186 = ampedWave7_116 * ((modPanVal7_185 + 1) / (number)2);
        number stereoR7_187 = ampedWave7_116 * ((-modPanVal7_185 + 1) / (number)2);

        number modPanVal8_188 = this->gen_02_modPanDel_read(
            ((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.53333333333333,
            0
        );

        number stereoL8_189 = ampedWave8_122 * ((modPanVal8_188 + 1) / (number)2);
        number stereoR8_190 = ampedWave8_122 * ((-modPanVal8_188 + 1) / (number)2);
        number modPanVal9_191 = this->gen_02_modPanDel_read(((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.6, 0);
        number stereoL9_192 = ampedWave9_128 * ((modPanVal9_191 + 1) / (number)2);
        number stereoR9_193 = ampedWave9_128 * ((-modPanVal9_191 + 1) / (number)2);

        number modPanVal10_194 = this->gen_02_modPanDel_read(
            ((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.66666666666667,
            0
        );

        number stereoL10_195 = ampedWave10_134 * ((modPanVal10_194 + 1) / (number)2);
        number stereoR10_196 = ampedWave10_134 * ((-modPanVal10_194 + 1) / (number)2);

        number modPanVal11_197 = this->gen_02_modPanDel_read(
            ((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.73333333333333,
            0
        );

        number stereoL11_198 = ampedWave11_140 * ((modPanVal11_197 + 1) / (number)2);
        number stereoR11_199 = ampedWave11_140 * ((-modPanVal11_197 + 1) / (number)2);
        number modPanVal12_200 = this->gen_02_modPanDel_read(((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.8, 0);
        number stereoL12_201 = ampedWave12_146 * ((modPanVal12_200 + 1) / (number)2);
        number stereoR12_202 = ampedWave12_146 * ((-modPanVal12_200 + 1) / (number)2);

        number modPanVal13_203 = this->gen_02_modPanDel_read(
            ((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.86666666666667,
            0
        );

        number stereoL13_204 = ampedWave13_152 * ((modPanVal13_203 + 1) / (number)2);
        number stereoR13_205 = ampedWave13_152 * ((-modPanVal13_203 + 1) / (number)2);

        number modPanVal14_206 = this->gen_02_modPanDel_read(
            ((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 0.93333333333333,
            0
        );

        number stereoL14_207 = ampedWave14_158 * ((modPanVal14_206 + 1) / (number)2);
        number stereoR14_208 = ampedWave14_158 * ((-modPanVal14_206 + 1) / (number)2);
        number modPanVal15_209 = this->gen_02_modPanDel_read(((modPanFreq == 0. ? 0. : this->samplerate() / modPanFreq)) * 1, 0);
        number stereoL15_210 = ampedWave15_164 * ((modPanVal15_209 + 1) / (number)2);
        number stereoR15_211 = ampedWave15_164 * ((-modPanVal15_209 + 1) / (number)2);
        number synthesizedWaveL_212 = 0;
        number synthesizedWaveR_213 = 0;

        if (stereoMode == 0) {
            synthesizedWaveL_212 = ampedWave1_80 + ampedWave3_92 + ampedWave5_104 + ampedWave7_116 + ampedWave9_128 + ampedWave11_140 + ampedWave13_152 + ampedWave15_164;
            synthesizedWaveR_213 = ampedWave2_86 + ampedWave4_98 + ampedWave6_110 + ampedWave8_122 + ampedWave10_134 + ampedWave12_146 + ampedWave14_158;
        } else {
            synthesizedWaveL_212 = stereoL1_168 + stereoL2_171 + stereoL3_174 + stereoL4_177 + stereoL5_180 + stereoL6_183 + stereoL7_186 + stereoL8_189 + stereoL9_192 + stereoL10_195 + stereoL11_198 + stereoL12_201 + stereoL13_204 + stereoL14_207 + stereoL15_210;
            synthesizedWaveR_213 = stereoR1_169 + stereoR2_172 + stereoR3_175 + stereoR4_178 + stereoR5_181 + stereoR6_184 + stereoR7_187 + stereoR8_190 + stereoR9_193 + stereoR10_196 + stereoR11_199 + stereoR12_202 + stereoR13_205 + stereoR14_208 + stereoR15_211;
        }

        number expr_116_214 = (synthesizedWaveL_212 * 1.5 > 1 ? 1 : (synthesizedWaveL_212 * 1.5 < -1 ? -1 : synthesizedWaveL_212 * 1.5));
        number expr_117_215 = (synthesizedWaveR_213 * 1.5 > 1 ? 1 : (synthesizedWaveR_213 * 1.5 < -1 ? -1 : synthesizedWaveR_213 * 1.5));
        out1[(Index)i0] = expr_116_214;
        out2[(Index)i0] = expr_117_215;
        this->gen_02_modPanDel_step();
        this->gen_02_modFreqDel_step();
        this->gen_02_delay15_step();
        this->gen_02_delay14_step();
        this->gen_02_delay13_step();
        this->gen_02_delay12_step();
        this->gen_02_delay11_step();
        this->gen_02_delay10_step();
        this->gen_02_delay9_step();
        this->gen_02_delay8_step();
        this->gen_02_delay7_step();
        this->gen_02_delay6_step();
        this->gen_02_delay5_step();
        this->gen_02_delay4_step();
        this->gen_02_delay3_step();
        this->gen_02_delay2_step();
        this->gen_02_delay1_step();
    }
}

void gen_01_perform(
    const Sample * in1,
    const Sample * in2,
    number mix,
    number delayCom,
    number delayAll,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    number div_10_0 = mix / (number)100;
    auto scale_11_1 = this->scale(mix, 0, 100, 1, 0.175, 0.1);
    Index i;

    for (i = 0; i < n; i++) {
        number mul_12_2 = in1[(Index)i] * scale_11_1;
        number mul_13_3 = mul_12_2 * 0.9;
        number mul_14_4 = in2[(Index)i] * scale_11_1;
        number mul_15_5 = mul_14_4 * 0.9;
        number mul1_6 = this->gen_01_del2_read(this->mstosamps(delayCom) / (number)1.321, 0) * 0.75;
        number mul2_7 = this->gen_01_del3_read(this->mstosamps(delayCom) / (number)1.321, 0) * 0.75;
        number add2_8 = mul_13_3 + mul1_6;
        number add3_9 = mul_15_5 + mul2_7;
        number expr_16_10 = this->__wrapped_op_clamp(rnbo_atan(add2_8), -1, 1);
        number expr_17_11 = this->__wrapped_op_clamp(rnbo_atan(add3_9), -1, 1);
        this->gen_01_del2_write(add2_8 + mul_13_3);
        this->gen_01_del3_write(add3_9 + mul_15_5);
        number mul_18_12 = this->gen_01_del_1_read(this->mstosamps(delayCom), 0) * 0.72;
        number mul_19_13 = this->gen_01_del_2_read(this->mstosamps(delayCom), 0) * 0.72;
        number add_20_14 = mul_13_3 + mul_18_12;
        number add_21_15 = mul_15_5 + mul_19_13;
        number expr_22_16 = this->__wrapped_op_clamp(rnbo_atan(add_20_14), -1, 1);
        number expr_23_17 = this->__wrapped_op_clamp(rnbo_atan(add_21_15), -1, 1);
        this->gen_01_del_1_write(add_20_14 + mul_13_3);
        this->gen_01_del_2_write(add_21_15 + mul_15_5);
        number mul_24_18 = this->gen_01_del_3_read(this->mstosamps(delayCom) / (number)1.103, 0) * 0.73;
        number mul_25_19 = this->gen_01_del_4_read(this->mstosamps(delayCom) / (number)1.103, 0) * 0.73;
        number add_26_20 = mul_13_3 + mul_24_18;
        number add_27_21 = mul_15_5 + mul_25_19;
        number expr_28_22 = this->__wrapped_op_clamp(rnbo_atan(add_26_20), -1, 1);
        number expr_29_23 = this->__wrapped_op_clamp(rnbo_atan(add_27_21), -1, 1);
        this->gen_01_del_3_write(add_26_20 + mul_13_3);
        this->gen_01_del_4_write(add_27_21 + mul_15_5);
        number mul_30_24 = this->gen_01_del_5_read(this->mstosamps(delayCom) / (number)1.197, 0) * 0.74;
        number mul_31_25 = this->gen_01_del_6_read(this->mstosamps(delayCom) / (number)1.197, 0) * 0.74;
        number add_32_26 = mul_13_3 + mul_30_24;
        number add_33_27 = mul_15_5 + mul_31_25;
        number expr_34_28 = this->__wrapped_op_clamp(rnbo_atan(add_32_26), -1, 1);
        number expr_35_29 = this->__wrapped_op_clamp(rnbo_atan(add_33_27), -1, 1);
        this->gen_01_del_5_write(add_32_26 + mul_13_3);
        this->gen_01_del_6_write(add_33_27 + mul_15_5);
        number add_36_30 = expr_34_28 + expr_22_16 + (expr_28_22 + expr_16_10);
        number add_37_31 = expr_29_23 + expr_17_11 + expr_35_29 + expr_23_17 + 0;
        number tap1_32 = this->gen_01_del_7_read(this->mstosamps(delayAll), 0);
        number tap2_33 = this->gen_01_del_7_read(this->mstosamps(delayAll), 0);
        number add1_34 = add_36_30 + tap1_32 * 0.7 * -1;
        number add_38_35 = add_37_31 + tap2_33 * 0.7 * -1;
        number expr_39_36 = (rnbo_atan(add_38_35) * 0.6 + tap1_32 > 1 ? 1 : (rnbo_atan(add_38_35) * 0.6 + tap1_32 < -1 ? -1 : rnbo_atan(add_38_35) * 0.6 + tap1_32));
        number expr_40_37 = (rnbo_atan(add3_9) * 0.6 + tap1_32 > 1 ? 1 : (rnbo_atan(add3_9) * 0.6 + tap1_32 < -1 ? -1 : rnbo_atan(add3_9) * 0.6 + tap1_32));
        this->gen_01_del1_write(add1_34);
        this->gen_01_del_7_write(add_38_35);
        number tap_41_38 = this->gen_01_del_8_read(this->mstosamps(delayAll) / (number)2.941, 0);
        number tap_42_39 = this->gen_01_del_9_read(this->mstosamps(delayAll) / (number)2.941, 0);
        number add_43_40 = expr_39_36 + tap_41_38 * 0.7 * -1;
        number add_44_41 = expr_40_37 + tap_42_39 * 0.7 * -1;
        number expr_45_42 = (rnbo_atan(add_44_41) * 0.6 + tap_41_38 > 1 ? 1 : (rnbo_atan(add_44_41) * 0.6 + tap_41_38 < -1 ? -1 : rnbo_atan(add_44_41) * 0.6 + tap_41_38));
        number expr_46_43 = (rnbo_atan(add3_9) * 0.6 + tap_41_38 > 1 ? 1 : (rnbo_atan(add3_9) * 0.6 + tap_41_38 < -1 ? -1 : rnbo_atan(add3_9) * 0.6 + tap_41_38));
        this->gen_01_del_8_write(add_43_40);
        this->gen_01_del_9_write(add_44_41);
        number add_47_44 = expr_46_43 + 0;
        number mix_48_45 = in2[(Index)i] + div_10_0 * (add_47_44 - in2[(Index)i]);
        out2[(Index)i] = mix_48_45;
        number add_49_46 = expr_45_42 + 0;
        number mix_50_47 = in1[(Index)i] + div_10_0 * (add_49_46 - in1[(Index)i]);
        out1[(Index)i] = mix_50_47;
        this->gen_01_del_9_step();
        this->gen_01_del_8_step();
        this->gen_01_del_7_step();
        this->gen_01_del1_step();
        this->gen_01_del_6_step();
        this->gen_01_del_5_step();
        this->gen_01_del_4_step();
        this->gen_01_del_3_step();
        this->gen_01_del_2_step();
        this->gen_01_del_1_step();
        this->gen_01_del3_step();
        this->gen_01_del2_step();
    }
}

void dspexpr_01_perform(
    const Sample * in1,
    const Sample * in2,
    number in3,
    SampleValue * out1,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] + in3 * (in2[(Index)i] - in1[(Index)i]);//#map:_###_obj_###_:1
    }
}

void dspexpr_03_perform(
    const Sample * in1,
    const Sample * in2,
    number in3,
    SampleValue * out1,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] + in3 * (in2[(Index)i] - in1[(Index)i]);//#map:_###_obj_###_:1
    }
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

void data_01_srout_set(number ) {}

void data_01_chanout_set(number ) {}

void data_01_sizeout_set(number v) {
    this->data_01_sizeout = v;
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

void param_05_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_05_value;
}

void param_05_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_05_value_set(preset["value"]);
}

void param_06_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_06_value;
}

void param_06_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_06_value_set(preset["value"]);
}

void gen_01_del_9_step() {
    this->gen_01_del_9_reader++;

    if (this->gen_01_del_9_reader >= (int)(this->gen_01_del_9_buffer->getSize()))
        this->gen_01_del_9_reader = 0;
}

number gen_01_del_9_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_9_buffer->getSize()) + this->gen_01_del_9_reader - ((size > this->gen_01_del_9__maxdelay ? this->gen_01_del_9__maxdelay : (size < (this->gen_01_del_9_reader != this->gen_01_del_9_writer) ? this->gen_01_del_9_reader != this->gen_01_del_9_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_9_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_9_wrap))
        ), this->gen_01_del_9_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_9_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_9_buffer->getSize()) + this->gen_01_del_9_reader - ((size > this->gen_01_del_9__maxdelay ? this->gen_01_del_9__maxdelay : (size < (this->gen_01_del_9_reader != this->gen_01_del_9_writer) ? this->gen_01_del_9_reader != this->gen_01_del_9_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_9_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_9_wrap))
    );
}

void gen_01_del_9_write(number v) {
    this->gen_01_del_9_writer = this->gen_01_del_9_reader;
    this->gen_01_del_9_buffer[(Index)this->gen_01_del_9_writer] = v;
}

number gen_01_del_9_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_9__maxdelay : size);
    number val = this->gen_01_del_9_read(effectiveSize, 0);
    this->gen_01_del_9_write(v);
    this->gen_01_del_9_step();
    return val;
}

array<Index, 2> gen_01_del_9_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_9_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_9_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_9_init() {
    auto result = this->gen_01_del_9_calcSizeInSamples();
    this->gen_01_del_9__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_9_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_9_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_9_clear() {
    this->gen_01_del_9_buffer->setZero();
}

void gen_01_del_9_reset() {
    auto result = this->gen_01_del_9_calcSizeInSamples();
    this->gen_01_del_9__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_9_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_9_buffer);
    this->gen_01_del_9_wrap = this->gen_01_del_9_buffer->getSize() - 1;
    this->gen_01_del_9_clear();

    if (this->gen_01_del_9_reader >= this->gen_01_del_9__maxdelay || this->gen_01_del_9_writer >= this->gen_01_del_9__maxdelay) {
        this->gen_01_del_9_reader = 0;
        this->gen_01_del_9_writer = 0;
    }
}

void gen_01_del_9_dspsetup() {
    this->gen_01_del_9_reset();
}

number gen_01_del_9_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_9_size() {
    return this->gen_01_del_9__maxdelay;
}

void gen_01_del_8_step() {
    this->gen_01_del_8_reader++;

    if (this->gen_01_del_8_reader >= (int)(this->gen_01_del_8_buffer->getSize()))
        this->gen_01_del_8_reader = 0;
}

number gen_01_del_8_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_8_buffer->getSize()) + this->gen_01_del_8_reader - ((size > this->gen_01_del_8__maxdelay ? this->gen_01_del_8__maxdelay : (size < (this->gen_01_del_8_reader != this->gen_01_del_8_writer) ? this->gen_01_del_8_reader != this->gen_01_del_8_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_8_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_8_wrap))
        ), this->gen_01_del_8_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_8_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_8_buffer->getSize()) + this->gen_01_del_8_reader - ((size > this->gen_01_del_8__maxdelay ? this->gen_01_del_8__maxdelay : (size < (this->gen_01_del_8_reader != this->gen_01_del_8_writer) ? this->gen_01_del_8_reader != this->gen_01_del_8_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_8_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_8_wrap))
    );
}

void gen_01_del_8_write(number v) {
    this->gen_01_del_8_writer = this->gen_01_del_8_reader;
    this->gen_01_del_8_buffer[(Index)this->gen_01_del_8_writer] = v;
}

number gen_01_del_8_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_8__maxdelay : size);
    number val = this->gen_01_del_8_read(effectiveSize, 0);
    this->gen_01_del_8_write(v);
    this->gen_01_del_8_step();
    return val;
}

array<Index, 2> gen_01_del_8_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_8_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_8_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_8_init() {
    auto result = this->gen_01_del_8_calcSizeInSamples();
    this->gen_01_del_8__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_8_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_8_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_8_clear() {
    this->gen_01_del_8_buffer->setZero();
}

void gen_01_del_8_reset() {
    auto result = this->gen_01_del_8_calcSizeInSamples();
    this->gen_01_del_8__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_8_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_8_buffer);
    this->gen_01_del_8_wrap = this->gen_01_del_8_buffer->getSize() - 1;
    this->gen_01_del_8_clear();

    if (this->gen_01_del_8_reader >= this->gen_01_del_8__maxdelay || this->gen_01_del_8_writer >= this->gen_01_del_8__maxdelay) {
        this->gen_01_del_8_reader = 0;
        this->gen_01_del_8_writer = 0;
    }
}

void gen_01_del_8_dspsetup() {
    this->gen_01_del_8_reset();
}

number gen_01_del_8_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_8_size() {
    return this->gen_01_del_8__maxdelay;
}

void gen_01_del_7_step() {
    this->gen_01_del_7_reader++;

    if (this->gen_01_del_7_reader >= (int)(this->gen_01_del_7_buffer->getSize()))
        this->gen_01_del_7_reader = 0;
}

number gen_01_del_7_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_7_buffer->getSize()) + this->gen_01_del_7_reader - ((size > this->gen_01_del_7__maxdelay ? this->gen_01_del_7__maxdelay : (size < (this->gen_01_del_7_reader != this->gen_01_del_7_writer) ? this->gen_01_del_7_reader != this->gen_01_del_7_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_7_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_7_wrap))
        ), this->gen_01_del_7_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_7_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_7_buffer->getSize()) + this->gen_01_del_7_reader - ((size > this->gen_01_del_7__maxdelay ? this->gen_01_del_7__maxdelay : (size < (this->gen_01_del_7_reader != this->gen_01_del_7_writer) ? this->gen_01_del_7_reader != this->gen_01_del_7_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_7_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_7_wrap))
    );
}

void gen_01_del_7_write(number v) {
    this->gen_01_del_7_writer = this->gen_01_del_7_reader;
    this->gen_01_del_7_buffer[(Index)this->gen_01_del_7_writer] = v;
}

number gen_01_del_7_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_7__maxdelay : size);
    number val = this->gen_01_del_7_read(effectiveSize, 0);
    this->gen_01_del_7_write(v);
    this->gen_01_del_7_step();
    return val;
}

array<Index, 2> gen_01_del_7_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_7_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_7_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_7_init() {
    auto result = this->gen_01_del_7_calcSizeInSamples();
    this->gen_01_del_7__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_7_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_7_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_7_clear() {
    this->gen_01_del_7_buffer->setZero();
}

void gen_01_del_7_reset() {
    auto result = this->gen_01_del_7_calcSizeInSamples();
    this->gen_01_del_7__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_7_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_7_buffer);
    this->gen_01_del_7_wrap = this->gen_01_del_7_buffer->getSize() - 1;
    this->gen_01_del_7_clear();

    if (this->gen_01_del_7_reader >= this->gen_01_del_7__maxdelay || this->gen_01_del_7_writer >= this->gen_01_del_7__maxdelay) {
        this->gen_01_del_7_reader = 0;
        this->gen_01_del_7_writer = 0;
    }
}

void gen_01_del_7_dspsetup() {
    this->gen_01_del_7_reset();
}

number gen_01_del_7_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_7_size() {
    return this->gen_01_del_7__maxdelay;
}

void gen_01_del1_step() {
    this->gen_01_del1_reader++;

    if (this->gen_01_del1_reader >= (int)(this->gen_01_del1_buffer->getSize()))
        this->gen_01_del1_reader = 0;
}

number gen_01_del1_read(number size, Int interp) {
    if (interp == 0) {
        number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (this->gen_01_del1_reader != this->gen_01_del1_writer) ? this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del1_wrap))
        ));
    } else if (interp == 1) {
        number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (1 + this->gen_01_del1_reader != this->gen_01_del1_writer) ? 1 + this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        Index index2 = (Index)(index1 + 1);
        Index index3 = (Index)(index2 + 1);
        Index index4 = (Index)(index3 + 1);

        return this->cubicinterp(frac, this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index3 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index4 & (BinOpInt)this->gen_01_del1_wrap))
        ));
    } else if (interp == 2) {
        number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (1 + this->gen_01_del1_reader != this->gen_01_del1_writer) ? 1 + this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        Index index2 = (Index)(index1 + 1);
        Index index3 = (Index)(index2 + 1);
        Index index4 = (Index)(index3 + 1);

        return this->splineinterp(frac, this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index3 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index4 & (BinOpInt)this->gen_01_del1_wrap))
        ));
    } else if (interp == 3) {
        number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (this->gen_01_del1_reader != this->gen_01_del1_writer) ? this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        Index index2 = (Index)(index1 + 1);

        return this->cosineinterp(frac, this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del1_wrap))
        ));
    }

    number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (this->gen_01_del1_reader != this->gen_01_del1_writer) ? this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del1_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
    );
}

void gen_01_del1_write(number v) {
    this->gen_01_del1_writer = this->gen_01_del1_reader;
    this->gen_01_del1_buffer[(Index)this->gen_01_del1_writer] = v;
}

number gen_01_del1_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del1__maxdelay : size);
    number val = this->gen_01_del1_read(effectiveSize, 0);
    this->gen_01_del1_write(v);
    this->gen_01_del1_step();
    return val;
}

array<Index, 2> gen_01_del1_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del1_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del1_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del1_init() {
    auto result = this->gen_01_del1_calcSizeInSamples();
    this->gen_01_del1__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del1_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del1_wrap = requestedSizeInSamples - 1;
}

void gen_01_del1_clear() {
    this->gen_01_del1_buffer->setZero();
}

void gen_01_del1_reset() {
    auto result = this->gen_01_del1_calcSizeInSamples();
    this->gen_01_del1__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del1_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del1_buffer);
    this->gen_01_del1_wrap = this->gen_01_del1_buffer->getSize() - 1;
    this->gen_01_del1_clear();

    if (this->gen_01_del1_reader >= this->gen_01_del1__maxdelay || this->gen_01_del1_writer >= this->gen_01_del1__maxdelay) {
        this->gen_01_del1_reader = 0;
        this->gen_01_del1_writer = 0;
    }
}

void gen_01_del1_dspsetup() {
    this->gen_01_del1_reset();
}

number gen_01_del1_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del1_size() {
    return this->gen_01_del1__maxdelay;
}

void gen_01_del_6_step() {
    this->gen_01_del_6_reader++;

    if (this->gen_01_del_6_reader >= (int)(this->gen_01_del_6_buffer->getSize()))
        this->gen_01_del_6_reader = 0;
}

number gen_01_del_6_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_6_buffer->getSize()) + this->gen_01_del_6_reader - ((size > this->gen_01_del_6__maxdelay ? this->gen_01_del_6__maxdelay : (size < (this->gen_01_del_6_reader != this->gen_01_del_6_writer) ? this->gen_01_del_6_reader != this->gen_01_del_6_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_6_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_6_wrap))
        ), this->gen_01_del_6_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_6_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_6_buffer->getSize()) + this->gen_01_del_6_reader - ((size > this->gen_01_del_6__maxdelay ? this->gen_01_del_6__maxdelay : (size < (this->gen_01_del_6_reader != this->gen_01_del_6_writer) ? this->gen_01_del_6_reader != this->gen_01_del_6_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_6_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_6_wrap))
    );
}

void gen_01_del_6_write(number v) {
    this->gen_01_del_6_writer = this->gen_01_del_6_reader;
    this->gen_01_del_6_buffer[(Index)this->gen_01_del_6_writer] = v;
}

number gen_01_del_6_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_6__maxdelay : size);
    number val = this->gen_01_del_6_read(effectiveSize, 0);
    this->gen_01_del_6_write(v);
    this->gen_01_del_6_step();
    return val;
}

array<Index, 2> gen_01_del_6_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_6_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_6_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_6_init() {
    auto result = this->gen_01_del_6_calcSizeInSamples();
    this->gen_01_del_6__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_6_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_6_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_6_clear() {
    this->gen_01_del_6_buffer->setZero();
}

void gen_01_del_6_reset() {
    auto result = this->gen_01_del_6_calcSizeInSamples();
    this->gen_01_del_6__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_6_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_6_buffer);
    this->gen_01_del_6_wrap = this->gen_01_del_6_buffer->getSize() - 1;
    this->gen_01_del_6_clear();

    if (this->gen_01_del_6_reader >= this->gen_01_del_6__maxdelay || this->gen_01_del_6_writer >= this->gen_01_del_6__maxdelay) {
        this->gen_01_del_6_reader = 0;
        this->gen_01_del_6_writer = 0;
    }
}

void gen_01_del_6_dspsetup() {
    this->gen_01_del_6_reset();
}

number gen_01_del_6_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_6_size() {
    return this->gen_01_del_6__maxdelay;
}

void gen_01_del_5_step() {
    this->gen_01_del_5_reader++;

    if (this->gen_01_del_5_reader >= (int)(this->gen_01_del_5_buffer->getSize()))
        this->gen_01_del_5_reader = 0;
}

number gen_01_del_5_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_5_buffer->getSize()) + this->gen_01_del_5_reader - ((size > this->gen_01_del_5__maxdelay ? this->gen_01_del_5__maxdelay : (size < (this->gen_01_del_5_reader != this->gen_01_del_5_writer) ? this->gen_01_del_5_reader != this->gen_01_del_5_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_5_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_5_wrap))
        ), this->gen_01_del_5_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_5_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_5_buffer->getSize()) + this->gen_01_del_5_reader - ((size > this->gen_01_del_5__maxdelay ? this->gen_01_del_5__maxdelay : (size < (this->gen_01_del_5_reader != this->gen_01_del_5_writer) ? this->gen_01_del_5_reader != this->gen_01_del_5_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_5_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_5_wrap))
    );
}

void gen_01_del_5_write(number v) {
    this->gen_01_del_5_writer = this->gen_01_del_5_reader;
    this->gen_01_del_5_buffer[(Index)this->gen_01_del_5_writer] = v;
}

number gen_01_del_5_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_5__maxdelay : size);
    number val = this->gen_01_del_5_read(effectiveSize, 0);
    this->gen_01_del_5_write(v);
    this->gen_01_del_5_step();
    return val;
}

array<Index, 2> gen_01_del_5_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_5_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_5_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_5_init() {
    auto result = this->gen_01_del_5_calcSizeInSamples();
    this->gen_01_del_5__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_5_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_5_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_5_clear() {
    this->gen_01_del_5_buffer->setZero();
}

void gen_01_del_5_reset() {
    auto result = this->gen_01_del_5_calcSizeInSamples();
    this->gen_01_del_5__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_5_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_5_buffer);
    this->gen_01_del_5_wrap = this->gen_01_del_5_buffer->getSize() - 1;
    this->gen_01_del_5_clear();

    if (this->gen_01_del_5_reader >= this->gen_01_del_5__maxdelay || this->gen_01_del_5_writer >= this->gen_01_del_5__maxdelay) {
        this->gen_01_del_5_reader = 0;
        this->gen_01_del_5_writer = 0;
    }
}

void gen_01_del_5_dspsetup() {
    this->gen_01_del_5_reset();
}

number gen_01_del_5_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_5_size() {
    return this->gen_01_del_5__maxdelay;
}

void gen_01_del_4_step() {
    this->gen_01_del_4_reader++;

    if (this->gen_01_del_4_reader >= (int)(this->gen_01_del_4_buffer->getSize()))
        this->gen_01_del_4_reader = 0;
}

number gen_01_del_4_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_4_buffer->getSize()) + this->gen_01_del_4_reader - ((size > this->gen_01_del_4__maxdelay ? this->gen_01_del_4__maxdelay : (size < (this->gen_01_del_4_reader != this->gen_01_del_4_writer) ? this->gen_01_del_4_reader != this->gen_01_del_4_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_4_wrap))
        ), this->gen_01_del_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_4_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_4_buffer->getSize()) + this->gen_01_del_4_reader - ((size > this->gen_01_del_4__maxdelay ? this->gen_01_del_4__maxdelay : (size < (this->gen_01_del_4_reader != this->gen_01_del_4_writer) ? this->gen_01_del_4_reader != this->gen_01_del_4_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_4_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_4_wrap))
    );
}

void gen_01_del_4_write(number v) {
    this->gen_01_del_4_writer = this->gen_01_del_4_reader;
    this->gen_01_del_4_buffer[(Index)this->gen_01_del_4_writer] = v;
}

number gen_01_del_4_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_4__maxdelay : size);
    number val = this->gen_01_del_4_read(effectiveSize, 0);
    this->gen_01_del_4_write(v);
    this->gen_01_del_4_step();
    return val;
}

array<Index, 2> gen_01_del_4_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_4_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_4_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_4_init() {
    auto result = this->gen_01_del_4_calcSizeInSamples();
    this->gen_01_del_4__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_4_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_4_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_4_clear() {
    this->gen_01_del_4_buffer->setZero();
}

void gen_01_del_4_reset() {
    auto result = this->gen_01_del_4_calcSizeInSamples();
    this->gen_01_del_4__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_4_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_4_buffer);
    this->gen_01_del_4_wrap = this->gen_01_del_4_buffer->getSize() - 1;
    this->gen_01_del_4_clear();

    if (this->gen_01_del_4_reader >= this->gen_01_del_4__maxdelay || this->gen_01_del_4_writer >= this->gen_01_del_4__maxdelay) {
        this->gen_01_del_4_reader = 0;
        this->gen_01_del_4_writer = 0;
    }
}

void gen_01_del_4_dspsetup() {
    this->gen_01_del_4_reset();
}

number gen_01_del_4_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_4_size() {
    return this->gen_01_del_4__maxdelay;
}

void gen_01_del_3_step() {
    this->gen_01_del_3_reader++;

    if (this->gen_01_del_3_reader >= (int)(this->gen_01_del_3_buffer->getSize()))
        this->gen_01_del_3_reader = 0;
}

number gen_01_del_3_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_3_buffer->getSize()) + this->gen_01_del_3_reader - ((size > this->gen_01_del_3__maxdelay ? this->gen_01_del_3__maxdelay : (size < (this->gen_01_del_3_reader != this->gen_01_del_3_writer) ? this->gen_01_del_3_reader != this->gen_01_del_3_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_3_wrap))
        ), this->gen_01_del_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_3_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_3_buffer->getSize()) + this->gen_01_del_3_reader - ((size > this->gen_01_del_3__maxdelay ? this->gen_01_del_3__maxdelay : (size < (this->gen_01_del_3_reader != this->gen_01_del_3_writer) ? this->gen_01_del_3_reader != this->gen_01_del_3_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_3_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_3_wrap))
    );
}

void gen_01_del_3_write(number v) {
    this->gen_01_del_3_writer = this->gen_01_del_3_reader;
    this->gen_01_del_3_buffer[(Index)this->gen_01_del_3_writer] = v;
}

number gen_01_del_3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_3__maxdelay : size);
    number val = this->gen_01_del_3_read(effectiveSize, 0);
    this->gen_01_del_3_write(v);
    this->gen_01_del_3_step();
    return val;
}

array<Index, 2> gen_01_del_3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_3_init() {
    auto result = this->gen_01_del_3_calcSizeInSamples();
    this->gen_01_del_3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_3_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_3_clear() {
    this->gen_01_del_3_buffer->setZero();
}

void gen_01_del_3_reset() {
    auto result = this->gen_01_del_3_calcSizeInSamples();
    this->gen_01_del_3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_3_buffer);
    this->gen_01_del_3_wrap = this->gen_01_del_3_buffer->getSize() - 1;
    this->gen_01_del_3_clear();

    if (this->gen_01_del_3_reader >= this->gen_01_del_3__maxdelay || this->gen_01_del_3_writer >= this->gen_01_del_3__maxdelay) {
        this->gen_01_del_3_reader = 0;
        this->gen_01_del_3_writer = 0;
    }
}

void gen_01_del_3_dspsetup() {
    this->gen_01_del_3_reset();
}

number gen_01_del_3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_3_size() {
    return this->gen_01_del_3__maxdelay;
}

void gen_01_del_2_step() {
    this->gen_01_del_2_reader++;

    if (this->gen_01_del_2_reader >= (int)(this->gen_01_del_2_buffer->getSize()))
        this->gen_01_del_2_reader = 0;
}

number gen_01_del_2_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_2_buffer->getSize()) + this->gen_01_del_2_reader - ((size > this->gen_01_del_2__maxdelay ? this->gen_01_del_2__maxdelay : (size < (this->gen_01_del_2_reader != this->gen_01_del_2_writer) ? this->gen_01_del_2_reader != this->gen_01_del_2_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_2_wrap))
        ), this->gen_01_del_2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_2_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_2_buffer->getSize()) + this->gen_01_del_2_reader - ((size > this->gen_01_del_2__maxdelay ? this->gen_01_del_2__maxdelay : (size < (this->gen_01_del_2_reader != this->gen_01_del_2_writer) ? this->gen_01_del_2_reader != this->gen_01_del_2_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_2_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_2_wrap))
    );
}

void gen_01_del_2_write(number v) {
    this->gen_01_del_2_writer = this->gen_01_del_2_reader;
    this->gen_01_del_2_buffer[(Index)this->gen_01_del_2_writer] = v;
}

number gen_01_del_2_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_2__maxdelay : size);
    number val = this->gen_01_del_2_read(effectiveSize, 0);
    this->gen_01_del_2_write(v);
    this->gen_01_del_2_step();
    return val;
}

array<Index, 2> gen_01_del_2_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_2_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_2_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_2_init() {
    auto result = this->gen_01_del_2_calcSizeInSamples();
    this->gen_01_del_2__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_2_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_2_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_2_clear() {
    this->gen_01_del_2_buffer->setZero();
}

void gen_01_del_2_reset() {
    auto result = this->gen_01_del_2_calcSizeInSamples();
    this->gen_01_del_2__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_2_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_2_buffer);
    this->gen_01_del_2_wrap = this->gen_01_del_2_buffer->getSize() - 1;
    this->gen_01_del_2_clear();

    if (this->gen_01_del_2_reader >= this->gen_01_del_2__maxdelay || this->gen_01_del_2_writer >= this->gen_01_del_2__maxdelay) {
        this->gen_01_del_2_reader = 0;
        this->gen_01_del_2_writer = 0;
    }
}

void gen_01_del_2_dspsetup() {
    this->gen_01_del_2_reset();
}

number gen_01_del_2_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_2_size() {
    return this->gen_01_del_2__maxdelay;
}

void gen_01_del_1_step() {
    this->gen_01_del_1_reader++;

    if (this->gen_01_del_1_reader >= (int)(this->gen_01_del_1_buffer->getSize()))
        this->gen_01_del_1_reader = 0;
}

number gen_01_del_1_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del_1_buffer->getSize()) + this->gen_01_del_1_reader - ((size > this->gen_01_del_1__maxdelay ? this->gen_01_del_1__maxdelay : (size < (this->gen_01_del_1_reader != this->gen_01_del_1_writer) ? this->gen_01_del_1_reader != this->gen_01_del_1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del_1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_1_wrap))
        ), this->gen_01_del_1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del_1_wrap))
        ));
    }

    number r = (int)(this->gen_01_del_1_buffer->getSize()) + this->gen_01_del_1_reader - ((size > this->gen_01_del_1__maxdelay ? this->gen_01_del_1__maxdelay : (size < (this->gen_01_del_1_reader != this->gen_01_del_1_writer) ? this->gen_01_del_1_reader != this->gen_01_del_1_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del_1_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del_1_wrap))
    );
}

void gen_01_del_1_write(number v) {
    this->gen_01_del_1_writer = this->gen_01_del_1_reader;
    this->gen_01_del_1_buffer[(Index)this->gen_01_del_1_writer] = v;
}

number gen_01_del_1_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del_1__maxdelay : size);
    number val = this->gen_01_del_1_read(effectiveSize, 0);
    this->gen_01_del_1_write(v);
    this->gen_01_del_1_step();
    return val;
}

array<Index, 2> gen_01_del_1_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del_1_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del_1_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del_1_init() {
    auto result = this->gen_01_del_1_calcSizeInSamples();
    this->gen_01_del_1__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_1_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del_1_wrap = requestedSizeInSamples - 1;
}

void gen_01_del_1_clear() {
    this->gen_01_del_1_buffer->setZero();
}

void gen_01_del_1_reset() {
    auto result = this->gen_01_del_1_calcSizeInSamples();
    this->gen_01_del_1__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del_1_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del_1_buffer);
    this->gen_01_del_1_wrap = this->gen_01_del_1_buffer->getSize() - 1;
    this->gen_01_del_1_clear();

    if (this->gen_01_del_1_reader >= this->gen_01_del_1__maxdelay || this->gen_01_del_1_writer >= this->gen_01_del_1__maxdelay) {
        this->gen_01_del_1_reader = 0;
        this->gen_01_del_1_writer = 0;
    }
}

void gen_01_del_1_dspsetup() {
    this->gen_01_del_1_reset();
}

number gen_01_del_1_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del_1_size() {
    return this->gen_01_del_1__maxdelay;
}

void gen_01_del3_step() {
    this->gen_01_del3_reader++;

    if (this->gen_01_del3_reader >= (int)(this->gen_01_del3_buffer->getSize()))
        this->gen_01_del3_reader = 0;
}

number gen_01_del3_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del3_buffer->getSize()) + this->gen_01_del3_reader - ((size > this->gen_01_del3__maxdelay ? this->gen_01_del3__maxdelay : (size < (this->gen_01_del3_reader != this->gen_01_del3_writer) ? this->gen_01_del3_reader != this->gen_01_del3_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del3_wrap))
        ), this->gen_01_del3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del3_wrap))
        ));
    }

    number r = (int)(this->gen_01_del3_buffer->getSize()) + this->gen_01_del3_reader - ((size > this->gen_01_del3__maxdelay ? this->gen_01_del3__maxdelay : (size < (this->gen_01_del3_reader != this->gen_01_del3_writer) ? this->gen_01_del3_reader != this->gen_01_del3_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del3_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del3_wrap))
    );
}

void gen_01_del3_write(number v) {
    this->gen_01_del3_writer = this->gen_01_del3_reader;
    this->gen_01_del3_buffer[(Index)this->gen_01_del3_writer] = v;
}

number gen_01_del3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del3__maxdelay : size);
    number val = this->gen_01_del3_read(effectiveSize, 0);
    this->gen_01_del3_write(v);
    this->gen_01_del3_step();
    return val;
}

array<Index, 2> gen_01_del3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del3_init() {
    auto result = this->gen_01_del3_calcSizeInSamples();
    this->gen_01_del3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del3_wrap = requestedSizeInSamples - 1;
}

void gen_01_del3_clear() {
    this->gen_01_del3_buffer->setZero();
}

void gen_01_del3_reset() {
    auto result = this->gen_01_del3_calcSizeInSamples();
    this->gen_01_del3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del3_buffer);
    this->gen_01_del3_wrap = this->gen_01_del3_buffer->getSize() - 1;
    this->gen_01_del3_clear();

    if (this->gen_01_del3_reader >= this->gen_01_del3__maxdelay || this->gen_01_del3_writer >= this->gen_01_del3__maxdelay) {
        this->gen_01_del3_reader = 0;
        this->gen_01_del3_writer = 0;
    }
}

void gen_01_del3_dspsetup() {
    this->gen_01_del3_reset();
}

number gen_01_del3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del3_size() {
    return this->gen_01_del3__maxdelay;
}

void gen_01_del2_step() {
    this->gen_01_del2_reader++;

    if (this->gen_01_del2_reader >= (int)(this->gen_01_del2_buffer->getSize()))
        this->gen_01_del2_reader = 0;
}

number gen_01_del2_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del2_buffer->getSize()) + this->gen_01_del2_reader - ((size > this->gen_01_del2__maxdelay ? this->gen_01_del2__maxdelay : (size < (this->gen_01_del2_reader != this->gen_01_del2_writer) ? this->gen_01_del2_reader != this->gen_01_del2_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del2_wrap))
        ), this->gen_01_del2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del2_wrap))
        ));
    }

    number r = (int)(this->gen_01_del2_buffer->getSize()) + this->gen_01_del2_reader - ((size > this->gen_01_del2__maxdelay ? this->gen_01_del2__maxdelay : (size < (this->gen_01_del2_reader != this->gen_01_del2_writer) ? this->gen_01_del2_reader != this->gen_01_del2_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del2_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del2_wrap))
    );
}

void gen_01_del2_write(number v) {
    this->gen_01_del2_writer = this->gen_01_del2_reader;
    this->gen_01_del2_buffer[(Index)this->gen_01_del2_writer] = v;
}

number gen_01_del2_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del2__maxdelay : size);
    number val = this->gen_01_del2_read(effectiveSize, 0);
    this->gen_01_del2_write(v);
    this->gen_01_del2_step();
    return val;
}

array<Index, 2> gen_01_del2_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del2_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del2_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del2_init() {
    auto result = this->gen_01_del2_calcSizeInSamples();
    this->gen_01_del2__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del2_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del2_wrap = requestedSizeInSamples - 1;
}

void gen_01_del2_clear() {
    this->gen_01_del2_buffer->setZero();
}

void gen_01_del2_reset() {
    auto result = this->gen_01_del2_calcSizeInSamples();
    this->gen_01_del2__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del2_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del2_buffer);
    this->gen_01_del2_wrap = this->gen_01_del2_buffer->getSize() - 1;
    this->gen_01_del2_clear();

    if (this->gen_01_del2_reader >= this->gen_01_del2__maxdelay || this->gen_01_del2_writer >= this->gen_01_del2__maxdelay) {
        this->gen_01_del2_reader = 0;
        this->gen_01_del2_writer = 0;
    }
}

void gen_01_del2_dspsetup() {
    this->gen_01_del2_reset();
}

number gen_01_del2_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del2_size() {
    return this->gen_01_del2__maxdelay;
}

void gen_01_dspsetup(bool force) {
    if ((bool)(this->gen_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_01_setupDone = true;
    this->gen_01_del_9_dspsetup();
    this->gen_01_del_8_dspsetup();
    this->gen_01_del_7_dspsetup();
    this->gen_01_del1_dspsetup();
    this->gen_01_del_6_dspsetup();
    this->gen_01_del_5_dspsetup();
    this->gen_01_del_4_dspsetup();
    this->gen_01_del_3_dspsetup();
    this->gen_01_del_2_dspsetup();
    this->gen_01_del_1_dspsetup();
    this->gen_01_del3_dspsetup();
    this->gen_01_del2_dspsetup();
}

void gen_02_prev_exprdata_init() {
    number sizeInSamples = this->gen_02_prev_exprdata_evaluateSizeExpr(this->samplerate(), this->vectorsize());
    this->gen_02_prev_exprdata_buffer->requestSize(sizeInSamples, 2);
}

void gen_02_prev_exprdata_dspsetup() {
    number sizeInSamples = this->gen_02_prev_exprdata_evaluateSizeExpr(this->samplerate(), this->vectorsize());
    this->gen_02_prev_exprdata_buffer = this->gen_02_prev_exprdata_buffer->setSize(sizeInSamples);
    updateDataRef(this, this->gen_02_prev_exprdata_buffer);
}

number gen_02_prev_exprdata_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(samplerate);
    return vectorsize;
}

void gen_02_prev_exprdata_reset() {}

void gen_02_modPanDel_step() {
    this->gen_02_modPanDel_reader++;

    if (this->gen_02_modPanDel_reader >= (int)(this->gen_02_modPanDel_buffer->getSize()))
        this->gen_02_modPanDel_reader = 0;
}

number gen_02_modPanDel_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_02_modPanDel_buffer->getSize()) + this->gen_02_modPanDel_reader - ((size > this->gen_02_modPanDel__maxdelay ? this->gen_02_modPanDel__maxdelay : (size < (this->gen_02_modPanDel_reader != this->gen_02_modPanDel_writer) ? this->gen_02_modPanDel_reader != this->gen_02_modPanDel_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_02_modPanDel_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_modPanDel_wrap))
        ), this->gen_02_modPanDel_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_02_modPanDel_wrap))
        ));
    }

    number r = (int)(this->gen_02_modPanDel_buffer->getSize()) + this->gen_02_modPanDel_reader - ((size > this->gen_02_modPanDel__maxdelay ? this->gen_02_modPanDel__maxdelay : (size < (this->gen_02_modPanDel_reader != this->gen_02_modPanDel_writer) ? this->gen_02_modPanDel_reader != this->gen_02_modPanDel_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_modPanDel_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_modPanDel_wrap))
    );
}

void gen_02_modPanDel_write(number v) {
    this->gen_02_modPanDel_writer = this->gen_02_modPanDel_reader;
    this->gen_02_modPanDel_buffer[(Index)this->gen_02_modPanDel_writer] = v;
}

number gen_02_modPanDel_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_modPanDel__maxdelay : size);
    number val = this->gen_02_modPanDel_read(effectiveSize, 0);
    this->gen_02_modPanDel_write(v);
    this->gen_02_modPanDel_step();
    return val;
}

array<Index, 2> gen_02_modPanDel_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_modPanDel_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_modPanDel_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_modPanDel_init() {
    auto result = this->gen_02_modPanDel_calcSizeInSamples();
    this->gen_02_modPanDel__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_modPanDel_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_modPanDel_wrap = requestedSizeInSamples - 1;
}

void gen_02_modPanDel_clear() {
    this->gen_02_modPanDel_buffer->setZero();
}

void gen_02_modPanDel_reset() {
    auto result = this->gen_02_modPanDel_calcSizeInSamples();
    this->gen_02_modPanDel__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_modPanDel_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_modPanDel_buffer);
    this->gen_02_modPanDel_wrap = this->gen_02_modPanDel_buffer->getSize() - 1;
    this->gen_02_modPanDel_clear();

    if (this->gen_02_modPanDel_reader >= this->gen_02_modPanDel__maxdelay || this->gen_02_modPanDel_writer >= this->gen_02_modPanDel__maxdelay) {
        this->gen_02_modPanDel_reader = 0;
        this->gen_02_modPanDel_writer = 0;
    }
}

void gen_02_modPanDel_dspsetup() {
    this->gen_02_modPanDel_reset();
}

number gen_02_modPanDel_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_02_modPanDel_size() {
    return this->gen_02_modPanDel__maxdelay;
}

void gen_02_modFreqDel_step() {
    this->gen_02_modFreqDel_reader++;

    if (this->gen_02_modFreqDel_reader >= (int)(this->gen_02_modFreqDel_buffer->getSize()))
        this->gen_02_modFreqDel_reader = 0;
}

number gen_02_modFreqDel_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_02_modFreqDel_buffer->getSize()) + this->gen_02_modFreqDel_reader - ((size > this->gen_02_modFreqDel__maxdelay ? this->gen_02_modFreqDel__maxdelay : (size < (this->gen_02_modFreqDel_reader != this->gen_02_modFreqDel_writer) ? this->gen_02_modFreqDel_reader != this->gen_02_modFreqDel_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_02_modFreqDel_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_modFreqDel_wrap))
        ), this->gen_02_modFreqDel_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_02_modFreqDel_wrap))
        ));
    }

    number r = (int)(this->gen_02_modFreqDel_buffer->getSize()) + this->gen_02_modFreqDel_reader - ((size > this->gen_02_modFreqDel__maxdelay ? this->gen_02_modFreqDel__maxdelay : (size < (this->gen_02_modFreqDel_reader != this->gen_02_modFreqDel_writer) ? this->gen_02_modFreqDel_reader != this->gen_02_modFreqDel_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_modFreqDel_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_modFreqDel_wrap))
    );
}

void gen_02_modFreqDel_write(number v) {
    this->gen_02_modFreqDel_writer = this->gen_02_modFreqDel_reader;
    this->gen_02_modFreqDel_buffer[(Index)this->gen_02_modFreqDel_writer] = v;
}

number gen_02_modFreqDel_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_modFreqDel__maxdelay : size);
    number val = this->gen_02_modFreqDel_read(effectiveSize, 0);
    this->gen_02_modFreqDel_write(v);
    this->gen_02_modFreqDel_step();
    return val;
}

array<Index, 2> gen_02_modFreqDel_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_modFreqDel_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_modFreqDel_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_modFreqDel_init() {
    auto result = this->gen_02_modFreqDel_calcSizeInSamples();
    this->gen_02_modFreqDel__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_modFreqDel_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_modFreqDel_wrap = requestedSizeInSamples - 1;
}

void gen_02_modFreqDel_clear() {
    this->gen_02_modFreqDel_buffer->setZero();
}

void gen_02_modFreqDel_reset() {
    auto result = this->gen_02_modFreqDel_calcSizeInSamples();
    this->gen_02_modFreqDel__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_modFreqDel_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_modFreqDel_buffer);
    this->gen_02_modFreqDel_wrap = this->gen_02_modFreqDel_buffer->getSize() - 1;
    this->gen_02_modFreqDel_clear();

    if (this->gen_02_modFreqDel_reader >= this->gen_02_modFreqDel__maxdelay || this->gen_02_modFreqDel_writer >= this->gen_02_modFreqDel__maxdelay) {
        this->gen_02_modFreqDel_reader = 0;
        this->gen_02_modFreqDel_writer = 0;
    }
}

void gen_02_modFreqDel_dspsetup() {
    this->gen_02_modFreqDel_reset();
}

number gen_02_modFreqDel_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_02_modFreqDel_size() {
    return this->gen_02_modFreqDel__maxdelay;
}

void gen_02_delay15_step() {
    this->gen_02_delay15_reader++;

    if (this->gen_02_delay15_reader >= (int)(this->gen_02_delay15_buffer->getSize()))
        this->gen_02_delay15_reader = 0;
}

number gen_02_delay15_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay15_buffer->getSize()) + this->gen_02_delay15_reader - ((1024 > this->gen_02_delay15__maxdelay ? this->gen_02_delay15__maxdelay : (1024 < (this->gen_02_delay15_reader != this->gen_02_delay15_writer) ? this->gen_02_delay15_reader != this->gen_02_delay15_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay15_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay15_wrap))
    );
}

void gen_02_delay15_write(number v) {
    this->gen_02_delay15_writer = this->gen_02_delay15_reader;
    this->gen_02_delay15_buffer[(Index)this->gen_02_delay15_writer] = v;
}

number gen_02_delay15_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay15__maxdelay : size);
    number val = this->gen_02_delay15_read(effectiveSize, 0);
    this->gen_02_delay15_write(v);
    this->gen_02_delay15_step();
    return val;
}

array<Index, 2> gen_02_delay15_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay15_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay15_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay15_init() {
    auto result = this->gen_02_delay15_calcSizeInSamples();
    this->gen_02_delay15__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay15_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay15_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay15_clear() {
    this->gen_02_delay15_buffer->setZero();
}

void gen_02_delay15_reset() {
    auto result = this->gen_02_delay15_calcSizeInSamples();
    this->gen_02_delay15__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay15_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay15_buffer);
    this->gen_02_delay15_wrap = this->gen_02_delay15_buffer->getSize() - 1;
    this->gen_02_delay15_clear();

    if (this->gen_02_delay15_reader >= this->gen_02_delay15__maxdelay || this->gen_02_delay15_writer >= this->gen_02_delay15__maxdelay) {
        this->gen_02_delay15_reader = 0;
        this->gen_02_delay15_writer = 0;
    }
}

void gen_02_delay15_dspsetup() {
    this->gen_02_delay15_reset();
}

number gen_02_delay15_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay15_size() {
    return this->gen_02_delay15__maxdelay;
}

void gen_02_delay14_step() {
    this->gen_02_delay14_reader++;

    if (this->gen_02_delay14_reader >= (int)(this->gen_02_delay14_buffer->getSize()))
        this->gen_02_delay14_reader = 0;
}

number gen_02_delay14_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay14_buffer->getSize()) + this->gen_02_delay14_reader - ((1024 > this->gen_02_delay14__maxdelay ? this->gen_02_delay14__maxdelay : (1024 < (this->gen_02_delay14_reader != this->gen_02_delay14_writer) ? this->gen_02_delay14_reader != this->gen_02_delay14_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay14_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay14_wrap))
    );
}

void gen_02_delay14_write(number v) {
    this->gen_02_delay14_writer = this->gen_02_delay14_reader;
    this->gen_02_delay14_buffer[(Index)this->gen_02_delay14_writer] = v;
}

number gen_02_delay14_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay14__maxdelay : size);
    number val = this->gen_02_delay14_read(effectiveSize, 0);
    this->gen_02_delay14_write(v);
    this->gen_02_delay14_step();
    return val;
}

array<Index, 2> gen_02_delay14_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay14_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay14_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay14_init() {
    auto result = this->gen_02_delay14_calcSizeInSamples();
    this->gen_02_delay14__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay14_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay14_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay14_clear() {
    this->gen_02_delay14_buffer->setZero();
}

void gen_02_delay14_reset() {
    auto result = this->gen_02_delay14_calcSizeInSamples();
    this->gen_02_delay14__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay14_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay14_buffer);
    this->gen_02_delay14_wrap = this->gen_02_delay14_buffer->getSize() - 1;
    this->gen_02_delay14_clear();

    if (this->gen_02_delay14_reader >= this->gen_02_delay14__maxdelay || this->gen_02_delay14_writer >= this->gen_02_delay14__maxdelay) {
        this->gen_02_delay14_reader = 0;
        this->gen_02_delay14_writer = 0;
    }
}

void gen_02_delay14_dspsetup() {
    this->gen_02_delay14_reset();
}

number gen_02_delay14_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay14_size() {
    return this->gen_02_delay14__maxdelay;
}

void gen_02_delay13_step() {
    this->gen_02_delay13_reader++;

    if (this->gen_02_delay13_reader >= (int)(this->gen_02_delay13_buffer->getSize()))
        this->gen_02_delay13_reader = 0;
}

number gen_02_delay13_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay13_buffer->getSize()) + this->gen_02_delay13_reader - ((1024 > this->gen_02_delay13__maxdelay ? this->gen_02_delay13__maxdelay : (1024 < (this->gen_02_delay13_reader != this->gen_02_delay13_writer) ? this->gen_02_delay13_reader != this->gen_02_delay13_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay13_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay13_wrap))
    );
}

void gen_02_delay13_write(number v) {
    this->gen_02_delay13_writer = this->gen_02_delay13_reader;
    this->gen_02_delay13_buffer[(Index)this->gen_02_delay13_writer] = v;
}

number gen_02_delay13_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay13__maxdelay : size);
    number val = this->gen_02_delay13_read(effectiveSize, 0);
    this->gen_02_delay13_write(v);
    this->gen_02_delay13_step();
    return val;
}

array<Index, 2> gen_02_delay13_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay13_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay13_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay13_init() {
    auto result = this->gen_02_delay13_calcSizeInSamples();
    this->gen_02_delay13__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay13_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay13_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay13_clear() {
    this->gen_02_delay13_buffer->setZero();
}

void gen_02_delay13_reset() {
    auto result = this->gen_02_delay13_calcSizeInSamples();
    this->gen_02_delay13__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay13_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay13_buffer);
    this->gen_02_delay13_wrap = this->gen_02_delay13_buffer->getSize() - 1;
    this->gen_02_delay13_clear();

    if (this->gen_02_delay13_reader >= this->gen_02_delay13__maxdelay || this->gen_02_delay13_writer >= this->gen_02_delay13__maxdelay) {
        this->gen_02_delay13_reader = 0;
        this->gen_02_delay13_writer = 0;
    }
}

void gen_02_delay13_dspsetup() {
    this->gen_02_delay13_reset();
}

number gen_02_delay13_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay13_size() {
    return this->gen_02_delay13__maxdelay;
}

void gen_02_delay12_step() {
    this->gen_02_delay12_reader++;

    if (this->gen_02_delay12_reader >= (int)(this->gen_02_delay12_buffer->getSize()))
        this->gen_02_delay12_reader = 0;
}

number gen_02_delay12_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay12_buffer->getSize()) + this->gen_02_delay12_reader - ((1024 > this->gen_02_delay12__maxdelay ? this->gen_02_delay12__maxdelay : (1024 < (this->gen_02_delay12_reader != this->gen_02_delay12_writer) ? this->gen_02_delay12_reader != this->gen_02_delay12_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay12_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay12_wrap))
    );
}

void gen_02_delay12_write(number v) {
    this->gen_02_delay12_writer = this->gen_02_delay12_reader;
    this->gen_02_delay12_buffer[(Index)this->gen_02_delay12_writer] = v;
}

number gen_02_delay12_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay12__maxdelay : size);
    number val = this->gen_02_delay12_read(effectiveSize, 0);
    this->gen_02_delay12_write(v);
    this->gen_02_delay12_step();
    return val;
}

array<Index, 2> gen_02_delay12_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay12_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay12_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay12_init() {
    auto result = this->gen_02_delay12_calcSizeInSamples();
    this->gen_02_delay12__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay12_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay12_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay12_clear() {
    this->gen_02_delay12_buffer->setZero();
}

void gen_02_delay12_reset() {
    auto result = this->gen_02_delay12_calcSizeInSamples();
    this->gen_02_delay12__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay12_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay12_buffer);
    this->gen_02_delay12_wrap = this->gen_02_delay12_buffer->getSize() - 1;
    this->gen_02_delay12_clear();

    if (this->gen_02_delay12_reader >= this->gen_02_delay12__maxdelay || this->gen_02_delay12_writer >= this->gen_02_delay12__maxdelay) {
        this->gen_02_delay12_reader = 0;
        this->gen_02_delay12_writer = 0;
    }
}

void gen_02_delay12_dspsetup() {
    this->gen_02_delay12_reset();
}

number gen_02_delay12_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay12_size() {
    return this->gen_02_delay12__maxdelay;
}

void gen_02_delay11_step() {
    this->gen_02_delay11_reader++;

    if (this->gen_02_delay11_reader >= (int)(this->gen_02_delay11_buffer->getSize()))
        this->gen_02_delay11_reader = 0;
}

number gen_02_delay11_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay11_buffer->getSize()) + this->gen_02_delay11_reader - ((1024 > this->gen_02_delay11__maxdelay ? this->gen_02_delay11__maxdelay : (1024 < (this->gen_02_delay11_reader != this->gen_02_delay11_writer) ? this->gen_02_delay11_reader != this->gen_02_delay11_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay11_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay11_wrap))
    );
}

void gen_02_delay11_write(number v) {
    this->gen_02_delay11_writer = this->gen_02_delay11_reader;
    this->gen_02_delay11_buffer[(Index)this->gen_02_delay11_writer] = v;
}

number gen_02_delay11_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay11__maxdelay : size);
    number val = this->gen_02_delay11_read(effectiveSize, 0);
    this->gen_02_delay11_write(v);
    this->gen_02_delay11_step();
    return val;
}

array<Index, 2> gen_02_delay11_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay11_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay11_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay11_init() {
    auto result = this->gen_02_delay11_calcSizeInSamples();
    this->gen_02_delay11__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay11_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay11_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay11_clear() {
    this->gen_02_delay11_buffer->setZero();
}

void gen_02_delay11_reset() {
    auto result = this->gen_02_delay11_calcSizeInSamples();
    this->gen_02_delay11__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay11_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay11_buffer);
    this->gen_02_delay11_wrap = this->gen_02_delay11_buffer->getSize() - 1;
    this->gen_02_delay11_clear();

    if (this->gen_02_delay11_reader >= this->gen_02_delay11__maxdelay || this->gen_02_delay11_writer >= this->gen_02_delay11__maxdelay) {
        this->gen_02_delay11_reader = 0;
        this->gen_02_delay11_writer = 0;
    }
}

void gen_02_delay11_dspsetup() {
    this->gen_02_delay11_reset();
}

number gen_02_delay11_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay11_size() {
    return this->gen_02_delay11__maxdelay;
}

void gen_02_delay10_step() {
    this->gen_02_delay10_reader++;

    if (this->gen_02_delay10_reader >= (int)(this->gen_02_delay10_buffer->getSize()))
        this->gen_02_delay10_reader = 0;
}

number gen_02_delay10_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay10_buffer->getSize()) + this->gen_02_delay10_reader - ((1024 > this->gen_02_delay10__maxdelay ? this->gen_02_delay10__maxdelay : (1024 < (this->gen_02_delay10_reader != this->gen_02_delay10_writer) ? this->gen_02_delay10_reader != this->gen_02_delay10_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay10_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay10_wrap))
    );
}

void gen_02_delay10_write(number v) {
    this->gen_02_delay10_writer = this->gen_02_delay10_reader;
    this->gen_02_delay10_buffer[(Index)this->gen_02_delay10_writer] = v;
}

number gen_02_delay10_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay10__maxdelay : size);
    number val = this->gen_02_delay10_read(effectiveSize, 0);
    this->gen_02_delay10_write(v);
    this->gen_02_delay10_step();
    return val;
}

array<Index, 2> gen_02_delay10_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay10_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay10_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay10_init() {
    auto result = this->gen_02_delay10_calcSizeInSamples();
    this->gen_02_delay10__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay10_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay10_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay10_clear() {
    this->gen_02_delay10_buffer->setZero();
}

void gen_02_delay10_reset() {
    auto result = this->gen_02_delay10_calcSizeInSamples();
    this->gen_02_delay10__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay10_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay10_buffer);
    this->gen_02_delay10_wrap = this->gen_02_delay10_buffer->getSize() - 1;
    this->gen_02_delay10_clear();

    if (this->gen_02_delay10_reader >= this->gen_02_delay10__maxdelay || this->gen_02_delay10_writer >= this->gen_02_delay10__maxdelay) {
        this->gen_02_delay10_reader = 0;
        this->gen_02_delay10_writer = 0;
    }
}

void gen_02_delay10_dspsetup() {
    this->gen_02_delay10_reset();
}

number gen_02_delay10_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay10_size() {
    return this->gen_02_delay10__maxdelay;
}

void gen_02_delay9_step() {
    this->gen_02_delay9_reader++;

    if (this->gen_02_delay9_reader >= (int)(this->gen_02_delay9_buffer->getSize()))
        this->gen_02_delay9_reader = 0;
}

number gen_02_delay9_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay9_buffer->getSize()) + this->gen_02_delay9_reader - ((1024 > this->gen_02_delay9__maxdelay ? this->gen_02_delay9__maxdelay : (1024 < (this->gen_02_delay9_reader != this->gen_02_delay9_writer) ? this->gen_02_delay9_reader != this->gen_02_delay9_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay9_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay9_wrap))
    );
}

void gen_02_delay9_write(number v) {
    this->gen_02_delay9_writer = this->gen_02_delay9_reader;
    this->gen_02_delay9_buffer[(Index)this->gen_02_delay9_writer] = v;
}

number gen_02_delay9_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay9__maxdelay : size);
    number val = this->gen_02_delay9_read(effectiveSize, 0);
    this->gen_02_delay9_write(v);
    this->gen_02_delay9_step();
    return val;
}

array<Index, 2> gen_02_delay9_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay9_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay9_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay9_init() {
    auto result = this->gen_02_delay9_calcSizeInSamples();
    this->gen_02_delay9__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay9_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay9_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay9_clear() {
    this->gen_02_delay9_buffer->setZero();
}

void gen_02_delay9_reset() {
    auto result = this->gen_02_delay9_calcSizeInSamples();
    this->gen_02_delay9__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay9_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay9_buffer);
    this->gen_02_delay9_wrap = this->gen_02_delay9_buffer->getSize() - 1;
    this->gen_02_delay9_clear();

    if (this->gen_02_delay9_reader >= this->gen_02_delay9__maxdelay || this->gen_02_delay9_writer >= this->gen_02_delay9__maxdelay) {
        this->gen_02_delay9_reader = 0;
        this->gen_02_delay9_writer = 0;
    }
}

void gen_02_delay9_dspsetup() {
    this->gen_02_delay9_reset();
}

number gen_02_delay9_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay9_size() {
    return this->gen_02_delay9__maxdelay;
}

void gen_02_delay8_step() {
    this->gen_02_delay8_reader++;

    if (this->gen_02_delay8_reader >= (int)(this->gen_02_delay8_buffer->getSize()))
        this->gen_02_delay8_reader = 0;
}

number gen_02_delay8_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay8_buffer->getSize()) + this->gen_02_delay8_reader - ((1024 > this->gen_02_delay8__maxdelay ? this->gen_02_delay8__maxdelay : (1024 < (this->gen_02_delay8_reader != this->gen_02_delay8_writer) ? this->gen_02_delay8_reader != this->gen_02_delay8_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay8_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay8_wrap))
    );
}

void gen_02_delay8_write(number v) {
    this->gen_02_delay8_writer = this->gen_02_delay8_reader;
    this->gen_02_delay8_buffer[(Index)this->gen_02_delay8_writer] = v;
}

number gen_02_delay8_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay8__maxdelay : size);
    number val = this->gen_02_delay8_read(effectiveSize, 0);
    this->gen_02_delay8_write(v);
    this->gen_02_delay8_step();
    return val;
}

array<Index, 2> gen_02_delay8_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay8_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay8_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay8_init() {
    auto result = this->gen_02_delay8_calcSizeInSamples();
    this->gen_02_delay8__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay8_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay8_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay8_clear() {
    this->gen_02_delay8_buffer->setZero();
}

void gen_02_delay8_reset() {
    auto result = this->gen_02_delay8_calcSizeInSamples();
    this->gen_02_delay8__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay8_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay8_buffer);
    this->gen_02_delay8_wrap = this->gen_02_delay8_buffer->getSize() - 1;
    this->gen_02_delay8_clear();

    if (this->gen_02_delay8_reader >= this->gen_02_delay8__maxdelay || this->gen_02_delay8_writer >= this->gen_02_delay8__maxdelay) {
        this->gen_02_delay8_reader = 0;
        this->gen_02_delay8_writer = 0;
    }
}

void gen_02_delay8_dspsetup() {
    this->gen_02_delay8_reset();
}

number gen_02_delay8_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay8_size() {
    return this->gen_02_delay8__maxdelay;
}

void gen_02_delay7_step() {
    this->gen_02_delay7_reader++;

    if (this->gen_02_delay7_reader >= (int)(this->gen_02_delay7_buffer->getSize()))
        this->gen_02_delay7_reader = 0;
}

number gen_02_delay7_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay7_buffer->getSize()) + this->gen_02_delay7_reader - ((1024 > this->gen_02_delay7__maxdelay ? this->gen_02_delay7__maxdelay : (1024 < (this->gen_02_delay7_reader != this->gen_02_delay7_writer) ? this->gen_02_delay7_reader != this->gen_02_delay7_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay7_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay7_wrap))
    );
}

void gen_02_delay7_write(number v) {
    this->gen_02_delay7_writer = this->gen_02_delay7_reader;
    this->gen_02_delay7_buffer[(Index)this->gen_02_delay7_writer] = v;
}

number gen_02_delay7_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay7__maxdelay : size);
    number val = this->gen_02_delay7_read(effectiveSize, 0);
    this->gen_02_delay7_write(v);
    this->gen_02_delay7_step();
    return val;
}

array<Index, 2> gen_02_delay7_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay7_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay7_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay7_init() {
    auto result = this->gen_02_delay7_calcSizeInSamples();
    this->gen_02_delay7__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay7_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay7_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay7_clear() {
    this->gen_02_delay7_buffer->setZero();
}

void gen_02_delay7_reset() {
    auto result = this->gen_02_delay7_calcSizeInSamples();
    this->gen_02_delay7__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay7_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay7_buffer);
    this->gen_02_delay7_wrap = this->gen_02_delay7_buffer->getSize() - 1;
    this->gen_02_delay7_clear();

    if (this->gen_02_delay7_reader >= this->gen_02_delay7__maxdelay || this->gen_02_delay7_writer >= this->gen_02_delay7__maxdelay) {
        this->gen_02_delay7_reader = 0;
        this->gen_02_delay7_writer = 0;
    }
}

void gen_02_delay7_dspsetup() {
    this->gen_02_delay7_reset();
}

number gen_02_delay7_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay7_size() {
    return this->gen_02_delay7__maxdelay;
}

void gen_02_delay6_step() {
    this->gen_02_delay6_reader++;

    if (this->gen_02_delay6_reader >= (int)(this->gen_02_delay6_buffer->getSize()))
        this->gen_02_delay6_reader = 0;
}

number gen_02_delay6_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay6_buffer->getSize()) + this->gen_02_delay6_reader - ((1024 > this->gen_02_delay6__maxdelay ? this->gen_02_delay6__maxdelay : (1024 < (this->gen_02_delay6_reader != this->gen_02_delay6_writer) ? this->gen_02_delay6_reader != this->gen_02_delay6_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay6_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay6_wrap))
    );
}

void gen_02_delay6_write(number v) {
    this->gen_02_delay6_writer = this->gen_02_delay6_reader;
    this->gen_02_delay6_buffer[(Index)this->gen_02_delay6_writer] = v;
}

number gen_02_delay6_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay6__maxdelay : size);
    number val = this->gen_02_delay6_read(effectiveSize, 0);
    this->gen_02_delay6_write(v);
    this->gen_02_delay6_step();
    return val;
}

array<Index, 2> gen_02_delay6_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay6_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay6_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay6_init() {
    auto result = this->gen_02_delay6_calcSizeInSamples();
    this->gen_02_delay6__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay6_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay6_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay6_clear() {
    this->gen_02_delay6_buffer->setZero();
}

void gen_02_delay6_reset() {
    auto result = this->gen_02_delay6_calcSizeInSamples();
    this->gen_02_delay6__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay6_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay6_buffer);
    this->gen_02_delay6_wrap = this->gen_02_delay6_buffer->getSize() - 1;
    this->gen_02_delay6_clear();

    if (this->gen_02_delay6_reader >= this->gen_02_delay6__maxdelay || this->gen_02_delay6_writer >= this->gen_02_delay6__maxdelay) {
        this->gen_02_delay6_reader = 0;
        this->gen_02_delay6_writer = 0;
    }
}

void gen_02_delay6_dspsetup() {
    this->gen_02_delay6_reset();
}

number gen_02_delay6_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay6_size() {
    return this->gen_02_delay6__maxdelay;
}

void gen_02_delay5_step() {
    this->gen_02_delay5_reader++;

    if (this->gen_02_delay5_reader >= (int)(this->gen_02_delay5_buffer->getSize()))
        this->gen_02_delay5_reader = 0;
}

number gen_02_delay5_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay5_buffer->getSize()) + this->gen_02_delay5_reader - ((1024 > this->gen_02_delay5__maxdelay ? this->gen_02_delay5__maxdelay : (1024 < (this->gen_02_delay5_reader != this->gen_02_delay5_writer) ? this->gen_02_delay5_reader != this->gen_02_delay5_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay5_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay5_wrap))
    );
}

void gen_02_delay5_write(number v) {
    this->gen_02_delay5_writer = this->gen_02_delay5_reader;
    this->gen_02_delay5_buffer[(Index)this->gen_02_delay5_writer] = v;
}

number gen_02_delay5_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay5__maxdelay : size);
    number val = this->gen_02_delay5_read(effectiveSize, 0);
    this->gen_02_delay5_write(v);
    this->gen_02_delay5_step();
    return val;
}

array<Index, 2> gen_02_delay5_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay5_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay5_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay5_init() {
    auto result = this->gen_02_delay5_calcSizeInSamples();
    this->gen_02_delay5__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay5_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay5_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay5_clear() {
    this->gen_02_delay5_buffer->setZero();
}

void gen_02_delay5_reset() {
    auto result = this->gen_02_delay5_calcSizeInSamples();
    this->gen_02_delay5__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay5_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay5_buffer);
    this->gen_02_delay5_wrap = this->gen_02_delay5_buffer->getSize() - 1;
    this->gen_02_delay5_clear();

    if (this->gen_02_delay5_reader >= this->gen_02_delay5__maxdelay || this->gen_02_delay5_writer >= this->gen_02_delay5__maxdelay) {
        this->gen_02_delay5_reader = 0;
        this->gen_02_delay5_writer = 0;
    }
}

void gen_02_delay5_dspsetup() {
    this->gen_02_delay5_reset();
}

number gen_02_delay5_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay5_size() {
    return this->gen_02_delay5__maxdelay;
}

void gen_02_delay4_step() {
    this->gen_02_delay4_reader++;

    if (this->gen_02_delay4_reader >= (int)(this->gen_02_delay4_buffer->getSize()))
        this->gen_02_delay4_reader = 0;
}

number gen_02_delay4_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay4_buffer->getSize()) + this->gen_02_delay4_reader - ((1024 > this->gen_02_delay4__maxdelay ? this->gen_02_delay4__maxdelay : (1024 < (this->gen_02_delay4_reader != this->gen_02_delay4_writer) ? this->gen_02_delay4_reader != this->gen_02_delay4_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay4_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay4_wrap))
    );
}

void gen_02_delay4_write(number v) {
    this->gen_02_delay4_writer = this->gen_02_delay4_reader;
    this->gen_02_delay4_buffer[(Index)this->gen_02_delay4_writer] = v;
}

number gen_02_delay4_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay4__maxdelay : size);
    number val = this->gen_02_delay4_read(effectiveSize, 0);
    this->gen_02_delay4_write(v);
    this->gen_02_delay4_step();
    return val;
}

array<Index, 2> gen_02_delay4_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay4_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay4_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay4_init() {
    auto result = this->gen_02_delay4_calcSizeInSamples();
    this->gen_02_delay4__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay4_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay4_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay4_clear() {
    this->gen_02_delay4_buffer->setZero();
}

void gen_02_delay4_reset() {
    auto result = this->gen_02_delay4_calcSizeInSamples();
    this->gen_02_delay4__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay4_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay4_buffer);
    this->gen_02_delay4_wrap = this->gen_02_delay4_buffer->getSize() - 1;
    this->gen_02_delay4_clear();

    if (this->gen_02_delay4_reader >= this->gen_02_delay4__maxdelay || this->gen_02_delay4_writer >= this->gen_02_delay4__maxdelay) {
        this->gen_02_delay4_reader = 0;
        this->gen_02_delay4_writer = 0;
    }
}

void gen_02_delay4_dspsetup() {
    this->gen_02_delay4_reset();
}

number gen_02_delay4_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay4_size() {
    return this->gen_02_delay4__maxdelay;
}

void gen_02_delay3_step() {
    this->gen_02_delay3_reader++;

    if (this->gen_02_delay3_reader >= (int)(this->gen_02_delay3_buffer->getSize()))
        this->gen_02_delay3_reader = 0;
}

number gen_02_delay3_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay3_buffer->getSize()) + this->gen_02_delay3_reader - ((1024 > this->gen_02_delay3__maxdelay ? this->gen_02_delay3__maxdelay : (1024 < (this->gen_02_delay3_reader != this->gen_02_delay3_writer) ? this->gen_02_delay3_reader != this->gen_02_delay3_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay3_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay3_wrap))
    );
}

void gen_02_delay3_write(number v) {
    this->gen_02_delay3_writer = this->gen_02_delay3_reader;
    this->gen_02_delay3_buffer[(Index)this->gen_02_delay3_writer] = v;
}

number gen_02_delay3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay3__maxdelay : size);
    number val = this->gen_02_delay3_read(effectiveSize, 0);
    this->gen_02_delay3_write(v);
    this->gen_02_delay3_step();
    return val;
}

array<Index, 2> gen_02_delay3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay3_init() {
    auto result = this->gen_02_delay3_calcSizeInSamples();
    this->gen_02_delay3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay3_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay3_clear() {
    this->gen_02_delay3_buffer->setZero();
}

void gen_02_delay3_reset() {
    auto result = this->gen_02_delay3_calcSizeInSamples();
    this->gen_02_delay3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay3_buffer);
    this->gen_02_delay3_wrap = this->gen_02_delay3_buffer->getSize() - 1;
    this->gen_02_delay3_clear();

    if (this->gen_02_delay3_reader >= this->gen_02_delay3__maxdelay || this->gen_02_delay3_writer >= this->gen_02_delay3__maxdelay) {
        this->gen_02_delay3_reader = 0;
        this->gen_02_delay3_writer = 0;
    }
}

void gen_02_delay3_dspsetup() {
    this->gen_02_delay3_reset();
}

number gen_02_delay3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay3_size() {
    return this->gen_02_delay3__maxdelay;
}

void gen_02_delay2_step() {
    this->gen_02_delay2_reader++;

    if (this->gen_02_delay2_reader >= (int)(this->gen_02_delay2_buffer->getSize()))
        this->gen_02_delay2_reader = 0;
}

number gen_02_delay2_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay2_buffer->getSize()) + this->gen_02_delay2_reader - ((1024 > this->gen_02_delay2__maxdelay ? this->gen_02_delay2__maxdelay : (1024 < (this->gen_02_delay2_reader != this->gen_02_delay2_writer) ? this->gen_02_delay2_reader != this->gen_02_delay2_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay2_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay2_wrap))
    );
}

void gen_02_delay2_write(number v) {
    this->gen_02_delay2_writer = this->gen_02_delay2_reader;
    this->gen_02_delay2_buffer[(Index)this->gen_02_delay2_writer] = v;
}

number gen_02_delay2_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay2__maxdelay : size);
    number val = this->gen_02_delay2_read(effectiveSize, 0);
    this->gen_02_delay2_write(v);
    this->gen_02_delay2_step();
    return val;
}

array<Index, 2> gen_02_delay2_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay2_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay2_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay2_init() {
    auto result = this->gen_02_delay2_calcSizeInSamples();
    this->gen_02_delay2__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay2_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay2_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay2_clear() {
    this->gen_02_delay2_buffer->setZero();
}

void gen_02_delay2_reset() {
    auto result = this->gen_02_delay2_calcSizeInSamples();
    this->gen_02_delay2__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay2_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay2_buffer);
    this->gen_02_delay2_wrap = this->gen_02_delay2_buffer->getSize() - 1;
    this->gen_02_delay2_clear();

    if (this->gen_02_delay2_reader >= this->gen_02_delay2__maxdelay || this->gen_02_delay2_writer >= this->gen_02_delay2__maxdelay) {
        this->gen_02_delay2_reader = 0;
        this->gen_02_delay2_writer = 0;
    }
}

void gen_02_delay2_dspsetup() {
    this->gen_02_delay2_reset();
}

number gen_02_delay2_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay2_size() {
    return this->gen_02_delay2__maxdelay;
}

void gen_02_delay1_step() {
    this->gen_02_delay1_reader++;

    if (this->gen_02_delay1_reader >= (int)(this->gen_02_delay1_buffer->getSize()))
        this->gen_02_delay1_reader = 0;
}

number gen_02_delay1_read(number size, Int interp) {
    RNBO_UNUSED(interp);
    RNBO_UNUSED(size);

    {
        {
            {}
        }
    }

    number r = (int)(this->gen_02_delay1_buffer->getSize()) + this->gen_02_delay1_reader - ((1024 > this->gen_02_delay1__maxdelay ? this->gen_02_delay1__maxdelay : (1024 < (this->gen_02_delay1_reader != this->gen_02_delay1_writer) ? this->gen_02_delay1_reader != this->gen_02_delay1_writer : 1024)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_02_delay1_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_02_delay1_wrap))
    );
}

void gen_02_delay1_write(number v) {
    this->gen_02_delay1_writer = this->gen_02_delay1_reader;
    this->gen_02_delay1_buffer[(Index)this->gen_02_delay1_writer] = v;
}

number gen_02_delay1_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_02_delay1__maxdelay : size);
    number val = this->gen_02_delay1_read(effectiveSize, 0);
    this->gen_02_delay1_write(v);
    this->gen_02_delay1_step();
    return val;
}

array<Index, 2> gen_02_delay1_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_02_delay1_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_02_delay1_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_02_delay1_init() {
    auto result = this->gen_02_delay1_calcSizeInSamples();
    this->gen_02_delay1__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay1_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_02_delay1_wrap = requestedSizeInSamples - 1;
}

void gen_02_delay1_clear() {
    this->gen_02_delay1_buffer->setZero();
}

void gen_02_delay1_reset() {
    auto result = this->gen_02_delay1_calcSizeInSamples();
    this->gen_02_delay1__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_02_delay1_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_02_delay1_buffer);
    this->gen_02_delay1_wrap = this->gen_02_delay1_buffer->getSize() - 1;
    this->gen_02_delay1_clear();

    if (this->gen_02_delay1_reader >= this->gen_02_delay1__maxdelay || this->gen_02_delay1_writer >= this->gen_02_delay1__maxdelay) {
        this->gen_02_delay1_reader = 0;
        this->gen_02_delay1_writer = 0;
    }
}

void gen_02_delay1_dspsetup() {
    this->gen_02_delay1_reset();
}

number gen_02_delay1_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 1024;
}

number gen_02_delay1_size() {
    return this->gen_02_delay1__maxdelay;
}

number gen_02_latch_21_next(number x, number control) {
    if (control != 0.)
        this->gen_02_latch_21_value = x;

    return this->gen_02_latch_21_value;
}

void gen_02_latch_21_dspsetup() {
    this->gen_02_latch_21_reset();
}

void gen_02_latch_21_reset() {
    this->gen_02_latch_21_value = 0;
}

number gen_02_cycle_50_ph_next(number freq, number reset) {
    RNBO_UNUSED(reset);

    {
        {}
    }

    number pincr = freq * this->gen_02_cycle_50_ph_conv;

    if (this->gen_02_cycle_50_ph_currentPhase < 0.)
        this->gen_02_cycle_50_ph_currentPhase = 1. + this->gen_02_cycle_50_ph_currentPhase;

    if (this->gen_02_cycle_50_ph_currentPhase > 1.)
        this->gen_02_cycle_50_ph_currentPhase = this->gen_02_cycle_50_ph_currentPhase - 1.;

    number tmp = this->gen_02_cycle_50_ph_currentPhase;
    this->gen_02_cycle_50_ph_currentPhase += pincr;
    return tmp;
}

void gen_02_cycle_50_ph_reset() {
    this->gen_02_cycle_50_ph_currentPhase = 0;
}

void gen_02_cycle_50_ph_dspsetup() {
    this->gen_02_cycle_50_ph_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

array<number, 2> gen_02_cycle_50_next(number frequency, number phase_offset) {
    RNBO_UNUSED(phase_offset);

    {
        uint32_t uint_phase;

        {
            uint_phase = this->gen_02_cycle_50_phasei;
        }

        uint32_t idx = (uint32_t)(uint32_rshift(uint_phase, 18));
        number frac = ((BinOpInt)((BinOpInt)uint_phase & (BinOpInt)262143)) * 3.81471181759574e-6;
        number y0 = this->gen_02_cycle_50_buffer[(Index)idx];
        number y1 = this->gen_02_cycle_50_buffer[(Index)((BinOpInt)(idx + 1) & (BinOpInt)16383)];
        number y = y0 + frac * (y1 - y0);

        {
            uint32_t pincr = (uint32_t)(uint32_trunc(frequency * this->gen_02_cycle_50_f2i));
            this->gen_02_cycle_50_phasei = uint32_add(this->gen_02_cycle_50_phasei, pincr);
        }

        return {y, uint_phase * 0.232830643653869629e-9};
    }
}

void gen_02_cycle_50_dspsetup() {
    this->gen_02_cycle_50_phasei = 0;
    this->gen_02_cycle_50_f2i = (this->samplerate() == 0. ? 0. : (number)4294967296 / this->samplerate());
    this->gen_02_cycle_50_wrap = (long)(this->gen_02_cycle_50_buffer->getSize()) - 1;
}

void gen_02_cycle_50_reset() {
    this->gen_02_cycle_50_phasei = 0;
}

void gen_02_cycle_50_bufferUpdated() {
    this->gen_02_cycle_50_wrap = (long)(this->gen_02_cycle_50_buffer->getSize()) - 1;
}

number gen_02_phasor_79_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_79_conv;

    if (this->gen_02_phasor_79_currentPhase < 0.)
        this->gen_02_phasor_79_currentPhase = 1. + this->gen_02_phasor_79_currentPhase;

    if (this->gen_02_phasor_79_currentPhase > 1.)
        this->gen_02_phasor_79_currentPhase = this->gen_02_phasor_79_currentPhase - 1.;

    number tmp = this->gen_02_phasor_79_currentPhase;
    this->gen_02_phasor_79_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_79_reset() {
    this->gen_02_phasor_79_currentPhase = 0;
}

void gen_02_phasor_79_dspsetup() {
    this->gen_02_phasor_79_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_85_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_85_conv;

    if (this->gen_02_phasor_85_currentPhase < 0.)
        this->gen_02_phasor_85_currentPhase = 1. + this->gen_02_phasor_85_currentPhase;

    if (this->gen_02_phasor_85_currentPhase > 1.)
        this->gen_02_phasor_85_currentPhase = this->gen_02_phasor_85_currentPhase - 1.;

    number tmp = this->gen_02_phasor_85_currentPhase;
    this->gen_02_phasor_85_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_85_reset() {
    this->gen_02_phasor_85_currentPhase = 0;
}

void gen_02_phasor_85_dspsetup() {
    this->gen_02_phasor_85_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_91_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_91_conv;

    if (this->gen_02_phasor_91_currentPhase < 0.)
        this->gen_02_phasor_91_currentPhase = 1. + this->gen_02_phasor_91_currentPhase;

    if (this->gen_02_phasor_91_currentPhase > 1.)
        this->gen_02_phasor_91_currentPhase = this->gen_02_phasor_91_currentPhase - 1.;

    number tmp = this->gen_02_phasor_91_currentPhase;
    this->gen_02_phasor_91_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_91_reset() {
    this->gen_02_phasor_91_currentPhase = 0;
}

void gen_02_phasor_91_dspsetup() {
    this->gen_02_phasor_91_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_97_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_97_conv;

    if (this->gen_02_phasor_97_currentPhase < 0.)
        this->gen_02_phasor_97_currentPhase = 1. + this->gen_02_phasor_97_currentPhase;

    if (this->gen_02_phasor_97_currentPhase > 1.)
        this->gen_02_phasor_97_currentPhase = this->gen_02_phasor_97_currentPhase - 1.;

    number tmp = this->gen_02_phasor_97_currentPhase;
    this->gen_02_phasor_97_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_97_reset() {
    this->gen_02_phasor_97_currentPhase = 0;
}

void gen_02_phasor_97_dspsetup() {
    this->gen_02_phasor_97_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_103_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_103_conv;

    if (this->gen_02_phasor_103_currentPhase < 0.)
        this->gen_02_phasor_103_currentPhase = 1. + this->gen_02_phasor_103_currentPhase;

    if (this->gen_02_phasor_103_currentPhase > 1.)
        this->gen_02_phasor_103_currentPhase = this->gen_02_phasor_103_currentPhase - 1.;

    number tmp = this->gen_02_phasor_103_currentPhase;
    this->gen_02_phasor_103_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_103_reset() {
    this->gen_02_phasor_103_currentPhase = 0;
}

void gen_02_phasor_103_dspsetup() {
    this->gen_02_phasor_103_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_109_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_109_conv;

    if (this->gen_02_phasor_109_currentPhase < 0.)
        this->gen_02_phasor_109_currentPhase = 1. + this->gen_02_phasor_109_currentPhase;

    if (this->gen_02_phasor_109_currentPhase > 1.)
        this->gen_02_phasor_109_currentPhase = this->gen_02_phasor_109_currentPhase - 1.;

    number tmp = this->gen_02_phasor_109_currentPhase;
    this->gen_02_phasor_109_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_109_reset() {
    this->gen_02_phasor_109_currentPhase = 0;
}

void gen_02_phasor_109_dspsetup() {
    this->gen_02_phasor_109_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_115_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_115_conv;

    if (this->gen_02_phasor_115_currentPhase < 0.)
        this->gen_02_phasor_115_currentPhase = 1. + this->gen_02_phasor_115_currentPhase;

    if (this->gen_02_phasor_115_currentPhase > 1.)
        this->gen_02_phasor_115_currentPhase = this->gen_02_phasor_115_currentPhase - 1.;

    number tmp = this->gen_02_phasor_115_currentPhase;
    this->gen_02_phasor_115_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_115_reset() {
    this->gen_02_phasor_115_currentPhase = 0;
}

void gen_02_phasor_115_dspsetup() {
    this->gen_02_phasor_115_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_121_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_121_conv;

    if (this->gen_02_phasor_121_currentPhase < 0.)
        this->gen_02_phasor_121_currentPhase = 1. + this->gen_02_phasor_121_currentPhase;

    if (this->gen_02_phasor_121_currentPhase > 1.)
        this->gen_02_phasor_121_currentPhase = this->gen_02_phasor_121_currentPhase - 1.;

    number tmp = this->gen_02_phasor_121_currentPhase;
    this->gen_02_phasor_121_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_121_reset() {
    this->gen_02_phasor_121_currentPhase = 0;
}

void gen_02_phasor_121_dspsetup() {
    this->gen_02_phasor_121_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_127_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_127_conv;

    if (this->gen_02_phasor_127_currentPhase < 0.)
        this->gen_02_phasor_127_currentPhase = 1. + this->gen_02_phasor_127_currentPhase;

    if (this->gen_02_phasor_127_currentPhase > 1.)
        this->gen_02_phasor_127_currentPhase = this->gen_02_phasor_127_currentPhase - 1.;

    number tmp = this->gen_02_phasor_127_currentPhase;
    this->gen_02_phasor_127_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_127_reset() {
    this->gen_02_phasor_127_currentPhase = 0;
}

void gen_02_phasor_127_dspsetup() {
    this->gen_02_phasor_127_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_133_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_133_conv;

    if (this->gen_02_phasor_133_currentPhase < 0.)
        this->gen_02_phasor_133_currentPhase = 1. + this->gen_02_phasor_133_currentPhase;

    if (this->gen_02_phasor_133_currentPhase > 1.)
        this->gen_02_phasor_133_currentPhase = this->gen_02_phasor_133_currentPhase - 1.;

    number tmp = this->gen_02_phasor_133_currentPhase;
    this->gen_02_phasor_133_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_133_reset() {
    this->gen_02_phasor_133_currentPhase = 0;
}

void gen_02_phasor_133_dspsetup() {
    this->gen_02_phasor_133_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_139_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_139_conv;

    if (this->gen_02_phasor_139_currentPhase < 0.)
        this->gen_02_phasor_139_currentPhase = 1. + this->gen_02_phasor_139_currentPhase;

    if (this->gen_02_phasor_139_currentPhase > 1.)
        this->gen_02_phasor_139_currentPhase = this->gen_02_phasor_139_currentPhase - 1.;

    number tmp = this->gen_02_phasor_139_currentPhase;
    this->gen_02_phasor_139_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_139_reset() {
    this->gen_02_phasor_139_currentPhase = 0;
}

void gen_02_phasor_139_dspsetup() {
    this->gen_02_phasor_139_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_145_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_145_conv;

    if (this->gen_02_phasor_145_currentPhase < 0.)
        this->gen_02_phasor_145_currentPhase = 1. + this->gen_02_phasor_145_currentPhase;

    if (this->gen_02_phasor_145_currentPhase > 1.)
        this->gen_02_phasor_145_currentPhase = this->gen_02_phasor_145_currentPhase - 1.;

    number tmp = this->gen_02_phasor_145_currentPhase;
    this->gen_02_phasor_145_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_145_reset() {
    this->gen_02_phasor_145_currentPhase = 0;
}

void gen_02_phasor_145_dspsetup() {
    this->gen_02_phasor_145_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_151_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_151_conv;

    if (this->gen_02_phasor_151_currentPhase < 0.)
        this->gen_02_phasor_151_currentPhase = 1. + this->gen_02_phasor_151_currentPhase;

    if (this->gen_02_phasor_151_currentPhase > 1.)
        this->gen_02_phasor_151_currentPhase = this->gen_02_phasor_151_currentPhase - 1.;

    number tmp = this->gen_02_phasor_151_currentPhase;
    this->gen_02_phasor_151_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_151_reset() {
    this->gen_02_phasor_151_currentPhase = 0;
}

void gen_02_phasor_151_dspsetup() {
    this->gen_02_phasor_151_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_157_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_157_conv;

    if (this->gen_02_phasor_157_currentPhase < 0.)
        this->gen_02_phasor_157_currentPhase = 1. + this->gen_02_phasor_157_currentPhase;

    if (this->gen_02_phasor_157_currentPhase > 1.)
        this->gen_02_phasor_157_currentPhase = this->gen_02_phasor_157_currentPhase - 1.;

    number tmp = this->gen_02_phasor_157_currentPhase;
    this->gen_02_phasor_157_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_157_reset() {
    this->gen_02_phasor_157_currentPhase = 0;
}

void gen_02_phasor_157_dspsetup() {
    this->gen_02_phasor_157_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_phasor_163_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_163_conv;

    if (this->gen_02_phasor_163_currentPhase < 0.)
        this->gen_02_phasor_163_currentPhase = 1. + this->gen_02_phasor_163_currentPhase;

    if (this->gen_02_phasor_163_currentPhase > 1.)
        this->gen_02_phasor_163_currentPhase = this->gen_02_phasor_163_currentPhase - 1.;

    number tmp = this->gen_02_phasor_163_currentPhase;
    this->gen_02_phasor_163_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_163_reset() {
    this->gen_02_phasor_163_currentPhase = 0;
}

void gen_02_phasor_163_dspsetup() {
    this->gen_02_phasor_163_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_02_cycle_165_ph_next(number freq, number reset) {
    RNBO_UNUSED(reset);

    {
        {}
    }

    number pincr = freq * this->gen_02_cycle_165_ph_conv;

    if (this->gen_02_cycle_165_ph_currentPhase < 0.)
        this->gen_02_cycle_165_ph_currentPhase = 1. + this->gen_02_cycle_165_ph_currentPhase;

    if (this->gen_02_cycle_165_ph_currentPhase > 1.)
        this->gen_02_cycle_165_ph_currentPhase = this->gen_02_cycle_165_ph_currentPhase - 1.;

    number tmp = this->gen_02_cycle_165_ph_currentPhase;
    this->gen_02_cycle_165_ph_currentPhase += pincr;
    return tmp;
}

void gen_02_cycle_165_ph_reset() {
    this->gen_02_cycle_165_ph_currentPhase = 0;
}

void gen_02_cycle_165_ph_dspsetup() {
    this->gen_02_cycle_165_ph_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

array<number, 2> gen_02_cycle_165_next(number frequency, number phase_offset) {
    RNBO_UNUSED(phase_offset);

    {
        uint32_t uint_phase;

        {
            uint_phase = this->gen_02_cycle_165_phasei;
        }

        uint32_t idx = (uint32_t)(uint32_rshift(uint_phase, 18));
        number frac = ((BinOpInt)((BinOpInt)uint_phase & (BinOpInt)262143)) * 3.81471181759574e-6;
        number y0 = this->gen_02_cycle_165_buffer[(Index)idx];
        number y1 = this->gen_02_cycle_165_buffer[(Index)((BinOpInt)(idx + 1) & (BinOpInt)16383)];
        number y = y0 + frac * (y1 - y0);

        {
            uint32_t pincr = (uint32_t)(uint32_trunc(frequency * this->gen_02_cycle_165_f2i));
            this->gen_02_cycle_165_phasei = uint32_add(this->gen_02_cycle_165_phasei, pincr);
        }

        return {y, uint_phase * 0.232830643653869629e-9};
    }
}

void gen_02_cycle_165_dspsetup() {
    this->gen_02_cycle_165_phasei = 0;
    this->gen_02_cycle_165_f2i = (this->samplerate() == 0. ? 0. : (number)4294967296 / this->samplerate());
    this->gen_02_cycle_165_wrap = (long)(this->gen_02_cycle_165_buffer->getSize()) - 1;
}

void gen_02_cycle_165_reset() {
    this->gen_02_cycle_165_phasei = 0;
}

void gen_02_cycle_165_bufferUpdated() {
    this->gen_02_cycle_165_wrap = (long)(this->gen_02_cycle_165_buffer->getSize()) - 1;
}

void gen_02_dspsetup(bool force) {
    if ((bool)(this->gen_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_02_setupDone = true;
    this->gen_02_prev_exprdata_dspsetup();
    this->gen_02_modPanDel_dspsetup();
    this->gen_02_modFreqDel_dspsetup();
    this->gen_02_delay15_dspsetup();
    this->gen_02_delay14_dspsetup();
    this->gen_02_delay13_dspsetup();
    this->gen_02_delay12_dspsetup();
    this->gen_02_delay11_dspsetup();
    this->gen_02_delay10_dspsetup();
    this->gen_02_delay9_dspsetup();
    this->gen_02_delay8_dspsetup();
    this->gen_02_delay7_dspsetup();
    this->gen_02_delay6_dspsetup();
    this->gen_02_delay5_dspsetup();
    this->gen_02_delay4_dspsetup();
    this->gen_02_delay3_dspsetup();
    this->gen_02_delay2_dspsetup();
    this->gen_02_delay1_dspsetup();
    this->gen_02_latch_21_dspsetup();
    this->gen_02_cycle_50_ph_dspsetup();
    this->gen_02_cycle_50_dspsetup();
    this->gen_02_phasor_79_dspsetup();
    this->gen_02_phasor_85_dspsetup();
    this->gen_02_phasor_91_dspsetup();
    this->gen_02_phasor_97_dspsetup();
    this->gen_02_phasor_103_dspsetup();
    this->gen_02_phasor_109_dspsetup();
    this->gen_02_phasor_115_dspsetup();
    this->gen_02_phasor_121_dspsetup();
    this->gen_02_phasor_127_dspsetup();
    this->gen_02_phasor_133_dspsetup();
    this->gen_02_phasor_139_dspsetup();
    this->gen_02_phasor_145_dspsetup();
    this->gen_02_phasor_151_dspsetup();
    this->gen_02_phasor_157_dspsetup();
    this->gen_02_phasor_163_dspsetup();
    this->gen_02_cycle_165_ph_dspsetup();
    this->gen_02_cycle_165_dspsetup();
}

template <typename T> void fftstream_tilde_01_fft_next(T& buffer, int fftsize) {
    if (this->fftstream_tilde_01_fft_lastsize != fftsize) {
        for (Index i = 0; i < 32; i++) {
            if ((BinOpInt)((BinOpInt)1 << imod_nocast((UBinOpInt)i, 32)) == fftsize) {
                this->fftstream_tilde_01_fft_levels = i;
            }
        }

        for (Index i = 0; i < fftsize; i++) {
            this->fftstream_tilde_01_fft_costab->push(rnbo_cos(6.28318530717958647692 * i / fftsize));
            this->fftstream_tilde_01_fft_sintab->push(rnbo_sin(6.28318530717958647692 * i / fftsize));
        }

        this->fftstream_tilde_01_fft_lastsize = fftsize;
    }

    if (this->fftstream_tilde_01_fft_levels != -1) {
        for (Index i = 0; i < fftsize; i++) {
            Index x = (Index)(i);
            Index j = 0;

            for (Index k = 0; k < this->fftstream_tilde_01_fft_levels; k++) {
                j = (BinOpInt)((BinOpInt)((BinOpInt)j << imod_nocast((UBinOpInt)1, 32)) | (BinOpInt)((BinOpInt)x & (BinOpInt)1));
                x = (BinOpInt)((BinOpInt)x >> imod_nocast((UBinOpInt)1, 32));
            }

            if (j > i) {
                number temp = buffer[(Index)(2 * i)];
                buffer[(Index)(2 * i)] = buffer[(Index)(2 * j)];
                buffer[(Index)(2 * j)] = temp;
                temp = buffer[(Index)(2 * i + 1)];
                buffer[(Index)(2 * i + 1)] = buffer[(Index)(2 * j + 1)];
                buffer[(Index)(2 * j + 1)] = temp;
            }
        }

        for (Index size = 2; size <= fftsize; size *= 2) {
            Index halfsize = (Index)(size / (number)2);
            Index tablestep = (Index)(fftsize / size);

            for (Index i = 0; i < fftsize; i += size) {
                Index k = 0;

                for (Index j = (Index)(i); j < i + halfsize; j++) {
                    Index l = (Index)(j + halfsize);
                    number real_l = buffer[(Index)(2 * l)];
                    number imag_l = buffer[(Index)(2 * l + 1)];
                    number tpre = real_l * this->fftstream_tilde_01_fft_costab[(Index)k] + imag_l * this->fftstream_tilde_01_fft_sintab[(Index)k];
                    number tpim = -real_l * this->fftstream_tilde_01_fft_sintab[(Index)k] + imag_l * this->fftstream_tilde_01_fft_costab[(Index)k];
                    number real_j = buffer[(Index)(2 * j)];
                    number imag_j = buffer[(Index)(2 * j + 1)];
                    buffer[(Index)(2 * l)] = real_j - tpre;
                    buffer[(Index)(2 * l + 1)] = imag_j - tpim;
                    buffer[(Index)(2 * j)] = real_j + tpre;
                    buffer[(Index)(2 * j + 1)] = imag_j + tpim;
                    k += tablestep;
                }
            }
        }
    }
}

void fftstream_tilde_01_fft_reset() {
    this->fftstream_tilde_01_fft_lastsize = 0;
    this->fftstream_tilde_01_fft_costab = {};
    this->fftstream_tilde_01_fft_sintab = {};
    this->fftstream_tilde_01_fft_levels = -1;
}

void fftstream_tilde_01_dspsetup(bool force) {
    if ((bool)(this->fftstream_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    for (int i = 0; i < 1024 * 2; i++) {
        this->fftstream_tilde_01_inWorkspace[(Index)i] = 0;
        this->fftstream_tilde_01_outWorkspace[(Index)i] = 0;
    }

    int safeframesize = (int)(1024);

    {
        safeframesize = nextpoweroftwo(1024);
    }

    int framepos = (int)(0 % safeframesize);

    if (framepos < 1024) {
        this->fftstream_tilde_01_datapos = framepos;
    } else {
        this->fftstream_tilde_01_datapos = 0;
    }

    this->fftstream_tilde_01_setupDone = true;
}

void data_01_init() {
    this->data_01_buffer->setWantsFill(true);
}

Index data_01_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    number size = 0;
    return (Index)(size);
}

void data_01_dspsetup(bool force) {
    if ((bool)(this->data_01_setupDone) && (bool)(!(bool)(force)))
        return;

    if (this->data_01_sizemode == 2) {
        this->data_01_buffer = this->data_01_buffer->setSize((Index)(this->mstosamps(this->data_01_sizems)));
        updateDataRef(this, this->data_01_buffer);
    } else if (this->data_01_sizemode == 3) {
        this->data_01_buffer = this->data_01_buffer->setSize(this->data_01_evaluateSizeExpr(this->samplerate(), this->vectorsize()));
        updateDataRef(this, this->data_01_buffer);
    }

    this->data_01_setupDone = true;
}

void data_01_bufferUpdated() {
    this->data_01_report();
}

void data_01_report() {
    this->data_01_srout_set(this->data_01_buffer->getSampleRate());
    this->data_01_chanout_set(this->data_01_buffer->getChannels());
    this->data_01_sizeout_set(this->data_01_buffer->getSize());
}

void param_07_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_07_value;
}

void param_07_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_07_value_set(preset["value"]);
}

void param_08_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_08_value;
}

void param_08_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_08_value_set(preset["value"]);
}

void param_09_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_09_value;
}

void param_09_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_09_value_set(preset["value"]);
}

void param_10_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_10_value;
}

void param_10_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_10_value_set(preset["value"]);
}

void param_11_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_11_value;
}

void param_11_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_11_value_set(preset["value"]);
}

void param_12_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_12_value;
}

void param_12_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_12_value_set(preset["value"]);
}

void param_13_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_13_value;
}

void param_13_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_13_value_set(preset["value"]);
}

void param_14_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_14_value;
}

void param_14_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_14_value_set(preset["value"]);
}

void param_15_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_15_value;
}

void param_15_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_15_value_set(preset["value"]);
}

void param_16_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_16_value;
}

void param_16_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_16_value_set(preset["value"]);
}

void param_17_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_17_value;
}

void param_17_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_17_value_set(preset["value"]);
}

void param_18_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_18_value;
}

void param_18_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_18_value_set(preset["value"]);
}

void param_19_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_19_value;
}

void param_19_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_19_value_set(preset["value"]);
}

void param_20_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_20_value;
}

void param_20_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_20_value_set(preset["value"]);
}

void param_21_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_21_value;
}

void param_21_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_21_value_set(preset["value"]);
}

void param_22_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_22_value;
}

void param_22_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_22_value_set(preset["value"]);
}

void param_23_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_23_value;
}

void param_23_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_23_value_set(preset["value"]);
}

void param_24_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_24_value;
}

void param_24_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_24_value_set(preset["value"]);
}

void param_25_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_25_value;
}

void param_25_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_25_value_set(preset["value"]);
}

void param_26_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_26_value;
}

void param_26_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_26_value_set(preset["value"]);
}

void param_27_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_27_value;
}

void param_27_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_27_value_set(preset["value"]);
}

void param_28_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_28_value;
}

void param_28_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_28_value_set(preset["value"]);
}

void param_29_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_29_value;
}

void param_29_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_29_value_set(preset["value"]);
}

void param_30_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_30_value;
}

void param_30_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_30_value_set(preset["value"]);
}

Index globaltransport_getSampleOffset(MillisecondTime time) {
    return this->mstosamps(this->maximum(0, time - this->getEngine()->getCurrentTime()));
}

number globaltransport_getTempoAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_tempo[(Index)sampleOffset] : this->globaltransport_lastTempo);
}

number globaltransport_getStateAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_state[(Index)sampleOffset] : this->globaltransport_lastState);
}

number globaltransport_getState(MillisecondTime time) {
    return this->globaltransport_getStateAtSample(this->globaltransport_getSampleOffset(time));
}

number globaltransport_getBeatTime(MillisecondTime time) {
    number i = 2;

    while (i < this->globaltransport_beatTimeChanges->length && this->globaltransport_beatTimeChanges[(Index)(i + 1)] <= time) {
        i += 2;
    }

    i -= 2;
    number beatTimeBase = this->globaltransport_beatTimeChanges[(Index)i];

    if (this->globaltransport_getState(time) == 0)
        return beatTimeBase;

    number beatTimeBaseMsTime = this->globaltransport_beatTimeChanges[(Index)(i + 1)];
    number diff = time - beatTimeBaseMsTime;
    return beatTimeBase + this->mstobeats(diff);
}

bool globaltransport_setTempo(MillisecondTime time, number tempo, bool notify) {
    if ((bool)(notify)) {
        this->processTempoEvent(time, tempo);
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getTempoAtSample(offset) != tempo) {
            this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
            this->globaltransport_beatTimeChanges->push(time);
            fillSignal(this->globaltransport_tempo, this->vs, tempo, offset);
            this->globaltransport_lastTempo = tempo;
            this->globaltransport_tempoNeedsReset = true;
            return true;
        }
    }

    return false;
}

number globaltransport_getTempo(MillisecondTime time) {
    return this->globaltransport_getTempoAtSample(this->globaltransport_getSampleOffset(time));
}

bool globaltransport_setState(MillisecondTime time, number state, bool notify) {
    if ((bool)(notify)) {
        this->processTransportEvent(time, TransportState(state));
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getStateAtSample(offset) != state) {
            fillSignal(this->globaltransport_state, this->vs, state, offset);
            this->globaltransport_lastState = TransportState(state);
            this->globaltransport_stateNeedsReset = true;

            if (state == 0) {
                this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
                this->globaltransport_beatTimeChanges->push(time);
            }

            return true;
        }
    }

    return false;
}

bool globaltransport_setBeatTime(MillisecondTime time, number beattime, bool notify) {
    if ((bool)(notify)) {
        this->processBeatTimeEvent(time, beattime);
        this->globaltransport_notify = true;
        return false;
    } else {
        bool beatTimeHasChanged = false;
        float oldBeatTime = (float)(this->globaltransport_getBeatTime(time));
        float newBeatTime = (float)(beattime);

        if (oldBeatTime != newBeatTime) {
            beatTimeHasChanged = true;
        }

        this->globaltransport_beatTimeChanges->push(beattime);
        this->globaltransport_beatTimeChanges->push(time);
        return beatTimeHasChanged;
    }
}

number globaltransport_getBeatTimeAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getBeatTime(this->getEngine()->getCurrentTime() + msOffset);
}

array<number, 2> globaltransport_getTimeSignature(MillisecondTime time) {
    number i = 3;

    while (i < this->globaltransport_timeSignatureChanges->length && this->globaltransport_timeSignatureChanges[(Index)(i + 2)] <= time) {
        i += 3;
    }

    i -= 3;

    return {
        this->globaltransport_timeSignatureChanges[(Index)i],
        this->globaltransport_timeSignatureChanges[(Index)(i + 1)]
    };
}

array<number, 2> globaltransport_getTimeSignatureAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getTimeSignature(this->getEngine()->getCurrentTime() + msOffset);
}

bool globaltransport_setTimeSignature(MillisecondTime time, number numerator, number denominator, bool notify) {
    if ((bool)(notify)) {
        this->processTimeSignatureEvent(time, (int)(numerator), (int)(denominator));
        this->globaltransport_notify = true;
    } else {
        array<number, 2> currentSig = this->globaltransport_getTimeSignature(time);

        if (currentSig[0] != numerator || currentSig[1] != denominator) {
            this->globaltransport_timeSignatureChanges->push(numerator);
            this->globaltransport_timeSignatureChanges->push(denominator);
            this->globaltransport_timeSignatureChanges->push(time);
            return true;
        }
    }

    return false;
}

void globaltransport_advance() {
    if ((bool)(this->globaltransport_tempoNeedsReset)) {
        fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
        this->globaltransport_tempoNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTempoEvent(this->globaltransport_lastTempo);
        }
    }

    if ((bool)(this->globaltransport_stateNeedsReset)) {
        fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
        this->globaltransport_stateNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTransportEvent(TransportState(this->globaltransport_lastState));
        }
    }

    if (this->globaltransport_beatTimeChanges->length > 2) {
        this->globaltransport_beatTimeChanges[0] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 2)];
        this->globaltransport_beatTimeChanges[1] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 1)];
        this->globaltransport_beatTimeChanges->length = 2;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendBeatTimeEvent(this->globaltransport_beatTimeChanges[0]);
        }
    }

    if (this->globaltransport_timeSignatureChanges->length > 3) {
        this->globaltransport_timeSignatureChanges[0] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 3)];
        this->globaltransport_timeSignatureChanges[1] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 2)];
        this->globaltransport_timeSignatureChanges[2] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 1)];
        this->globaltransport_timeSignatureChanges->length = 3;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTimeSignatureEvent(
                (int)(this->globaltransport_timeSignatureChanges[0]),
                (int)(this->globaltransport_timeSignatureChanges[1])
            );
        }
    }

    this->globaltransport_notify = false;
}

void globaltransport_dspsetup(bool force) {
    if ((bool)(this->globaltransport_setupDone) && (bool)(!(bool)(force)))
        return;

    fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
    this->globaltransport_tempoNeedsReset = false;
    fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
    this->globaltransport_stateNeedsReset = false;
    this->globaltransport_setupDone = true;
}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

void updateTime(MillisecondTime time) {
    this->_currentTime = time;
    this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr)));

    if (this->sampleOffsetIntoNextAudioBuffer >= (SampleIndex)(this->vs))
        this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(this->vs) - 1;

    if (this->sampleOffsetIntoNextAudioBuffer < 0)
        this->sampleOffsetIntoNextAudioBuffer = 0;
}

void assign_defaults()
{
    dspexpr_01_in1 = 0;
    dspexpr_01_in2 = 0;
    dspexpr_01_in3 = 0;
    param_01_value = 1;
    param_02_value = 1;
    expr_01_in1 = 0;
    expr_01_in2 = 100;
    expr_01_out1 = 0;
    param_03_value = 0;
    param_04_value = 5;
    param_05_value = 39.85;
    param_06_value = 0;
    gen_01_in1 = 0;
    gen_01_in2 = 0;
    gen_01_mix = 0;
    gen_01_delayCom = 0;
    gen_01_delayAll = 0;
    dspexpr_02_in1 = 0;
    dspexpr_02_in2 = 0;
    gen_02_in1 = 0;
    gen_02_in2 = 0;
    gen_02_in3 = 0;
    gen_02_amp15 = 0;
    gen_02_amp14 = 0;
    gen_02_amp13 = 0;
    gen_02_amp12 = 0;
    gen_02_amp11 = 0;
    gen_02_amp10 = 0;
    gen_02_amp9 = 0;
    gen_02_amp8 = 0;
    gen_02_amp7 = 0;
    gen_02_amp6 = 0;
    gen_02_amp5 = 0;
    gen_02_amp4 = 0;
    gen_02_amp3 = 0;
    gen_02_amp2 = 0;
    gen_02_amp1 = 0;
    gen_02_stereoMode = 0;
    gen_02_modPanFreq = 0;
    gen_02_modAmp = 0;
    gen_02_modFreq = 0;
    gen_02_freqScale = 0;
    gen_02_tuning = 0;
    gen_02_equalTemperament = 0;
    gen_02_equalTemperamentMode = 0;
    gen_02_ampSmooth = 0;
    gen_02_freqSmooth = 0;
    gen_02_binSmooth = 0;
    fftstream_tilde_01_realIn = 0;
    fftstream_tilde_01_imagIn = 0;
    data_01_sizeout = 0;
    data_01_size = 15;
    data_01_sizems = 0;
    data_01_normalize = 0.995;
    data_01_channels = 2;
    dspexpr_03_in1 = 0;
    dspexpr_03_in2 = 0;
    dspexpr_03_in3 = 0;
    param_07_value = 1;
    param_08_value = 1;
    param_09_value = 0.99;
    param_10_value = 0.99;
    param_11_value = 0.5;
    param_12_value = 10;
    param_13_value = 440;
    param_14_value = 12;
    param_15_value = 0;
    param_16_value = 1;
    param_17_value = 1;
    param_18_value = 1;
    param_19_value = 1;
    param_20_value = 1;
    param_21_value = 1;
    param_22_value = 1;
    param_23_value = 1;
    param_24_value = 1;
    param_25_value = 1;
    param_26_value = 1;
    param_27_value = 1;
    param_28_value = 1;
    param_29_value = 1;
    param_30_value = 1;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    signals[0] = nullptr;
    signals[1] = nullptr;
    signals[2] = nullptr;
    signals[3] = nullptr;
    signals[4] = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    param_01_lastValue = 0;
    param_02_lastValue = 0;
    param_03_lastValue = 0;
    param_04_lastValue = 0;
    param_05_lastValue = 0;
    param_06_lastValue = 0;
    gen_01_del_9__maxdelay = 0;
    gen_01_del_9_sizemode = 0;
    gen_01_del_9_wrap = 0;
    gen_01_del_9_reader = 0;
    gen_01_del_9_writer = 0;
    gen_01_del_8__maxdelay = 0;
    gen_01_del_8_sizemode = 0;
    gen_01_del_8_wrap = 0;
    gen_01_del_8_reader = 0;
    gen_01_del_8_writer = 0;
    gen_01_del_7__maxdelay = 0;
    gen_01_del_7_sizemode = 0;
    gen_01_del_7_wrap = 0;
    gen_01_del_7_reader = 0;
    gen_01_del_7_writer = 0;
    gen_01_del1__maxdelay = 0;
    gen_01_del1_sizemode = 0;
    gen_01_del1_wrap = 0;
    gen_01_del1_reader = 0;
    gen_01_del1_writer = 0;
    gen_01_del_6__maxdelay = 0;
    gen_01_del_6_sizemode = 0;
    gen_01_del_6_wrap = 0;
    gen_01_del_6_reader = 0;
    gen_01_del_6_writer = 0;
    gen_01_del_5__maxdelay = 0;
    gen_01_del_5_sizemode = 0;
    gen_01_del_5_wrap = 0;
    gen_01_del_5_reader = 0;
    gen_01_del_5_writer = 0;
    gen_01_del_4__maxdelay = 0;
    gen_01_del_4_sizemode = 0;
    gen_01_del_4_wrap = 0;
    gen_01_del_4_reader = 0;
    gen_01_del_4_writer = 0;
    gen_01_del_3__maxdelay = 0;
    gen_01_del_3_sizemode = 0;
    gen_01_del_3_wrap = 0;
    gen_01_del_3_reader = 0;
    gen_01_del_3_writer = 0;
    gen_01_del_2__maxdelay = 0;
    gen_01_del_2_sizemode = 0;
    gen_01_del_2_wrap = 0;
    gen_01_del_2_reader = 0;
    gen_01_del_2_writer = 0;
    gen_01_del_1__maxdelay = 0;
    gen_01_del_1_sizemode = 0;
    gen_01_del_1_wrap = 0;
    gen_01_del_1_reader = 0;
    gen_01_del_1_writer = 0;
    gen_01_del3__maxdelay = 0;
    gen_01_del3_sizemode = 0;
    gen_01_del3_wrap = 0;
    gen_01_del3_reader = 0;
    gen_01_del3_writer = 0;
    gen_01_del2__maxdelay = 0;
    gen_01_del2_sizemode = 0;
    gen_01_del2_wrap = 0;
    gen_01_del2_reader = 0;
    gen_01_del2_writer = 0;
    gen_01_setupDone = false;
    gen_02_modPanDel__maxdelay = 0;
    gen_02_modPanDel_sizemode = 0;
    gen_02_modPanDel_wrap = 0;
    gen_02_modPanDel_reader = 0;
    gen_02_modPanDel_writer = 0;
    gen_02_modFreqDel__maxdelay = 0;
    gen_02_modFreqDel_sizemode = 0;
    gen_02_modFreqDel_wrap = 0;
    gen_02_modFreqDel_reader = 0;
    gen_02_modFreqDel_writer = 0;
    gen_02_delay15__maxdelay = 0;
    gen_02_delay15_sizemode = 0;
    gen_02_delay15_wrap = 0;
    gen_02_delay15_reader = 0;
    gen_02_delay15_writer = 0;
    gen_02_delay14__maxdelay = 0;
    gen_02_delay14_sizemode = 0;
    gen_02_delay14_wrap = 0;
    gen_02_delay14_reader = 0;
    gen_02_delay14_writer = 0;
    gen_02_delay13__maxdelay = 0;
    gen_02_delay13_sizemode = 0;
    gen_02_delay13_wrap = 0;
    gen_02_delay13_reader = 0;
    gen_02_delay13_writer = 0;
    gen_02_delay12__maxdelay = 0;
    gen_02_delay12_sizemode = 0;
    gen_02_delay12_wrap = 0;
    gen_02_delay12_reader = 0;
    gen_02_delay12_writer = 0;
    gen_02_delay11__maxdelay = 0;
    gen_02_delay11_sizemode = 0;
    gen_02_delay11_wrap = 0;
    gen_02_delay11_reader = 0;
    gen_02_delay11_writer = 0;
    gen_02_delay10__maxdelay = 0;
    gen_02_delay10_sizemode = 0;
    gen_02_delay10_wrap = 0;
    gen_02_delay10_reader = 0;
    gen_02_delay10_writer = 0;
    gen_02_delay9__maxdelay = 0;
    gen_02_delay9_sizemode = 0;
    gen_02_delay9_wrap = 0;
    gen_02_delay9_reader = 0;
    gen_02_delay9_writer = 0;
    gen_02_delay8__maxdelay = 0;
    gen_02_delay8_sizemode = 0;
    gen_02_delay8_wrap = 0;
    gen_02_delay8_reader = 0;
    gen_02_delay8_writer = 0;
    gen_02_delay7__maxdelay = 0;
    gen_02_delay7_sizemode = 0;
    gen_02_delay7_wrap = 0;
    gen_02_delay7_reader = 0;
    gen_02_delay7_writer = 0;
    gen_02_delay6__maxdelay = 0;
    gen_02_delay6_sizemode = 0;
    gen_02_delay6_wrap = 0;
    gen_02_delay6_reader = 0;
    gen_02_delay6_writer = 0;
    gen_02_delay5__maxdelay = 0;
    gen_02_delay5_sizemode = 0;
    gen_02_delay5_wrap = 0;
    gen_02_delay5_reader = 0;
    gen_02_delay5_writer = 0;
    gen_02_delay4__maxdelay = 0;
    gen_02_delay4_sizemode = 0;
    gen_02_delay4_wrap = 0;
    gen_02_delay4_reader = 0;
    gen_02_delay4_writer = 0;
    gen_02_delay3__maxdelay = 0;
    gen_02_delay3_sizemode = 0;
    gen_02_delay3_wrap = 0;
    gen_02_delay3_reader = 0;
    gen_02_delay3_writer = 0;
    gen_02_delay2__maxdelay = 0;
    gen_02_delay2_sizemode = 0;
    gen_02_delay2_wrap = 0;
    gen_02_delay2_reader = 0;
    gen_02_delay2_writer = 0;
    gen_02_delay1__maxdelay = 0;
    gen_02_delay1_sizemode = 0;
    gen_02_delay1_wrap = 0;
    gen_02_delay1_reader = 0;
    gen_02_delay1_writer = 0;
    gen_02_latch_21_value = 0;
    gen_02_cycle_50_ph_currentPhase = 0;
    gen_02_cycle_50_ph_conv = 0;
    gen_02_cycle_50_wrap = 0;
    gen_02_phasor_79_currentPhase = 0;
    gen_02_phasor_79_conv = 0;
    gen_02_phasor_85_currentPhase = 0;
    gen_02_phasor_85_conv = 0;
    gen_02_phasor_91_currentPhase = 0;
    gen_02_phasor_91_conv = 0;
    gen_02_phasor_97_currentPhase = 0;
    gen_02_phasor_97_conv = 0;
    gen_02_phasor_103_currentPhase = 0;
    gen_02_phasor_103_conv = 0;
    gen_02_phasor_109_currentPhase = 0;
    gen_02_phasor_109_conv = 0;
    gen_02_phasor_115_currentPhase = 0;
    gen_02_phasor_115_conv = 0;
    gen_02_phasor_121_currentPhase = 0;
    gen_02_phasor_121_conv = 0;
    gen_02_phasor_127_currentPhase = 0;
    gen_02_phasor_127_conv = 0;
    gen_02_phasor_133_currentPhase = 0;
    gen_02_phasor_133_conv = 0;
    gen_02_phasor_139_currentPhase = 0;
    gen_02_phasor_139_conv = 0;
    gen_02_phasor_145_currentPhase = 0;
    gen_02_phasor_145_conv = 0;
    gen_02_phasor_151_currentPhase = 0;
    gen_02_phasor_151_conv = 0;
    gen_02_phasor_157_currentPhase = 0;
    gen_02_phasor_157_conv = 0;
    gen_02_phasor_163_currentPhase = 0;
    gen_02_phasor_163_conv = 0;
    gen_02_cycle_165_ph_currentPhase = 0;
    gen_02_cycle_165_ph_conv = 0;
    gen_02_cycle_165_wrap = 0;
    gen_02_setupDone = false;
    fftstream_tilde_01_datapos = 0;
    fftstream_tilde_01_fft_lastsize = 0;
    fftstream_tilde_01_fft_levels = -1;
    fftstream_tilde_01_setupDone = false;
    data_01_sizemode = 1;
    data_01_setupDone = false;
    param_07_lastValue = 0;
    param_08_lastValue = 0;
    param_09_lastValue = 0;
    param_10_lastValue = 0;
    param_11_lastValue = 0;
    param_12_lastValue = 0;
    param_13_lastValue = 0;
    param_14_lastValue = 0;
    param_15_lastValue = 0;
    param_16_lastValue = 0;
    param_17_lastValue = 0;
    param_18_lastValue = 0;
    param_19_lastValue = 0;
    param_20_lastValue = 0;
    param_21_lastValue = 0;
    param_22_lastValue = 0;
    param_23_lastValue = 0;
    param_24_lastValue = 0;
    param_25_lastValue = 0;
    param_26_lastValue = 0;
    param_27_lastValue = 0;
    param_28_lastValue = 0;
    param_29_lastValue = 0;
    param_30_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_tempoNeedsReset = false;
    globaltransport_lastTempo = 120;
    globaltransport_state = nullptr;
    globaltransport_stateNeedsReset = false;
    globaltransport_lastState = 0;
    globaltransport_beatTimeChanges = { 0, 0 };
    globaltransport_timeSignatureChanges = { 4, 4, 0 };
    globaltransport_notify = false;
    globaltransport_setupDone = false;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number dspexpr_01_in1;
    number dspexpr_01_in2;
    number dspexpr_01_in3;
    number param_01_value;
    number param_02_value;
    number expr_01_in1;
    number expr_01_in2;
    number expr_01_out1;
    number param_03_value;
    number param_04_value;
    number param_05_value;
    number param_06_value;
    number gen_01_in1;
    number gen_01_in2;
    number gen_01_mix;
    number gen_01_delayCom;
    number gen_01_delayAll;
    number dspexpr_02_in1;
    number dspexpr_02_in2;
    number gen_02_in1;
    number gen_02_in2;
    number gen_02_in3;
    number gen_02_amp15;
    number gen_02_amp14;
    number gen_02_amp13;
    number gen_02_amp12;
    number gen_02_amp11;
    number gen_02_amp10;
    number gen_02_amp9;
    number gen_02_amp8;
    number gen_02_amp7;
    number gen_02_amp6;
    number gen_02_amp5;
    number gen_02_amp4;
    number gen_02_amp3;
    number gen_02_amp2;
    number gen_02_amp1;
    number gen_02_stereoMode;
    number gen_02_modPanFreq;
    number gen_02_modAmp;
    number gen_02_modFreq;
    number gen_02_freqScale;
    number gen_02_tuning;
    number gen_02_equalTemperament;
    number gen_02_equalTemperamentMode;
    number gen_02_ampSmooth;
    number gen_02_freqSmooth;
    number gen_02_binSmooth;
    number fftstream_tilde_01_realIn;
    number fftstream_tilde_01_imagIn;
    number data_01_sizeout;
    number data_01_size;
    number data_01_sizems;
    number data_01_normalize;
    number data_01_channels;
    number dspexpr_03_in1;
    number dspexpr_03_in2;
    number dspexpr_03_in3;
    number param_07_value;
    number param_08_value;
    number param_09_value;
    number param_10_value;
    number param_11_value;
    number param_12_value;
    number param_13_value;
    number param_14_value;
    number param_15_value;
    number param_16_value;
    number param_17_value;
    number param_18_value;
    number param_19_value;
    number param_20_value;
    number param_21_value;
    number param_22_value;
    number param_23_value;
    number param_24_value;
    number param_25_value;
    number param_26_value;
    number param_27_value;
    number param_28_value;
    number param_29_value;
    number param_30_value;
    MillisecondTime _currentTime;
    SampleIndex audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    SampleValue * signals[5];
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    number param_01_lastValue;
    number param_02_lastValue;
    number param_03_lastValue;
    number param_04_lastValue;
    number param_05_lastValue;
    number param_06_lastValue;
    Float64BufferRef gen_01_del_9_buffer;
    Index gen_01_del_9__maxdelay;
    Int gen_01_del_9_sizemode;
    Index gen_01_del_9_wrap;
    Int gen_01_del_9_reader;
    Int gen_01_del_9_writer;
    Float64BufferRef gen_01_del_8_buffer;
    Index gen_01_del_8__maxdelay;
    Int gen_01_del_8_sizemode;
    Index gen_01_del_8_wrap;
    Int gen_01_del_8_reader;
    Int gen_01_del_8_writer;
    Float64BufferRef gen_01_del_7_buffer;
    Index gen_01_del_7__maxdelay;
    Int gen_01_del_7_sizemode;
    Index gen_01_del_7_wrap;
    Int gen_01_del_7_reader;
    Int gen_01_del_7_writer;
    Float64BufferRef gen_01_del1_buffer;
    Index gen_01_del1__maxdelay;
    Int gen_01_del1_sizemode;
    Index gen_01_del1_wrap;
    Int gen_01_del1_reader;
    Int gen_01_del1_writer;
    Float64BufferRef gen_01_del_6_buffer;
    Index gen_01_del_6__maxdelay;
    Int gen_01_del_6_sizemode;
    Index gen_01_del_6_wrap;
    Int gen_01_del_6_reader;
    Int gen_01_del_6_writer;
    Float64BufferRef gen_01_del_5_buffer;
    Index gen_01_del_5__maxdelay;
    Int gen_01_del_5_sizemode;
    Index gen_01_del_5_wrap;
    Int gen_01_del_5_reader;
    Int gen_01_del_5_writer;
    Float64BufferRef gen_01_del_4_buffer;
    Index gen_01_del_4__maxdelay;
    Int gen_01_del_4_sizemode;
    Index gen_01_del_4_wrap;
    Int gen_01_del_4_reader;
    Int gen_01_del_4_writer;
    Float64BufferRef gen_01_del_3_buffer;
    Index gen_01_del_3__maxdelay;
    Int gen_01_del_3_sizemode;
    Index gen_01_del_3_wrap;
    Int gen_01_del_3_reader;
    Int gen_01_del_3_writer;
    Float64BufferRef gen_01_del_2_buffer;
    Index gen_01_del_2__maxdelay;
    Int gen_01_del_2_sizemode;
    Index gen_01_del_2_wrap;
    Int gen_01_del_2_reader;
    Int gen_01_del_2_writer;
    Float64BufferRef gen_01_del_1_buffer;
    Index gen_01_del_1__maxdelay;
    Int gen_01_del_1_sizemode;
    Index gen_01_del_1_wrap;
    Int gen_01_del_1_reader;
    Int gen_01_del_1_writer;
    Float64BufferRef gen_01_del3_buffer;
    Index gen_01_del3__maxdelay;
    Int gen_01_del3_sizemode;
    Index gen_01_del3_wrap;
    Int gen_01_del3_reader;
    Int gen_01_del3_writer;
    Float64BufferRef gen_01_del2_buffer;
    Index gen_01_del2__maxdelay;
    Int gen_01_del2_sizemode;
    Index gen_01_del2_wrap;
    Int gen_01_del2_reader;
    Int gen_01_del2_writer;
    bool gen_01_setupDone;
    Float64BufferRef gen_02_smoothedData;
    Float32BufferRef gen_02_detectionData;
    Float64BufferRef gen_02_phaseData;
    Float64BufferRef gen_02_lastIndex;
    Float64BufferRef gen_02_lastRadius;
    Float64BufferRef gen_02_currentIndex;
    Float64BufferRef gen_02_currentRadius;
    Float64BufferRef gen_02_prev_exprdata_buffer;
    Float64BufferRef gen_02_prev;
    Float64BufferRef gen_02_modPanDel_buffer;
    Index gen_02_modPanDel__maxdelay;
    Int gen_02_modPanDel_sizemode;
    Index gen_02_modPanDel_wrap;
    Int gen_02_modPanDel_reader;
    Int gen_02_modPanDel_writer;
    Float64BufferRef gen_02_modFreqDel_buffer;
    Index gen_02_modFreqDel__maxdelay;
    Int gen_02_modFreqDel_sizemode;
    Index gen_02_modFreqDel_wrap;
    Int gen_02_modFreqDel_reader;
    Int gen_02_modFreqDel_writer;
    Float64BufferRef gen_02_delay15_buffer;
    Index gen_02_delay15__maxdelay;
    Int gen_02_delay15_sizemode;
    Index gen_02_delay15_wrap;
    Int gen_02_delay15_reader;
    Int gen_02_delay15_writer;
    Float64BufferRef gen_02_delay14_buffer;
    Index gen_02_delay14__maxdelay;
    Int gen_02_delay14_sizemode;
    Index gen_02_delay14_wrap;
    Int gen_02_delay14_reader;
    Int gen_02_delay14_writer;
    Float64BufferRef gen_02_delay13_buffer;
    Index gen_02_delay13__maxdelay;
    Int gen_02_delay13_sizemode;
    Index gen_02_delay13_wrap;
    Int gen_02_delay13_reader;
    Int gen_02_delay13_writer;
    Float64BufferRef gen_02_delay12_buffer;
    Index gen_02_delay12__maxdelay;
    Int gen_02_delay12_sizemode;
    Index gen_02_delay12_wrap;
    Int gen_02_delay12_reader;
    Int gen_02_delay12_writer;
    Float64BufferRef gen_02_delay11_buffer;
    Index gen_02_delay11__maxdelay;
    Int gen_02_delay11_sizemode;
    Index gen_02_delay11_wrap;
    Int gen_02_delay11_reader;
    Int gen_02_delay11_writer;
    Float64BufferRef gen_02_delay10_buffer;
    Index gen_02_delay10__maxdelay;
    Int gen_02_delay10_sizemode;
    Index gen_02_delay10_wrap;
    Int gen_02_delay10_reader;
    Int gen_02_delay10_writer;
    Float64BufferRef gen_02_delay9_buffer;
    Index gen_02_delay9__maxdelay;
    Int gen_02_delay9_sizemode;
    Index gen_02_delay9_wrap;
    Int gen_02_delay9_reader;
    Int gen_02_delay9_writer;
    Float64BufferRef gen_02_delay8_buffer;
    Index gen_02_delay8__maxdelay;
    Int gen_02_delay8_sizemode;
    Index gen_02_delay8_wrap;
    Int gen_02_delay8_reader;
    Int gen_02_delay8_writer;
    Float64BufferRef gen_02_delay7_buffer;
    Index gen_02_delay7__maxdelay;
    Int gen_02_delay7_sizemode;
    Index gen_02_delay7_wrap;
    Int gen_02_delay7_reader;
    Int gen_02_delay7_writer;
    Float64BufferRef gen_02_delay6_buffer;
    Index gen_02_delay6__maxdelay;
    Int gen_02_delay6_sizemode;
    Index gen_02_delay6_wrap;
    Int gen_02_delay6_reader;
    Int gen_02_delay6_writer;
    Float64BufferRef gen_02_delay5_buffer;
    Index gen_02_delay5__maxdelay;
    Int gen_02_delay5_sizemode;
    Index gen_02_delay5_wrap;
    Int gen_02_delay5_reader;
    Int gen_02_delay5_writer;
    Float64BufferRef gen_02_delay4_buffer;
    Index gen_02_delay4__maxdelay;
    Int gen_02_delay4_sizemode;
    Index gen_02_delay4_wrap;
    Int gen_02_delay4_reader;
    Int gen_02_delay4_writer;
    Float64BufferRef gen_02_delay3_buffer;
    Index gen_02_delay3__maxdelay;
    Int gen_02_delay3_sizemode;
    Index gen_02_delay3_wrap;
    Int gen_02_delay3_reader;
    Int gen_02_delay3_writer;
    Float64BufferRef gen_02_delay2_buffer;
    Index gen_02_delay2__maxdelay;
    Int gen_02_delay2_sizemode;
    Index gen_02_delay2_wrap;
    Int gen_02_delay2_reader;
    Int gen_02_delay2_writer;
    Float64BufferRef gen_02_delay1_buffer;
    Index gen_02_delay1__maxdelay;
    Int gen_02_delay1_sizemode;
    Index gen_02_delay1_wrap;
    Int gen_02_delay1_reader;
    Int gen_02_delay1_writer;
    number gen_02_latch_21_value;
    number gen_02_cycle_50_ph_currentPhase;
    number gen_02_cycle_50_ph_conv;
    Float64BufferRef gen_02_cycle_50_buffer;
    long gen_02_cycle_50_wrap;
    uint32_t gen_02_cycle_50_phasei;
    SampleValue gen_02_cycle_50_f2i;
    number gen_02_phasor_79_currentPhase;
    number gen_02_phasor_79_conv;
    number gen_02_phasor_85_currentPhase;
    number gen_02_phasor_85_conv;
    number gen_02_phasor_91_currentPhase;
    number gen_02_phasor_91_conv;
    number gen_02_phasor_97_currentPhase;
    number gen_02_phasor_97_conv;
    number gen_02_phasor_103_currentPhase;
    number gen_02_phasor_103_conv;
    number gen_02_phasor_109_currentPhase;
    number gen_02_phasor_109_conv;
    number gen_02_phasor_115_currentPhase;
    number gen_02_phasor_115_conv;
    number gen_02_phasor_121_currentPhase;
    number gen_02_phasor_121_conv;
    number gen_02_phasor_127_currentPhase;
    number gen_02_phasor_127_conv;
    number gen_02_phasor_133_currentPhase;
    number gen_02_phasor_133_conv;
    number gen_02_phasor_139_currentPhase;
    number gen_02_phasor_139_conv;
    number gen_02_phasor_145_currentPhase;
    number gen_02_phasor_145_conv;
    number gen_02_phasor_151_currentPhase;
    number gen_02_phasor_151_conv;
    number gen_02_phasor_157_currentPhase;
    number gen_02_phasor_157_conv;
    number gen_02_phasor_163_currentPhase;
    number gen_02_phasor_163_conv;
    number gen_02_cycle_165_ph_currentPhase;
    number gen_02_cycle_165_ph_conv;
    Float64BufferRef gen_02_cycle_165_buffer;
    long gen_02_cycle_165_wrap;
    uint32_t gen_02_cycle_165_phasei;
    SampleValue gen_02_cycle_165_f2i;
    bool gen_02_setupDone;
    SampleValue fftstream_tilde_01_inWorkspace[2048] = { };
    SampleValue fftstream_tilde_01_outWorkspace[2048] = { };
    Float32BufferRef fftstream_tilde_01_win_buf;
    SampleIndex fftstream_tilde_01_datapos;
    int fftstream_tilde_01_fft_lastsize;
    list fftstream_tilde_01_fft_costab;
    list fftstream_tilde_01_fft_sintab;
    int fftstream_tilde_01_fft_levels;
    bool fftstream_tilde_01_setupDone;
    Float32BufferRef data_01_buffer;
    Int data_01_sizemode;
    bool data_01_setupDone;
    number param_07_lastValue;
    number param_08_lastValue;
    number param_09_lastValue;
    number param_10_lastValue;
    number param_11_lastValue;
    number param_12_lastValue;
    number param_13_lastValue;
    number param_14_lastValue;
    number param_15_lastValue;
    number param_16_lastValue;
    number param_17_lastValue;
    number param_18_lastValue;
    number param_19_lastValue;
    number param_20_lastValue;
    number param_21_lastValue;
    number param_22_lastValue;
    number param_23_lastValue;
    number param_24_lastValue;
    number param_25_lastValue;
    number param_26_lastValue;
    number param_27_lastValue;
    number param_28_lastValue;
    number param_29_lastValue;
    number param_30_lastValue;
    signal globaltransport_tempo;
    bool globaltransport_tempoNeedsReset;
    number globaltransport_lastTempo;
    signal globaltransport_state;
    bool globaltransport_stateNeedsReset;
    number globaltransport_lastState;
    list globaltransport_beatTimeChanges;
    list globaltransport_timeSignatureChanges;
    bool globaltransport_notify;
    bool globaltransport_setupDone;
    number stackprotect_count;
    DataRef gen_01_del_9_bufferobj;
    DataRef gen_01_del_8_bufferobj;
    DataRef gen_01_del_7_bufferobj;
    DataRef gen_01_del1_bufferobj;
    DataRef gen_01_del_6_bufferobj;
    DataRef gen_01_del_5_bufferobj;
    DataRef gen_01_del_4_bufferobj;
    DataRef gen_01_del_3_bufferobj;
    DataRef gen_01_del_2_bufferobj;
    DataRef gen_01_del_1_bufferobj;
    DataRef gen_01_del3_bufferobj;
    DataRef gen_01_del2_bufferobj;
    DataRef smoothedData;
    DataRef ExDetectionData;
    DataRef phaseData;
    DataRef lastIndex;
    DataRef lastRadius;
    DataRef currentIndex;
    DataRef currentRadius;
    DataRef prev;
    DataRef gen_02_modPanDel_bufferobj;
    DataRef gen_02_modFreqDel_bufferobj;
    DataRef gen_02_delay15_bufferobj;
    DataRef gen_02_delay14_bufferobj;
    DataRef gen_02_delay13_bufferobj;
    DataRef gen_02_delay12_bufferobj;
    DataRef gen_02_delay11_bufferobj;
    DataRef gen_02_delay10_bufferobj;
    DataRef gen_02_delay9_bufferobj;
    DataRef gen_02_delay8_bufferobj;
    DataRef gen_02_delay7_bufferobj;
    DataRef gen_02_delay6_bufferobj;
    DataRef gen_02_delay5_bufferobj;
    DataRef gen_02_delay4_bufferobj;
    DataRef gen_02_delay3_bufferobj;
    DataRef gen_02_delay2_bufferobj;
    DataRef gen_02_delay1_bufferobj;
    DataRef RNBODefaultSinus;
    DataRef RNBODefaultFftWindow;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* creaternbomatic()
{
    return new rnbomatic();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr rnbomaticFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return creaternbomatic;
}

} // end RNBO namespace

