#include "CallbackRoutines.h"



FLT_PREOP_CALLBACK_STATUS
PreCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObejcts,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObejcts);
	UNREFERENCED_PARAMETER(CompletionContext);

	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS
PostCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
{

	BOOLEAN isMonitored = FALSE;
	NTSTATUS status;
	PSTREAM_CONTEXT pStreamCtx = NULL;
	BOOLEAN bNewCreated = FALSE;
	KIRQL oldIrql;

	try
	{
		isMonitored = IsFilteredFileProcess(Data, FltObjects,"IO_POST_CREATE");
		if (!isMonitored)
		{
			leave;
		}

		//
		// find the stream context, if not, then create a new one
		//

		status = Ctx_FindOrCreateStreamContext(Data, FltObjects, TRUE, &pStreamCtx, &bNewCreated);
		if (!NT_SUCCESS(status))
		{
			leave;
		}

		//
		// if the stream context exist, then increase the reference
		//

		if (!bNewCreated)
		{
			SC_LOCK(pStreamCtx, &oldIrql);

			pStreamCtx->refCount++;
			//DbgPrint("PostCreate RefCount: %d\n", pStreamCtx->refCount);

			SC_UNLOCK(pStreamCtx, oldIrql);
			leave;
		}

		pStreamCtx->refCount++;
		//DbgPrint("PostCreate RefCount: %d\n", pStreamCtx->refCount);

		//
		// get the file size
		//

		LARGE_INTEGER FileSize = { 0 };
		status = File_GetFileSize(Data, FltObjects, &FileSize);
		if (!NT_SUCCESS(status))
		{
			leave;
		}

		DbgPrint("PostCreate File Len: %d\n", FileSize.QuadPart);


	}
	finally
	{
		if (pStreamCtx != NULL)
		{
			FltReleaseContext(pStreamCtx);
		}
	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}