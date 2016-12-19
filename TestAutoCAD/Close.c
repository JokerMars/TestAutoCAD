#include "CallbackRoutines.h"

static int cnt = 0;

FLT_PREOP_CALLBACK_STATUS
PreClose(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	try
	{
		if (!IsFilteredFileProcess(Data, FltObjects))
		{
			leave;
		}

		DbgPrint("PreClose: %d\n", cnt++);
	}
	finally
	{

	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

