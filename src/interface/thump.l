D                       [0-9]
L                       [a-zA-Z_]
H                       [a-fA-F0-9]
E                       [Ee][+-]?{D}+

%{

#include <stdio.h>
#include <string.h>
#include "thump.h"
#include "syntree.h"
#include "symtable.h"
#include "thump.tab.hpp"

// int fileno(FILE *stream);

void count();
int line = 1;
extern int column;
void comment();
int check_type();
char ayytext[1048576];

void acount()
{
        strcat(ayytext, yytext);
        count();
}

%}

%option always-interactive

%%
"append"                      { acount(); return(APPEND); }
"as"                          { acount(); return(AS); }
"find"                        { acount(); return(FIND); }
"help"                        { acount(); return(HELP); }
"in"                          { acount(); return(IN); }
"list"                        { acount(); return(LIST); }
"replace"                      { acount(); return(REPLACE); }
"show"                        { acount(); return(SHOW); }
"sort"                        { acount(); return(SORT); }
"unique"                        { acount(); return(UNIQUE); }
"user"                        { acount(); return(USER); }

":::descending"                 { acount(); return(DESCENDING); }
":::d"                          { acount(); return(DESCENDING); }

"isnull"                 { acount(); return(ISNULL); }

"GIS_AsText"               { acount(); return(GIS_ASTEXT); }
"GIS_AsKML"               { acount(); return(GIS_ASKML); }
"GIS_GeomFromLongLat"         { acount(); return(GIS_GEOMFROMLONGLAT); }
"GIS_Distance"             { acount(); return(GIS_DISTANCE); }
"GIS_DWithin"             { acount(); return(GIS_DWITHIN); }
"GIS_Within"             { acount(); return(GIS_WITHIN); }
"GIS_Union"             { acount(); return(GIS_UNION); }
"GIS_LongLat"             { acount(); return(GIS_LONGLAT); }

"median"             { acount(); return(MEDIAN); }

"HTTP/1."{D}        {
				acount(); 
				return(HTTP);
			}
\r?\n([^\r\n]+":"[^\r\n]*\r?\n)*\r?\n	        {
				acount(); 
				return(END);
			}

":or"                    { acount(); return(OR_OP); }
"%3aor"                    { acount(); return(OR_OP); }
":and"                    { acount(); return(AND_OP); }
"%3aand"                    { acount(); return(AND_OP); }

"%7C%7C"                    { acount(); return(OR_OP); }
"%26%26"                    { acount(); return(AND_OP); }
"%26"                     { acount(); return('&'); }
"%7C"                   { acount(); return('|'); }
"%3C"                   { acount(); return('<'); }
"%3E"                   { acount(); return('>'); }
"%3C="                    { acount(); return(LE_OP); }
"%3E="                    { acount(); return(GE_OP); }
"%28"                { acount(); return('('); }
"%29"                { acount(); return(')'); }

{L}({L}|{D})*           { acount(); return(check_type()); }

{D}+               { acount(); yylval.sym = symadd(yytext); return(CONSTANT); }
{D}+{E}            { acount(); yylval.sym = symadd(yytext); return(CONSTANT); }
{D}*"."{D}+({E})?  { acount(); yylval.sym = symadd(yytext); return(CONSTANT); }
{D}+"."{D}*({E})?  { acount(); yylval.sym = symadd(yytext); return(CONSTANT); }

(\"|\%22)(\\.|\%([a-zA-z0-13-9][a-zA-z0-9]|[a-zA-z0-9][a-zA-z0-13-9])|[^\\"\%\r\n])*(\"|\%22) {
                                          acount(); 
                                          yylval.sym = symaddstr(yytext);
                                          return(STRING_LITERAL);
                                      }

"::"                    { acount(); return(DCOLON); }

"="                     { acount(); return('='); }
"<"                     { acount(); return('<'); }
">"                     { acount(); return('>'); }
"&&"                    { acount(); return(AND_OP); }
"||"                    { acount(); return(OR_OP); }
"<="                    { acount(); return(LE_OP); }
">="                    { acount(); return(GE_OP); }
"=="                    { acount(); return(EQ_OP); }
"!="                    { acount(); return(NE_OP); }


"-"                     { acount(); return('-'); }
"*"                     { acount(); return('*'); }
","                     { acount(); return(','); }
";"                     { acount(); return(';'); }
":"                     { acount(); return(':'); }
"("                     { acount(); return('('); }
")"                     { acount(); return(')'); }
"{"                     { acount(); return('{'); }
"}"                     { acount(); return('}'); }
"."                     { acount(); return('.'); }
"|"                     { acount(); return('|'); }
"&"                     { acount(); return('&'); }
"?"                     { acount(); return('?'); }
"!"                     { acount(); return('!'); }
"/"                     { acount(); return('/'); }

"@"                     { acount(); return('/'); }

([ \t\v\r\n\f]|"%20"|[+])   { acount(); }
.                       { acount(); }

%%

/*
({L}|{D})({L}|{D}|".")*  {
				acount(); 
				yylval.sym = symadd(yytext);
				return(FILENAME);
			}
*/

int yywrap()
{
        return(1);
}


void comment()
{
        char c, c1;

loop:
        while ((c = yyinput()) != '*' && c != 0)
                putchar(c);

        if ((c1 = yyinput()) != '/' && c != 0)
        {
                unput(c1);
                goto loop;
        }

        if (c != 0)
                putchar(c1);
}


int column = 0;

void count()
{
        int i;

        for (i = 0; yytext[i] != '\0'; i++)
                if (yytext[i] == '\n') {
                        column = 0;
			line++;
		}
                else if (yytext[i] == '\t')
                        column += 8 - (column % 8);
                else
                        column++;

/*        ECHO; */
}


int check_type()
{
/*
* pseudo code --- this is what it should check
*
*       if (yytext == type_name)
*               return(TYPE_NAME);
*
*       return(IDENTIFIER);
*/

/*
*       it actually will only return IDENTIFIER
*/

	yylval.sym = symadd(yytext);
        return(IDENTIFIER);
}

