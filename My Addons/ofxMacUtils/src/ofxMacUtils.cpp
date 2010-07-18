/*
 *  ofxMacUtils.cpp
 *  emptyExample
 *
 *  Created by Roee Kremer on 6/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>
using namespace std;

#include "ofxMacUtils.h"

CFURLRef GetOpenFileFromUser(void)
{
	NavDialogCreationOptions dialogOptions;
	NavDialogRef dialog;
	NavReplyRecord replyRecord;
	CFURLRef fileAsCFURLRef = NULL;
	FSRef fileAsFSRef;
	OSStatus status;
	
	// Get the standard set of defaults
	status = NavGetDefaultDialogCreationOptions(&dialogOptions);
	require_noerr( status, CantGetNavOptions );
	
	// Make the window app-wide modal
	dialogOptions.modality = kWindowModalityAppModal;
	
	// Create the dialog
	status = NavCreateGetFileDialog(&dialogOptions, NULL, NULL, NULL, NULL, NULL, &dialog);
	require_noerr( status, CantCreateDialog );
	
	// Show it
	status = NavDialogRun(dialog);
	require_noerr( status, CantRunDialog );
	
	// Get the reply
	status = NavDialogGetReply(dialog, &replyRecord);
	require( ((status == noErr) || (status == userCanceledErr)), CantGetReply );
	
	// If the user clicked "Cancel", just bail
	if ( status == userCanceledErr ) goto UserCanceled;
	
	// Get the file
	status = AEGetNthPtr(&(replyRecord.selection), 1, typeFSRef, NULL, NULL, &fileAsFSRef, sizeof(FSRef), NULL);
	require_noerr( status, CantExtractFSRef );
	
	// Convert it to a CFURL
	fileAsCFURLRef = CFURLCreateFromFSRef(NULL, &fileAsFSRef);
	
	// Cleanup
CantExtractFSRef:
UserCanceled:
	verify_noerr( NavDisposeReply(&replyRecord) );
CantGetReply:
CantRunDialog:
	NavDialogDispose(dialog);
CantCreateDialog:
CantGetNavOptions:
    return fileAsCFURLRef;
}

string ofxFileDialog() {
	//CFStringRef strRef = CFURLGetString(GetOpenFileFromUser());
	CFStringRef strRef = CFURLCopyFileSystemPath(GetOpenFileFromUser(), kCFURLPOSIXPathStyle);
	char buffer[255];
	CFStringGetCString (strRef,buffer,255,kCFStringEncodingUTF8);
	string buf(buffer);
	return buf;
}