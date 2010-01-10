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

/*
 * libarchive wrapper. Like the library, it only supports simple enumeration.
 * That way it can skip reading the data if not needed.
 *
 * Requires garbage collection and probably OS X 10.6.
 */

#import <Cocoa/Cocoa.h>

struct archive;
struct archive_entry;

/**********************************************************************
 * Exceptions
 **********************************************************************/

/*
 * Raised by the accessors if the data is not available.
 */
extern NSString * const ArchiveMemberInfoNotAvailableException;

/*
 * Raised by -data if data was skipped, or loaded by -data but load failed.
 */
extern NSString * const ArchiveMemberDataNotAvailableException;

/*
 * Raised by read functions that take error pointers if error occurs and the pointer is nil.
 * Also raised by fast enumerations on read errors.
 * Contains the underlying NSError.
 */
extern NSString * const ArchiveReadException;

/**********************************************************************
 * Errors
 **********************************************************************/

/*
 * NSError domain for libarchive, errno and error_string in userInfo.
 */
extern NSString * const ArchiveErrorDomain;

/**********************************************************************
 * Classes
 **********************************************************************/

@interface ArchiveMember : NSObject
{
	struct archive *archive;
	struct archive_entry *entry;
	NSStringEncoding encoding;
	
	BOOL dataAvailable;
	NSData *data;
}

@property(readonly) struct archive_entry *entry;
@property(readonly) NSStringEncoding encoding;

@property(readonly) BOOL pathnameAvailable;
@property(readonly) NSString *pathname;
@property(readonly) const char *cPathname;
 
@property(readonly) BOOL sizeAvailable;
@property(readonly) int64_t size;

/* XXX add more accessors here. */

/*
 * If data is to be fetched, you have to do it before enumerating the next object,
 * since that will call skipData (per necessity).
 * Returns YES if successful or NO if failed (the other one already called, or archive error).
 * Notice that error can be set to a warning even if YES is returned.
 */
- (BOOL)fetchDataWithError:(NSError**)error;
- (BOOL)skipDataWithError:(NSError**)error;

/* -data will do fetchData if needed. */
@property(readonly) BOOL dataAvailable;
@property(readonly) NSData *data;

/*
 * It is invalid to pass a nil error to this function.
 * If data is not loaded it will be skipped during extraction.
 */
- (BOOL)extractToURL:(NSURL *)dst createDirectories:(BOOL)create error:(NSError **)error;

@end

@interface Archive : NSObject <NSFastEnumeration>
{
	struct archive *archive;
	NSStringEncoding encoding;
	
	ArchiveMember *lastMember;
}

/* Only file URLs are supported for now. */
+ (Archive*)archiveForReadingFromURL:(NSURL *)url encoding:(NSStringEncoding)encoding error:(NSError **)error;

- (id)initForReadingFromURL:(NSURL *)url encoding:(NSStringEncoding)enc error:(NSError **)error;

- (ArchiveMember *)nextMemberWithError:(NSError**)error;

@end
