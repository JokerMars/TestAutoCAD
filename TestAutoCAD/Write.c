#include "CallbackRoutines.h"

FLT_PREOP_CALLBACK_STATUS
PreWrite(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
	ULONG writeLen = iopb->Parameters.Write.Length;
	BOOLEAN isMonitored = FALSE;

	try
	{
		
	
		isMonitored = IsFilteredFileProcess(Data, FltObjects,"IO_PRE_WRITE");
		if (!isMonitored)
		{
			leave;
		}


		DbgPrint("***PreWrite Len: %d\n", writeLen);

		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	finally
	{
	
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS
PostWrite(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
{
	DbgPrint("PostWrite Len: %d\n", Data->IoStatus.Information);

	return FLT_POSTOP_FINISHED_PROCESSING;
}
