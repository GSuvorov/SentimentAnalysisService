
%option c++
%option noyywrap
%option nostdinit
%option prefix="Walker"

  /*Цитаты*/
%s QUOTATIONS
  /*Даты*/
%s DATES			
  /*Имена*/
%s NAMES
  /*Организации*/
%s ORGANIZATIONS
  /*Места*/
%s PLACES
  /*Повторно анализируемые объекты*/
%s REPARSEDITEMS
  /**/
%s LSEQUENCES
  /*Запрос*/
%s QUERY
  /**/
%s MULTINAMES
  /*Подпоследовательности*/
%s INSEQUENCES
  /* Телефоны */
%s TELEPHONES

/*Слово*/
Word				\x11	
/*Число*/
Number				\x12	
/*Месяц + Время года*/
Month				\x13	
/**/
LWord				\x14	
/*Фамилия (Last Name)*/
LName         		\x15	
/*Имя(First Name)*/
FName         		\x16	
/*Отчество (Middle Name)*/
MName         		\x17	
/*Инициалы*/
Initial       		\x18	
/**/
Genitive      		\x19	
/*Число - День*/
NumberDay     		\x1A	
/*Число -Год*/
NumberYear    		\x1B	
/*Запятая ,*/
Comma         		\x1C	
/*Апостроф*/
Apostroph			\x1D	
/**/
SmallWord			\x1E	
/*Слово - Век*/
WordCentury   		\x1F	
/*Слово - год, полугодие*/
WordYear      		\x20	
/*Дефис*/
Defis         		\x21	
/*Слово - день*/
WordDay       		\x22	
/**/
DMY           		\x23	
/**/
GenConj       		\x24	
/**/
Org           		\x25	
/**/
SpecificOrg   		\x26	
/**/
Quote				\x27	
/**/
RQuote				\x28	
/*Город*/
City          		\x29	
/*Штат/государство*/
State         		\x2A	
/*Страна*/
Country       		\x2B	
/**/
Quotation     		\x2C	
/* Часть имени: "д" "фон" "ди" "ван" "да" "де" "von" "di" "van" "da" "de" "d" */
NameParticle  		\x2D	
/* & */
Ampersand     		\x2E	
/*Тире*/
Tire          		\x2F	
/**/
ThreeDigits      		\x30	
/**/
MeasureDay			\x31	
/**/
MeasureMonth		\x32	
/*Имя*/
Name				\x33	
/**/
NonSyntax			\x34	
/* : */
Colon				\x35	
/* / */
Slash				\x36	
/*Точка*/
Point				\x37	
/*Английское слово*/
EngWord				\x38	
/*Буква S*/
LetterS				\x39	
/*Часть URL*/
URLPart				\x3A	
/* @ */
At					\x3B	
/*Номер римский*/
RomanNumber			\x3C	
/**/
STerminated			\x3D	
/*Порядковое окончание (ый, ой...)*/
OrdinalParticle 	\x3E	
/*Буква T*/
LetterT         	\x3F	
/**/
Volume          	\x40	
/*Годовой квартал*/
Quarter         	\x41	
/**/
To					\x42	
/**/
LSmallWord			\x43	
/**/
CityState			\x44	
/**/
QListWord			\x45	
/**/
QThe				\x46	
/**/
QNames				\x47	
/**/
QOf					\x48	
/**/
QWhat				\x49	
/**/
QAre				\x4A	
/*Глагол*/
Verb				\x4B	
/*Имя собственное*/
ProperWord			\x4C	
/*область, автономный округ*/
GeoSemantic			\x4D	
/*Предлог перед датой*/
/*с/С/по/По/from/to/до/До*/
DatePreposition     \x4E	
/**/
ProperWordUnknown   \x4F	
/**/
OrgAbbr				\x50	
/**/
GeoConj				\x51	
/**/
GeoSemanticPre      \x52	
/**/
GenetiveAdj			\x53	
/**/
GenetiveNoun		\x54	
/**/
NounSeqRoot			\x55	
/**/
DativeAdj			\x56	
/**/
DativeNoun			\x57	
/**/
DativeAndGenAdj     \x58	
/**/
DativeAndGenNoun    \x59	
/**/
LQuote				\x5A
/**/
OrdinalNumber		\x5B	
/**/
YearQuarter			\x5C
/*2 цифры*/
TwoDigits      		\x5D	
/*1 цифра*/
OneDigit      		\x5E	
/*Время дня - утро, вечер*/
DayTime      		\x5F
/*Десятилетие, Декада (semtitIndef)*/
WordDecade      	\x60	
/*4 цифры*/
FourDigits      	\x61
/*5 цифр*/
FiveDigits      	\x62
/*6 цифр*/
SixDigits      		\x63
/*7 цифр*/
SevenDigits      	\x64
/*8 цифр*/
EightDigits      	\x65
/*9 цифр*/
NineDigits      	\x66
/*10 цифр*/
TenDigits      		\x67
/*11 цифр*/
ElevenDigits		\x68
/*Левая скобка*/	
LeftBracket			\x69
/*Правая скобка*/
RightBracket		\x6A
/*Слова - номер телефона,телефон, факс*/
TelephoneWord		\x6B
/*URL*/
URL					\x6C
/*Знак "+"*/
PlusSign			\x6D
/* Слова - доб., доб - означающие добавочный номер */
TelephoneAdditionWord \x6E


NotLastName [^\x4F\x15]

NotQuote    [^\x27\x28\x5A]
NotVerbAndQuote         [^\x4B\x27\x28\x5A]

NotApostroph  [^\x1D]
NotVerbAndApostroph [^\x4B\x1D]

SumProperWord  {ProperWord}|{ProperWordUnknown}


F       ({NameParticle}{Apostroph}?({LName}|{Name}|{FName}|{MName}))|{LName}|{Name}|{MName}
I       {Name}|{FName}
O       {MName}

Day       {NumberDay}
Year      {NumberYear}
Century     {NumberDay}|{NumberYear}|{RomanNumber}

  /* 15th of September | 15 of September | 15 сентября | September 15th */
Date      ({Day}{GenConj}?{Month})|({Month}{Day})

/*Разделительная черта*/
Hyfen     {Defis}|{Tire} 

LNoun     {LWord}|{Genitive}
ComplNoun   {LNoun}(({Defis}|{Ampersand}){LNoun})?
ComplSequence {ComplNoun}{0,3}

OneToTenLengthNumber	{OneDigit}|{TwoDigits}|{ThreeDigits}|{FourDigits}|{FiveDigits}|{SixDigits}|{SevenDigits}|{EightDigits}|{NineDigits}|{TenDigits}
AnyNumber   			{Number}|{NumberDay}|{NumberYear}|{OneToTenLengthNumber}
LongNumber    			({Number}|{OneToTenLengthNumber}){OneToTenLengthNumber}*

					/* OLD VERSIONS */
	/* AnyNumber   			{Number}|{NumberDay}|{NumberYear} */
	/* LongNumber    			({Number}|{ThreeDigits}){ThreeDigits}* */


NumBreak    {Point}|{Comma}|{Apostroph}
DateBreak   {Defis}|{Point}|{Slash}
PhoneBreak   {Hyfen}

AnyEngWord    {EngWord}|{URLPart}|{LetterS}|{GenConj}|{STerminated}|{OrdinalParticle}|{LetterT}
InetPath    ({Slash}{AnyEngWord})*(({Point}{AnyEngWord})|{Slash})?
InetLocation  ({AnyEngWord}{Point})+{URLPart}
ProtoBreak    {Colon}{Slash}{2}

AnyWord     {Word}|{LWord}|{LSmallWord}
LSLWord     {LWord}|{LSmallWord}|{Initial}|{City}

RCity     {City}|{CityState}
RState      {State}|{CityState}

Decade {WordDecade}|{MeasureMonth}

TelephonePrefix			{TelephoneWord}{Point}?
CountryCode				{PlusSign}?({OneDigit}|{TwoDigits}|{ThreeDigits})
CityCode				{LeftBracket}?({OneDigit}|{TwoDigits}|{ThreeDigits}|{FourDigits}|{FiveDigits}){RightBracket}?
SevenDigitsNumber		({ThreeDigits}{PhoneBreak}?{TwoDigits}{PhoneBreak}?{TwoDigits})|({ThreeDigits}{PhoneBreak}?{FourDigits})|{SevenDigits}
 /* ({TwoDigits}{PhoneBreak}?{TwoDigits}{PhoneBreak}?{TwoDigits})| */
SixDigitsNumber			{SixDigits}
FiveDigitsNumber		({OneDigit}{PhoneBreak}?{TwoDigits}{PhoneBreak}?{TwoDigits})|({ThreeDigits}{PhoneBreak}?{TwoDigits})|{FiveDigits}
TenDigitsNumber			{OneDigit}?{TenDigits}
ElevenDigitsNumber		({PlusSign}?{ElevenDigits})|({CountryCode}{PhoneBreak}?{CityCode}{PhoneBreak}?{SevenDigitsNumber})
AdditionalNumber		({Comma}?{TelephoneAdditionWord}{Point}?({ThreeDigits}|{FourDigits}))

Time					({TwoDigits}|{NumberDay}){Colon}({TwoDigits}|{NumberDay})({Colon}({TwoDigits}|NumberDay))?

%%

  /* flex provides a mechanism for conditionally activating rules. */
  /* Any rule whose pattern is prefixed with `<sc>' will only be active when the scanner is in the start condition named sc. */
  /* примеры -> <QUOTATIONS> <REPARSEDITEMS> <DATES> <MULTINAMES> <NAMES> <ORGANIZATIONS> <LSEQUENCES> <QUERY>*/

<QUOTATIONS>
{
  /* «Газпром нефть» */
  /* «Газпром-Медиа Холдинг» */
{Quote}({ProperWord})({NotVerbAndQuote}{0,4}){Quote}          return lpQuotation; 
{LQuote}({ProperWord})({NotVerbAndQuote}{0,4}){RQuote}          return lpQuotation;
{Apostroph}({ProperWord})({NotVerbAndApostroph}{0,4}){Apostroph}      return lpQuotation;
{Quote}{NotVerbAndQuote}{Quote}									return lpQuotationSimple;

}

<REPARSEDITEMS>
{
  /* 1223.253,144'213 */
({Number}|{ThreeDigits})({NumBreak}?{ThreeDigits})+         return lpComplexNumber;

  /* с 15:50 до 16:00 */
{DatePreposition}{Time}{DatePreposition}{Time}				return lpTimeInterval;

  /* 22:25[:14] */ /*+*/
{Time}               										return lpTime;

  /* 22-25 */ /*+*/
({TwoDigits}|{NumberDay}){Hyfen}({TwoDigits}|{NumberDay})	return lpTime1;

  /*8/11/1989*/
  /*8-11-1989*/
  /*8.11.1989*/ /*+*/
  ({TwoDigits}|{OneDigit}){DateBreak}({TwoDigits}|{OneDigit}){DateBreak}({FourDigits}|{ThreeDigits}|{TwoDigits})          return lpDMY6;

  /*1989/11/8*/
  /*1989-11-8*/
  /*1989.11.8*/ 
  {FourDigits}{DateBreak}({TwoDigits}|{OneDigit}){DateBreak}({TwoDigits}|{OneDigit}) return lpYMD;

  					/* OLD VERSIONS */
	/* {Number}{DateBreak}{Number}{DateBreak}{Number}          return lpDMY6; - OLD VERSION*/

  /*138ой*/
({AnyNumber}{Hyfen}?{OrdinalParticle})|({OrdinalNumber})          return lpOrdinalNumber;
  
  					/* OLD VERSIONS */
	/*(({Number}|{ThreeDigits}){Hyfen}?{OrdinalParticle})|({OrdinalNumber})          return lpOrdinalNumber;*/
 
  /* 12/3 , 123/321 */
{AnyNumber}{Slash}{AnyNumber}       return lpSimpleFraction;
  
    					/* OLD VERSIONS */
	/*({Number}|{ThreeDigits}){Slash}({Number}|{ThreeDigits})       return lpSimpleFraction;*/

  /* it's , what's*/
{EngWord}{Apostroph}{LetterS}                 return lpGenitiveEng;
  /* Newtons' */
{STerminated}{Apostroph}                    return lpGenitiveEng;

  /* bozhko@mail.ru */
{AnyEngWord}{At}{InetLocation}                  return lpInetLocation;

  /* http://www.cs.man.ac.uk/~pjj/cs211/flexdoc.html */
{AnyEngWord}({ProtoBreak}|{Point}){InetLocation}{InetPath}    return lpInetLocation;

  /* google.com */
{InetLocation}                          return lpInetLocation;

  /* don't , aren't */
{AnyEngWord}{Apostroph}{LetterT}                return lpNegVerb;

}

<TELEPHONES>
{
{TelephonePrefix}?(({CountryCode}{CityCode})|({CityCode}))?{SevenDigitsNumber}{AdditionalNumber}?			return lpSevenDigitsPhoneNumber;
{TelephonePrefix}?(({CountryCode}{CityCode})|({CityCode}))?{SixDigitsNumber}{AdditionalNumber}?				return lpSixDigitsPhoneNumber;
({TelephonePrefix}|({CountryCode}?{CityCode}))({TwoDigits}{PhoneBreak}?{FourDigits}){AdditionalNumber}?		return lpSixDigitsPhoneNumber;
{TelephonePrefix}?(({CountryCode}{CityCode})|({CityCode}))?{FiveDigitsNumber}{AdditionalNumber}?			return lpFiveDigitsPhoneNumber;

{TelephonePrefix}?{TenDigitsNumber}{AdditionalNumber}?														return lpTenDigitsPhoneNumber;
{TelephonePrefix}?{ElevenDigitsNumber}{AdditionalNumber}?													return lpElevenDigitsPhoneNumber;
}

<DATES>
{ 
  /*ночь, утро, день, вечер, полдень, полночь*/ 
{DayTime} 						return lpDayTime; 
    
 /* с 13 по 15 июня 2000 года*/ /*+*/ 
{DatePreposition}{AnyNumber}{DatePreposition}{Date}{Comma}?{Year}{WordYear}? 	return lpDYI;
 
  /* C 15 по 20 сентября*/
{DatePreposition}{NumberDay}{DatePreposition}{Date}   return lpDI;

  /* 15th of September | 15 of September | 15 сентября | September 15th */ /*+*/ 
{Date}        return lpDMW2;

  /* 2001-2003th years, 2001-2003-е годы */ 
{Year}{Hyfen}{Year}{WordYear}                 return lpYear;

  /* 21th century, 21-й век, XXth century, XX-й век */
{Century}{Hyfen}?{SmallWord}?{WordCentury}                      return lpCentury;


  /* 2000-ом    */
{Year}{Hyfen}{OrdinalParticle}                 return lpNumericYear;

  /* 1999 September 15th; 2009 год, 13 сентября */
{Year}{WordYear}?{Comma}?{Date}                 return lpDMY4;

  /* 2001`s years */
{Year}{Apostroph}{LetterS}{WordYear}				return lpYear1;

  /* 2001th year, 2001-й год */ /*+*/ 
{Year}{WordYear}                        return lpYear2;

  /* 90`s */ /*+*/
{TwoDigits}{Apostroph}?({LetterS}|{OrdinalParticle})                return lpNumericYear;

  /* I полугодие, II полугодие*/
({AnyNumber}|{RomanNumber}){WordYear} 						return lpYearPart1; 
 
  /* первое полугодие, 1*е полугодие, второе полугодие, 2*е полугодие*/
{AnyNumber}(({Apostroph})|({Hyfen}))?{OrdinalParticle}			return lpYearPart2; 

  /* 1-2 мая */ /*+*/
{Day}{Hyfen}{Day}{Month}                    return lpDM;

  /* 1-2 of May */ /*+*/
{Day}{Hyfen}{Day}{GenConj}{Month}               return lpDM1;

  /* Mon[,] [the] 15th of November[,] 1999 [year] */ /*+*/
{WordDay}{Comma}?{SmallWord}?{Date}{Comma}?{Year}{WordYear}?  return lpDMYW;

  /* mon[,] [the] 5th of May */ /*+*/
{WordDay}{Comma}?{SmallWord}?{Date}               return lpDMW;

  /* 15th of May, Mon */ /*+*/
{Date}{Comma}{WordDay}                      return lpDMW1;

  /* Июнь[,] 1999 [года] */ /*+*/
{Month}{Comma}?{Year}{WordYear}?                return lpYM;

  /* 15th of November[,] 1999 [year] */
{Date}{Comma}?{Year}{WordYear}?                 return lpDMY;

  /* 15th of November[,] 1999, Mon */ /*+-*/
{Date}{Comma}?{Year}{Comma}{WordDay}              return lpDMYW1;

  /* 1999-Jan-14 */ /*+*/
{Year}{Hyfen}{Month}{Hyfen}{Day}                return lpDMY1;

  /* 1999-Jan-14, Monday */ /*+*/
{Year}{Hyfen}{Month}{Hyfen}{Day}{Comma}{WordDay}    return lpDMYW3;


  /* 18-Jul-2005 [year] */ /*+*/
{Day}{Hyfen}{Month}{Hyfen}{Year}{WordYear}?           return lpDMY2;

  /* 18-Jul-2005 [year], Monday */ /*+*/
{Day}{Hyfen}{Month}{Hyfen}{Year}{WordYear}?{Comma}{WordDay}       return lpDMYW4;


  /* 15th of November' 1999 [year] */ /*+*/
{Date}{Apostroph}{Year}{WordYear}?                return lpDMY3;
  
  /* Tue[,] the 21-10-1997 [year] */ /*+*/
{WordDay}{Comma}?{SmallWord}?{DMY}{WordYear}?         return lpDMYW2;

  /* 29.03.2005 г. */ /*+*/
{DMY}{WordYear}                         return lpDMY5;

  /* quarter to seven */
  /*{Quarter}{To}{AnyNumber}                    return lpTime1; */

  /* Квартал */
{YearQuarter}							return lpMI;

  /* 1 квартал, IV квартал */
({AnyNumber}|{RomanNumber}){YearQuarter}			return lpMI1;

  /* квартал 1, квартал IV*/
{YearQuarter}({AnyNumber}|{RomanNumber})			return lpMI2;

  /* 2ой квартал, 2-ой квартал*/
{AnyNumber}({Apostroph}|{Hyfen})?{OrdinalParticle}{YearQuarter}  return lpMI3;
 
  /* Месяц */
{Month} 							return lpM;

   /* Год */
{Year}                             return lpYear2;

  /* 1ая декада века, I десятилетие [20] века */
({AnyNumber}|{RomanNumber}){Decade}({AnyNumber}|{RomanNumber})?{WordCentury}		 return lpDecade;

  /* Начало [20] века, середина века*/
{WordDecade}({AnyNumber}|{RomanNumber})?{WordCentury}								return lpDecade1;

  /* 1 тысячелетие, II тысячелетие*/
({AnyNumber}|{RomanNumber}){WordDecade}												return lpDecade2;

  /* выходные, выходные дни, weekend */
{WordDecade}{WordDay}?																return lpWeekEnd;

}

<MULTINAMES>
{
  /* Armin van Buuren , Армин ван Бюрен */
  {ProperWord}{NameParticle}{ProperWord}         return lpFI;
  
  /* Иванов Иван Иванович */
  {FName}{FName}{FName}                    return lpFIO;
  
  /* Михаил Булгаков */
  {FName}{FName}                     return lpFI;
}

<NAMES>
{


  /* de Marco */
{F}                               return lpF;

  /* Владимир Владимирович Путин, Путин Владимир В. */
{F}{I}({O}|{Initial})                 return lpFIO;


  /* Богдан Юрьевич Гаркушин, George W. Bush */
{I}({I}|{O}|{Initial}){F}                   return lpFIO;

  /* George Bush */
{I}{F}                              return lpFI;

  /* Путин Владимир */
{F}{I}                              return lpFI;

  /* Путин В. В. */
({F}|{ProperWord}|{ProperWordUnknown}){Initial}{2}        return lpFIO;

  /* В. В. Путин */
{Initial}{2}({F}|{ProperWord}|{ProperWordUnknown})        return lpFIO;

  /* Путин В. */
{F}{Initial}                          return lpFI;

  /* В. Путин */
{Initial}{F}                          return lpFI;

  /*{F}{I}{ProperWordUnknown}                   return lpFIO;*/

  /*{ProperWordUnknown}{F}{I}                   return lpFIO;*/



}

<ORGANIZATIONS>
{

  /* Банк "Русский Кредит" */
{Org}{Quotation}                        return lpOrg;

  /* HSBC Holdings */
{Quotation}{Org}                        return lpOrg;

  /* Organization of United Nations */
{ComplSequence}{Org}{GenConj}{SmallWord}?{LWord}{1,3}     return lpOrg;

  /* Организация Объединенных Наций */
{ComplSequence}{Org}{Genitive}{1,3}               return lpOrg;

  /* World Trade Organization, Компания Кока-Кола */
{ComplSequence}{Org}{ComplSequence}               return lpOrg;

  /* Hangzhou MSD Pharmaceutical Co. */
{SpecificOrg}{Org}                        return lpOrg;

  /* Компания Hangzhou MSD Pharmaceutical */
{Org}{SpecificOrg}                        return lpOrg;

  /* */
{ProperWord}{OrgAbbr}{1,3}                    return lpOrg;

  /* ВВЦ */
{OrgAbbr}{2,3}                          return lpOrg;


{OrgAbbr}{1,2}{GeoSemantic}                   return lpOrg;

}

<PLACES>
{

  /* Moscow, Russia */
{RCity}{Comma}{Country}                     return lpCityCountry;

  /* Dallas, Texas */
{RCity}{Comma}{RState}                      return lpCityState;

  /* Texas, USA */
{RState}{Comma}{Country}                    return lpStateCountry;

  /* автономный округ Корякский*/
{GeoSemantic}{ProperWordUnknown}                return lpGeoSemantic;

  /* Удмуртская область, Корякский автономный округ,  */
{ProperWordUnknown}({GeoSemantic}{1,2})                 return lpGeoSemantic; 

({ProperWordUnknown}{Comma}){0,4}{ProperWordUnknown}({GeoConj}{ProperWordUnknown})?({GeoSemantic}{1,2})   return lpGeoSemantic; 

  /* гоод Урюпинск, городов Москва и Сыктывкар */
{GeoSemanticPre}{ProperWord}({GeoConj}({ProperWord})){0,1} return lpGeoSemantic;  

{GeoSemanticPre}{ProperWordUnknown}({GeoConj}({ProperWordUnknown})){0,1} return lpGeoSemantic;  
}


<LSEQUENCES>
{

  /* R&B */
{LWord}{1,4}{Ampersand}{LWord}{1,3}               return lpSimpleSequence;

  /* any sequence of LWords */
{LSLWord}{2,}                         return lpLSequence;

{LSLWord}{1,2}{GeoSemantic}                   return lpLSequence;

{LSLWord}{GeoSemantic}{LSLWord}                 return lpLSequence;

  /* float numbers should be joined after dates */
{LongNumber}({Point}|{Comma}){LongNumber}           return lpFloat;
}

<INSEQUENCES>
{
   /*{NounSeqRoot}{GenetiveNoun} return lpNounSequence;*/
  
    /*{NounSeqRoot}({GenetiveAdj}?{GenetiveNoun}{Comma}){1,3} return lpNounSequence;*/
 
  /*{NounSeqRoot}({GenetiveAdj}?{GenetiveNoun}{Comma}){1,3}({GenetiveAdj}?{GenetiveNoun})({GenConj}({GenetiveAdj}?{GenetiveNoun}))? return lpNounSequence;*/
   
  /*{NounSeqRoot}{GenetiveAdj}?{GenetiveNoun}{GenConj}{GenetiveAdj}?{GenetiveNoun}  return lpNounSequence;*/
   
  /*{NounSeqRoot}({DativeAdj}?{DativeNoun}{Comma}){1,3}({DativeAdj}?{DativeNoun})({GenConj}({DativeAdj}?{DativeNoun}))? return lpNounSequence;*/
   
  /*{NounSeqRoot}{DativeAdj}?{DativeNoun}{GenConj}{DativeAdj}?{DativeNoun}  return lpNounSequence;*/

 /*{NounSeqRoot}{1,2}({DativeAndGenAdj}?{DativeAndGenNoun}{Comma}){1,3}({DativeAndGenAdj}?{DativeAndGenNoun})({GenConj}({DativeAndGenAdj}?{DativeAndGenNoun}))? return lpNounSequence;*/
   
 /*{NounSeqRoot}{DativeAndGenAdj}?{DativeAndGenNoun}{GenConj}{DativeAndGenAdj}?{DativeAndGenNoun} return lpNounSequence;*/


}

<QUERY>
{

  /* List the names of... */
{QListWord}{QThe}?{QNames}{QOf}                 return lpList;

  /* What are the names of... */
{QWhat}{QAre}{QThe}?{QNames}{QOf}               return lpList;

  /* List/Name... */
{QListWord}                           return lpList;

}

.                               return lpWord;


%%
