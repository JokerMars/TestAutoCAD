#include "CallbackRoutines.h"

FLT_PREOP_CALLBACK_STATUS
PreRead(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
	ULONG readLen = iopb->Parameters.Read.Length;
	BOOLEAN isMonitored = FALSE;

	try
	{
		isMonitored = IsFilteredFileProcess(Data, FltObjects,"IO_PRE_READ");
		if (!isMonitored)
		{
			leave;
		}

		DbgPrint("***PreRead Len: %d\n", readLen);
		DbgPrint("***Read Offset: %d\n", iopb->Parameters.Read.ByteOffset.QuadPart);

		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	finally
	{

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
	DbgPrint("PostRead Len: %d\n", Data->IoStatus.Information);

	return FLT_POSTOP_FINISHED_PROCESSING;
}