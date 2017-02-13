
%option c++
%option noyywrap
%option nostdinit
%option prefix="WordBreaker"

%x MODE_CONFIG
%x MODE_TEXT


blank		[  \t]
lat_letter	[346a-zA-Z\xC0-\xD6\xD8-\xF6\xF8-\x17F'`]
rus_letter	[\x401-\x451]
number		[0-9]
line_break	"\n"|"\r\n"

%%

<MODE_CONFIG>
{

{number}{1,4}													return ltWeight;
{rus_letter}*													return ltRus;
{lat_letter}*													return ltLatNum;
";"																return ltBreaker;
{line_break}													m_iCharCount = 1; yylineno++;
{blank}*														m_iCharCount += yyleng;
\0																return 0;
.																return ltNoWord;

}

<MODE_TEXT>
{

{number}*														return ltNoWord;
{rus_letter}{1,25}												return ltRus;
{lat_letter}{1,25}												return ltWord;
\0																return 0;
({blank}|{line_break})*											return ltNoWord;
.																return ltNoWord;

}

%%
