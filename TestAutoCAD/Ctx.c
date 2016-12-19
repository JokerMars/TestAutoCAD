#include "Ctx.h"


static NTSTATUS iCtx_CreateStreamContext(PFLT_RELATED_OBJECTS FltObjects, PSTREAM_CONTEXT *StreamContext);


VOID
SC_LOCK(PSTREAM_CONTEXT SC, PKIRQL OldIrql)
{
	if (KeGetCurrentIrql() <= APC_LEVEL)
	{
		SC_iLOCK(SC->Resource);
	}
	else
	{
		KeAcquireSpinLock(&SC->Resource1, OldIrql);
	}
}

VOID
SC_UNLOCK(PSTREAM_CONTEXT SC, KIRQL OldIrql)
{
	if (KeGetCurrentIrql() <= APC_LEVEL)
	{
		SC_iUNLOCK(SC->Resource);
	}
	else
	{
		KeReleaseSpinLock(&SC->Resource1, OldIrql);
	}
}


NTSTATUS
Ctx_FindOrCreateStreamContext(
	__in PFLT_CALLBACK_DATA Data,
	__in PFLT_RELATED_OBJECTS FltObjects,
	__in BOOLEAN CreateIfNotFound,
	__deref_out PSTREAM_CONTEXT *StreamContext,
	__out_opt BOOLEAN* ContextCreated
)
{
	NTSTATUS status;
	PSTREAM_CONTEXT streamContext;
	PSTREAM_CONTEXT oldStreamContext;

	PAGED_CODE();

	*StreamContext = NULL;
	if (ContextCreated != NULL) *ContextCreated = FALSE;

	//  First try to get the stream context.
	status = FltGetStreamContext(Data->Iopb->TargetInstance,
		Data->Iopb->TargetFileObject, &streamContext);

	if (!NT_SUCCESS(status) &&
		(status == STATUS_NOT_FOUND) &&
		CreateIfNotFound)
	{
		status = iCtx_CreateStreamContext(FltObjects, &streamContext);
		if (!NT_SUCCESS(status))
			return status;

		status = FltSetStreamContext(Data->Iopb->TargetInstance,
			Data->Iopb->TargetFileObject,
			FLT_SET_CONTEXT_KEEP_IF_EXISTS,
			streamContext,
			&oldStreamContext);

		if (!NT_SUCCESS(status))
		{
			FltReleaseContext(streamContext);

			if (status != STATUS_FLT_CONTEXT_ALREADY_DEFINED)
			{
				//  FltSetStreamContext failed for a reason other than the context already
				//  existing on the stream. So the object now does not have any context set
				//  on it. So we return failure to the caller.
				return status;
			}
			streamContext = oldStreamContext;
			status = STATUS_SUCCESS;
		}
		else
		{
			if (ContextCreated != NULL) *ContextCreated = TRUE;
		}
	}
	*StreamContext = streamContext;

	return status;
}


NTSTATUS
iCtx_CreateStreamContext(
	__in PFLT_RELATED_OBJECTS FltObjects,
	__deref_out PSTREAM_CONTEXT *StreamContext
)
/*++

Routine Description:

This routine creates a new stream context

Arguments:

StreamContext         - Returns the stream context

Return Value:

Status

--*/
{
	NTSTATUS status;
	PSTREAM_CONTEXT streamContext;

	PAGED_CODE();

	status = FltAllocateContext(FltObjects->Filter,
		FLT_STREAM_CONTEXT,
		sizeof(STREAM_CONTEXT),
		NonPagedPool,
		&streamContext);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	//  Initialize the newly created context
	RtlZeroMemory(streamContext, sizeof(STREAM_CONTEXT));

	streamContext->Resource = ExAllocatePoolWithTag(NonPagedPool, sizeof(ERESOURCE), RESOURCE_TAG);
	if (streamContext->Resource == NULL)
	{
		FltReleaseContext(streamContext);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	ExInitializeResourceLite(streamContext->Resource);

	KeInitializeSpinLock(&streamContext->Resource1);

	*StreamContext = streamContext;

	return STATUS_SUCCESS;
}


NTSTATUS
Ctx_UpdateNameInStreamContext(
	__in PUNICODE_STRING DirectoryName,
	__inout PSTREAM_CONTEXT StreamContext
)
/*++

Routine Description:

This routine updates the name of the target in the supplied stream context

Arguments:

DirectoryName         - Supplies the directory name
StreamContext    - Returns the updated name in the stream context

Return Value:

Status

Note:

The caller must synchronize access to the context. This routine does no
synchronization

--*/
{
	NTSTATUS status = STATUS_SUCCESS;

	PAGED_CODE();

	//Free any existing name
	if (StreamContext->fileName.Buffer != NULL)
	{
		ExFreePoolWithTag(StreamContext->fileName.Buffer, STRING_TAG);

		StreamContext->fileName.Length = StreamContext->fileName.MaximumLength = 0;
		StreamContext->fileName.Buffer = NULL;
	}

	//Allocate and copy off the directory name
	StreamContext->fileName.MaximumLength = DirectoryName->Length;
	StreamContext->fileName.Buffer = ExAllocatePoolWithTag(PagedPool,
		StreamContext->fileName.MaximumLength,
		STRING_TAG);
	if (StreamContext->fileName.Buffer == NULL)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlCopyUnicodeString(&StreamContext->fileName, DirectoryName);

	return status;
}


VOID
CleanupContext(
	__in PFLT_CONTEXT Context,
	__in FLT_CONTEXT_TYPE ContextType
)
{
	PSTREAM_CONTEXT streamCtx = NULL;

	PAGED_CODE();

	UNREFERENCED_PARAMETER(ContextType);



	KIRQL OldIrql;

	streamCtx = (PSTREAM_CONTEXT)Context;

	if (streamCtx == NULL)
		return;

	if (streamCtx->fileName.Buffer != NULL)
	{
		ExFreePoolWithTag(streamCtx->fileName.Buffer, STRING_TAG);

		streamCtx->fileName.Length = streamCtx->fileName.MaximumLength = 0;
		streamCtx->fileName.Buffer = NULL;
	}

	if (NULL != streamCtx->Resource)
	{
		ExDeleteResourceLite(streamCtx->Resource);
		ExFreePoolWithTag(streamCtx->Resource, RESOURCE_TAG);
	}


}
