#pragma once
#ifndef ALSACONFIGSPACETOSTRING_H
#define ASLACONFIGSPACETOSTRING_H

#include<string>
#include<alsa/asoundlib.h>

std::string configspace2string(snd_pcm_hw_params_t* conf);
std::string config2string(snd_pcm_hw_params_t* conf);

#endif // !ALSACONFIGSPACETOSTRING_H
