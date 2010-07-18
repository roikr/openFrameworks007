 
////////////////////////////////////////////////////////////////////////
// This file is part of the SndObj library
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
//
// SndCoreAudio.cpp:
// Core Audio Realtime IO implementation
//
//
//#ifdef MACOSX
#include "SndCoreAudio.h"
/*
OSStatus
SndCoreAudio::ADIOProc(AudioDeviceID indev,
                        const AudioTimeStamp * , const AudioBufferList * input,
		        const AudioTimeStamp * , AudioBufferList *output,
			const AudioTimeStamp *, void* p){

  SndCoreAudio *cdata = (SndCoreAudio *)p;  
  int chans = cdata->m_channels;
  int buff = cdata->m_iocurbuff;
  float *ibufp = cdata->m_inbuffs[buff];
  float *obufp = cdata->m_outbuffs[buff];
  int no_input = cdata->m_dont_use_input;
  int no_output = cdata->m_dont_use_output;

  if (input->mNumberBuffers > 1) cdata->m_interleaved = false; 

  if(cdata->m_interleaved) {
    int items = cdata->m_buffitems,i,maxi;
    maxi = input->mBuffers[0].mDataByteSize/sizeof(float);
    output->mNumberBuffers = 1;
    output->mBuffers[0].mDataByteSize = cdata->m_buffsize;
    output->mBuffers[0].mNumberChannels = chans;
    float *outp = (float *) output->mBuffers[0].mData;
    float *inp = (float *) input->mBuffers[0].mData;
    for(i = 0; i < items; i++){
      if(!no_output) 
        outp[i]  = obufp[i];
	if(i < maxi && !no_input) 
        ibufp[i] = inp[i];
      else ibufp[i] = 0.f;
      obufp[i] = 0;
    }
  }
  else {
    int  nibuffs = input->mNumberBuffers, buffs, i, j, cnt;
    int  nobuffs = output->mNumberBuffers;
    int items = cdata->m_bufframes * chans;
    buffs = nibuffs > nobuffs ? nibuffs : nobuffs;
    output->mNumberBuffers = buffs;
    chans = chans > buffs ? buffs : chans;
    float *outp, *inp;
    for (j = 0; j < chans; j++) {
      if(!no_output) 
      outp = (float *) output[0].mBuffers[j].mData;
      if(!no_input) 
      inp = (float *) input[0].mBuffers[j].mData;
      for (i = j, cnt = 0; i < items; i += chans, cnt++) {
	if(!no_output) 
        outp[cnt] = obufp[i];
	obufp[i] = 0.0f;
	if(!no_input) 
        ibufp[i] = inp[cnt];
      }
      output->mBuffers[j].mDataByteSize = input[0].mBuffers[j].mDataByteSize;
      output->mBuffers[j].mNumberChannels = 1;
    }
  }
    
  cdata->m_outused[buff] = cdata->m_inused[buff] = true;      
  buff++;
  if(buff == cdata->m_buffnos) buff=0;
  cdata->m_iocurbuff = buff;
  return 0;
}
*/
void SndCoreAudio::AQBufferCallback(void *					inUserData,
								AudioQueueRef			inAQ,
								AudioQueueBufferRef		inCompleteAQBuffer) 
{
		
	SndCoreAudio *cdata = (SndCoreAudio *)inUserData;  
	int chans = cdata->m_channels;
	int buff = cdata->m_iocurbuff;
	short *ibufp = cdata->m_inbuffs[buff];
	short *obufp = cdata->m_outbuffs[buff];
	int no_input = cdata->m_dont_use_input;
	int no_output = cdata->m_dont_use_output;
	
	
	int items = cdata->m_buffitems,i,maxi;
	maxi = 0; //input->mBuffers[0].mDataByteSize/sizeof(float);
	//output->mNumberBuffers = 1;
	inCompleteAQBuffer->mAudioDataByteSize = cdata->m_buffsize;
	//inCompleteAQBuffer->mNumberChannels = chans;
	short *outp = (short *) inCompleteAQBuffer->mAudioData;
	//float *inp = (float *) input->mBuffers[0].mData;
	for(i = 0; i < items; i++){
		if(!no_output) 
			outp[i]  = obufp[i];
		//if(i < maxi && !no_input) 
//			ibufp[i] = inp[i];
//		else ibufp[i] = 0.f;
		obufp[i] = 0;
	}
	    
	cdata->m_outused[buff] = cdata->m_inused[buff] = true;      
	buff++;
	if(buff == cdata->m_buffnos) buff=0;
	cdata->m_iocurbuff = buff;
	
	OSStatus res = AudioQueueEnqueueBuffer(inAQ, inCompleteAQBuffer, 0, NULL);
	//cout << res << endl;
	
}



/*
int SndCoreAudio::OpenDevice(bool isInput){
  
  UInt32 psize, bufframes;
  double rsr;
  OSStatus err;
  AudioStreamBasicDescription format;
  // set the buffersize
  psize = sizeof(UInt32);
  bufframes = m_bufframes;
  err = AudioDeviceSetProperty(m_dev,NULL,0,isInput,
     			 kAudioDevicePropertyBufferFrameSize,
			 psize, &m_bufframes);
  if(!err)
  err = AudioDeviceGetProperty(m_dev,0,isInput,
			 kAudioDevicePropertyBufferFrameSize,
			 &psize, &bufframes);
 
  if(bufframes != m_bufframes) m_error = 21;
 
  psize = sizeof(double);
  rsr = m_sr;
  err = AudioDeviceSetProperty(m_dev, NULL, 0, isInput,
			 kAudioDevicePropertyNominalSampleRate, psize, &rsr);
  if(!err)
  err = AudioDeviceGetProperty(m_dev, 0, isInput,
			 kAudioDevicePropertyNominalSampleRate, &psize, &rsr); 
 
  if(rsr != m_sr) m_error = 22;
   
  psize = sizeof(AudioStreamBasicDescription);
  AudioDeviceGetProperty(m_dev,0,isInput,      
			 kAudioDevicePropertyStreamFormat,
			 &psize, &format);
  
  m_interleaved = true;
  m_format.mSampleRate = m_sr;
  m_format.mFormatID = kAudioFormatLinearPCM;
  m_format.mFormatFlags = format.mFormatFlags;
  m_format.mBytesPerPacket = sizeof(float)*m_channels;
  m_format.mFramesPerPacket = 1;
  m_format.mBytesPerFrame = format.mBytesPerPacket;
  m_format.mChannelsPerFrame = m_channels;
  m_format.mBitsPerChannel = sizeof(float)*8;

  err = AudioDeviceSetProperty(m_dev,NULL,0,isInput,      
			 kAudioDevicePropertyStreamFormat,
			 psize, &m_format);
  if(!err)
  AudioDeviceGetProperty(m_dev,0,isInput,      
			 kAudioDevicePropertyStreamFormat,
			 &psize, &format);

  //cout << format.mSampleRate << "\n";
  //if(memcmp(m_format, format,psize)!=0) m_error = 25;
  
  return err;
}
*/

int SndCoreAudio::OpenDevice(bool isInput){
	/*
	try {					
		
		UInt32 bufferByteSize;		
		// we need to calculate how many packets we read at a time, and how big a buffer we need
		// we base this on the size of the packets in the file and an approximate duration for each buffer
		// first check to see what the max size of a packet is - if it is bigger
		// than our allocation default size, that needs to become larger
		UInt32 maxPacketSize;
		UInt32 size = sizeof(maxPacketSize);
		XThrowIfError(AudioFileGetProperty(mAudioFile, 
										   kAudioFilePropertyPacketSizeUpperBound, &size, &maxPacketSize), "couldn't get file's max packet size");
		
		// adjust buffer size to represent about a half second of audio based on this format
		CalculateBytesForTime (mDataFormat, maxPacketSize, kBufferDurationSeconds, &bufferByteSize, &mNumPacketsToRead);
		
		//printf ("Buffer Byte Size: %d, Num Packets to Read: %d\n", (int)bufferByteSize, (int)mNumPacketsToRead);
		
		// (2) If the file has a cookie, we should get it and set it on the AQ
		size = sizeof(UInt32);
		OSStatus result = AudioFileGetPropertyInfo (mAudioFile, kAudioFilePropertyMagicCookieData, &size, NULL);
		
		if (!result && size) {
			char* cookie = new char [size];		
			XThrowIfError (AudioFileGetProperty (mAudioFile, kAudioFilePropertyMagicCookieData, &size, cookie), "get cookie from file");
			XThrowIfError (AudioQueueSetProperty(mQueue, kAudioQueueProperty_MagicCookie, cookie, size), "set cookie on queue");
			delete [] cookie;
		}
		
		// channel layout?
		result = AudioFileGetPropertyInfo(mAudioFile, kAudioFilePropertyChannelLayout, &size, NULL);
		if (result == noErr && size > 0) {
			AudioChannelLayout *acl = (AudioChannelLayout *)malloc(size);
			XThrowIfError(AudioFileGetProperty(mAudioFile, kAudioFilePropertyChannelLayout, &size, acl), "get audio file's channel layout");
			XThrowIfError(AudioQueueSetProperty(mQueue, kAudioQueueProperty_ChannelLayout, acl, size), "set channel layout on queue");
			free(acl);
		}
		
		XThrowIfError(AudioQueueAddPropertyListener(mQueue, kAudioQueueProperty_IsRunning, isRunningProc, this), "adding property listener");
		
		bool isFormatVBR = (mDataFormat.mBytesPerPacket == 0 || mDataFormat.mFramesPerPacket == 0);
		for (int i = 0; i < kNumberBuffers; ++i) {
			XThrowIfError(AudioQueueAllocateBufferWithPacketDescriptions(mQueue, bufferByteSize, (isFormatVBR ? mNumPacketsToRead : 0), &mBuffers[i]), "AudioQueueAllocateBuffer failed");
		}	
		
		// set the volume of the queue
		XThrowIfError (AudioQueueSetParameter(mQueue, kAudioQueueParam_Volume, 1.0), "set queue volume");
		
		mIsInitialized = true;	
	}
	*/
	
	
	
	
	UInt32 psize, bufframes;
	double rsr;
	OSStatus err;
	
	try {
		//CAStreamBasicDescription format;
		
		/*
		// set the buffersize
		psize = sizeof(UInt32);
		bufframes = m_bufframes;
		
		
		err = AudioDeviceSetProperty(m_dev,NULL,0,isInput,
									 kAudioDevicePropertyBufferFrameSize,
									 psize, &m_bufframes);
		if(!err)
			err = AudioDeviceGetProperty(m_dev,0,isInput,
										 kAudioDevicePropertyBufferFrameSize,
										 &psize, &bufframes);
		
		if(bufframes != m_bufframes) m_error = 21;
		 
		
		psize = sizeof(double);
		rsr = m_sr;
		err = AudioDeviceSetProperty(m_dev, NULL, 0, isInput,
									 kAudioDevicePropertyNominalSampleRate, psize, &rsr);
		if(!err)
			err = AudioDeviceGetProperty(m_dev, 0, isInput,
										 kAudioDevicePropertyNominalSampleRate, &psize, &rsr); 
		
		if(rsr != m_sr) m_error = 22;
		
		psize = sizeof(CAStreamBasicDescription);
		AudioDeviceGetProperty(m_dev,0,isInput,      
							   kAudioDevicePropertyStreamFormat,
							   &psize, &format);
		*/
		m_interleaved = true;
		
		memset(&m_format, 0, sizeof(m_format));
		
		m_format.mSampleRate = m_sr;
		m_format.mChannelsPerFrame = m_channels;
		m_format.mFormatID = kAudioFormatLinearPCM;
		m_format.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger ;//| kLinearPCMFormatFlagIsPacked;
		m_format.mBitsPerChannel = 16;
		m_format.mBytesPerPacket = m_format.mBytesPerFrame = (m_format.mBitsPerChannel / 8) * m_format.mChannelsPerFrame;
		m_format.mFramesPerPacket = 1;
		
		
		
	
		//cout << format.mSampleRate << "\n";
		//if(memcmp(m_format, format,psize)!=0) m_error = 25;
		 
		
		//XThrowIfError(err=AudioQueueNewOutput(&m_format, SndCoreAudio::AQBufferCallback, this, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &mQueue), "AudioQueueNew failed");
		XThrowIfError(err=AudioQueueNewOutput(&m_format, SndCoreAudio::AQBufferCallback, this, NULL, NULL, 0, &mQueue), "AudioQueueNew failed");
		
		
		//AudioQueueNewOutput (&((*(buffers.begin()))->mDataFormat), HandleOutputBuffer, que, NULL,NULL, 0, &(que->mQueue) ); 
		
		UInt32 bufferByteSize = m_buffsize;
		
		for (int i = 0; i < kNumberBuffers; ++i) {
			XThrowIfError(AudioQueueAllocateBufferWithPacketDescriptions(mQueue, bufferByteSize,  0, &mBuffers[i]), "AudioQueueAllocateBuffer failed");
		}	
	}
	catch (CAXException e) {
		char buf[256];
		fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
	}
	
	return err;
}


SndCoreAudio::SndCoreAudio(int channels,int bufframes, int buffnos, float norm, SndObj** inObjs, 
			   int dev,  int vecsize, float sr):
  SndIO((channels < 2 ?  2 : channels), 16, inObjs, vecsize, sr) {
                  
  UInt32 psize;
  int i;
  m_norm = norm ? norm : 1.f;

  m_called_read = false;  
  m_stopped = true;

/*
  if(dev==DEF_DEV){
    psize = sizeof(AudioDeviceID);
    AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,
			     &psize, &m_dev);
  }
  else m_dev = dev;
  */
 
  m_sleept = 5;
  m_bufframes = bufframes;
  m_buffsize = bufframes*sizeof(short)*m_channels;
  m_buffitems = bufframes*m_channels;
  m_buffnos = buffnos;

	  m_dont_use_input = true; //OpenDevice();
  m_dont_use_output = OpenDevice(false);
  
 
  m_outbuffs = new short*[m_buffnos];
  m_inbuffs = new short*[m_buffnos];
  m_inused = new bool[m_buffnos];
  m_outused = new bool[m_buffnos];

  for(i=0; i < m_buffnos; i++){

    if(!(m_inbuffs[i] = new short[m_bufframes*m_channels])){
      m_error = 24;
      return;
    }
    if(!(m_outbuffs[i] = new short[m_bufframes*m_channels])){
      m_error = 25;
      return;
    }
    memset(m_outbuffs[i], 0, m_bufframes*m_channels*sizeof(short));
    memset(m_inbuffs[i], 0, m_bufframes*m_channels*sizeof(short));
    m_inused[i] = m_outused[i] = false;
  }

  m_incurbuff = m_outcurbuff = m_iocurbuff = 0;
  m_incount = m_outcount = m_buffitems;
	  
 /*

  if(AudioDeviceAddIOProc(m_dev, SndObj_IOProcEntry, this) != 0) {
    m_error = 26;
    return;
  }
  */
  
}
    

SndCoreAudio::~SndCoreAudio(){

 // AudioDeviceStop(m_dev, SndObj_IOProcEntry);
  //AudioDeviceRemoveIOProc(m_dev, SndObj_IOProcEntry);

	OSStatus result = AudioQueueStop(mQueue, true);
	if (result) printf("ERROR STOPPING QUEUE!\n");
	
  delete[] m_outbuffs;
  delete[] m_inbuffs;
  delete[] m_inused;
  delete[] m_outused;

}    


short
SndCoreAudio::Write(){
	if(!m_error){
		int i;
		if(m_stopped){
			//AudioDeviceStart(m_dev, SndObj_IOProcEntry);
			// prime the queue with some data before starting
			for (int i = 0; i < kNumberBuffers; ++i) {
				AQBufferCallback (this, mQueue, mBuffers[i]);			
			}
			OSStatus res = AudioQueueStart(mQueue, NULL);
			m_stopped = false;
		}
		//if(!m_called_read) Read(); // for input
		for(m_vecpos = 0; m_vecpos < m_vecsize; m_vecpos++) {
			if(m_outcount == m_buffitems){           
				m_outused[m_outcurbuff] = false;  
				m_outcurbuff++;
				if(m_outcurbuff == m_buffnos) 
					m_outcurbuff=0;
				m_outcount = 0; 
				while(!m_outused[m_outcurbuff]) usleep(m_sleept);
			}
			for(i = 0; i < m_channels; i++){
				m_outbuffs[m_outcurbuff][m_outcount+i] = (m_IOobjs[i] ? (short)m_IOobjs[i]->Output(m_vecpos): 0);
			} 
			m_outcount+=m_channels;
		  
		} // for
		m_called_read = false;
		return 1;
	} // if no error
	return 0;
}    

/*SndRTIO::Writes(){
 
 if(m_count == m_items){
 write(m_dev, m_sp, m_buffsize);
 m_count = 0;
 for(int n = 0; n < m_channels; n++)
 if(m_IOobjs[n])
 m_sp[n+m_count] = (short) m_IOobjs[n]->Output(m_vecpos);
 }
 else
 for(int n = 0; n < m_channels; n++)
 if(m_IOobjs[n])
 m_sp[n+m_count] = (short) m_IOobjs[n]->Output(m_vecpos);
 
 }
*/ 


short
SndCoreAudio::Read(){

  if(!m_error){
    if(m_stopped){
      //if(AudioDeviceStart(m_dev, SndObj_IOProcEntry)==0)
		if(AudioQueueStart(mQueue, NULL)==0) 
			m_stopped = false;
      else {
	m_error = 26;
        return 0;
      }
    }
    for(m_vecpos = 0; m_vecpos < m_vecsize*m_channels; 
	m_vecpos++){
      if(m_incount == m_buffitems){           
	m_inused[m_incurbuff] = false;  
	m_incurbuff++;
	if(m_incurbuff == m_buffnos) m_incurbuff = 0;
	m_incount = 0;
	//while(!m_inused[m_incurbuff]) usleep(m_sleept);
      }
      m_output[m_vecpos] = m_inbuffs[m_incurbuff][m_incount]*m_norm;
      m_inbuffs[m_incurbuff][m_incount] = 0.f;
      m_incount++;
      
    } // for
    m_called_read = true;
    return 1;
  } // if no error
  return 0;
}

char* SndCoreAudio::ErrorMessage(){

  char* mess;

  switch(m_error){
  case 21:
    mess="cannot set the requested buffer size \n";
    break;
  case 22:
    mess="error setting the sampling rate \n";
    break;
  case 23:
    mess="error setting the number of channels \n";
    break;
  case 24:
    mess="error allocating memory for input\n";
    break;
  case 25:
    mess="error allocating memory for output\n";
    break;
  case 26:
    mess="can't start device";
    break;

  default:
    return SndIO::ErrorMessage();
  }
  return mess;

}

/*
int ListDevices(char **devs, int devnos){
  UInt32 psize;
  AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &psize, NULL);
  UInt32 *list = new UInt32[psize/4];
  AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &psize, list);
  
  for(int i=0; i < psize/4 && i < devnos; i++){
  AudioDeviceGetPropertyInfo(list[i], 0, 0, kAudioDevicePropertyDeviceName, &psize, NULL);
    char *name = new char[psize];
    char *name2 = new char[psize+10];
    sprintf(name2, "%u: ", list[i]); 
    AudioDeviceGetProperty(list[i], 0, 0, kAudioDevicePropertyDeviceName, &psize, name);
    strcat(name2, name);
    devs[i] = name2;
    delete[] name;
  }
  return psize/4;
}

void PrintDevices(){
  char *devs[50];
  int devn = ListDevices(devs);
  for(int i=0; i < devn; i++) { 
    cout << devs[i] << "\n";
    delete[] devs[i];
  }
  
}
*/

//#endif
