#ifndef BISON_D__WORK_SS_SOURCES_CORE_TRANSLIT_CONFIGREADER_TAB_H
# define BISON_D__WORK_SS_SOURCES_CORE_TRANSLIT_CONFIGREADER_TAB_H

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

# define	t_rus	257
# define	t_lat	258
# define	t_weight	259
# define	t_breaker	260
# define	t_unknown	261


#endif /* not BISON_D__WORK_SS_SOURCES_CORE_TRANSLIT_CONFIGREADER_TAB_H */
