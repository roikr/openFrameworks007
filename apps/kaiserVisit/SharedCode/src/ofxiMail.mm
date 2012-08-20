//
//  ofxiMail.cpp
//  beenThere
//
//  Created by Roee Kremer on 8/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxiMail.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

#import <MessageUI/MessageUI.h>
#import <MessageUI/MFMailComposeViewController.h>

@interface MyDelegateObject : NSObject <MFMailComposeViewControllerDelegate> 

@end

@implementation MyDelegateObject



// Dismisses the email composition interface when users tap Cancel or Send. Proceeds to update the 
// message field with the result of the operation.
- (void)mailComposeController:(MFMailComposeViewController*)controller 
          didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error {
	
	int res;
   	// Notifies users about errors associated with the interface
	switch (result)
	{
		case MFMailComposeResultCancelled:
			res = OFXIMAIL_SENDING_CANCELED;
			break;
		case MFMailComposeResultSaved:
			res = OFXIMAIL_SAVED;
			break;
		case MFMailComposeResultSent:
			res = OFXIMAIL_SEND;
			break;
		case MFMailComposeResultFailed:
			res = OFXIMAIL_FAILED;
			break;
		default:
			res = OFXIMAIL_NOT_SEND;
			break;
	}
	[ofxiPhoneGetViewController()  dismissModalViewControllerAnimated:YES];
    
    
    ofNotifyEvent(ofxiMailEvent , res);
}

- (void)dealloc {
	[super dealloc];
}


@end


ofEvent<int> ofxiMailEvent;

void ofxiMail::setup() {
    delegate = [[MyDelegateObject alloc] init];
}

void ofxiMail::exit() {
    [delegate release];
}

bool ofxiMail::getCanSendMail() {
    Class mailClass = (NSClassFromString(@"MFMailComposeViewController"));
    
	if (mailClass != nil) {
        //[self displayMailComposerSheet];
		// We must always check whether the current device is configured for sending emails
		if ([mailClass canSendMail]) {
			return true;
		}
		else {
            //			feedbackMsg.hidden = NO;
            //			feedbackMsg.text = @"Device not configured to send mail.";
		}
	}
    //	else	{
    //		feedbackMsg.hidden = NO;
    //		feedbackMsg.text = @"Device not configured to send mail.";
    //	}
    
    return false;
}

void ofxiMail::sendMail(mailStruct mail) {
    MFMailComposeViewController *picker = [[MFMailComposeViewController alloc] init];
    
    picker.mailComposeDelegate = delegate;
    
    [picker setSubject:ofxStringToNSString(mail.subject)];
    
    
    // Set up recipients
    NSMutableArray *toRecipients = [NSMutableArray arrayWithCapacity:mail.toRecipients.size()];
    for (vector<string>::iterator iter=mail.toRecipients.begin(); iter!=mail.toRecipients.end(); iter++) {
        [toRecipients addObject:ofxStringToNSString(*iter)];
    }
    
    NSMutableArray *ccRecipients = [NSMutableArray arrayWithCapacity:mail.ccRecipients.size()];
    for (vector<string>::iterator iter=mail.ccRecipients.begin(); iter!=mail.ccRecipients.end(); iter++) {
        [ccRecipients addObject:ofxStringToNSString(*iter)];
    }
    
    NSMutableArray *bccRecipients = [NSMutableArray arrayWithCapacity:mail.bccRecipients.size()];
    for (vector<string>::iterator iter=mail.bccRecipients.begin(); iter!=mail.bccRecipients.end(); iter++) {
        [bccRecipients addObject:ofxStringToNSString(*iter)];
    }
  
    [picker setToRecipients:toRecipients];
    [picker setCcRecipients:ccRecipients];	
    [picker setBccRecipients:bccRecipients];
    
    for (vector<attachmentStruct >::iterator iter=mail.attachments.begin(); iter!=mail.attachments.end(); iter++) {
        
        //    NSString *path = [[NSBundle mainBundle] pathForResource:@"rainy" ofType:@"jpg"];
        //    NSData *myData = [NSData dataWithContentsOfFile:path];
        //    [picker addAttachmentData:myData mimeType:@"image/jpeg" fileName:@"rainy"];
        
//        cout << iter->filename << "\t" << iter->data.size() << endl;
        [picker addAttachmentData:[NSData dataWithBytes:iter->data.getBinaryBuffer() length:iter->data.size()] mimeType:ofxStringToNSString(iter->mimetype) fileName:ofxStringToNSString(iter->filename)];
    }
    
   

    
    // Fill out the email body text
    [picker setMessageBody:ofxStringToNSString(mail.body) isHTML:NO];
    
    [ofxiPhoneGetViewController() presentModalViewController:picker animated:YES];
    [picker release];
}

/*

void ofxiMail::composerFinished(int result) {
    
case MFMailComposeResultCancelled:
    text = @"Result: Mail sending canceled";
    break;
case MFMailComposeResultSaved:
    text = @"Result: Mail saved";
    break;
case MFMailComposeResultSent:
    text = @"Result: Mail sent";
    break;
case MFMailComposeResultFailed:
    text = @"Result: Mail sending failed";
    break;
default:
    text = @"Result: Mail not sent";
    break;
    
    
}
*/