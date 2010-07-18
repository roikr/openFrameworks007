
#include "testApp.h"


#define	BUFFER_LEN		(1 << 16)

static const char *
format_duration_str (double seconds)
{	static char str [128] ;
	int hrs, min ;
	double sec ;
	
	memset (str, 0, sizeof (str)) ;
	
	hrs = (int) (seconds / 3600.0) ;
	min = (int) ((seconds - (hrs * 3600.0)) / 60.0) ;
	sec = seconds - (hrs * 3600.0) - (min * 60.0) ;
	
	snprintf (str, sizeof (str) - 1, "%02d:%02d:%06.3f", hrs, min, sec) ;
	
	return str ;
} /* format_duration_str */


static double total_seconds = 0.0 ;

static const char *
generate_duration_str (SF_INFO *sfinfo)
{
	double seconds ;
	
	if (sfinfo->samplerate < 1)
		return NULL ;
	
	if (sfinfo->frames / sfinfo->samplerate > 0x7FFFFFFF)
		return "unknown" ;
	
	seconds = (1.0 * sfinfo->frames) / sfinfo->samplerate ;
	
	/* Accumulate the total of all known file durations */
	total_seconds += seconds ;
	
	return format_duration_str (seconds) ;
} /* generate_duration_str */

static double
get_signal_max (SNDFILE *file)
{	double	max ;
	
	sf_command (file, SFC_CALC_SIGNAL_MAX, &max, sizeof (max)) ;
	
	return max ;
} /* get_signal_max */

static double
calc_decibels (SF_INFO * sfinfo, double max)
{	double decibels ;
	
	switch (sfinfo->format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_U8 :
		case SF_FORMAT_PCM_S8 :
			decibels = max / 0x80 ;
			break ;
			
		case SF_FORMAT_PCM_16 :
			decibels = max / 0x8000 ;
			break ;
			
		case SF_FORMAT_PCM_24 :
			decibels = max / 0x800000 ;
			break ;
			
		case SF_FORMAT_PCM_32 :
			decibels = max / 0x80000000 ;
			break ;
			
		case SF_FORMAT_FLOAT :
		case SF_FORMAT_DOUBLE :
			decibels = max / 1.0 ;
			break ;
			
		default :
			decibels = max / 0x8000 ;
			break ;
	} ;
	
	return 20.0 * log10 (decibels) ;
} /* calc_decibels */


static double	data [BUFFER_LEN] ;
static void
info_dump (const char *filename)
{	static	char	strbuffer [BUFFER_LEN] ;
	SNDFILE	 	*file ;
	SF_INFO	 	sfinfo ;
	double		signal_max, decibels ;
	
	memset (&sfinfo, 0, sizeof (sfinfo)) ;
	
	if ((file = sf_open (filename, SFM_READ, &sfinfo)) == NULL)
	{	printf ("Error : Not able to open input file %s.\n", filename) ;
		fflush (stdout) ;
		memset (data, 0, sizeof (data)) ;
		sf_command (file, SFC_GET_LOG_INFO, strbuffer, BUFFER_LEN) ;
		puts (strbuffer) ;
		puts (sf_strerror (NULL)) ;
		return ;
	} ;
	
	printf ("========================================\n") ;
	sf_command (file, SFC_GET_LOG_INFO, strbuffer, BUFFER_LEN) ;
	puts (strbuffer) ;
	printf ("----------------------------------------\n") ;
	
	printf ("Sample Rate : %d\n", sfinfo.samplerate) ;
	//printf ("Frames      : %" PRId64 "\n", sfinfo.frames) ;
	printf ("Channels    : %d\n", sfinfo.channels) ;
	printf ("Format      : 0x%08X\n", sfinfo.format) ;
	printf ("Sections    : %d\n", sfinfo.sections) ;
	printf ("Seekable    : %s\n", (sfinfo.seekable ? "TRUE" : "FALSE")) ;
	printf ("Duration    : %s\n", generate_duration_str (&sfinfo)) ;
	
	if (sfinfo.frames < 100 * 1024 * 1024)
	{	/* Do not use sf_signal_max because it doesn't work for non-seekable files . */
		signal_max = get_signal_max (file) ;
		decibels = calc_decibels (&sfinfo, signal_max) ;
		printf ("Signal Max  : %g (%4.2f dB)\n", signal_max, decibels) ;
	} ;
	putchar ('\n') ;
	
	sf_close (file) ;
	
} 
//--------------------------------------------------------------
void testApp::setup(){	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	// touch events will be sent to testApp
	ofxMultiTouch.addListener(this);
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	ofBackground(255,255,255);
	
	pan = 0.5f;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.5f;
	bNoise 				= false;
	lAudio = new float[256];
	rAudio = new float[256];
	
	targetFrequency = 2000.0f;
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
	
	ofSoundStreamSetup(2,0,this, sampleRate,BUFFER_LENGTH, 4);
	
	ofSetFrameRate(60);
	
	string filename = ofToDataPath("VOC_BOY_1.aif");
	
	//info_dump(filename.c_str());
	
	
	len=BUFFER_LENGTH;
	sf = NULL;
	memset (&sfinfo, 0, sizeof (SF_INFO)) ;
	sf = sf_open (filename.c_str(), SFM_READ, &sfinfo) ;
	if (sf == NULL)
	{	printf ("Error opening %s.\n", filename.c_str()) ;
		//retval = 1 ;
		//goto out ;
	} ;
	
	
	
	
}


//--------------------------------------------------------------
void testApp::update() {
	
}

//--------------------------------------------------------------
void testApp::draw() {

}

void testApp::exit() {
}


//--------------------------------------------------------------
void testApp::touchDown(float x, float y, int touchId, ofxMultiTouchCustomData *data){
}

//--------------------------------------------------------------
void testApp::touchMoved(float x, float y, int touchId, ofxMultiTouchCustomData *data){
}

//--------------------------------------------------------------
void testApp::touchUp(float x, float y, int touchId, ofxMultiTouchCustomData *data){
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(float x, float y, int touchId, ofxMultiTouchCustomData *data){
}

//--------------------------------------------------------------
void testApp::lostFocus() {
}

//--------------------------------------------------------------
void testApp::gotFocus() {
}

//--------------------------------------------------------------
void testApp::gotMemoryWarning() {
}

void testApp::audioRequested 	(float * output, int bufferSize, int nChannels){
	
	sf_count_t nread ;
	
	
	nread = sf_readf_double (sf, buf, len) ;
	cout<<nread<<endl;
	//
	float leftScale = 1 - pan;
	float rightScale = pan;
	
	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}
	
	phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
	for (int i = 0; i < bufferSize; i++){
		phase += phaseAdder;
		float sample = sin(phase);
		//lAudio[i] = output[i*nChannels    ] = sample * volume * leftScale;
		//rAudio[i] = output[i*nChannels + 1] = sample * volume * rightScale;
		lAudio[i] = output[i*nChannels    ] = buf[i];
		rAudio[i] = output[i*nChannels + 1] = buf[i];
	}
	
	
}