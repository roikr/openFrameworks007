/*
 *  Constants.h
 *  SingingCard
 *
 *  Created by Roee Kremer on 12/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#define VIDEO_BITRATE 700.0
#define SONG_SWITCH_DELAY 1000

enum {
	SONG_IDLE,
	SONG_PLAY,
	SONG_RENDER_AUDIO,
	SONG_RENDER_AUDIO_FINISHED,
	SONG_CANCEL_RENDER_AUDIO,
	SONG_RENDER_VIDEO,
	SONG_EXPORT_RINGTONE
};

enum {
	STATE_LIVE,
	STATE_RECORD,
	STATE_PLAY
};
	