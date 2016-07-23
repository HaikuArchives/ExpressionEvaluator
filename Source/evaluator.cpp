
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "evaluator.h"

enum OPERAND_TYPE {
   O_ADD=1, // +
     O_FIRST = O_ADD,
   O_SUB,   // -
   O_MUL,   // *
   O_DIV,   // /
   O_MOD,   // %
   O_PWR,   // ^
   O_SHL,   // <<
   O_SHR,   // >>
   O_AND,   // &
   O_OR,    // |
   O_NOT,   // ~
   O_BAND,  // &&, and
   O_BOR,   // ||, or
   O_BXOR,  // ^^, xor
   O_BNOT,  // !
   O_EQ,    // ==, =
   O_NE,    // !=, <>
   O_GT,    // >
   O_GE,    // >=
   O_LT,    // <
   O_LE,    // <=
     O_LAST = O_LE,
   F_ABS,   // abs()
     F_FIRST = F_ABS,
   F_NEG,   // neg()
   F_SIN,   // sin()
   F_COS,   // cos()
   F_TAN,   // tan(), tg()
   F_CTG,   // ctan(), ctg()
   F_ASIN,  // asin(), arcsin()
   F_ACOS,  // acos(), arccos()
   F_ATAN,  // atan(), arctan(), atg(), arctg()
   F_ACTAN, // actan(), arcctan(), actg(), arcctg()
   F_SQR,   // sqr()
   F_SQRT,  // sqrt()
   F_EXP,   // exp()
   F_LOG,   // log()
   F_LG,    // lg()
   F_LN,    // ln()
     F_LAST = F_LN,
   C_PI,    // pi
     C_FIRST = C_PI,
   C_E,     // e
   C_L2T,   // l2t [log2(10)]
   C_L2E,   // l2e [log2(e)]
   C_LG2,   // lg2 [log10(2)]
   C_LN2,   // ln2 [loge(2)]
     C_LAST = C_LN2
};

struct CONSTSTRING {
   char* name;
   int type;
} conststring[] = {
   {"+",    O_ADD },
   {"-",    O_SUB },
   {"*",    O_MUL },
   {"/",    O_DIV },
   {"div",    O_DIV },
   {"%",    O_MOD },
   {"mod",    O_MOD },
   {"^",    O_PWR },
   {"<<",   O_SHL },
   {">>",   O_SHR },
   {"&&",   O_BAND },
   {"||",   O_BOR },
   {"^^",   O_BXOR },
   {"and",  O_BAND },
   {"or",   O_BOR },
   {"xor",  O_BXOR },
   {"&",    O_AND },
   {"|",    O_OR },
   {"==",   O_EQ },
   {"=",    O_EQ },
   {"!=",   O_NE },
   {"<>",   O_NE },
   {"<=",   O_LE },
   {"<",    O_LT },
   {">=",   O_GE },
   {">",    O_GT },

   {"exp",  F_EXP },// must be before "e"

   {"pi",   C_PI },
   {"e",    C_E },
   {"l2t",  C_L2T },
   {"l2e",  C_L2E },
   {"lg2",  C_LG2 },
   {"ln2",  C_LN2 },

   {"abs",  F_ABS },
   {"neg",  F_NEG },
   {"sin",  F_SIN },
   {"cos",  F_COS },
   {"tan",  F_TAN },
   {"tg",   F_TAN },
   {"ctan", F_CTG },
   {"ctg",  F_CTG },
   {"asin", F_ASIN },
   {"arcsin",F_ASIN },
   {"acos", F_ACOS },
   {"arccos",F_ACOS },
   {"atan", F_ATAN },
   {"atg",  F_ATAN },
   {"arctan",F_ATAN },
   {"arctg",F_ATAN },
   {"actan",F_ACTAN },
   {"actg", F_ACTAN },
   {"arcctan",F_ACTAN },
   {"arcctg",F_ACTAN },
   {"sqrt", F_SQRT },
   {"sqr",  F_SQR },
   {"log",  F_LOG },
   {"lg",   F_LG },
   {"ln",   F_LN },
   {"",     0 }
};

//                    +  -  *  /  %  ^ << >>  &  !  ~ && ||  ! == !=  > >=  < <=
int priority[] = { 0, 2, 2, 3, 3, 3, 4, 3, 3, 5, 5, 5, 1, 1, 1, 0, 0, 0, 0, 0, 0 };

#ifndef M_PI
#define M_PI 3.1415926535897932384626
#endif

#ifndef M_E
#define M_E 2.7182818285
#endif

EEvaluator::EEvaluator(): overflow( "Overflow" ) { }

bool EEvaluator::evaluate( const char* expression, char*& dec, char*& hex, char*& bin, char*& oct, char*& exp )
{
   dec = hex = bin = oct = exp = 0;
   this->expression = expression;
   ftype result = evaluate( 0 );
   if ( status != STATUS_OK && status != STATUS_END ) return false;

   char tmp[256]; tmp[0] = 0;
   int i = (int)result;
   bool b_valid = result <= INT_MAX && result >= INT_MIN;
   if ( result < 1.E+253 && result > -1.E+253 ) {
      sprintf( tmp, "%.15f", (double)result );
      int m;
      for ( m=strlen( tmp )-1; m > 0; m-- )
         if ( tmp[m] != '0' ) break;
      if ( m >= 0 ) {
         if ( tmp[m] == '.' ) m--;
         tmp[m+1] = 0;
      }
      dec = salloc( tmp );
   } else
      dec = salloc( overflow );

   if ( b_valid ) {
      sprintf( tmp, "%X", i );
      hex = salloc( tmp );

      int k = 0;
      unsigned int j = i;
      while ( j != 0 ) { tmp[k++] = '0' + (j & 1); j >>= 1; }
      if ( k == 0 ) tmp[k++] = '0';
      tmp[k] = 0;
      for ( j=0; (int)j < (k>>1); j++ ) {
         char c = tmp[j];
         tmp[j] = tmp[k-j-1];
         tmp[k-j-1] = c;
      }
      bin = salloc( tmp );

      sprintf( tmp, "%o", i );
      oct = salloc( tmp );

   } else {
      hex = salloc( overflow );
      bin = salloc( overflow );
      oct = salloc( overflow );
   }

   sprintf( tmp, "%.14E", (double)result );
   exp = salloc( tmp );

   return true;
}

ftype EEvaluator::evaluate( int minlevel )
{
   // ([a] + b + ...
   ftype f = get_operand();
   if ( status == STATUS_CLOSE ) status = STATUS_ERROR;
   if ( status != STATUS_OK ) return 0;

   // (a [+] b + ...
   const char* saveo = expression;
   int o = get_operator();
   if ( status != STATUS_OK ) return f;

   for (;;) {
      // Check operator priority
      if ( priority[o] < minlevel ) { restore( saveo ); return f; }

      // a + [b] + ...
      const char* savef = expression;
      ftype f2 = get_operand();
      if ( status != STATUS_OK ) { status = STATUS_ERROR; return 0; }

      // a + b [+] ...
      saveo = expression;
      int o2 = get_operator();
      if ( status != STATUS_OK ) {
         exec( f, o, f2 );
         return f;
      } else
         if ( priority[o2] > priority[o] ) {
            restore( savef );
            f2 = evaluate( priority[o] + 1 );
            exec( f, o, f2 );
            if ( status != STATUS_OK ) return f;
            saveo = expression;
            o = get_operator();
            if ( status != STATUS_OK ) return f;
         } else {
            exec( f, o, f2 );
            o = o2;
         }
   }
}

ftype EEvaluator::get_operand()
{
   skip_spaces();
   if ( ! get_char() ) { status = STATUS_END; return 0; }
   if ( get_char() == ')' ) { status = STATUS_CLOSE; advance(); return 0; }

   enum EXTRAFUN {
      EF_NONE,
      EF_MINUS,
      EF_BOOLNEG,
      EF_NOT
   } ef = EF_NONE;
   switch ( get_char() ) {
   case '-': ef = EF_MINUS; break;
   case '!': ef = EF_BOOLNEG; break;
   case '~': ef = EF_NOT; break;
   }
   if ( get_char() == '-' || get_char() == '~' || get_char() == '!' ) {
      advance();
      skip_spaces();
   }

   ftype f = 0;

   // Expression in parentheses
   if ( get_char() == '(' ) {
      advance();
      f = evaluate( 0 );
      status = status == STATUS_CLOSE ? STATUS_OK : STATUS_ERROR;
   } else
      if ( ! is_number() ) {
         int o = identify();

         // Function
         if ( o >= F_FIRST && o <= F_LAST ) {
            skip_spaces();
            if ( get_char() != '(' ) { status = STATUS_ERROR; return 0; }
            advance();
            f = evaluate( 0 );
            status = status == STATUS_CLOSE ? STATUS_OK : STATUS_ERROR;
            if ( status == STATUS_ERROR ) return 0;
            switch ( o ) {
            case F_ABS: f = fabs( f ); break;
            case F_NEG: f = -f; break;
            case F_SIN: f = sin( f ); break;
            case F_COS: f = cos( f ); break;
            case F_TAN: f = tan( f ); break;
            case F_CTG: f = 1 / tan( f ); break;
            case F_ASIN: f = asin( f ); break;
            case F_ACOS: f = acos( f ); break;
            case F_ATAN: f = atan( f ); break;
            case F_ACTAN: f = M_PI*0.5f - atan( f ); break;
            case F_SQR: f *= f; break;
            case F_SQRT: f = sqrt( f ); break;
            case F_EXP: f = exp( f ); break;
            case F_LOG: f = log( f ) / log( 2 ); break;
            case F_LG: f = log10( f ); break;
            case F_LN: f = log( f ); break;
            }
         } else

            // Constant
            if ( o >= C_FIRST && o <= C_LAST ) {
               status = STATUS_OK;
               switch( o ) {
               case C_PI: f = M_PI; break;
               case C_E: f = M_E; break;
               case C_L2T: f = 1 / log10( 2 ); break;
               case C_L2E: f = 1 / log( 2 ); break;
               case C_LG2: f = log10( 2 ); break;
               case C_LN2: f = log( 2 ); break;
               }
            } else
               status = STATUS_ERROR;

      // Number
      } else {
         enum NUMTYPE {
            NT_DEC, NT_HEX, NT_BIN, NT_OCT, NT_FLOAT
         } numtype = NT_DEC;
         char num[256];
         int size = 0;

         // Pascal-style hex
         if ( get_char() == '$' ) {
            advance();
            for (;;) {
               skip_spaces();
               char c = tolower( get_char() );
               if ( (c < '0' || c > '9') && (c < 'a' || c > 'f') ) break;
               num[size++] = c;
               advance();
            }
            numtype = NT_HEX;
         } else {

            // C-style hex or octal (or float...)
            if ( get_char() == '0' ) {
               advance();
               if ( tolower( get_char() ) == 'x' ) {
                  numtype = NT_HEX;
                  advance();
               } else
                  numtype = NT_OCT;
            }

            // Feed the number in and determine its type
            for (;;) {
               skip_spaces();
               char c = tolower( get_char() );
               if ( (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') )
                  num[size++] = c;
               else
               if ( c == '.' ||
                    ((c == '+' || c == '-') && size > 0 && num[size-1] == 'e') ) {
                  num[size++] = c;
                  if ( numtype == NT_HEX ) { status = STATUS_ERROR; return 0; }
                  numtype = NT_FLOAT;
               } else
               if ( c == 'h' ) { numtype = NT_HEX; advance(); break;
               } else
               if ( c == 'o' || c == 'q' ) {
                  num[size++] = 'o';
                  numtype = NT_OCT; advance(); break;
               } else break;
               advance();
            }

            // Further type determination
            if ( size > 0 && ( numtype == NT_DEC || numtype == NT_OCT ) )
               switch ( num[size-1] ) {
               case 'f': numtype = NT_FLOAT; size--; break;
               case 'o': numtype = NT_OCT; size--; break;
               case 'b': numtype = NT_BIN; size--; break;
               case 'd': numtype = NT_DEC; size--; break;
               }
            else
               if ( size > 0 && numtype == NT_FLOAT && num[size-1] == 'f' )
                  size --;
         }

         // Check type correctness
         int i;
         switch ( numtype ) {
         case NT_DEC:
            for ( i=0; i < size; i++ )
               if ( num[i] < '0' || num[i] > '9' ) { status = STATUS_ERROR; return 0; }
            break;
         case NT_OCT:
            for ( i=0; i < size; i++ )
               if ( num[i] < '0' || num[i] > '7' ) { status = STATUS_ERROR; return 0; }
            break;
         case NT_BIN:
            for ( i=0; i < size; i++ )
               if ( num[i] < '0' || num[i] > '1' ) { status = STATUS_ERROR; return 0; }
            break;
         case NT_HEX:
            for ( i=0; i < size; i++ )
               if ( (num[i] < '0' || num[i] > '9') &&
                    (num[i] < 'a' || num[i] > 'f') ) { status = STATUS_ERROR; return 0; }
            break;
         case NT_FLOAT:
            {
            bool dot = false; bool e = false;
            for ( i=0; i < size; i++ )
               if ( num[i] == '.' )
                  if ( dot ) { status = STATUS_ERROR; return 0; }
                  else dot = true;
               else
               if ( num[i] == 'e' )
                  if ( e ) { status = STATUS_ERROR; return 0; }
                  else e = true;
               else
               if ( (num[i] < '0' || num[i] > '9') && num[i] != '+' && num[i] != '-' ) {
                  status = STATUS_ERROR; return 0;
               }
            }
            break;
         }

         // Interpret number
         num[size] = 0;
         double d;
         switch ( numtype ) {
         case NT_HEX: sscanf( num, "%x", &i ); f = i; break;
         case NT_OCT: sscanf( num, "%o", &i ); f = i; break;
         case NT_DEC:
         case NT_FLOAT: sscanf( num, "%lf", &d ); f = d; break;
         case NT_BIN:
            i = 0;
            for ( int k=0; k < size; k++ ) {
               i <<= 1;
               if ( num[k] == '1' ) i |= 1;
            }
            f = i;
            break;
         }
         status = STATUS_OK;
      }

   if ( status == STATUS_OK )
      switch( ef ) {
      case EF_MINUS: f = -f; break;
      case EF_BOOLNEG: f = f != 0 ? 0 : 1; break;
      case EF_NOT: f = (ftype)( ~ (unsigned long)f ); break;
      default: break;
      }
   return f;
}

int EEvaluator::get_operator()
{
   skip_spaces();
   if ( ! get_char() ) { status = STATUS_END; return 0; }
   if ( get_char() == ')' ) { status = STATUS_CLOSE; advance(); return 0; }
   status = STATUS_ERROR;
   if ( get_char() == '(' || is_number() ) return 0;
   int o = identify();
   if ( o >= O_FIRST && o <= O_LAST ) status = STATUS_OK;
   else o = 0;
   return o;
}

void EEvaluator::exec( ftype& f1, int oper, ftype f2 ) const
{
   switch ( oper ) {
   case O_ADD: f1 += f2; break;
   case O_SUB: f1 -= f2; break;
   case O_MUL: f1 *= f2; break;
   case O_DIV: f1 /= f2; break;
   case O_MOD: f1 = fmod( f1, f2 ); break;
   case O_PWR: f1 = pow( f1, f2 ); break;
   case O_SHL: f1 = (ftype)((int)f1 << (int)f2); break;
   case O_SHR: f1 = (ftype)((int)f1 >> (int)f2); break;
   case O_AND: f1 = (ftype)((int)f1 & (int)f2); break;
   case O_OR:  f1 = (ftype)((int)f1 | (int)f2); break;
   case O_BAND:f1 = f1 && f2 ? 1 : 0; break;
   case O_BOR: f1 = f1 || f2 ? 1 : 0; break;
   case O_BXOR: f1 = (f1 && ! f2) || ( !f1 && f2 ) ? 1 : 0; break;
   case O_EQ:  f1 = f1 == f2 ? 1 : 0; break;
   case O_NE:  f1 = f1 != f2 ? 1 : 0; break;
   case O_GT:  f1 = f1 > f2 ? 1 : 0; break;
   case O_GE:  f1 = f1 >= f2 ? 1 : 0; break;
   case O_LT:  f1 = f1 < f2 ? 1 : 0; break;
   case O_LE:  f1 = f1 <= f2 ? 1 : 0; break;
   }
}

int EEvaluator::identify()
{
   int i, j;
   for ( i=0; conststring[i].type; i++ ) {
      for ( j=0; conststring[i].name[j] == tolower( get_char( j ) ) && get_char( j ); j++ );
      if ( conststring[i].name[j] == 0 ) {
         advance( j );
         return conststring[i].type;
      }
   }
   return 0;
}

char* EEvaluator::salloc( const char* string ) const
{
   char* s = new char[strlen( string ) + 1];
   if ( s ) strcpy( s, string );
   return s;
}
