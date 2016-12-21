#include "CallbackRoutines.h"

static int cnt = 0;

FLT_PREOP_CALLBACK_STATUS
PreClose(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	NTSTATUS status;
	PSTREAM_CONTEXT pStreamCtx = NULL;

	try
	{
		status = FltGetStreamContext(FltObjects->Instance, FltObjects->FileObject, &pStreamCtx);
		if (!NT_SUCCESS(status) || pStreamCtx == NULL)
		{
			leave;
		}


		PCHAR procName = GetProcessName();
		if (procName == NULL)
		{
			leave;
		}

		DbgPrint("\nIO_PRE_CLOSE\n");
		DbgPrint("    File Name: %wZ\n", &(pStreamCtx->fileName));
		DbgPrint("    Process Name: %s\n", procName);
		DbgPrint("    PreClose: %d\n", cnt++);
	
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

