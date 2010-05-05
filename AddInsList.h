/* Copyright (c) 2010 Per Johansson, per at morth.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import "DataStore.h"
#import "DataStoreObject.h"
#import "ArchiveWrapper.h"

@interface AddInsList : NSPersistentDocument
{
	NSMetadataQuery *spotlightQuery;
	NSMetadataItem *spotlightGameItem;
	
	NSOperationQueue *operationQueue;
	BOOL isBusy;
	NSString *statusMessage;
	
	IBOutlet NSToolbarItem *launchGameButton;
	IBOutlet NSArrayController *itemsController;
	IBOutlet WebView *detailsView;
	
	IBOutlet NSWindow *progressWindow;
	IBOutlet NSProgressIndicator *progressIndicator;
}

+ (AddInsList*)sharedAddInsList;

- (void)itemsControllerChanged;

- (void)selectItemWithUid:(NSString*)uid;

- (void)addContents:(NSString *)contents forURL:(NSURL *)url;
- (void)addContentsForURL:(NSDictionary*)data;

@property(readonly) NSOperationQueue *queue;
@property(readonly) BOOL isBusy;
@property(readonly) NSString *statusMessage;

- (void)updateOperationCount;

@end


@interface AddInsList (Installing)

- (BOOL)installItems:(NSArray *)items withArchive:(NSURL*)url uncompressedSize:(NSUInteger)sz error:(NSError**)error;
- (void)progressChanged:(ArchiveWrapper*)archive session:(NSModalSession)session;

@end


@interface AddInsList (Uninstalling)

- (IBAction)askUninstall:(id)sender;

- (BOOL)uninstall:(Item*)item error:(NSError**)error;

@end


@interface AddInsList (GameLaunching)

@property NSMetadataItem *spotlightGameItem;

- (void)updateLaunchButtonImage;

- (BOOL)searchSpotlightForGame;

- (void)spotlightQueryChanged;

- (NSURL*)gameURL;

- (IBAction)launchGame:(id)sender;

@end
