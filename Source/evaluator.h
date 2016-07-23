
#ifndef __EXPRESSION_EVALUATOR_H_INCLUDED
#define __EXPRESSION_EVALUATOR_H_INCLUDED

typedef long double ftype;

class EEvaluator {
public:
   EEvaluator();
   bool evaluate( const char* expression, char*& dec, char*& hex, char*& bin, char*& oct, char*& exp );
   const char* get_position() const { return expression; }
   void set_overflow_string( const char* new_ov ) { overflow = new_ov; }

private:
   const char *expression, *overflow;
   enum STATUS_TYPE {
      STATUS_OK,
      STATUS_END,
      STATUS_CLOSE,
      STATUS_ERROR
   } status;

protected:
   ftype evaluate( int minlevel );
   ftype get_operand();
   int get_operator();
   void exec( ftype& f1, int oper, ftype f2 ) const;
   int get_status() const { return status; }
   char get_char() const { return *expression; }
   char get_char( int i ) const { return expression[i]; }
   void advance() { expression++; }
   void advance( int i ) { expression += i; }
   bool is_number() const { return (*expression >= '0' && *expression <= '9') || *expression == '$'; }
   void skip_spaces() { while ( (unsigned char)get_char() <= ' ' && get_char() ) expression++; }
   char tolower( char c ) const { return ( c >= 'A' && c <= 'Z' ) ? ( c + 'a' - 'A' ) : c; }
   int identify();
   void restore( const char* saved ) { expression = saved; }
   char* salloc( const char* string ) const;
};

#endif // __EXPRESSION_EVALUATOR_H_INCLUDED
