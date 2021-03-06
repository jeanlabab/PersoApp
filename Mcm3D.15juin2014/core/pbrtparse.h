/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_CORE_PBRTPARSE_HPP_INCLUDED
# define YY_YY_CORE_PBRTPARSE_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    STRING = 258,
    ID = 259,
    NUM = 260,
    LBRACK = 261,
    RBRACK = 262,
    ACCELERATOR = 263,
    AREALIGHTSOURCE = 264,
    ATTRIBUTEBEGIN = 265,
    ATTRIBUTEEND = 266,
    CAMERA = 267,
    CONCATTRANSFORM = 268,
    COORDINATESYSTEM = 269,
    COORDSYSTRANSFORM = 270,
    FILM = 271,
    IDENTITY = 272,
    LIGHTSOURCE = 273,
    LOOKAT = 274,
    MATERIAL = 275,
    OBJECTBEGIN = 276,
    OBJECTEND = 277,
    OBJECTINSTANCE = 278,
    PIXELFILTER = 279,
    REVERSEORIENTATION = 280,
    ROTATE = 281,
    SAMPLER = 282,
    SCALE = 283,
    SEARCHPATH = 284,
    SHAPE = 285,
    SURFACEINTEGRATOR = 286,
    TEXTURE = 287,
    TRANSFORMBEGIN = 288,
    TRANSFORMEND = 289,
    TRANSFORM = 290,
    TRANSLATE = 291,
    VOLUME = 292,
    VOLUMEINTEGRATOR = 293,
    WORLDBEGIN = 294,
    WORLDEND = 295,
    HIGH_PRECEDENCE = 296
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 152 "core/pbrtparse.y" /* yacc.c:1909  */

char string[1024];
float num;
ParamArray *ribarray;

#line 102 "core/pbrtparse.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_CORE_PBRTPARSE_HPP_INCLUDED  */
