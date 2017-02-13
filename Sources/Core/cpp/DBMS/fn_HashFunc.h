#pragma once

#include <math.h>
#include "..\ASCInterface\IDBMS.h"

#undef  DBMS_SRC_FILE
#define DBMS_SRC_FILE "fn_HashFunc.h"

///	Таблица для ускоренного расчета CRC16
extern UINT16 Crc16Tbl[256];

///	Таблица для ускоренного расчета CRC32
extern UINT32 Crc32Tbl[256];
