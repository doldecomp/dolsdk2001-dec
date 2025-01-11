#include <dolphin/axart.h>
#include <dolphin/os.h>
#include <dolphin/mix.h>
#include "macros.h"

static AXART_SOUND* __AXARTSoundList;

void AXARTInit(void) {
    __AXARTSoundList = 0;
    AXARTSet3DDistanceScale(40.0f);
    AXARTSet3DDopplerScale(20.0f);
}

void AXARTQuit(void) {
    BOOL old;
    AXART_SOUND* sound;

    old = OSDisableInterrupts();

    for (sound = __AXARTSoundList; sound != 0; sound = (AXART_SOUND*)sound->next) {
        MIXReleaseChannel(sound->axvpb);
    }

    __AXARTSoundList = 0;
    OSRestoreInterrupts(old);
}

void AXARTServiceSounds(void) {
    AXART_SOUND* sound;

    for (sound = __AXARTSoundList; sound != 0; sound = (AXART_SOUND*)sound->next) {
        AXARTServiceSound(sound);
    }
}

void AXARTAddSound(AXART_SOUND* sound) {
    BOOL old;

    ASSERTLINE(125, sound);
    ASSERTLINE(126, sound->axvpb);

    AXSetVoiceItdOn(sound->axvpb);
    MIXInitChannel(sound->axvpb, 0, -904, -904, -904, 64, 64, 0);
    old = OSDisableInterrupts();

    if (__AXARTSoundList != 0) {
        __AXARTSoundList->prev = sound;
        sound->next = __AXARTSoundList;
    } else {
        sound->next = 0;
    }

    sound->prev = 0;
    __AXARTSoundList = sound;
    OSRestoreInterrupts(old);
}

void AXARTRemoveSound(AXART_SOUND* sound) {
    BOOL old;

    ASSERTLINE(176, sound);

    old = OSDisableInterrupts();

    if (sound == __AXARTSoundList) {
        __AXARTSoundList = sound->next;
        if (__AXARTSoundList != 0) {
            __AXARTSoundList->prev = 0;
        }
    } else {
        AXART_SOUND* prev = sound->prev;
        AXART_SOUND* next = sound->next;

        prev->next = next;
        if (next != 0) {
            next->prev = prev;
        }
    }

    OSRestoreInterrupts(old);
    MIXReleaseChannel(sound->axvpb);
}

void AXARTInitLfo(AXART_LFO* lfo, f32* samples, u32 length, f32 delta) {
    ASSERTLINE(224, samples);
    ASSERTLINE(225, length);

    lfo->lfo = samples;
    lfo->length = length;
    lfo->delta = delta;
    lfo->sampleIndex = 0;
    lfo->counter = lfo->sample1 = lfo->sample = lfo->output = 0.0f;
}

void AXARTInitArt3D(AXART_3D* articulator) {
    ASSERTLINE(253, articulator);
    
    articulator->art.type = AXART_TYPE_3D;
    articulator->hAngle = articulator->vAngle = articulator->dist = articulator->closingSpeed = articulator->update = 0.0f;
    articulator->pan = 64;
    articulator->span = 127;
    articulator->src = 1;
    articulator->itdL = articulator->itdR = 0;
    articulator->pitch = 1.0f;
    articulator->attenuation = -0x03C00000;
}

void AXARTInitArtPanning(AXART_PANNING* articulator) {
    ASSERTLINE(288, articulator);
    
    articulator->art.type = AXART_TYPE_PANNING;
    articulator->pan = 64;
    articulator->span = 127;
}

void AXARTInitArtItd(AXART_ITD* articulator) {
    ASSERTLINE(310, articulator);

    articulator->art.type = AXART_TYPE_ITD;
    articulator->itdL = articulator->itdR = 0;
}

void AXARTInitArtSrctype(AXART_SRC* articulator) {
    ASSERTLINE(333, articulator);

    articulator->art.type = AXART_TYPE_SRC;
    articulator->src = 1;
}

void AXARTInitArtPitch(AXART_PITCH* articulator) {
    ASSERTLINE(354, articulator);

    articulator->art.type = AXART_TYPE_PITCH;
    articulator->cents = 0;
}

void AXARTInitArtPitchEnv(AXART_PITCH_ENV* articulator) {
    ASSERTLINE(376, articulator);

    articulator->art.type = AXART_TYPE_PITCH_ENV;
    articulator->delta = articulator->target = articulator->cents = 0;
}

void AXARTInitArtPitchMod(AXART_PITCH_MOD* articulator) {
    ASSERTLINE(401, articulator);

    articulator->art.type = AXART_TYPE_PITCH_MOD;
    articulator->cents = 0;
    AXARTInitLfo(&articulator->lfo, AXARTSine, AXART_SINE_CNT, 0.0f);
}

void AXARTInitArtVolume(AXART_VOLUME* articulator) {
    ASSERTLINE(424, articulator);

    articulator->art.type = AXART_TYPE_VOLUME;
    articulator->attenuation = 0;
}

void AXARTInitArtAuxAVolume(AXART_AUXA_VOLUME* articulator) {
    ASSERTLINE(446, articulator);

    articulator->art.type = AXART_TYPE_AUX_A_VOLUME;
    articulator->attenuation = 0;
}

void AXARTInitArtAuxBVolume(AXART_AUXB_VOLUME* articulator) {
    ASSERTLINE(468, articulator);

    articulator->art.type = AXART_TYPE_AUX_B_VOLUME;
    articulator->attenuation = 0;
}

void AXARTInitArtVolumeEnv(AXART_VOLUME_ENV* articulator) {
    ASSERTLINE(490, articulator);

    articulator->art.type = AXART_TYPE_VOLUME_ENV;
    articulator->delta = articulator->target = articulator->attenuation = 0;
}

void AXARTInitArtAuxAVolumeEnv(AXART_AUXA_VOLUME_ENV* articulator) {
    ASSERTLINE(514, articulator);

    articulator->art.type = AXART_TYPE_AUX_A_VOLUME_ENV;
    articulator->delta = articulator->target = articulator->attenuation = 0;
}

void AXARTInitArtAuxBVolumeEnv(AXART_AUXB_VOLUME_ENV* articulator) {
    ASSERTLINE(538, articulator);

    articulator->art.type = AXART_TYPE_AUX_B_VOLUME_ENV;
    articulator->delta = articulator->target = articulator->attenuation = 0;
}

void AXARTInitArtVolumeMod(AXART_VOLUME_MOD* articulator) {
    ASSERTLINE(563, articulator);

    articulator->art.type = AXART_TYPE_VOLUME_MOD;
    articulator->attenuation = 0;
    AXARTInitLfo(&articulator->lfo, AXARTSine, AXART_SINE_CNT, 0.0f);
}

void AXARTInitArtAuxAVolumeMod(AXART_AUXA_VOLUME_MOD* articulator) {
    ASSERTLINE(588, articulator);

    articulator->art.type = AXART_TYPE_AUX_A_VOLUME_MOD;
    articulator->attenuation = 0;
    AXARTInitLfo(&articulator->lfo, AXARTSine, AXART_SINE_CNT, 0.0f);
}

void AXARTInitArtAuxBVolumeMod(AXART_AUXB_VOLUME_MOD* articulator) {
    ASSERTLINE(613, articulator);

    articulator->art.type = AXART_TYPE_AUX_B_VOLUME_MOD;
    articulator->attenuation = 0;
    AXARTInitLfo(&articulator->lfo, AXARTSine, AXART_SINE_CNT, 0.0f);
}
