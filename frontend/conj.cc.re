// A Bison parser, made by GNU Bison 3.5.1.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2020 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.





// "%code requires" blocks.
#line 9 "conj.y"

#include <map>
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#define ENUM_IDENTIFIERS(o) \
        o(undefined)                              /* undefined */ \
        o(function)                               /* a pointer to given function */ \
        o(parameter)                              /* one of the function params */ \
        o(variable)                               /* a local variable */
#define o(n) n,
enum class id_type {  ENUM_IDENTIFIERS(o) };
#undef o

struct identifier
{
    id_type type  = id_type::undefined;
    std::size_t     index = 0; // function#, parameter# within surrounding function, variable#
    std::string     name;
};

#define ENUM_EXPRESSIONS(o) \
        o(nop) o(string) o(number) o(ident)       /* atoms */ \
        o(add) o(neg) o(eq)                       /* transformation */ \
        o(cor) o(cand) o(loop)                    /* logic. Loop is: while(param0) { param1..n } */ \
        o(addrof) o(deref)                        /* pointer handling */ \
        o(fcall)                                  /* function param0 call with param1..n */ \
        o(copy)                                   /* assign: param1 <<- param0 */ \
        o(comma)                                  /* a sequence of expressions */ \
        o(ret)                                    /* return(param0) */

#define o(n) n,
enum class ex_type {  ENUM_EXPRESSIONS(o) };
#undef o

typedef std::list<struct expression> expr_vec;
struct expression
{
    ex_type type;
    identifier      ident{};    // For ident
    std::string     strvalue{}; // For string
    long            numvalue=0; // For number
    expr_vec        params;
    // For while() and if(), the first item is the condition and the rest are the contingent code
    // For fcall, the first parameter is the variable to use as function

    template<typename... T>
    expression(ex_type t, T&&... args) : type(t), params{ std::forward<T>(args)... } {}

    expression()                    : type(ex_type::nop) {}
    expression(const identifier& i) : type(ex_type::ident),  ident(i)            { }
    expression(identifier&& i)      : type(ex_type::ident),  ident(std::move(i)) { }
    expression(std::string&& s)     : type(ex_type::string), strvalue(std::move(s)) { }
    expression(long v)              : type(ex_type::number), numvalue(v) {}

    bool is_pure() const;

    expression operator%=(expression&& b) && { return expression(ex_type::copy, std::move(b), std::move(*this)); }
};

#define o(n) \
template<typename... T> \
inline expression e_##n(T&&... args) { return expression(ex_type::n, std::forward<T>(args)...); }
ENUM_EXPRESSIONS(o)
#undef o

struct function
{
    std::string name;
    expression  code;
    unsigned num_vars = 0, num_params = 0;
};

struct lexcontext;


#line 122 "conj.cc.re"

# include <cassert>
# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif

#include <typeinfo>
#ifndef YY_ASSERT
# include <cassert>
# define YY_ASSERT assert
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

namespace yy {
#line 256 "conj.cc.re"


  /// A point in a source file.
  class position
  {
  public:
    /// Type for line and column numbers.
    typedef int counter_type;

    /// Construct a position.
    explicit position (std::string* f = YY_NULLPTR,
                       counter_type l = 1,
                       counter_type c = 1)
      : filename (f)
      , line (l)
      , column (c)
    {}


    /// Initialization.
    void initialize (std::string* fn = YY_NULLPTR,
                     counter_type l = 1,
                     counter_type c = 1)
    {
      filename = fn;
      line = l;
      column = c;
    }

    /** \name Line and Column related manipulators
     ** \{ */
    /// (line related) Advance to the COUNT next lines.
    void lines (counter_type count = 1)
    {
      if (count)
        {
          column = 1;
          line = add_ (line, count, 1);
        }
    }

    /// (column related) Advance to the COUNT next columns.
    void columns (counter_type count = 1)
    {
      column = add_ (column, count, 1);
    }
    /** \} */

    /// File name to which this position refers.
    std::string* filename;
    /// Current line number.
    counter_type line;
    /// Current column number.
    counter_type column;

  private:
    /// Compute max (min, lhs+rhs).
    static counter_type add_ (counter_type lhs, counter_type rhs, counter_type min)
    {
      return lhs + rhs < min ? min : lhs + rhs;
    }
  };

  /// Add \a width columns, in place.
  inline position&
  operator+= (position& res, position::counter_type width)
  {
    res.columns (width);
    return res;
  }

  /// Add \a width columns.
  inline position
  operator+ (position res, position::counter_type width)
  {
    return res += width;
  }

  /// Subtract \a width columns, in place.
  inline position&
  operator-= (position& res, position::counter_type width)
  {
    return res += -width;
  }

  /// Subtract \a width columns.
  inline position
  operator- (position res, position::counter_type width)
  {
    return res -= width;
  }

  /// Compare two position objects.
  inline bool
  operator== (const position& pos1, const position& pos2)
  {
    return (pos1.line == pos2.line
            && pos1.column == pos2.column
            && (pos1.filename == pos2.filename
                || (pos1.filename && pos2.filename
                    && *pos1.filename == *pos2.filename)));
  }

  /// Compare two position objects.
  inline bool
  operator!= (const position& pos1, const position& pos2)
  {
    return !(pos1 == pos2);
  }

  /** \brief Intercept output stream redirection.
   ** \param ostr the destination output stream
   ** \param pos a reference to the position to redirect
   */
  template <typename YYChar>
  std::basic_ostream<YYChar>&
  operator<< (std::basic_ostream<YYChar>& ostr, const position& pos)
  {
    if (pos.filename)
      ostr << *pos.filename << ':';
    return ostr << pos.line << '.' << pos.column;
  }

  /// Two points in a source file.
  class location
  {
  public:
    /// Type for line and column numbers.
    typedef position::counter_type counter_type;

    /// Construct a location from \a b to \a e.
    location (const position& b, const position& e)
      : begin (b)
      , end (e)
    {}

    /// Construct a 0-width location in \a p.
    explicit location (const position& p = position ())
      : begin (p)
      , end (p)
    {}

    /// Construct a 0-width location in \a f, \a l, \a c.
    explicit location (std::string* f,
                       counter_type l = 1,
                       counter_type c = 1)
      : begin (f, l, c)
      , end (f, l, c)
    {}


    /// Initialization.
    void initialize (std::string* f = YY_NULLPTR,
                     counter_type l = 1,
                     counter_type c = 1)
    {
      begin.initialize (f, l, c);
      end = begin;
    }

    /** \name Line and Column related manipulators
     ** \{ */
  public:
    /// Reset initial location to final location.
    void step ()
    {
      begin = end;
    }

    /// Extend the current location to the COUNT next columns.
    void columns (counter_type count = 1)
    {
      end += count;
    }

    /// Extend the current location to the COUNT next lines.
    void lines (counter_type count = 1)
    {
      end.lines (count);
    }
    /** \} */


  public:
    /// Beginning of the located region.
    position begin;
    /// End of the located region.
    position end;
  };

  /// Join two locations, in place.
  inline location&
  operator+= (location& res, const location& end)
  {
    res.end = end.end;
    return res;
  }

  /// Join two locations.
  inline location
  operator+ (location res, const location& end)
  {
    return res += end;
  }

  /// Add \a width columns to the end position, in place.
  inline location&
  operator+= (location& res, location::counter_type width)
  {
    res.columns (width);
    return res;
  }

  /// Add \a width columns to the end position.
  inline location
  operator+ (location res, location::counter_type width)
  {
    return res += width;
  }

  /// Subtract \a width columns to the end position, in place.
  inline location&
  operator-= (location& res, location::counter_type width)
  {
    return res += -width;
  }

  /// Subtract \a width columns to the end position.
  inline location
  operator- (location res, location::counter_type width)
  {
    return res -= width;
  }

  /// Compare two location objects.
  inline bool
  operator== (const location& loc1, const location& loc2)
  {
    return loc1.begin == loc2.begin && loc1.end == loc2.end;
  }

  /// Compare two location objects.
  inline bool
  operator!= (const location& loc1, const location& loc2)
  {
    return !(loc1 == loc2);
  }

  /** \brief Intercept output stream redirection.
   ** \param ostr the destination output stream
   ** \param loc a reference to the location to redirect
   **
   ** Avoid duplicate information.
   */
  template <typename YYChar>
  std::basic_ostream<YYChar>&
  operator<< (std::basic_ostream<YYChar>& ostr, const location& loc)
  {
    location::counter_type end_col
      = 0 < loc.end.column ? loc.end.column - 1 : 0;
    ostr << loc.begin;
    if (loc.end.filename
        && (!loc.begin.filename
            || *loc.begin.filename != *loc.end.filename))
      ostr << '-' << loc.end.filename << ':' << loc.end.line << '.' << end_col;
    else if (loc.begin.line < loc.end.line)
      ostr << '-' << loc.end.line << '.' << end_col;
    else if (loc.begin.column < end_col)
      ostr << '-' << end_col;
    return ostr;
  }


  /// A Bison parser.
  class conj_parser
  {
  public:
#ifndef YYSTYPE
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class semantic_type
  {
  public:
    /// Type of *this.
    typedef semantic_type self_type;

    /// Empty construction.
    semantic_type () YY_NOEXCEPT
      : yybuffer_ ()
      , yytypeid_ (YY_NULLPTR)
    {}

    /// Construct and fill.
    template <typename T>
    semantic_type (YY_RVREF (T) t)
      : yytypeid_ (&typeid (T))
    {
      YY_ASSERT (sizeof (T) <= size);
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

    /// Destruction, allowed only if empty.
    ~semantic_type () YY_NOEXCEPT
    {
      YY_ASSERT (!yytypeid_);
    }

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      YY_ASSERT (!yytypeid_);
      YY_ASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == typeid (T));
      YY_ASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == typeid (T));
      YY_ASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      YY_ASSERT (yytypeid_);
      YY_ASSERT (*yytypeid_ == *that.yytypeid_);
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
      yytypeid_ = YY_NULLPTR;
    }

  private:
    /// Prohibit blind copies.
    self_type& operator= (const self_type&);
    semantic_type (const self_type&);

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yybuffer_.yyraw;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yybuffer_.yyraw;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // stmt1
      // exprs1
      // expr1
      // p_expr1
      // stmt
      // com_stmt
      // var_defs
      // var_def1
      // exprs
      // c_expr1
      // expr
      char dummy1[sizeof (expression)];

      // NUMCONST
      char dummy2[sizeof (long)];

      // IDENTIFIER
      // STRINGCONST
      // identifier1
      char dummy3[sizeof (std::string)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me;
      /// A buffer large enough to store any of the semantic values.
      char yyraw[size];
    } yybuffer_;

    /// Whether the content is built: if defined, the name of the stored type.
    const std::type_info *yytypeid_;
  };

#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Tokens.
    struct token
    {
      enum yytokentype
      {
        END = 0,
        RETURN = 258,
        WHILE = 259,
        IF = 260,
        VAR = 261,
        IDENTIFIER = 262,
        NUMCONST = 263,
        STRINGCONST = 264,
        OR = 265,
        AND = 266,
        EQ = 267,
        NE = 268,
        PP = 269,
        MM = 270,
        PL_EQ = 271,
        MI_EQ = 272
      };
    };

    /// (External) token type, as returned by yylex.
    typedef token::yytokentype token_type;

    /// Symbol type: an internal symbol number.
    typedef int symbol_number_type;

    /// The symbol type number to denote an empty symbol.
    enum { empty_symbol = -2 };

    /// Internal symbol number for tokens (subsumed by symbol_number_type).
    typedef signed char token_number_type;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol type
    /// via type_get ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ()
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that);
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, expression&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const expression& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, long&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const long& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::string&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::string& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }

      /// Destroy contents, and record that is empty.
      void clear ()
      {
        // User destructor.
        symbol_number_type yytype = this->type_get ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yytype)
        {
       default:
          break;
        }

        // Type destructor.
switch (yytype)
    {
      case 47: // stmt1
      case 48: // exprs1
      case 49: // expr1
      case 50: // p_expr1
      case 51: // stmt
      case 52: // com_stmt
      case 53: // var_defs
      case 54: // var_def1
      case 55: // exprs
      case 56: // c_expr1
      case 57: // expr
        value.template destroy< expression > ();
        break;

      case 8: // NUMCONST
        value.template destroy< long > ();
        break;

      case 7: // IDENTIFIER
      case 9: // STRINGCONST
      case 41: // identifier1
        value.template destroy< std::string > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_type
    {
      /// Default constructor.
      by_type ();

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_type (by_type&& that);
#endif

      /// Copy constructor.
      by_type (const by_type& that);

      /// The symbol type as needed by the constructor.
      typedef token_type kind_type;

      /// Constructor from (external) token numbers.
      by_type (kind_type t);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_type& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_number_type type_get () const YY_NOEXCEPT;

      /// The symbol type.
      /// \a empty_symbol when empty.
      /// An int, not token_number_type, to be able to store empty_symbol.
      int type;
    };

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_type>
    {
      /// Superclass.
      typedef basic_symbol<by_type> super_type;

      /// Empty symbol.
      symbol_type () {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type(token_type (tok), std::move (l))
      {
        YY_ASSERT (tok == token::END || tok == token::RETURN || tok == token::WHILE || tok == token::IF || tok == token::VAR || tok == token::OR || tok == token::AND || tok == token::EQ || tok == token::NE || tok == token::PP || tok == token::MM || tok == token::PL_EQ || tok == token::MI_EQ || tok == 44 || tok == 63 || tok == 58 || tok == 61 || tok == 43 || tok == 45 || tok == 42 || tok == 38 || tok == 40 || tok == 91 || tok == 59 || tok == 125 || tok == 93 || tok == 41 || tok == 123 || tok == 33);
      }
#else
      symbol_type (int tok, const location_type& l)
        : super_type(token_type (tok), l)
      {
        YY_ASSERT (tok == token::END || tok == token::RETURN || tok == token::WHILE || tok == token::IF || tok == token::VAR || tok == token::OR || tok == token::AND || tok == token::EQ || tok == token::NE || tok == token::PP || tok == token::MM || tok == token::PL_EQ || tok == token::MI_EQ || tok == 44 || tok == 63 || tok == 58 || tok == 61 || tok == 43 || tok == 45 || tok == 42 || tok == 38 || tok == 40 || tok == 91 || tok == 59 || tok == 125 || tok == 93 || tok == 41 || tok == 123 || tok == 33);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, long v, location_type l)
        : super_type(token_type (tok), std::move (v), std::move (l))
      {
        YY_ASSERT (tok == token::NUMCONST);
      }
#else
      symbol_type (int tok, const long& v, const location_type& l)
        : super_type(token_type (tok), v, l)
      {
        YY_ASSERT (tok == token::NUMCONST);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, std::string v, location_type l)
        : super_type(token_type (tok), std::move (v), std::move (l))
      {
        YY_ASSERT (tok == token::IDENTIFIER || tok == token::STRINGCONST);
      }
#else
      symbol_type (int tok, const std::string& v, const location_type& l)
        : super_type(token_type (tok), v, l)
      {
        YY_ASSERT (tok == token::IDENTIFIER || tok == token::STRINGCONST);
      }
#endif
    };

    /// Build a parser object.
    conj_parser (lexcontext& ctx_yyarg);
    virtual ~conj_parser ();

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    // Implementation of make_symbol for each symbol type.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_END (location_type l)
      {
        return symbol_type (token::END, std::move (l));
      }
#else
      static
      symbol_type
      make_END (const location_type& l)
      {
        return symbol_type (token::END, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RETURN (location_type l)
      {
        return symbol_type (token::RETURN, std::move (l));
      }
#else
      static
      symbol_type
      make_RETURN (const location_type& l)
      {
        return symbol_type (token::RETURN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_WHILE (location_type l)
      {
        return symbol_type (token::WHILE, std::move (l));
      }
#else
      static
      symbol_type
      make_WHILE (const location_type& l)
      {
        return symbol_type (token::WHILE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IF (location_type l)
      {
        return symbol_type (token::IF, std::move (l));
      }
#else
      static
      symbol_type
      make_IF (const location_type& l)
      {
        return symbol_type (token::IF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VAR (location_type l)
      {
        return symbol_type (token::VAR, std::move (l));
      }
#else
      static
      symbol_type
      make_VAR (const location_type& l)
      {
        return symbol_type (token::VAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IDENTIFIER (std::string v, location_type l)
      {
        return symbol_type (token::IDENTIFIER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_IDENTIFIER (const std::string& v, const location_type& l)
      {
        return symbol_type (token::IDENTIFIER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NUMCONST (long v, location_type l)
      {
        return symbol_type (token::NUMCONST, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_NUMCONST (const long& v, const location_type& l)
      {
        return symbol_type (token::NUMCONST, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRINGCONST (std::string v, location_type l)
      {
        return symbol_type (token::STRINGCONST, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_STRINGCONST (const std::string& v, const location_type& l)
      {
        return symbol_type (token::STRINGCONST, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OR (location_type l)
      {
        return symbol_type (token::OR, std::move (l));
      }
#else
      static
      symbol_type
      make_OR (const location_type& l)
      {
        return symbol_type (token::OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AND (location_type l)
      {
        return symbol_type (token::AND, std::move (l));
      }
#else
      static
      symbol_type
      make_AND (const location_type& l)
      {
        return symbol_type (token::AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EQ (location_type l)
      {
        return symbol_type (token::EQ, std::move (l));
      }
#else
      static
      symbol_type
      make_EQ (const location_type& l)
      {
        return symbol_type (token::EQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NE (location_type l)
      {
        return symbol_type (token::NE, std::move (l));
      }
#else
      static
      symbol_type
      make_NE (const location_type& l)
      {
        return symbol_type (token::NE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PP (location_type l)
      {
        return symbol_type (token::PP, std::move (l));
      }
#else
      static
      symbol_type
      make_PP (const location_type& l)
      {
        return symbol_type (token::PP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MM (location_type l)
      {
        return symbol_type (token::MM, std::move (l));
      }
#else
      static
      symbol_type
      make_MM (const location_type& l)
      {
        return symbol_type (token::MM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PL_EQ (location_type l)
      {
        return symbol_type (token::PL_EQ, std::move (l));
      }
#else
      static
      symbol_type
      make_PL_EQ (const location_type& l)
      {
        return symbol_type (token::PL_EQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MI_EQ (location_type l)
      {
        return symbol_type (token::MI_EQ, std::move (l));
      }
#else
      static
      symbol_type
      make_MI_EQ (const location_type& l)
      {
        return symbol_type (token::MI_EQ, l);
      }
#endif


  private:
    /// This class is not copyable.
    conj_parser (const conj_parser&);
    conj_parser& operator= (const conj_parser&);

    /// Stored state numbers (used for stacks).
    typedef unsigned char state_type;

    /// Generate an error message.
    /// \param yystate   the state where the error occurred.
    /// \param yyla      the lookahead token.
    virtual std::string yysyntax_error_ (state_type yystate,
                                         const symbol_type& yyla) const;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const signed char yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token number \a t to a symbol number.
    /// In theory \a t should be a token_type, but character literals
    /// are valid, yet not members of the token_type enum.
    static token_number_type yytranslate_ (int t);

    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const signed char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const signed char yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const short yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const signed char yycheck_[];

    // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
    // symbol of state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
    static const signed char yyr1_[];

    // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
    static const signed char yyr2_[];


    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *n);


    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const unsigned char yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol type, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol type as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol type from \a that.
      void move (by_state& that);

      /// The (internal) type number (corresponding to \a state).
      /// \a empty_symbol when empty.
      symbol_number_type type_get () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::reverse_iterator iterator;
      typedef typename S::const_reverse_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200)
        : seq_ (n)
      {}

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      std::ptrdiff_t
      ssize () const YY_NOEXCEPT
      {
        return std::ptrdiff_t (size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.rbegin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.rend ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range)
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
      stack (const stack&);
      stack& operator= (const stack&);
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1);

    /// Some specific tokens.
    static const token_number_type yy_error_token_ = 1;
    static const token_number_type yy_undef_token_ = 2;

    /// Constants.
    enum
    {
      yyeof_ = 0,
      yylast_ = 602,     ///< Last index in yytable_.
      yynnts_ = 24,  ///< Number of nonterminal symbols.
      yyfinal_ = 3, ///< Termination state number.
      yyntokens_ = 34  ///< Number of tokens.
    };


    // User arguments.
    lexcontext& ctx;
  };

  conj_parser::token_number_type
  conj_parser::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const token_number_type
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    33,     2,     2,     2,     2,    25,     2,
      26,    31,    24,    22,    18,    23,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    20,    28,
       2,    21,     2,    19,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    27,     2,    30,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,    29,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17
    };
    const int user_token_number_max_ = 272;

    if (t <= 0)
      return yyeof_;
    else if (t <= user_token_number_max_)
      return translate_table[t];
    else
      return yy_undef_token_;
  }

  // basic_symbol.
#if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  conj_parser::basic_symbol<Base>::basic_symbol (basic_symbol&& that)
    : Base (std::move (that))
    , value ()
    , location (std::move (that.location))
  {
    switch (this->type_get ())
    {
      case 47: // stmt1
      case 48: // exprs1
      case 49: // expr1
      case 50: // p_expr1
      case 51: // stmt
      case 52: // com_stmt
      case 53: // var_defs
      case 54: // var_def1
      case 55: // exprs
      case 56: // c_expr1
      case 57: // expr
        value.move< expression > (std::move (that.value));
        break;

      case 8: // NUMCONST
        value.move< long > (std::move (that.value));
        break;

      case 7: // IDENTIFIER
      case 9: // STRINGCONST
      case 41: // identifier1
        value.move< std::string > (std::move (that.value));
        break;

      default:
        break;
    }

  }
#endif

  template <typename Base>
  conj_parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->type_get ())
    {
      case 47: // stmt1
      case 48: // exprs1
      case 49: // expr1
      case 50: // p_expr1
      case 51: // stmt
      case 52: // com_stmt
      case 53: // var_defs
      case 54: // var_def1
      case 55: // exprs
      case 56: // c_expr1
      case 57: // expr
        value.copy< expression > (YY_MOVE (that.value));
        break;

      case 8: // NUMCONST
        value.copy< long > (YY_MOVE (that.value));
        break;

      case 7: // IDENTIFIER
      case 9: // STRINGCONST
      case 41: // identifier1
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }



  template <typename Base>
  bool
  conj_parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  void
  conj_parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->type_get ())
    {
      case 47: // stmt1
      case 48: // exprs1
      case 49: // expr1
      case 50: // p_expr1
      case 51: // stmt
      case 52: // com_stmt
      case 53: // var_defs
      case 54: // var_def1
      case 55: // exprs
      case 56: // c_expr1
      case 57: // expr
        value.move< expression > (YY_MOVE (s.value));
        break;

      case 8: // NUMCONST
        value.move< long > (YY_MOVE (s.value));
        break;

      case 7: // IDENTIFIER
      case 9: // STRINGCONST
      case 41: // identifier1
        value.move< std::string > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_type.
  conj_parser::by_type::by_type ()
    : type (empty_symbol)
  {}

#if 201103L <= YY_CPLUSPLUS
  conj_parser::by_type::by_type (by_type&& that)
    : type (that.type)
  {
    that.clear ();
  }
#endif

  conj_parser::by_type::by_type (const by_type& that)
    : type (that.type)
  {}

  conj_parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  void
  conj_parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  void
  conj_parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  int
  conj_parser::by_type::type_get () const YY_NOEXCEPT
  {
    return type;
  }

} // yy
#line 1871 "conj.cc.re"







// Unqualified %code blocks.
#line 92 "conj.y"


struct lexcontext
{
    const char* cursor;
    yy::location loc;
    std::vector<std::map<std::string, identifier>> scopes;
    std::vector<function> func_list;
    unsigned tempcounter = 0;
    function fun;
public:
    const identifier& define(const std::string& name, identifier&& f)
    {
        auto r = scopes.back().emplace(name, std::move(f));
        if(!r.second) throw yy::conj_parser::syntax_error(loc, "Duplicate definition <"+name+">");
        return r.first->second;
    }
    expression def(const std::string& name)     { return define(name, identifier{id_type::variable,  fun.num_vars++,   name}); }
    expression defun(const std::string& name)   { return define(name, identifier{id_type::function,  func_list.size(), name}); }
    expression defparm(const std::string& name) { return define(name, identifier{id_type::parameter, fun.num_params++, name}); }
    expression temp()                           { return def("$I" + std::to_string(tempcounter++)); }
    expression use(const std::string& name)
    {
        for(auto j = scopes.crbegin(); j != scopes.crend(); ++j)
            if(auto i = j->find(name); i != j->end())
                return i->second;
        throw yy::conj_parser::syntax_error(loc, "Undefined identifier <"+name+">");
    }
    void add_function(std::string&& name, expression&& code)
    {
        fun.code = e_comma(std::move(code), e_ret(0l)); // Add implicit "return 0;" at the end
        fun.name = std::move(name);
        func_list.push_back(std::move(fun));
        fun = {};
    }
    void operator ++() { scopes.emplace_back(); } // Enter scope
    void operator --() { scopes.pop_back();     } // Exit scope
};

namespace yy { conj_parser::symbol_type yylex(lexcontext& ctx); }

#define M(x) std::move(x)
#define C(x) expression(x)


#line 1926 "conj.cc.re"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 2017 "conj.cc.re"


  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  conj_parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  conj_parser::conj_parser (lexcontext& ctx_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      ctx (ctx_yyarg)
  {}

  conj_parser::~conj_parser ()
  {}

  conj_parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  conj_parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  conj_parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  conj_parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  conj_parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  conj_parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  conj_parser::symbol_number_type
  conj_parser::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[+state];
  }

  conj_parser::stack_symbol_type::stack_symbol_type ()
  {}

  conj_parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 47: // stmt1
      case 48: // exprs1
      case 49: // expr1
      case 50: // p_expr1
      case 51: // stmt
      case 52: // com_stmt
      case 53: // var_defs
      case 54: // var_def1
      case 55: // exprs
      case 56: // c_expr1
      case 57: // expr
        value.YY_MOVE_OR_COPY< expression > (YY_MOVE (that.value));
        break;

      case 8: // NUMCONST
        value.YY_MOVE_OR_COPY< long > (YY_MOVE (that.value));
        break;

      case 7: // IDENTIFIER
      case 9: // STRINGCONST
      case 41: // identifier1
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  conj_parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 47: // stmt1
      case 48: // exprs1
      case 49: // expr1
      case 50: // p_expr1
      case 51: // stmt
      case 52: // com_stmt
      case 53: // var_defs
      case 54: // var_def1
      case 55: // exprs
      case 56: // c_expr1
      case 57: // expr
        value.move< expression > (YY_MOVE (that.value));
        break;

      case 8: // NUMCONST
        value.move< long > (YY_MOVE (that.value));
        break;

      case 7: // IDENTIFIER
      case 9: // STRINGCONST
      case 41: // identifier1
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  conj_parser::stack_symbol_type&
  conj_parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 47: // stmt1
      case 48: // exprs1
      case 49: // expr1
      case 50: // p_expr1
      case 51: // stmt
      case 52: // com_stmt
      case 53: // var_defs
      case 54: // var_def1
      case 55: // exprs
      case 56: // c_expr1
      case 57: // expr
        value.copy< expression > (that.value);
        break;

      case 8: // NUMCONST
        value.copy< long > (that.value);
        break;

      case 7: // IDENTIFIER
      case 9: // STRINGCONST
      case 41: // identifier1
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  conj_parser::stack_symbol_type&
  conj_parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 47: // stmt1
      case 48: // exprs1
      case 49: // expr1
      case 50: // p_expr1
      case 51: // stmt
      case 52: // com_stmt
      case 53: // var_defs
      case 54: // var_def1
      case 55: // exprs
      case 56: // c_expr1
      case 57: // expr
        value.move< expression > (that.value);
        break;

      case 8: // NUMCONST
        value.move< long > (that.value);
        break;

      case 7: // IDENTIFIER
      case 9: // STRINGCONST
      case 41: // identifier1
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  conj_parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  conj_parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  void
  conj_parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  conj_parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  conj_parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  conj_parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  conj_parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  conj_parser::debug_level_type
  conj_parser::debug_level () const
  {
    return yydebug_;
  }

  void
  conj_parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  conj_parser::state_type
  conj_parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  conj_parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  conj_parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  conj_parser::operator() ()
  {
    return parse ();
  }

  int
  conj_parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (ctx));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case 47: // stmt1
      case 48: // exprs1
      case 49: // expr1
      case 50: // p_expr1
      case 51: // stmt
      case 52: // com_stmt
      case 53: // var_defs
      case 54: // var_def1
      case 55: // exprs
      case 56: // c_expr1
      case 57: // expr
        yylhs.value.emplace< expression > ();
        break;

      case 8: // NUMCONST
        yylhs.value.emplace< long > ();
        break;

      case 7: // IDENTIFIER
      case 9: // STRINGCONST
      case 41: // identifier1
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2:
#line 155 "conj.y"
                                { ++ctx; }
#line 2564 "conj.cc.re"
    break;

  case 3:
#line 155 "conj.y"
                                                     { --ctx; }
#line 2570 "conj.cc.re"
    break;

  case 4:
#line 156 "conj.y"
                                                      { ctx.defun(yystack_[0].value.as < std::string > ()); ++ctx; }
#line 2576 "conj.cc.re"
    break;

  case 5:
#line 156 "conj.y"
                                                                                                        { ctx.add_function(M(yystack_[4].value.as < std::string > ()), M(yystack_[0].value.as < expression > ())); --ctx; }
#line 2582 "conj.cc.re"
    break;

  case 9:
#line 159 "conj.y"
                                                            { ctx.defparm(yystack_[0].value.as < std::string > ()); }
#line 2588 "conj.cc.re"
    break;

  case 10:
#line 160 "conj.y"
                                                            { ctx.defparm(yystack_[0].value.as < std::string > ()); }
#line 2594 "conj.cc.re"
    break;

  case 11:
#line 161 "conj.y"
                         {}
#line 2600 "conj.cc.re"
    break;

  case 12:
#line 161 "conj.y"
                                                            { yylhs.value.as < std::string > () = M(yystack_[0].value.as < std::string > ()); }
#line 2606 "conj.cc.re"
    break;

  case 13:
#line 162 "conj.y"
                         {}
#line 2612 "conj.cc.re"
    break;

  case 15:
#line 163 "conj.y"
                         {}
#line 2618 "conj.cc.re"
    break;

  case 17:
#line 164 "conj.y"
                         {}
#line 2624 "conj.cc.re"
    break;

  case 19:
#line 165 "conj.y"
                         {}
#line 2630 "conj.cc.re"
    break;

  case 21:
#line 166 "conj.y"
                         {}
#line 2636 "conj.cc.re"
    break;

  case 23:
#line 167 "conj.y"
                         {}
#line 2642 "conj.cc.re"
    break;

  case 24:
#line 167 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[0].value.as < expression > ()); }
#line 2648 "conj.cc.re"
    break;

  case 25:
#line 168 "conj.y"
                         {}
#line 2654 "conj.cc.re"
    break;

  case 26:
#line 168 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[0].value.as < expression > ()); }
#line 2660 "conj.cc.re"
    break;

  case 27:
#line 169 "conj.y"
                         {}
#line 2666 "conj.cc.re"
    break;

  case 28:
#line 169 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[0].value.as < expression > ()); }
#line 2672 "conj.cc.re"
    break;

  case 29:
#line 170 "conj.y"
                         {}
#line 2678 "conj.cc.re"
    break;

  case 30:
#line 170 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[1].value.as < expression > ()); }
#line 2684 "conj.cc.re"
    break;

  case 31:
#line 171 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[1].value.as < expression > ()); --ctx; }
#line 2690 "conj.cc.re"
    break;

  case 32:
#line 172 "conj.y"
                                                            { yylhs.value.as < expression > () = e_cand(M(yystack_[1].value.as < expression > ()), M(yystack_[0].value.as < expression > ())); }
#line 2696 "conj.cc.re"
    break;

  case 33:
#line 173 "conj.y"
                                                            { yylhs.value.as < expression > () = e_loop(M(yystack_[1].value.as < expression > ()), M(yystack_[0].value.as < expression > ())); }
#line 2702 "conj.cc.re"
    break;

  case 34:
#line 174 "conj.y"
                                                            { yylhs.value.as < expression > () = e_ret(M(yystack_[1].value.as < expression > ()));         }
#line 2708 "conj.cc.re"
    break;

  case 35:
#line 175 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[1].value.as < expression > ());        }
#line 2714 "conj.cc.re"
    break;

  case 36:
#line 176 "conj.y"
                                                            { }
#line 2720 "conj.cc.re"
    break;

  case 37:
#line 177 "conj.y"
                                                            { yylhs.value.as < expression > () = e_comma(); ++ctx; }
#line 2726 "conj.cc.re"
    break;

  case 38:
#line 178 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[1].value.as < expression > ()); yylhs.value.as < expression > ().params.push_back(M(yystack_[0].value.as < expression > ())); }
#line 2732 "conj.cc.re"
    break;

  case 39:
#line 179 "conj.y"
                                                            { yylhs.value.as < expression > () = e_comma(M(yystack_[0].value.as < expression > ())); }
#line 2738 "conj.cc.re"
    break;

  case 40:
#line 180 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[2].value.as < expression > ()); yylhs.value.as < expression > ().params.push_back(M(yystack_[0].value.as < expression > ())); }
#line 2744 "conj.cc.re"
    break;

  case 41:
#line 181 "conj.y"
                                                            { yylhs.value.as < expression > () = ctx.def(yystack_[2].value.as < std::string > ()) %= M(yystack_[0].value.as < expression > ()); }
#line 2750 "conj.cc.re"
    break;

  case 42:
#line 182 "conj.y"
                                                            { yylhs.value.as < expression > () = ctx.def(yystack_[0].value.as < std::string > ()) %= 0l; }
#line 2756 "conj.cc.re"
    break;

  case 43:
#line 183 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[0].value.as < expression > ()); }
#line 2762 "conj.cc.re"
    break;

  case 44:
#line 184 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[0].value.as < expression > ()); }
#line 2768 "conj.cc.re"
    break;

  case 45:
#line 185 "conj.y"
                                                            { yylhs.value.as < expression > () = e_comma(M(yystack_[2].value.as < expression > ())); yylhs.value.as < expression > ().params.splice(yylhs.value.as < expression > ().params.end(), M(yystack_[0].value.as < expression > ().params)); }
#line 2774 "conj.cc.re"
    break;

  case 46:
#line 186 "conj.y"
                                                            { yylhs.value.as < expression > () = e_comma(M(yystack_[0].value.as < expression > ())); }
#line 2780 "conj.cc.re"
    break;

  case 47:
#line 187 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[2].value.as < expression > ()); yylhs.value.as < expression > ().params.push_back(M(yystack_[0].value.as < expression > ())); }
#line 2786 "conj.cc.re"
    break;

  case 48:
#line 188 "conj.y"
                                                            { yylhs.value.as < expression > () = yystack_[0].value.as < long > ();    }
#line 2792 "conj.cc.re"
    break;

  case 49:
#line 189 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[0].value.as < std::string > ()); }
#line 2798 "conj.cc.re"
    break;

  case 50:
#line 190 "conj.y"
                                                            { yylhs.value.as < expression > () = ctx.use(yystack_[0].value.as < std::string > ());   }
#line 2804 "conj.cc.re"
    break;

  case 51:
#line 191 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[1].value.as < expression > ()); }
#line 2810 "conj.cc.re"
    break;

  case 52:
#line 192 "conj.y"
                                                            { yylhs.value.as < expression > () = e_deref(e_add(M(yystack_[3].value.as < expression > ()), M(yystack_[1].value.as < expression > ()))); }
#line 2816 "conj.cc.re"
    break;

  case 53:
#line 193 "conj.y"
                                                            { yylhs.value.as < expression > () = e_fcall(M(yystack_[2].value.as < expression > ())); }
#line 2822 "conj.cc.re"
    break;

  case 54:
#line 194 "conj.y"
                                                            { yylhs.value.as < expression > () = e_fcall(M(yystack_[3].value.as < expression > ())); yylhs.value.as < expression > ().params.splice(yylhs.value.as < expression > ().params.end(), M(yystack_[1].value.as < expression > ().params)); }
#line 2828 "conj.cc.re"
    break;

  case 55:
#line 195 "conj.y"
                  {yylhs.value.as < expression > ()=M(yystack_[2].value.as < expression > ());}
#line 2834 "conj.cc.re"
    break;

  case 56:
#line 195 "conj.y"
                                                            { yylhs.value.as < expression > () = M(yystack_[2].value.as < expression > ()) %= M(yystack_[0].value.as < expression > ()); }
#line 2840 "conj.cc.re"
    break;

  case 57:
#line 196 "conj.y"
                  {yylhs.value.as < expression > ()=M(yystack_[2].value.as < expression > ());}
#line 2846 "conj.cc.re"
    break;

  case 58:
#line 196 "conj.y"
                                                            { yylhs.value.as < expression > () = e_add( M(yystack_[2].value.as < expression > ()), M(yystack_[0].value.as < expression > ())); }
#line 2852 "conj.cc.re"
    break;

  case 59:
#line 197 "conj.y"
                  {yylhs.value.as < expression > ()=M(yystack_[2].value.as < expression > ());}
#line 2858 "conj.cc.re"
    break;

  case 60:
#line 197 "conj.y"
                                                            { yylhs.value.as < expression > () = e_add( M(yystack_[2].value.as < expression > ()), e_neg(M(yystack_[0].value.as < expression > ()))); }
#line 2864 "conj.cc.re"
    break;

  case 61:
#line 198 "conj.y"
                  {yylhs.value.as < expression > ()=M(yystack_[2].value.as < expression > ());}
#line 2870 "conj.cc.re"
    break;

  case 62:
#line 198 "conj.y"
                                                            { if(!yystack_[0].value.as < expression > ().is_pure()) { yylhs.value.as < expression > () = ctx.temp() %= e_addrof(M(yystack_[2].value.as < expression > ())); yystack_[2].value.as < expression > () = e_deref(yylhs.value.as < expression > ().params.back()); }
                                                              yylhs.value.as < expression > () = e_comma(M(yylhs.value.as < expression > ()), M(yystack_[2].value.as < expression > ()) %= e_add(C(yystack_[2].value.as < expression > ()), M(yystack_[0].value.as < expression > ()))); }
#line 2877 "conj.cc.re"
    break;

  case 63:
#line 201 "conj.y"
                  {yylhs.value.as < expression > ()=M(yystack_[2].value.as < expression > ());}
#line 2883 "conj.cc.re"
    break;

  case 64:
#line 201 "conj.y"
                                                            { if(!yystack_[0].value.as < expression > ().is_pure()) { yylhs.value.as < expression > () = ctx.temp() %= e_addrof(M(yystack_[2].value.as < expression > ())); yystack_[2].value.as < expression > () = e_deref(yylhs.value.as < expression > ().params.back()); }
                                                              yylhs.value.as < expression > () = e_comma(M(yylhs.value.as < expression > ()), M(yystack_[2].value.as < expression > ()) %= e_add(C(yystack_[2].value.as < expression > ()), e_neg(M(yystack_[0].value.as < expression > ())))); }
#line 2890 "conj.cc.re"
    break;

  case 65:
#line 204 "conj.y"
             {}
#line 2896 "conj.cc.re"
    break;

  case 66:
#line 204 "conj.y"
                                                            { if(!yystack_[0].value.as < expression > ().is_pure()) { yylhs.value.as < expression > () = ctx.temp() %= e_addrof(M(yystack_[0].value.as < expression > ())); yystack_[0].value.as < expression > () = e_deref(yylhs.value.as < expression > ().params.back()); }
                                                              yylhs.value.as < expression > () = e_comma(M(yylhs.value.as < expression > ()), M(yystack_[0].value.as < expression > ()) %= e_add(C(yystack_[0].value.as < expression > ()),  1l)); }
#line 2903 "conj.cc.re"
    break;

  case 67:
#line 207 "conj.y"
             {}
#line 2909 "conj.cc.re"
    break;

  case 68:
#line 207 "conj.y"
                                                            { if(!yystack_[0].value.as < expression > ().is_pure()) { yylhs.value.as < expression > () = ctx.temp() %= e_addrof(M(yystack_[0].value.as < expression > ())); yystack_[0].value.as < expression > () = e_deref(yylhs.value.as < expression > ().params.back()); }
                                                              yylhs.value.as < expression > () = e_comma(M(yylhs.value.as < expression > ()), M(yystack_[0].value.as < expression > ()) %= e_add(C(yystack_[0].value.as < expression > ()), -1l)); }
#line 2916 "conj.cc.re"
    break;

  case 69:
#line 210 "conj.y"
                                                            { if(!yystack_[1].value.as < expression > ().is_pure()) { yylhs.value.as < expression > () = ctx.temp() %= e_addrof(M(yystack_[1].value.as < expression > ())); yystack_[1].value.as < expression > () = e_deref(yylhs.value.as < expression > ().params.back()); }
                                                              auto i = ctx.temp(); yylhs.value.as < expression > () = e_comma(M(yylhs.value.as < expression > ()), C(i) %= C(yystack_[1].value.as < expression > ()), C(yystack_[1].value.as < expression > ()) %= e_add(C(yystack_[1].value.as < expression > ()),  1l), C(i)); }
#line 2923 "conj.cc.re"
    break;

  case 70:
#line 213 "conj.y"
                                                            { if(!yystack_[1].value.as < expression > ().is_pure()) { yylhs.value.as < expression > () = ctx.temp() %= e_addrof(M(yystack_[1].value.as < expression > ())); yystack_[1].value.as < expression > () = e_deref(yylhs.value.as < expression > ().params.back()); }
                                                              auto i = ctx.temp(); yylhs.value.as < expression > () = e_comma(M(yylhs.value.as < expression > ()), C(i) %= C(yystack_[1].value.as < expression > ()), C(yystack_[1].value.as < expression > ()) %= e_add(C(yystack_[1].value.as < expression > ()), -1l), C(i)); }
#line 2930 "conj.cc.re"
    break;

  case 71:
#line 216 "conj.y"
                  {yylhs.value.as < expression > ()=M(yystack_[2].value.as < expression > ());}
#line 2936 "conj.cc.re"
    break;

  case 72:
#line 216 "conj.y"
                                                            { yylhs.value.as < expression > () = e_cor( M(yystack_[2].value.as < expression > ()), M(yystack_[0].value.as < expression > ())); }
#line 2942 "conj.cc.re"
    break;

  case 73:
#line 217 "conj.y"
                  {yylhs.value.as < expression > ()=M(yystack_[2].value.as < expression > ());}
#line 2948 "conj.cc.re"
    break;

  case 74:
#line 217 "conj.y"
                                                            { yylhs.value.as < expression > () = e_cand(M(yystack_[2].value.as < expression > ()), M(yystack_[0].value.as < expression > ())); }
#line 2954 "conj.cc.re"
    break;

  case 75:
#line 218 "conj.y"
                  {yylhs.value.as < expression > ()=M(yystack_[2].value.as < expression > ());}
#line 2960 "conj.cc.re"
    break;

  case 76:
#line 218 "conj.y"
                                                            { yylhs.value.as < expression > () = e_eq(  M(yystack_[2].value.as < expression > ()), M(yystack_[0].value.as < expression > ())); }
#line 2966 "conj.cc.re"
    break;

  case 77:
#line 219 "conj.y"
                  {yylhs.value.as < expression > ()=M(yystack_[2].value.as < expression > ());}
#line 2972 "conj.cc.re"
    break;

  case 78:
#line 219 "conj.y"
                                                            { yylhs.value.as < expression > () = e_eq(e_eq(M(yystack_[2].value.as < expression > ()), M(yystack_[0].value.as < expression > ())), 0l); }
#line 2978 "conj.cc.re"
    break;

  case 79:
#line 220 "conj.y"
           {}
#line 2984 "conj.cc.re"
    break;

  case 80:
#line 220 "conj.y"
                                                            { yylhs.value.as < expression > () = e_addrof(M(yystack_[0].value.as < expression > ())); }
#line 2990 "conj.cc.re"
    break;

  case 81:
#line 221 "conj.y"
           {}
#line 2996 "conj.cc.re"
    break;

  case 82:
#line 221 "conj.y"
                                                            { yylhs.value.as < expression > () = e_deref(M(yystack_[0].value.as < expression > ()));  }
#line 3002 "conj.cc.re"
    break;

  case 83:
#line 222 "conj.y"
           {}
#line 3008 "conj.cc.re"
    break;

  case 84:
#line 222 "conj.y"
                                                            { yylhs.value.as < expression > () = e_neg(M(yystack_[0].value.as < expression > ()));    }
#line 3014 "conj.cc.re"
    break;

  case 85:
#line 223 "conj.y"
           {}
#line 3020 "conj.cc.re"
    break;

  case 86:
#line 223 "conj.y"
                                                            { yylhs.value.as < expression > () = e_eq(M(yystack_[0].value.as < expression > ()), 0l); }
#line 3026 "conj.cc.re"
    break;

  case 87:
#line 224 "conj.y"
                  {yylhs.value.as < expression > ()=M(yystack_[2].value.as < expression > ());}
#line 3032 "conj.cc.re"
    break;

  case 88:
#line 224 "conj.y"
                                                            { auto i = ctx.temp();
                                                              yylhs.value.as < expression > () = e_comma(e_cor(e_cand(M(yystack_[4].value.as < expression > ()), e_comma(C(i) %= M(yystack_[2].value.as < expression > ()), 1l)), C(i) %= M(yystack_[0].value.as < expression > ())), C(i)); }
#line 3039 "conj.cc.re"
    break;


#line 3043 "conj.cc.re"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[+yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yy_error_token_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yy_error_token_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  conj_parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  // Generate an error message.
  std::string
  conj_parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    std::ptrdiff_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */
    if (!yyla.empty ())
      {
        symbol_number_type yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];

        int yyn = yypact_[+yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yy_error_token_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char conj_parser::yypact_ninf_ = -83;

  const signed char conj_parser::yytable_ninf_ = -4;

  const short
  conj_parser::yypact_[] =
  {
     -83,    51,   -83,   -83,    23,   -83,   -83,   -83,     8,   -83,
      30,    46,   -83,   -83,   111,    59,   -83,   139,    13,    13,
      59,   -83,   -83,   -83,    11,   179,   199,   219,   239,   139,
     -83,   -83,   259,   -83,   -83,    78,    49,    10,   516,   -83,
     -83,    10,   -83,   -83,   139,   111,   111,    47,   -83,   -83,
      62,   -83,    62,   -83,    62,   -83,    62,   -83,    62,    12,
     -83,    62,   -83,   -83,   -83,   -83,    59,   -83,   -83,   -83,
     279,   299,   319,   339,   -83,   -83,   359,   379,   399,   419,
     439,   459,   479,   159,   139,   -83,    12,   -83,   -83,   399,
     -83,   -83,   -83,   -83,   -83,   569,   -83,   575,   -83,    48,
     -83,    48,   -83,   552,   -83,   552,   -83,   -83,    55,   552,
     -83,   534,   -83,   552,   -83,    62,   -83,    62,   -83,     9,
      15,   -83,   -83,   399,   492,   -83,   -83,   -83,   -83,   -83,
     552
  };

  const signed char
  conj_parser::yydefact_[] =
  {
       2,     0,     6,     1,     0,    11,    12,     4,     8,    10,
       0,     7,    13,    14,     0,     0,    23,     0,     0,     0,
       0,    50,    48,    49,     0,     0,     0,     0,     0,     0,
      36,    37,     0,     5,    24,     0,    43,     0,    44,     9,
      25,     0,    26,    29,     0,     0,     0,    42,    39,    65,
      66,    67,    68,    83,    84,    81,    82,    79,    80,     0,
      85,    86,    17,    18,    31,    38,     0,    15,    16,    35,
       0,     0,     0,     0,    69,    70,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    34,     0,    33,    32,     0,
      21,    22,    51,    40,    71,    72,    73,    74,    75,    76,
      77,    78,    61,    62,    63,    64,    27,    46,    45,    28,
      87,     0,    55,    56,    57,    58,    59,    60,    53,     0,
       0,    30,    41,     0,     0,    54,    19,    20,    52,    47,
      88
  };

  const signed char
  conj_parser::yypgoto_[] =
  {
     -83,   -83,   -83,   -83,   -83,   -83,   -83,    18,   -83,    28,
     -83,   -83,   -77,   -17,   -23,   -82,    61,    43,   -83,   -83,
      24,   -12,    14,   -24
  };

  const short
  conj_parser::yydefgoto_[] =
  {
      -1,     1,     2,     4,     8,    10,    11,    47,    14,    69,
      64,   128,    92,    33,    41,   107,    45,    34,    35,    36,
      48,    37,   108,    38
  };

  const short
  conj_parser::yytable_[] =
  {
      50,    52,    54,    56,    58,    42,    59,   122,    61,   121,
      90,    67,    49,    90,    43,     9,   126,    42,    21,    22,
      23,    86,     7,    -3,     5,    24,    25,   123,    87,    88,
       6,    12,    42,    39,    26,    27,    28,    29,    68,    44,
      91,   129,   125,    91,    32,   127,    95,    97,    99,   101,
      13,     3,   103,   105,   109,   111,   113,   115,   117,   109,
       5,   120,    74,    75,    15,   109,     6,    66,    89,    85,
      81,    82,    42,   123,    83,    84,    74,    75,    65,    62,
      46,    17,    18,    19,    20,    21,    22,    23,    83,    84,
      93,     0,    24,    25,     0,     0,     0,   119,     0,   109,
     130,    26,    27,    28,    29,     0,    30,    63,     0,     0,
      31,    32,    16,     0,    17,    18,    19,    20,    21,    22,
      23,     0,     0,     0,     0,    24,    25,     0,     0,     0,
       0,     0,     0,     0,    26,    27,    28,    29,     0,    30,
      40,     0,     0,    31,    32,    20,    21,    22,    23,     0,
       0,     0,     0,    24,    25,     0,     0,     0,     0,     0,
     106,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
      51,     0,    26,    27,    28,    29,    21,    22,    23,     0,
     118,     0,    32,    24,    25,     0,     0,     0,     0,     0,
      53,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
      55,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
      57,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
      60,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
      94,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
      96,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
      98,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
     100,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
     102,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
     104,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
     106,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
     110,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
     112,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
     114,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,     0,
     116,     0,    26,    27,    28,    29,    21,    22,    23,     0,
       0,     0,    32,    24,    25,     0,     0,     0,     0,    21,
      22,    23,    26,    27,    28,    29,    24,    25,     0,     0,
       0,     0,    32,     0,     0,    26,    27,    28,    29,     0,
       0,     0,     0,     0,     0,    32,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,     0,    80,    81,    82,
       0,     0,    83,    84,    70,    71,    72,    73,    74,    75,
      76,    77,     0,    79,   124,    80,    81,    82,     0,     0,
      83,    84,    70,    71,    72,    73,    74,    75,    76,    77,
       0,    79,     0,    80,    81,    82,     0,     0,    83,    84,
      71,    72,    73,    74,    75,     0,     0,    72,    73,    74,
      75,    81,    82,     0,     0,    83,    84,    81,    82,     0,
       0,    83,    84
  };

  const signed char
  conj_parser::yycheck_[] =
  {
      24,    25,    26,    27,    28,    17,    29,    89,    32,    86,
       1,     1,     1,     1,     1,     7,     1,    29,     7,     8,
       9,    44,     4,     0,     1,    14,    15,    18,    45,    46,
       7,     1,    44,    15,    23,    24,    25,    26,    28,    26,
      31,   123,   119,    31,    33,    30,    70,    71,    72,    73,
      20,     0,    76,    77,    78,    79,    80,    81,    82,    83,
       1,    84,    14,    15,    18,    89,     7,    18,    21,    41,
      22,    23,    84,    18,    26,    27,    14,    15,    35,     1,
      19,     3,     4,     5,     6,     7,     8,     9,    26,    27,
      66,    -1,    14,    15,    -1,    -1,    -1,    83,    -1,   123,
     124,    23,    24,    25,    26,    -1,    28,    29,    -1,    -1,
      32,    33,     1,    -1,     3,     4,     5,     6,     7,     8,
       9,    -1,    -1,    -1,    -1,    14,    15,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    23,    24,    25,    26,    -1,    28,
       1,    -1,    -1,    32,    33,     6,     7,     8,     9,    -1,
      -1,    -1,    -1,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      31,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    23,    24,    25,    26,     7,     8,     9,    -1,
      -1,    -1,    33,    14,    15,    -1,    -1,    -1,    -1,     7,
       8,     9,    23,    24,    25,    26,    14,    15,    -1,    -1,
      -1,    -1,    33,    -1,    -1,    23,    24,    25,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    -1,    21,    22,    23,
      -1,    -1,    26,    27,    10,    11,    12,    13,    14,    15,
      16,    17,    -1,    19,    20,    21,    22,    23,    -1,    -1,
      26,    27,    10,    11,    12,    13,    14,    15,    16,    17,
      -1,    19,    -1,    21,    22,    23,    -1,    -1,    26,    27,
      11,    12,    13,    14,    15,    -1,    -1,    12,    13,    14,
      15,    22,    23,    -1,    -1,    26,    27,    22,    23,    -1,
      -1,    26,    27
  };

  const signed char
  conj_parser::yystos_[] =
  {
       0,    35,    36,     0,    37,     1,     7,    41,    38,     7,
      39,    40,     1,    20,    42,    18,     1,     3,     4,     5,
       6,     7,     8,     9,    14,    15,    23,    24,    25,    26,
      28,    32,    33,    47,    51,    52,    53,    55,    57,    41,
       1,    48,    55,     1,    26,    50,    50,    41,    54,     1,
      57,     1,    57,     1,    57,     1,    57,     1,    57,    48,
       1,    57,     1,    29,    44,    51,    18,     1,    28,    43,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      21,    22,    23,    26,    27,    43,    48,    47,    47,    21,
       1,    31,    46,    54,     1,    57,     1,    57,     1,    57,
       1,    57,     1,    57,     1,    57,     1,    49,    56,    57,
       1,    57,     1,    57,     1,    57,     1,    57,    31,    56,
      48,    46,    49,    18,    20,    46,     1,    30,    45,    49,
      57
  };

  const signed char
  conj_parser::yyr1_[] =
  {
       0,    34,    36,    35,    38,    37,    37,    39,    39,    40,
      40,    41,    41,    42,    42,    43,    43,    44,    44,    45,
      45,    46,    46,    47,    47,    48,    48,    49,    49,    50,
      50,    51,    51,    51,    51,    51,    51,    52,    52,    53,
      53,    54,    54,    55,    55,    55,    56,    56,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57
  };

  const signed char
  conj_parser::yyr2_[] =
  {
       0,     2,     0,     2,     0,     6,     0,     1,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     2,     3,     3,     3,     2,     1,     1,     2,     2,
       3,     3,     1,     1,     1,     3,     1,     3,     1,     1,
       1,     3,     4,     3,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     2,     2,     2,     2,     3,     5
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const conj_parser::yytname_[] =
  {
  "END", "error", "$undefined", "\"return\"", "\"while\"", "\"if\"",
  "\"var\"", "IDENTIFIER", "NUMCONST", "STRINGCONST", "\"||\"", "\"&&\"",
  "\"==\"", "\"!=\"", "\"++\"", "\"--\"", "\"+=\"", "\"-=\"", "','", "'?'",
  "':'", "'='", "'+'", "'-'", "'*'", "'&'", "'('", "'['", "';'", "'}'",
  "']'", "')'", "'{'", "'!'", "$accept", "library", "$@1", "functions",
  "$@2", "paramdecls", "paramdecl", "identifier1", "colon1", "semicolon1",
  "cl_brace1", "cl_bracket1", "cl_parens1", "stmt1", "exprs1", "expr1",
  "p_expr1", "stmt", "com_stmt", "var_defs", "var_def1", "exprs",
  "c_expr1", "expr", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  conj_parser::yyrline_[] =
  {
       0,   155,   155,   155,   156,   156,   157,   158,   158,   159,
     160,   161,   161,   162,   162,   163,   163,   164,   164,   165,
     165,   166,   166,   167,   167,   168,   168,   169,   169,   170,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   195,   196,   196,   197,
     197,   198,   198,   201,   201,   204,   204,   207,   207,   210,
     213,   216,   216,   217,   217,   218,   218,   219,   219,   220,
     220,   221,   221,   222,   222,   223,   223,   224,   224
  };

  // Print the state stack on the debug stream.
  void
  conj_parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  conj_parser::yy_reduce_print_ (int yyrule)
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 3615 "conj.cc.re"

#line 227 "conj.y"


yy::conj_parser::symbol_type yy::yylex(lexcontext& ctx)
{
    const char* anchor = ctx.cursor;
    ctx.loc.step();
    auto s = [&](auto func, auto&&... params) { ctx.loc.columns(ctx.cursor - anchor); return func(params..., ctx.loc); };
%{ /* Begin re2c lexer */
re2c:yyfill:enable   = 0;
re2c:define:YYCTYPE  = "char";
re2c:define:YYCURSOR = "ctx.cursor";

// Keywords:
"return"                { return s(conj_parser::make_RETURN); }
"while" | "for"         { return s(conj_parser::make_WHILE); }
"var"                   { return s(conj_parser::make_VAR); }
"if"                    { return s(conj_parser::make_IF); }

// Identifiers:
[a-zA-Z_] [a-zA-Z_0-9]* { return s(conj_parser::make_IDENTIFIER, std::string(anchor,ctx.cursor)); }

// String and integer literals:
"\"" [^"]* "\""         { return s(conj_parser::make_STRINGCONST, std::string(anchor+1, ctx.cursor-1)); }
[0-9]+                  { return s(conj_parser::make_NUMCONST, std::stol(std::string(anchor,ctx.cursor))); }

// Whitespace and comments:
"\000"                  { return s(conj_parser::make_END); }
"\r\n" | [\r\n]         { ctx.loc.lines();   return yylex(ctx); }
"//" [^\r\n]*           {                    return yylex(ctx); }
[\t\v\b\f ]             { ctx.loc.columns(); return yylex(ctx); }

// Multi-char operators and any other character (either an operator or an invalid symbol):
"&&"                    { return s(conj_parser::make_AND); }
"||"                    { return s(conj_parser::make_OR); }
"++"                    { return s(conj_parser::make_PP); }
"--"                    { return s(conj_parser::make_MM); }
"!="                    { return s(conj_parser::make_NE); }
"=="                    { return s(conj_parser::make_EQ); }
"+="                    { return s(conj_parser::make_PL_EQ); }
"-="                    { return s(conj_parser::make_MI_EQ); }
.                       { return s([](auto...s){return conj_parser::symbol_type(s...);}, conj_parser::token_type(ctx.cursor[-1]&0xFF)); } // Return that character
%} /* End lexer */
}

void yy::conj_parser::error(const location_type& l, const std::string& m)
{
    std::cerr << (l.begin.filename ? l.begin.filename->c_str() : "(undefined)");
    std::cerr << ':' << l.begin.line << ':' << l.begin.column << '-' << l.end.column << ": " << m << '\n';
}

#include <fstream>

// is_pure tells whether the expression is safe to duplicate,
// or even delete if the return value is not used,
// without changing the program semantics.
bool expression::is_pure() const
{
    // if any parameter is not pure, the expression is not pure
    for(const auto& e: params) if(!e.is_pure()) return false;
    switch(type)
    {
        // Function calls are assumed to be not pure for now.
        case ex_type::fcall: return false;
        // Assigns are not pure.
        case ex_type::copy:  return false;
        // Returns and not pure either, because they do not evaluate into a value.
        case ex_type::ret:   return false;
        // Loops are not pure, because they may be infinite,
        // in which case deleting the loop would alter the program behavior.
        case ex_type::loop:  return false;
        // Anything else is pure
        default:             return true;
    }
}

std::string stringify(const expression& e, bool stmt);
std::string stringify_op(const expression& e, const char* sep, const char* delim, bool stmt = false, unsigned first=0, unsigned limit=~0u)
{
    std::string result(1, delim[0]);
    const char* fsep = "";
    for(const auto& p: e.params) { if(first) { --first; continue; }
                                   if(!limit--) break;
                                   result += fsep; fsep = sep; result += stringify(p, stmt); }
    if(stmt) result += sep;
    result += delim[1];
    return result;
}
std::string stringify(const expression& e, bool stmt = false)
{
    auto expect1 = [&]{ return e.params.empty() ? "?" : e.params.size()==1 ? stringify(e.params.front()) : stringify_op(e, "??", "()"); };
    switch(e.type)
    {
        // atoms
        case ex_type::nop    : return "";
        case ex_type::string : return "\"" + e.strvalue + "\"";
        case ex_type::number : return std::to_string(e.numvalue);
        case ex_type::ident  : return "?FPVS"[(int)e.ident.type] + std::to_string(e.ident.index) + "\"" + e.ident.name + "\"";
        // binary & misc
        case ex_type::add    : return stringify_op(e, " + ",  "()");
        case ex_type::eq     : return stringify_op(e, " == ", "()");
        case ex_type::cand   : return stringify_op(e, " && ", "()");
        case ex_type::cor    : return stringify_op(e, " || ", "()");
        case ex_type::comma  : return stmt ? stringify_op(e, "; ", "{}", true) : stringify_op(e, ", ",  "()");
        // unary
        case ex_type::neg    : return "-(" + expect1() + ")";
        case ex_type::deref  : return "*(" + expect1() + ")";
        case ex_type::addrof : return "&(" + expect1() + ")";
        // special
        case ex_type::copy   : return "(" + stringify(e.params.back()) + " = " + stringify(e.params.front()) + ")";
        case ex_type::fcall  : return "(" + (e.params.empty() ? "?" : stringify(e.params.front()))+")"+stringify_op(e,", ","()",false,1);
        case ex_type::loop   : return "while " + stringify(e.params.front()) + " " + stringify_op(e, "; ", "{}", true, 1);
        case ex_type::ret    : return "return " + expect1();
    }
    return "?";
}
static std::string stringify(const function& f)
{
    return stringify(f.code, true);
}

#include "textbox.hh"

static std::string stringify_tree(const function& f)
{
    textbox result;
    result.putbox(2,0, create_tree_graph(f.code, 132-2,
        [](const expression& e)
        {
            std::string p = stringify(e), k = p;
            switch(e.type)
            {
                #define o(n) case ex_type::n: k.assign(#n,sizeof(#n)-1); break;
                ENUM_EXPRESSIONS(o)
                #undef o
            }
            return e.params.empty() ? (k + ' ' + p) : std::move(k);
        },
        [](const expression& e) { return std::make_pair(e.params.cbegin(), e.params.cend()); },
        [](const expression& e) { return e.params.size() >= 1; }, // whether simplified horizontal layout can be used
        [](const expression&  ) { return true; },                 // whether extremely simplified horiz layout can be used
        [](const expression& e) { return e.type == ex_type::loop; }));
    return "function " + f.name + ":\n" + stringify(f) + '\n' + result.to_string();
}

int main(int argc, char** argv)
{
    std::string filename = argv[1];
    std::ifstream f(filename);
    std::string buffer(std::istreambuf_iterator<char>(f), {});

    lexcontext ctx;
    ctx.cursor = buffer.c_str();
    ctx.loc.begin.filename = &filename;
    ctx.loc.end.filename   = &filename;

    yy::conj_parser parser(ctx);
    parser.parse();
    std::vector<function> func_list = std::move(ctx.func_list);

    for(const auto& f: func_list) std::cerr << stringify_tree(f);
}
