#pragma		once

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxDataStorage
{

//---------------------------------------------
//вспомогательные константы
const unsigned int EMPTY_OFFSET=0xffffffff;
const unsigned int EMPTY_VALUE=0xffffffff;
const unsigned int EMPTY_SOURCE=0xffffffff;
const unsigned int EMPTY_NODE=0xff;
const unsigned int UC_EMPTY=0xff;
const unsigned int INT_SIZE=4;

//---------------------------------------------
//настройки файлов
const unsigned int cuiChunkSize=64;

//---------------------------------------------
//константы для работы с коллекцией файлов

///максимальный размер файлов в коллекции
const unsigned int cuiMaxFileSize=1500000000;
///максимальное количество файлов в коллекции
const unsigned int cuiMaxFilesNumber=0xff; 
///индекс файла для смещения на верхушку координатного дерева
const unsigned int cuiRootFileIndex=254; 

//---------------------------------------------
//константы для буферизации

///размер системной страницы памяти
const unsigned int cuiPageSize=4096;
///размер буфера в файловом кэше обычного файла
const unsigned int cuiCommonFileBufferSize=(cuiPageSize*16);
///размер буфера чтения в файловом кэше обычного файла
const unsigned int cuiReadFileBufferSize=cuiPageSize;
///максимальное количество буферов в файловом кэше
const unsigned int cuiMaxBufferNumber=0xff;
///размер буфера в файловом кэше файла смещений в граничном файле
const unsigned int cuiOffsetsFileBufferSize=3000000;
///размер буфера в файловом кэше файла данных в граничном файле
const unsigned int cuiDataFileBufferSize=3000000;
///максимальный размер единовременно добавляемых в граничный файл данных
const unsigned int cuiMaxWorkedBoundDataSize=0xffffffff;
///минимальный размер файлового кэша который можно сбросить на диск
const unsigned int cuiCashMinSizeToFlush=0;//10000000;
///коэффициент уменьшения размера буферов файлового кэша
const double cdbBufferSizeReductionKoef=1.5;

//---------------------------------------------

}
}
}
}
