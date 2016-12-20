#pragma once

#ifndef _COMMON_H
#define _COMMON_H

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")


extern ULONG g_curProcessNameOffset;
extern UNICODE_STRING ext;

ULONG GetProcessNameOffset();

PCHAR GetProcessName();

BOOLEAN IsFilteredFileProcess(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects,PCHAR IrpType);


#endif