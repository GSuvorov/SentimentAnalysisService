#ifndef BISON_D__WORK_SS7_SOURCES_CORE_DICTIONARIES_NUMBERDICTIONARY_NUMBERGRAMMAR_TAB_H
# define BISON_D__WORK_SS7_SOURCES_CORE_DICTIONARIES_NUMBERDICTIONARY_NUMBERGRAMMAR_TAB_H

# ifndef YYSTYPE
#  define YYSTYPE int
#  define YYSTYPE_IS_TRIVIAL 1
# endif

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;

  int last_line;
  int last_column;
} yyltype;

# define YYLTYPE yyltype
# define YYLTYPE_IS_TRIVIAL 1
#endif

# define	trln	257
# define	mlrd	258
# define	mln	259
# define	thousand	260
# define	hundred	261
# define	hundreds	262
# define	dec	263
# define	ten	264
# define	unidec	265
# define	unit	266
# define	and	267
# define	hundreds_numeric	268
# define	digit	269
# define	unknown	270


#endif /* not BISON_D__WORK_SS7_SOURCES_CORE_DICTIONARIES_NUMBERDICTIONARY_NUMBERGRAMMAR_TAB_H */
