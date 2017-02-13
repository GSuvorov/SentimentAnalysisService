#pragma once

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

const unsigned int arrMasks[]={0x0000007f, 0x00003f80, 0x001fc000, 0x0fe00000, 0xf0000000};
const unsigned char arrOffsets[]={0, 7, 14, 21, 28};

//--------------------------------------------------------------------//

///функциональность для сжатия и расжатия методом ByteAlign
//в сжатом байте старщий бит является флагом того 
//что для сжатого числа требуется еще один байт (остальные 7 бит значения), и т.д.
//таким образом для сжатия 32 бит максимум нужно 36 бит, а минимум 8 бит.
class CByteAlignCompress
{
public:
	typedef vector<unsigned char> TBytesBuffer;

	///сжимает 4 байта и кладет буфер
	void EncodeToBuffer(unsigned int uiValue, TBytesBuffer* pBytesBuffer) const;
	///сжимает 4 байта и кладет буфер в обратном порядке
	void ReverseEncodeToBuffer(unsigned int uiValue, TBytesBuffer* pBytesBuffer) const;

	///разжимает в 4 байта из буфера, начиная с определенного байта, возвращает сколько байт доставалось
	unsigned int Decode(unsigned char* pBytesBuffer, unsigned int uiBytesBufferSize, unsigned int uiStartByte, unsigned int* puiWorkedBytesNumber) const;
	///разжимает в 4 байта из буфера сжатые при помощи ReverseEncodeToBuffer, 
	//начиная с определенного байта, возвращает сколько байт доставалось, движение происходит в обратном направлении от uiStartByte
	//к началу
	unsigned int ReverseDecode(unsigned char* pBytesBuffer, unsigned int uiBytesBufferSize, unsigned int uiStartByte, unsigned int* puiWorkedBytesNumber) const;

	///сжимает 4 байта и кладет сжатые данные по указателю возвращает количество значащих байт, 
	///если puiEncodedValue=NULL , просто вернет количество байт на закодированное значение
	unsigned int Encode(unsigned int uiValue, unsigned int* puiEncodedValue) const;
	///разжимает 4 байта 
	void StartDecode(void){m_ucCurrentDecodedByte=0;};
	bool Decode(unsigned char ucValue, unsigned int* puiDecodedValue);
	
	///маска для выделения 7 бит
	static const unsigned int m_cuiMask=0x0000007f;
	///маска для выставляения значащего 8-го бита
	static const unsigned char m_cucSignBit=0x80;
	///маска для снятия значащего 8-го бита
	static const unsigned char m_cucUnsignBit=0x7f;

	///возвращает количество байтов нужных для кодировки данного значения
	unsigned char GetNeededBytesNumber(unsigned int uiValue) const
	{
		unsigned char i=5;
		unsigned char ucBytesNumber=0;
		if(!uiValue){
			//если на входе ноль для сразу 1на 7-ми битка
			ucBytesNumber=1;
		}else{
			//для остального определяем количество 7-ми биток
			do{
				--i;
				if(uiValue&arrMasks[i]){
					ucBytesNumber=i+1;
					break;
				}
			}while(i!=0);
		}	
		return ucBytesNumber;
	};

protected:

	///текущий декодируемый байт
	unsigned char m_ucCurrentDecodedByte;
};

//--------------------------------------------------------------------//

}
}
}
}
}