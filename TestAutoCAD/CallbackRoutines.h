#pragma once
#ifndef _CALLBACK_ROUTINE_H
#define _CALLBACK_ROUTINE_H


#include "Common.h"

/*
*******Create routine*************
*/

FLT_PREOP_CALLBACK_STATUS
PreCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObejcts,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);


FLT_POSTOP_CALLBACK_STATUS
PostCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
);


FLT_PREOP_CALLBACK_STATUS
PreClose(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);








#endif