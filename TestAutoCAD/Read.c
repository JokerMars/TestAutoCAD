#include "CallbackRoutines.h"

FLT_PREOP_CALLBACK_STATUS
PreRead(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{

	NTSTATUS status;
	PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
	PSTREAM_CONTEXT pStreamCtx = NULL;


	try
	{
		//
		// get the current stream context
		//

		status = FltGetStreamContext(iopb->TargetInstance, iopb->TargetFileObject, &pStreamCtx);
		if (!NT_SUCCESS(status) || pStreamCtx == NULL)
		{
			leave;
		}

		//
		// Get Process Name
		//

		PCHAR procName = GetProcessName();
		if (procName == NULL)
		{
			leave;
		}
		if (strncmp(procName, MONITOR_PROCESS, strlen(MONITOR_PROCESS)) != 0)
		{
			leave;
		}

		DbgPrint("\nIO_PRE_READ\n");
		DbgPrint("    File Name: %wZ\n", &(pStreamCtx->fileName));
		DbgPrint("    Process Name: %s\n", procName);
		DbgPrint("    PreWrite Len: %d\n", iopb->Parameters.Write.Length);
		DbgPrint("    PreRead Len: %d\n", iopb->Parameters.Read.Length);

		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
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


FLT_POSTOP_CALLBACK_STATUS
PostRead(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
{
	DbgPrint("\nIO_POST_READ\n");
	DbgPrint("    PostRead Len: %d\n", Data->IoStatus.Information);
	return FLT_POSTOP_FINISHED_PROCESSING;
}