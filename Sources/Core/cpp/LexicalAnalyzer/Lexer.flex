
%option c++
%option noyywrap
%option nostdinit
%option stack
%option prefix="Lexer"

%x NUMBERWORD
%s RESTORE


tire		[\x96\x2013\x2014]
defis		[-]
blank		[† \t]

leftBracket		\x28
rightBracket	\x29

lat_word	[a-zA-Z_][a-zA-Z_0-9]*
dot			"."
location	({lat_word}{dot})+{lat_word}
path		("/"{lat_word})*(({dot}{lat_word})|"/")?
ip			{digit}{1,3}({dot}{digit}{1,3}){3}

  /* lat_letter	[a-zA-Z\xC0-\xD6\xD8-\xF6\xF8-\x17F] */
ruSmallLetter 			[\x430-\x44F]
ruBigLetter 			[\x410-\x42F]
ruSmallAdditionalLetter	[\x450-\x45F]
ruBigAdditionalLetter	[\x400-\x40F]
ruAdditionalLetter 		{ruBigAdditionalLetter}|[\x450\x451]

  /*ru_letter   		[\x401-\x451] */
ru_letter   			{ruSmallLetter}|{ruBigLetter}|{ruAdditionalLetter}

latSmallLatter [a-z]
latBigLatter [A-Z]
latLetter {latSmallLatter}|{latBigLatter}

  /* –асширенный латиница-1*/
  /* http://unicode-table.com/ru/#latin-1-supplement */
latSmallExtended1  	[\xE0-\xF6\xF8-\xFF] 
latBigExtended1 	[\xC0-\xD6\xD8-\xDE]
latBeta				\xDF

  /* latExtended1		[\xC0-\xD6\xD8-\xF6\xF8-\xFF] */
latExtended1		 {latSmallExtended1}|{latBigExtended1}|{latBeta}

  /* –асширенный латиница-ј */
  /* http://unicode-table.com/ru/#latin-extended-A */
latExtendedA [\x100-\x17F]


  /*letter		[a-zA-Z\x401-\x451\xC0-\xD6\xD8-\xF6\xF8-\x17F]  */
letter		{latLetter}|{latExtended1}|{latExtendedA}|{ru_letter}
  
  /*sletter		[a-z\x430-\x45F\xE0-\xF6\xF8-\xFF] */
sletter		{latSmallLatter}|{latSmallExtended1}|{ruSmallLetter}|{ruSmallAdditionalLetter}

  /* bigLetter		[A-Z\x410-\x42F\x401-\x42F\xC0-\xD6\xD8-\xDE]  */
bigLetter	{latBigLatter}|{ruBigLetter}|{latBigExtended1}|{ruBigAdditionalLetter}

digit		[0-9]
num2		{digit}{2}
num24		{digit}{2}|{digit}{4}

small_word	{letter}{1,5}{digit}?
lquote		[\"УЂ\x201C]
rquote		[\"Фї\x201D]
apostroph	[`'\x2018\x2019]
word_part	{letter}({letter}|{digit})*
word		{word_part}({defis}({word_part}|{digit}+))*
ru_word		{ru_letter}({ru_letter}|{defis})*

line_break	"\n"|"\r\n"
sent_break	[.;?!Е\x2026]|({defis}{2,})
para_break	{line_break}{2,}|({line_break}{blank})
list_break	[Х\x2022]
num_break	{apostroph}|[.,† ]
date_break	[-./]
colon		":"
slash		"/"
time_break	[:]

/*romanNumber [IVXLCDM]*/

wwwWord 		("www"|"www2")
httpWord		"http"
DomainName		("ru"|"com"|"su"|"org"|"co.uk"|"co.jp"|"co.in"|"co.at")
HTMLExtension	("htm"|"html")

%%

<NUMBERWORD>
{
	{digit}+									return accNumber;
	{letter}+									return accWord;
	{defis}										return accDefis;
	{line_break}								;
	\0											return 0;
	.											return accSign;

}

<RESTORE>
{


	
	
	{wwwWord}|{httpWord}|{DomainName}|{HTMLExtension}		return accURLPart;

}
	/*URL*/
	/* http://www.cs.man.ac.uk/~pjj/cs211/flexdoc.html */
	/*({httpWord}{colon}{slash}{slash})?(({wwwWord}|{word}){dot})?({word}{dot})+({DomainName}|{small_word}){path}		return accURL;*/

	/*********************************** Roman Numbers ***********************************************/
	/*({romanNumber})+{defis}?("th"|"ый"|"ом"|"ому")?								return accRomanNumber;*/
	
	/************************************* Acronym Citations *****************************************/

	({bigLetter})+								return accAcronymCitation; 
	{lquote}|{rquote}							return accLQuote;


	/***************************************** Numbers ***********************************************/

	{digit}+									return accNumber;
	/*											{*/
	/*												if(	yyleng > 8 ||*/
	/*													yyleng == 8 && (UINT)_ttoi(yytext) >= MAX_NUMERIC_VALUE)*/
	/*												{*/
	/*													return accLongNumber;*/
	/*												}*/
	/*												return accNumber;*/
	/*											}*/

	{digit}*"1st"								return accOrdinalNumber;
	{digit}*"2nd"								return accOrdinalNumber;
	{digit}*"3d"								return accOrdinalNumber;
	{digit}+"th"								return accOrdinalNumber;

	/********************************************* Words *********************************************/

	{digit}+({defis}{line_break}*{letter}+)+	return accNumberWord;

	{bigLetter}"."								return accInitial;

	{bigLetter}{letter}*{defis}{digit}+{letter}*	return accWord;
	
	{bigLetter}{letter}*{defis}{line_break}{digit}+{letter}*	{
																Normalize(yytext, _T("\r\n"));
																return accWord;
															}
	{letter}({sletter}|{digit}){0,4}			return accSmallWord;
	<RESTORE>{lat_word}({defis}{lat_word})*		return accEngWord;

	{word}({defis}{line_break}{word})+			{
													Normalize(yytext, _T("\r\n"));
													return accWord;
												}
	
	{word}										return accWord;	


	/******************************************* Breakers ********************************************/

	{para_break}								return accParaBreak;
	{line_break}								m_iCharCount += yyleng;
	"."											return accPoint;
	{sent_break}								{
#ifdef _ASCLIT
													if(*yytext == _T(';') || *yytext == _T('-'))
														return accWord;
#endif
													return accSentBreak;
												}


	/********************************************* Symbols *******************************************/

	{tire}										return accTire;

	{leftBracket}								return accLeftBracket;
	{rightBracket}								return accRightBracket;

	{blank}{defis}{blank}						{
													_tcscpy(yytext, _T("Ц"));
													return accTire;
												}
	{defis}										return accDefis;
	{blank}										m_iCharCount += yyleng;
	","											return accComma;
	slash										return accSlash;
	
	{apostroph}									{
													_tcscpy(yytext, _T("'"));
													return accApostroph;
												}

	"&"											return accAmpersand;
	"@"											return accAt;
	{colon}											return accColon;
	\0											return 0;
	.											return accSign;

%%
