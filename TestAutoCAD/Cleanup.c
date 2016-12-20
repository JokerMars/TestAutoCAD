#include "CallbackRoutines.h"

//static int cnt = 0;

FLT_PREOP_CALLBACK_STATUS
PreCleanup(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	NTSTATUS status;
	PSTREAM_CONTEXT pStreamCtx = NULL;
	KIRQL oldIrql;

	try
	{
		if (!IsFilteredFileProcess(Data, FltObjects,"IO_CLEANUP"))
		{
			leave;
		}

	/*	DbgPrint("PreCleanup: %d\n", cnt++);*/

		//
		// get the current stream context
		//

		status = FltGetStreamContext(FltObjects->Instance, FltObjects->FileObject, &pStreamCtx);
		if (!NT_SUCCESS(status) || pStreamCtx == NULL)
		{
			leave;
		}

		if (pStreamCtx->refCount > 0)
		{
			SC_LOCK(pStreamCtx, &oldIrql);

			pStreamCtx->refCount--;
			//DbgPrint("PreCleaup RefCount: %d\n", pStreamCtx->refCount);

			SC_UNLOCK(pStreamCtx, oldIrql);
		}

		//
		// get the file size
		//

		LARGE_INTEGER FileSize = { 0 };
		status = File_GetFileSize(Data, FltObjects, &FileSize);
		if (!NT_SUCCESS(status))
		{
			leave;
		}

		DbgPrint("PreCleanup File Len: %d\n", FileSize.QuadPart);


		//
		// clear the cache
		//



	}
	finally
	{
		if (pStreamCtx != NULL)
		{
			FltReleaseContext(pStreamCtx);
		}
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}
