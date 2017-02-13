
%option c++
%option noyywrap
%option nostdinit
%option prefix="IRLoader"


Undefined					\x00

Word						\x11
SentBreak					\x12
ParaBreak					\x13
Number						\x14
Abbrev						\x15
AcronymCitation				\x16
LQuote						\x17
RQuote						\x18
Initial						\x19
Comma						\x1A
Apostroph					\x1B
Ampersand					\x1C
SmallWord					\x1D
Defis						\x1E
Tire						\x1F
Sign						\x20
Point						\x21
Slash						\x22
Break						\x23
LongNumber					\x24
FirstComplexNumberPart		\x25
ComplexNumberPart			\x26
Colon						\x27
EngWord						\x28
URLPart						\x29
At							\x2A
UnclosedSentence			\x2B
TitleBreak					\x2C
Ordinal						\x2D
RomanNumber					\x2E


AbbrevBreaker				{Point}|{Slash}|({Point}{Slash})
AbbrevItem					{SmallWord}{AbbrevBreaker}

AnyWord						{EngWord}|{URLPart}|{Word}|{SmallWord}
AnyEngWord					{EngWord}|{URLPart}
InetPath					({Slash}{AnyEngWord})*(({Point}{AnyEngWord})|{Slash})?
InetLocation				({AnyEngWord}{Point})+{URLPart}
ProtoBreak					{Colon}{Slash}{2}


%%

	/* Prevent breaking sentence on {Point} */
{AnyEngWord}{At}{InetLocation}									return accWord;
{AnyEngWord}({ProtoBreak}|{Point}){InetLocation}{InetPath}		return accWord;
{InetLocation}													return accWord;

{Number}({Point}{Number})+										return accWord;

	/* Detect abbreviations */
{SmallWord}{Slash}{SmallWord}									return accAbbrev;
{AbbrevItem}{1,10}												return accAbbrev;
({AbbrevItem}|{Initial}){2,10}									return accAbbrev;

{AnyWord}{Defis}{ParaBreak}?{AnyWord}							return accBrokenWord;

{Point}+														return accPoint;
{SentBreak}+													return accSentBreak;

[^\x12\x13\x21\x23\x2C]{ParaBreak}								return accUnclosedSentence;

\0																return 0;
.																return *yytext;


%%
