/*
 *  ofxRKUtilities.cpp
 *  atlasCreatorFromImages
 *
 *  Created by Roee Kremer on 3/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxRKUtilities.h"
#include "ofMain.h"
#include <dirent.h> 
#include <sys/stat.h> 


//----------------------------------------------------------
vector<string> ofListFolders(string directory){
    
	vector<string> folders;
	
	
    if(directory.length() <= 0)return folders;
	
    //if the trailing slash was not added - then add it
	if( directory[directory.length()-1] != '/'){
        directory = directory + "/";
	}
	
	DIR *dir = NULL;
	struct dirent *entry;
	
    //open the directory
    ofLog(OF_LOG_VERBOSE, "ofListFolders - attempting to open %s", directory.c_str());
    dir = opendir(directory.c_str());
	
	if(dir == NULL){
		ofLog(OF_LOG_ERROR, "ofListFolders - error opening directory");
		return folders;
	}else{
		ofLog(OF_LOG_VERBOSE, "ofListFolders - success opening directory");
	}
	
    string entry_name = "";
   	
	while ((entry = readdir(dir)) != NULL){
		
		if (entry->d_type ==DT_DIR) {
			//turn it into a C++ string
			entry_name = entry->d_name;
			
			//lets get the length of the string here as we query it again
			int fileLen = entry_name.length();
			
			if(fileLen <= 0)continue; //if the name is not existant
			if(entry_name[0] == '.')continue; //ignore invisible files, ./ and ../
			
			folders.push_back(entry_name);
			ofLog(OF_LOG_VERBOSE, "ofListFolders - listing %s ", folders.back().c_str());
		}
        
		
		
	}
	
	ofLog(OF_LOG_VERBOSE, "ofListFolders - listed %i files in %s", folders.size(), directory.c_str());
	return folders;
}


bool ofIsFileExists(string filename) { 
	struct stat stFileInfo; 
	bool blnReturn; 
	int intStat; 
	
	// Attempt to get the file attributes 
	intStat = stat(filename.c_str(),&stFileInfo); 
	if(intStat == 0) { 
		// We were able to get the file attributes 
		// so the file obviously exists. 
		blnReturn = true; 
	} else { 
		// We were not able to get the file attributes. 
		// This may mean that we don't have permission to 
		// access the folder which contains this file. If you 
		// need to do that level of checking, lookup the 
		// return values of stat which will give you 
		// more details on why stat failed. 
		blnReturn = false; 
	} 
	
	return(blnReturn); 
}

/*
 bool ofIsFileExists( string filename )
 {
 FILE* fp = NULL;
 
 //will not work if you do not have read permissions
 //to the file, but if you don't have read, it
 //may as well not exist to begin with.
 
 fp = fopen( filename.c_str(), "rb" );
 if( fp != NULL )
 {
 fclose( fp );
 return true;
 }
 
 return false;
 }
 */
