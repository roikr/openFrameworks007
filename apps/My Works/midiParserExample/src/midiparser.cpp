/*************************************************************************
 * MIDI parser
 *
 * Copyright (c) 2007 Jari Komppa
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *  1. The origin of this software must not be misrepresented; you must not
 *  claim that you wrote the original software. If you use this software
 *  in a product, an acknowledgment in the product documentation would be
 *  appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be
 *  misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source
 *  distribution.
 *
 *************************************************************************
 *
 * MIDI spec details on
 * http://www.borg.com/~jglatt/tech/miditech.htm
 *
 * - Default BPM is 120
 * - Default time signature is 4:4
 *
 *************************************************************************
 *
 * I wrote this MIDI parser when I did a small softsynth 
 * for an AVR microcontroller.. This parser itself did not
 * run on the AVR; instead, I took the data that I wanted 
 * (basically just 'note on' events) from the midi and used that.
 *
 * Again, this was never really meant to be released, 
 * but I figured what the hey, someone might find it useful.
 * 
 * Cheers,
 *    Jari
 */

#include "midiparser.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char note[12][3] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

char controller[128][35] = {
"Bank Select (coarse)",
"Modulation Wheel (coarse)",
"Breath controller (coarse)",
"3:unknown",
"Foot Pedal (coarse)",
"Portamento Time (coarse)",
"Data Entry (coarse)",
"Volume (coarse)",
"Balance (coarse)",
"9:unknown",
"Pan position (coarse)",
"Expression (coarse)",
"Effect Control 1 (coarse)",
"Effect Control 2 (coarse)",
"14:unknown",
"15:unknown",
"General Purpose Slider 1",
"General Purpose Slider 2",
"General Purpose Slider 3",
"General Purpose Slider 4",
"20:unknown",
"21:unknown",
"22:unknown",
"23:unknown",
"24:unknown",
"25:unknown",
"26:unknown",
"27:unknown",
"28:unknown",
"29:unknown",
"30:unknown",
"31:unknown",
"Bank Select (fine)",
"Modulation Wheel (fine)",
"Breath controller (fine)",
"35:unknown",
"Foot Pedal (fine)",
"Portamento Time (fine)",
"Data Entry (fine)",
"Volume (fine)",
"Balance (fine)",
"41:unknown",
"Pan position (fine)",
"Expression (fine)",
"Effect Control 1 (fine)",
"Effect Control 2 (fine)",
"46:unknown",
"47:unknown",
"48:unknown",
"49:unknown",
"50:unknown",
"51:unknown",
"52:unknown",
"53:unknown",
"54:unknown",
"55:unknown",
"56:unknown",
"57:unknown",
"58:unknown",
"59:unknown",
"60:unknown",
"61:unknown",
"62:unknown",
"63:unknown",
"Hold Pedal (on/off)",
"Portamento (on/off)",
"Sustenuto Pedal (on/off)",
"Soft Pedal (on/off)",
"Legato Pedal (on/off)",
"Hold 2 Pedal (on/off)",
"Sound Variation",
"Sound Timbre",
"Sound Release Time",
"Sound Attack Time",
"Sound Brightness",
"Sound Control 6",
"Sound Control 7",
"Sound Control 8",
"Sound Control 9",
"Sound Control 10",
"General Purpose Button 1 (on/off)",
"General Purpose Button 2 (on/off)",
"General Purpose Button 3 (on/off)",
"General Purpose Button 4 (on/off)",
"84:unknown",
"85:unknown",
"86:unknown",
"87:unknown",
"88:unknown",
"89:unknown",
"90:unknown",
"Effects Level",
"Tremulo Level",
"Chorus Level",
"Celeste Level",
"Phaser Level",
"Data Button increment",
"Data Button decrement",
"Non-registered Parameter (fine)",
"Non-registered Parameter (coarse)",
"Registered Parameter (fine)",
"Registered Parameter (coarse)",
"102:unknown",
"103:unknown",
"104:unknown",
"105:unknown",
"106:unknown",
"107:unknown",
"108:unknown",
"109:unknown",
"110:unknown",
"111:unknown",
"112:unknown",
"113:unknown",
"114:unknown",
"115:unknown",
"116:unknown",
"117:unknown",
"118:unknown",
"119:unknown",
"All Sound Off",
"All Controllers Off",
"Local Keyboard (on/off)",
"All Notes Off",
"Omni Mode Off",
"Omni Mode On",
"Mono Operation",
"Poly Operation"};

#define SWAPDWORD(a) ((((a) & 0x000000ff) << 24) | \
                      (((a) & 0x0000ff00) << 8 ) | \
                      (((a) & 0x00ff0000) >> 8 ) | \
                      (((a) & 0xff000000) >> 24))

#define SWAPWORD(a) ((((a) & 0xff00) >> 8) | (((a) & 0xff) << 8))


// Read variable-length integer from stream
int readvar(FILE * f) 
{
	int d;
	d = getc(f);
	if (d & 0x80)
	{
		d &= 0x7f;
		int v;
		do
		{
			v = getc(f);
			d = (d << 7) + (v & 0x7f);
		}
		while (v & 0x80);
	}
	return d;
}

// Read doubleword from stream
int readdword(FILE * f)
{
	int d;
	fread(&d,4,1,f);
	d = SWAPDWORD(d);
	return d;
}

// Read word from stream
int readword(FILE * f)
{
	short int d;
	fread(&d,2,1,f);
	d = SWAPWORD(d);
	return d;
}

// Load chunk header
int loadchunkheader(FILE * f, int &length)
{
	int id;
	id = readdword(f);
	length = readdword(f);
	return id;
}

// Parse MIDI file
int parsemidi(string filename)
{
	FILE * f = fopen(filename.c_str(),"rb");
	if (!f) return -1;
	int len;
	int id = loadchunkheader(f,len);
	printf("%08x %d\n",id,len);
	if (id != 'MThd')
	{
		printf("Bad header id\n");
		return -1;
	}
	if (len < 6)
	{
		printf("Bad header block length\n");
		return -1;
	}
	int format = readword(f);
	printf("format %d\n", format);
	if (format != 1 && format != 0)
	{
		printf("Unsupported format\n");
		return -1;
	}
	int tracks = readword(f);
	printf("tracks %d\n", tracks);
	int ppqn = readword(f);
	printf("ppqn %d\n",ppqn); // pulses (clocks) per quater note
	if (ppqn < 0)
	{
		printf("negative ppqn formats not supported\n");
		return -1;
	}
	if (len > 6)
	{
		while (len > 6)
		{
			fgetc(f);
			len--;
		}
	}
	
	int uspertick = (500000 / ppqn);
	while (!feof(f) && tracks)
	{
		id = loadchunkheader(f,len);
		if (id != 'MTrk')
		{
			printf("Unknown chunk\n");
			return -1;
		}
		printf("\nNew track, length %d\n",len);
		int trackend = 0;
		int command = 0;
		int time = 0;
		while (!trackend)
		{
			int dtime = readvar(f);
			time += dtime;
			printf("%3.3f ",((float)time * (float)uspertick)/1000000.0f);
			int data1 = fgetc(f);
			if (data1 == 0xff)
			{
				data1 = fgetc(f); // sub-command
				int len = readvar(f);
				switch (data1)
				{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
					switch (data1)
					{
					case 1:
						printf("Text:\"");
						break;
					case 2:
						printf("Copyright:\"");
						break;
					case 3:
						printf("Track name:\"");
						break;
					case 4:
						printf("Instrument:\"");
						break;
					case 5:
						printf("Lyric:\"");
						break;
					case 6:
						printf("Marker:\"");
						break;
					case 7:
						printf("Cue point:\"");
						break;
					case 8:
						printf("Patch name:\"");
						break;
					case 9:
						printf("Port name:\"");
						break;
					}
					while (len)
					{
						printf("%c",fgetc(f));
						len--;
					}
					printf("\"\n");
					break;
				case 0x2f:
					{
						trackend = 1;
						printf("Track end\n");
					}
					break;
				case 0x58: // time signature
					{
						int nn = fgetc(f);
						int dd = fgetc(f);
						int cc = fgetc(f);
						int bb = fgetc(f);
						printf("Time sig: %d:%d, metronome:%d, quarter:%d\n",nn,dd,cc,bb);
					}
					break;
				case 0x59: // key signature
					{
						int sf = fgetc(f);
						int mi = fgetc(f);
						printf("Key sig: %d %s, %s\n",abs(sf),sf == 0?"c":(sf < 0 ? "flat":"sharp"), mi?"minor":"major");
					}
					break;
				case 0x51: // tempo
					{
						int t = 0;
						t = fgetc(f) << 16;
						t |= fgetc(f) << 8;
						t |= fgetc(f);
						printf("Tempo: quarter is %dus (%3.3fs) long - BPM = %3.3f\n",t,t/1000000.0f, 60000000.0f/t);						
						uspertick = t / ppqn;
					}
					break;
				case 0x21: // obsolete: midi port
					{
						int pp = fgetc(f);
						printf("[obsolete] midi port: %d\n",pp);
					}
					break;
				case 0x20: // obsolete: midi channel
					{
						int cc = fgetc(f);
						printf("[obsolete] midi channel: %d\n",cc);
					}
					break;
				case 0x54: // SMPTE offset
					{
						int hr = fgetc(f);
						int mn = fgetc(f);
						int se = fgetc(f);
						int fr = fgetc(f);
						int ff = fgetc(f);
						printf("SMPTE Offset: %dh %dm %ds %dfr %dff\n",hr,mn,se,fr,ff);
					}
					break;
				case 0x7f: // Proprietary event
					{
						printf("Proprietary event ");
						while (len)
						{
							int d = fgetc(f);
							printf("%02X ",d);
							len--;
						}
						printf("\n");
					}
					break;
				default:
					printf("meta command %02x %d\n", data1, len);
					while (len)
					{
						fgetc(f);
						len--;
					}
				}
			}
			else
			{
				if (data1 & 0x80) // new command?
				{
					command = data1;
					data1 = fgetc(f);
				}

				switch (command & 0xf0)
				{
				case 0x80: // note off
					{
						int data2 = fgetc(f);
						printf("Note off: channel %d, Oct %d Note %s Velocity %d\n",command & 0xf, (data1/12)-1,note[data1%12], data2);
					}
					break;
				case 0x90: // note on
					{
						int data2 = fgetc(f);
						printf("Note on: channel %d, Oct %d Note %s Velocity %d\n",command & 0xf, (data1/12)-1,note[data1%12], data2);
					}
					break;
				case 0xa0: // Note aftertouch
					{
						int data2 = fgetc(f);
						printf("Aftertouch: channel %d, Oct %d, Note %s Aftertouch %d\n",command & 0xf, (data1/12)-1,note[data1%12], data2);
					}
					break;
				case 0xb0: // Controller
					{
						int data2 = fgetc(f);
						printf("Controller: channel %d, Controller %s Value %d\n",command & 0xf, controller[data1], data2);
					}
					break;
				case 0xc0: // program change
					{
						printf("Program change: channel %d, program %d\n",command & 0xf, data1);
					}
					break;
				case 0xd0: // Channel aftertouch
					{
						printf("Channel aftertouch: channel %d, Aftertouch %d\n",command & 0xf, data1);
					}
					break;
				case 0xe0: // Pitch bend
					{
						int data2 = fgetc(f);
						printf("Pitchbend: channel %d, Pitch %d\n",command & 0xf, data1 + (data2 << 7));
					}
					break;
				case 0xf0: // general / immediate
					{
						switch (command)
						{
						case 0xf0: // SysEx
							{
								printf("SysEx ");
								while (data1 != 0xf7)
								{
									printf("%02X ", data1);
									data1 = fgetc(f);
								}
								printf("\n");
								// universal sysexes of note:
								// f0 (05) 7e 7f 09 01 f7 = "general midi enable"
								// f0 (05) 7e 7f 09 00 f7 = "general midi disable"
								// f0 (07) 7f 7f 04 01 ll mm f7 = "master volume", ll mm = 14bit value
								// spec doesn't say that the length byte should be there,
								// but it appears to be (the ones in brackets)
							}
							break;
						case 0xf1: // MTC quater frame
							{
								int dd = fgetc(f);
								printf("MTC quater frame %d\n",dd);
							}
							break;
						case 0xf2: // Song position pointer
							{
								int data1 = fgetc(f);
								int data2 = fgetc(f);
								printf("Song position pointer %d\n", data1 + (data2 << 7));
							}
							break;
						case 0xf3: // Song select
							{
								int song = fgetc(f);
								printf("Song select %d\n", song);
							}
							break;
						case 0xf6: // Tuning request
							printf("Tuning request\n");
							break;
						case 0xf8: // MIDI clock
							printf("MIDI clock\n");
							break;
						case 0xf9: // MIDI Tick
							printf("MIDI Tick\n");
							break;
						case 0xfa: // MIDI start
							printf("MIDI start\n");
							break;
						case 0xfc:
							printf("MIDI stop\n");
							break;
						case 0xfb:
							printf("MIDI continue\n");
							break;
						case 0xfe:
							printf("Active sense\n");
							break;
						case 0xff:
							printf("Reset\n");
							break;

						default:
							{
								printf("Unknown: command 0x%02x, data 0x%02x\n", command, data1);
							}
							break;
						}
					}
					break;
				default:
					{
						printf("Unknown: command 0x%02x, data 0x%02x\n", command, data1);
					}
					break;
				}
			}
		}

		tracks--;
	}
	return 0;
}
