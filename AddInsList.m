//
//  MyDocument.m
//  modazipin
//
//  Created by Pelle Johansson on 2010-01-05.
//  Copyright __MyCompanyName__ 2010 . All rights reserved.
//

#import "AddInsList.h"
#import "DataStore.h"
#import "ArchiveWrapper.h"

@implementation AddInsList

static AddInsList *sharedAddInsList;

+ (AddInsList*)sharedAddInsList
{
	return sharedAddInsList;
}

- (id)init 
{
	NSAssert(sharedAddInsList == nil, "Already a shared AddInsList");
	
    self = [super init];
    if (self != nil) {
        // initialization code
    }
	sharedAddInsList = self;
    return self;
}

- (NSString *)windowNibName 
{
    return @"AddInsList";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)windowController 
{
    [super windowControllerDidLoadNib:windowController];
    // user interface preparation code
}

- (NSString *)persistentStoreTypeForFileType:(NSString *)fileType
{
	return @"AddInsListStore";
}

- (NSURL *)baseDirectory
{
	NSURL *settings = [[self fileURL] URLByDeletingLastPathComponent];
	
	if ([[settings lastPathComponent] caseInsensitiveCompare:@"Settings"] != NSOrderedSame)
		return nil;
	
	return [settings URLByDeletingLastPathComponent];
}

- (BOOL)installAddInItem:(NSXMLElement *)node withArchive:(NSURL*)url error:(NSError**)error
{
	AddInsListStore *store = [[[[self managedObjectContext] persistentStoreCoordinator] persistentStores] objectAtIndex:0];
	Archive *archive = [Archive archiveForReadingFromURL:url encoding:NSWindowsCP1252StringEncoding error:error];
	NSURL *base = [self baseDirectory];
	
	if (!archive)
		return NO;
	
	for (ArchiveMember *entry in archive)
	{
		if ([[entry pathname] hasSuffix:@"/"])
			continue;
		
		/* XXX filter entries */
		NSEnumerator *path = [[[entry pathname] pathComponents] objectEnumerator];
		
		if ([[path nextObject] caseInsensitiveCompare:@"Contents"] == NSOrderedSame)
		{
			NSURL *dst = base;
			NSString *part;
			
			while ((part = [path nextObject]))
			{
				if ([part isEqualToString:@".."] || [part isEqualToString:@"."])
					continue;
				dst = [dst URLByAppendingPathComponent:part];
			}
			
			/* XXX delete all files on error. */
			if (![entry extractToURL:dst createDirectories:YES error:error])
				return NO;
		}
	}
	
	/* XXX delete all files on error. */
	return [store insertAddInNode:node error:error intoContext:[self managedObjectContext]];
}

- (BOOL)syncFilesFromContext:(NSError **)error
{
	NSURL *base = [self baseDirectory];	
	NSArray *addins = [[self managedObjectContext] executeFetchRequest:[[self managedObjectModel] fetchRequestTemplateForName:@"addinsWithPaths"] error:error];
	
	if (!addins)
		return NO;
	
	for (DataStoreObject *addin in addins)
	{
		NSSet *paths = [[addin valueForKey:@"modazipin"] valueForKey:@"paths"];
		BOOL isEnabled = [[addin valueForKey:@"Enabled"] boolValue];
		
		for (DataStoreObject *path in paths)
		{
			NSString *enabledPath = [path valueForKey:@"path"];
			NSRange slash = [enabledPath rangeOfString:@"/"];
			NSString *disabledPath = [enabledPath stringByReplacingCharactersInRange:slash withString:@" (disabled)/"];
			NSURL *expectedURL = [base URLByAppendingPathComponent:isEnabled ? enabledPath : disabledPath];
			NSURL *otherURL = [base URLByAppendingPathComponent:isEnabled ? disabledPath : enabledPath];
			
			if ([expectedURL checkResourceIsReachableAndReturnError:nil])
				continue;
			
			if (![otherURL checkResourceIsReachableAndReturnError:nil])
				continue; /* XXX more error handling */
			
			NSURL *dirURL = [expectedURL URLByDeletingLastPathComponent];
			
			[[NSFileManager defaultManager] createDirectoryAtPath:[dirURL path] withIntermediateDirectories:YES attributes:nil error:nil];
			if (![[NSFileManager defaultManager] moveItemAtURL:otherURL toURL:expectedURL error:error])
				NULL; /* XXX do something here. */
		}
	}
	return YES;
}

- (BOOL)writeToURL:(NSURL *)absoluteURL ofType:(NSString *)typeName forSaveOperation:(NSSaveOperationType)saveOperation originalContentsURL:(NSURL *)absoluteOriginalContentsURL error:(NSError **)error
{
	BOOL res = [super writeToURL:absoluteURL ofType:typeName forSaveOperation:saveOperation originalContentsURL:absoluteOriginalContentsURL error:error];
	
	if (!res)
		return NO;
	
	return [self syncFilesFromContext:error];
}

@end
