#pragma once

#ifndef _CONTEXT_FILE_H
#define _CONTEXT_FILE_H

#include "Common.h"

typedef struct _STREAM_CONTEXT
{
	//
	// name of the file associated with this context
	//

	UNICODE_STRING fileName;

	//
	// Number of times we saw create on this stream used
	// to verify whether a file flag can be written into end of file
	// and file data can be flush back
	//

	LONG refCount;

	//
	// Lock used to protect this context
	//

	PERESOURCE Resource;

	//
	// spin lock used to protect this context when irq is too high
	//

	KSPIN_LOCK Resource1;

}STREAM_CONTEXT, *PSTREAM_CONTEXT;



/*
******************************************
******** routines ************************
******************************************
*/



//
//  Memory Pool Tags
//

#define STRING_TAG                        'tSxC'
#define RESOURCE_TAG                      'cRxC'
#define STREAM_CONTEXT_TAG                'cSxC'

#define SC_iLOCK(SC)\
	(ASSERT(KeGetCurrentIrql() <= APC_LEVEL), \
	ASSERT(ExIsResourceAcquiredExclusiveLite(SC) || \
	       !ExIsResourceAcquiredSharedLite(SC)),\
	 KeEnterCriticalRegion(),\
	 ExAcquireResourceExclusiveLite(SC, TRUE))

#define SC_iUNLOCK(SC) \
	(ASSERT(KeGetCurrentIrql() <= APC_LEVEL), \
	 ASSERT(ExIsResourceAcquiredSharedLite(SC) ||\
	         ExIsResourceAcquiredExclusiveLite(SC)),\
	 ExReleaseResourceLite(SC),\
	 KeLeaveCriticalRegion())

VOID
SC_LOCK(PSTREAM_CONTEXT SC, PKIRQL OldIrql);

VOID
SC_UNLOCK(PSTREAM_CONTEXT SC, KIRQL OldIrql);

NTSTATUS
Ctx_FindOrCreateStreamContext(
	__in PFLT_CALLBACK_DATA Cbd,
	__in PFLT_RELATED_OBJECTS FltObjects,
	__in BOOLEAN CreateIfNotFound,
	__deref_out PSTREAM_CONTEXT *StreamContext,
	__out_opt BOOLEAN* ContextCreated
);

NTSTATUS
Ctx_UpdateNameInStreamContext(
	__in PUNICODE_STRING DirectoryName,
	__inout PSTREAM_CONTEXT StreamContext
);


VOID
CleanupContext(
	__in PFLT_CONTEXT Context,
	__in FLT_CONTEXT_TYPE ContextType
);

#endif