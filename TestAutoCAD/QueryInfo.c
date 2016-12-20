#include "CallbackRoutines.h"


static int cnt = 0;

FLT_PREOP_CALLBACK_STATUS
PreQueryInfo(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	BOOLEAN isMonitored = FALSE;
	FILE_INFORMATION_CLASS FileInfoClass = Data->Iopb->Parameters.QueryFileInformation.FileInformationClass;

	try
	{
		//
		// if the file and process is what we care, then go down
		//

		isMonitored = IsFilteredFileProcess(Data, FltObjects,"IO_PRE_QUERY_INFO");
		if (!isMonitored)
		{
			leave;
		}

		DbgPrint("PreQueryInfo: %d\n", cnt++);
		
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	finally
	{

	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS
PostQueryInfo(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
{
	FILE_INFORMATION_CLASS FileInfoClass = Data->Iopb->Parameters.QueryFileInformation.FileInformationClass;


	try
	{
		//
		// now we just print the prompt info, 
		// until we carry on the encrypt schedule, then we change this phrase
		//

		switch (FileInfoClass)
		{
			case FileAllInformation:
			{
				DbgPrint("FileAllInformation\n");
				break;
			}

			case FileAllocationInformation:
			{
				DbgPrint("FileAllocationInformation\n");
				break;
			}

			case FileValidDataLengthInformation:
			{
				DbgPrint("FileValidDataLengthInformation\n");
				break;
			}

			case FileStandardInformation:
			{
				DbgPrint("FileStandardInformation\n");
				break;
			}

			case FileEndOfFileInformation:
			{
				DbgPrint("FileEndOfFileInformation\n");
				break;
			}

			case FilePositionInformation:
			{
				DbgPrint("FilePositionInformation\n");
				break;
			}

			case FileBasicInformation:
			{
				DbgPrint("FileBasicInformation\n");
				break;
			}

		}
	}
	finally
	{

	}
	
	return FLT_POSTOP_FINISHED_PROCESSING;
}


