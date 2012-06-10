//
//  ofxPrimeSensor.cpp
//  primeSensor
//
//  Created by Roee Kremer on 6/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxPrimeSensor.h"


#define SAMPLE_XML_PATH "SamplesConfig.xml"



//--------------------------------------------------------------
void ofxPrimeSensor::setup(){
    
    
    XnStatus rc;
    
	EnumerationErrors errors;
	rc = g_context.InitFromXmlFile(ofToDataPath(SAMPLE_XML_PATH).c_str() , g_scriptNode, &errors);
	if (rc == XN_STATUS_NO_NODE_PRESENT)
	{
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		printf("%s\n", strError);
		std::exit(rc);
	}
	else if (rc != XN_STATUS_OK)
	{
		printf("Open failed: %s\n", xnGetStatusString(rc));
        std::exit(rc);
		
	}
    
	rc = g_context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_depth);
	if (rc != XN_STATUS_OK)
	{
		printf("No depth node exists! Check your XML.");
        std::exit(1);
	}
    
	rc = g_context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_image);
	if (rc != XN_STATUS_OK)
	{
		printf("No image node exists! Check your XML.");
		std::exit(1);
	}
    
	g_depth.GetMetaData(g_depthMD);
	g_image.GetMetaData(g_imageMD);
    
    //	// Hybrid mode isn't supported in this sample
    //	if (g_imageMD.FullXRes() != g_depthMD.FullXRes() || g_imageMD.FullYRes() != g_depthMD.FullYRes())
    //	{
    //		printf ("The device depth and image resolution must be equal!\n");
    //		return 1;
    //	}
    
	// RGB is the only image format supported.
	if (g_imageMD.PixelFormat() != XN_PIXEL_FORMAT_RGB24)
	{
		printf("The device image format must be RGB24\n");
		std::exit(1);
	}
    
	
    
	g_nZRes = g_depthMD.ZRes();
	g_pDepthHist = (float*)malloc(g_nZRes * sizeof(float));
    
    
    cout << g_imageMD.XRes() << "," << g_imageMD.YRes() << endl;
    colorPixels.allocate(g_imageMD.XRes(), g_imageMD.YRes(), OF_PIXELS_RGB);
    depthPixels.allocate(g_depthMD.XRes(), g_depthMD.YRes(), OF_PIXELS_MONO);
}

//--------------------------------------------------------------
void ofxPrimeSensor::update(){
    XnStatus rc = XN_STATUS_OK;
    
	// Read a new frame
	rc = g_context.WaitAnyUpdateAll();
	if (rc != XN_STATUS_OK)
	{
		printf("Read failed: %s\n", xnGetStatusString(rc));
		return;
	}
    
	g_depth.GetMetaData(g_depthMD);
	g_image.GetMetaData(g_imageMD);
    
    
    //    const XnRGB24Pixel* pImageRow = g_imageMD.RGB24Data();
    //    
    //    
    //    for (XnUInt y = 0; y < g_imageMD.YRes(); ++y)
    //    {
    //        const XnRGB24Pixel* pImage = pImageRow;
    //        
    //        
    //        for (XnUInt x = 0; x < g_imageMD.XRes(); ++x, ++pImage)
    //        {
    //            
    //            ofColor color(pImage->nRed,pImage->nGreen,pImage->nBlue);
    //            colorPixels.setColor(x, y, color);
    //           
    //        }
    //        
    //        pImageRow += g_imageMD.XRes();
    //       
    //    }
    //    
    //    
    //
    //    image.loadData(colorPixels);
    
    
    
//    image.loadData((unsigned char*) g_imageMD.RGB24Data(),g_imageMD.XRes(),g_imageMD.YRes(),GL_RGB);
    
    colorPixels.setFromPixels((unsigned char*) g_imageMD.RGB24Data(),g_imageMD.XRes(),g_imageMD.YRes(),OF_IMAGE_COLOR);
    
	const XnDepthPixel* pDepth = g_depthMD.Data();
    
	
	// Calculate the accumulative histogram (the yellow display...)
	xnOSMemSet(g_pDepthHist, 0, g_nZRes*sizeof(float));
    
	unsigned int nNumberOfPoints = 0;
	for (XnUInt y = 0; y < g_depthMD.YRes(); ++y)
	{
		for (XnUInt x = 0; x < g_depthMD.XRes(); ++x, ++pDepth)
		{
			if (*pDepth != 0)
			{
				g_pDepthHist[*pDepth]++;
				nNumberOfPoints++;
			}
		}
	}
	for (int nIndex=1; nIndex<g_nZRes; nIndex++)
	{
		g_pDepthHist[nIndex] += g_pDepthHist[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (int nIndex=1; nIndex<g_nZRes; nIndex++)
		{
			g_pDepthHist[nIndex] = (unsigned int)(256 * (1.0f - (g_pDepthHist[nIndex] / nNumberOfPoints)));
		}
	}
    
    
    
    depthPixels.set(0);
    const XnDepthPixel* pDepthRow = g_depthMD.Data();
    
    
    for (XnUInt y = 0; y < g_depthMD.YRes(); ++y)
    {
        const XnDepthPixel* pDepth = pDepthRow;
        
        
        for (XnUInt x = 0; x < g_depthMD.XRes(); ++x, ++pDepth)
        {
            if (*pDepth != 0)
            {
                depthPixels.setColor(x, y, ofColor(g_pDepthHist[*pDepth]));
            }
        }
        
        pDepthRow += g_depthMD.XRes();
    }
    
}


ofPixels& ofxPrimeSensor::getDepthPixels() {
    return depthPixels;
}

ofPixels& ofxPrimeSensor::getImagePixels() {
    return colorPixels;
}


