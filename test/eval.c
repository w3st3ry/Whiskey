#include "test.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "whiskey.h"

typedef wsky_ReturnValue ReturnValue;
typedef wsky_Value Value;


static void assertValueEq(const char *expected, Value value,
                          const char *testName, const char *position) {

  ReturnValue stringRv = wsky_toString(value);
  if (stringRv.exception) {
    yolo_assert_ptr_eq_impl(NULL, stringRv.exception, testName, position);
    printf("%s\n", stringRv.exception->message);
    return;
  }
  assert(wsky_isString(stringRv.v));
  wsky_String *string = (wsky_String *) stringRv.v.v.objectValue;
  yolo_assert_str_eq_impl(expected, string->string, testName, position);
}

static void assertReturnValueEq(const char *expected, ReturnValue rv,
                                const char *testName, const char *position) {
  if (rv.exception) {
    yolo_assert_ptr_eq_impl(NULL, rv.exception, testName, position);
    printf("%s\n", rv.exception->message);
    return;
  }
  assertValueEq(expected, rv.v, testName, position);
}

void assertEvalEqImpl(const char *expected, const char *source,
                      const char *testName, const char *position) {
  assertReturnValueEq(expected, wsky_evalString(source, NULL),
                      testName, position);
}


void assertExceptionImpl(const char *exceptionClass,
                         const char *expectedMessage,
                         const char *source,
                         const char *testName,
                         const char *position) {

  wsky_ReturnValue rv = wsky_evalString(source, NULL);
  yolo_assert_ptr_neq_impl(NULL, rv.exception, testName, position);
  if (!rv.exception) {
    return;
  }

  yolo_assert_str_eq_impl(exceptionClass,
                          rv.exception->class->name,
                          testName, position);

  if (strcmp(rv.exception->class->name, exceptionClass)) {
    printf("%s\n", rv.exception->message);
    return;
  }

  yolo_assert_str_eq_impl(expectedMessage, rv.exception->message,
                          testName, position);
}



static void syntaxError(void) {
  assertException("SyntaxError", "Unexpected end of file", "8 +");
  assertException("SyntaxError", "Expected end of string", "'");
}


static void comment(void) {
  assertEvalEq("null", "");
  assertEvalEq("null", "/* This is a comment */");
  assertEvalEq("null", "// This is a comment");
}

static void literals(void) {
  assertEvalEq("123","123");

  assertEvalEq("123.0","123.0");
  assertEvalEq("123.456","123.456");
  assertEvalEq("1e+23","100000000000000000000000.0");
  assertEvalEq("1e+23","100000000000000000000000.0f");
  assertEvalEq("1e+23","100000000000000000000000f");

  assertEvalEq("true","true");
  assertEvalEq("false","false");
  assertEvalEq("null","null");
}

static void strings(void) {
  assertEvalEq("lol","'lol'");
  assertEvalEq("abcdef","'abc' + 'def'");

  assertEvalEq("abctrue","'abc' + true");
  assertEvalEq("abcfalse","'abc' + false");
  assertEvalEq("trueabc","true + 'abc'");
  assertEvalEq("falseabc","false + 'abc'");

  assertEvalEq("abc-12","'abc' + -12");
  assertEvalEq("abc-6.7","'abc' + -6.7");
  assertEvalEq("-12abc","-12 + 'abc'");
  assertEvalEq("-6.7abc","-6.7 + 'abc'");

  assertEvalEq("ababab","'ab' * 3");
  assertEvalEq("ababab","3 * 'ab'");
  assertEvalEq("","0 * 'abc'");
  assertEvalEq("","3 * ''");

  assertException("TypeError",
                  "Unsupported classes for '*': Float and String",
                  "3.0 * 'abc'");
  assertException("ValueError", "The factor cannot be negative",
                  "-3 * 'abc'");
}

static void unaryOps(void) {
  assertException("TypeError", "Unsupported class for unary '-': String",
                  "-'abc'");
  assertException("TypeError", "Unsupported class for unary '-': Boolean",
                  "-false");

  assertEvalEq("-1", "-1");
  assertEvalEq("-1", "-+1");
  assertEvalEq("-1", "-+++--1");
  assertEvalEq("1", "-++-+--1");

  assertEvalEq("-0.0", "-0.0");
  assertEvalEq("-56.0", "-56.0");
  assertEvalEq("-56.0", "-+56.0");
  assertEvalEq("56.0", "-+-56.0");
}

static void binaryOps(void) {
  assertException("TypeError",
                  "Unsupported classes for '-': String and String",
                  "'def' - 'abc'");
  assertException("TypeError",
                  "Unsupported classes for '<': String and String",
                  "'def' < 'abc'");
  assertException("TypeError",
                  "Unsupported classes for 'and': String and String",
                  "'def' and 'abc'");
  assertException("TypeError",
                  "Unsupported classes for '+': Function and Integer",
                  "{} + 1");

  assertEvalEq("2", "1 + 1");
  assertEvalEq("20", "4 * 5");
  assertEvalEq("-1", "4 - 5");
  assertEvalEq("113", "567 / 5");
  assertEvalEq("0", "0 / 5");

  assertEvalEq("2.0", "1 + 1.0");
  assertEvalEq("20.0", "4 * 5.0");
  assertEvalEq("-1.0", "4 - 5.0");
  assertEvalEq("113.4", "567 / 5.0");

  assertEvalEq("2.0", "1.0 + 1");
  assertEvalEq("20.0", "4.0 * 5");
  assertEvalEq("-1.0", "4.0 - 5");
  assertEvalEq("113.4", "567.0 / 5");

  assertEvalEq("2.0", "1.0 + 1.0");
  assertEvalEq("20.0", "4.0 * 5.0");
  assertEvalEq("-1.0", "4.0 - 5.0");
  assertEvalEq("113.4", "567.0 / 5.0");

  assertEvalEq("NaN", "0 / 0.0");
  assertEvalEq("NaN", "0.0 / 0.0");
  assertEvalEq("NaN", "0.0 / 0");
  assertEvalEq("Infinity", "0.1 / 0.0");
  assertEvalEq("Infinity", "1 / 0.0");
  assertEvalEq("-Infinity", "-0.1 / 0.0");
  assertEvalEq("-Infinity", "-1 / 0.0");
}

static void binaryCmpOps(void) {
  assertEvalEq("false", "567 == 56");
  assertEvalEq("true", "567 == 567");
  assertEvalEq("false", "567 != 567");
  assertEvalEq("true", "567 != 566");
  assertEvalEq("true", "567 != 568");

  assertException("TypeError",
                  "Unsupported classes for '==': Float and Integer",
                  "0.0 == 0");
  assertException("TypeError",
                  "Unsupported classes for '==': Integer and Float",
                  "0 == 0.0");
  assertException("TypeError",
                  "Unsupported classes for '==': Float and Float",
                  "0.0 == 0.0");
  assertException("TypeError",
                  "Unsupported classes for '>=': Integer and Float",
                  "0 >= 0.0");
  assertException("TypeError",
                  "Unsupported classes for '<=': Integer and Float",
                  "0 <= 0.0");
  assertException("TypeError",
                  "Unsupported classes for '<=': Float and Integer",
                  "0.0 <= 0");

  assertEvalEq("false", "566 > 566");
  assertEvalEq("true", "567 > 566");
  assertEvalEq("false", "566 > 567");

  assertEvalEq("false", "566.0 > 566");
  assertEvalEq("true", "567.0 > 566");
  assertEvalEq("false", "566.0 > 567");

  assertEvalEq("false", "566 > 566.0");
  assertEvalEq("true", "567 > 566.0");
  assertEvalEq("false", "566 > 567.0");

  assertEvalEq("false", "566.0 > 566.0");
  assertEvalEq("true", "567.0 > 566.0");
  assertEvalEq("false", "566.0 > 567.0");

  assertEvalEq("true", "566 >= 566");
  assertEvalEq("true", "567 >= 566");
  assertEvalEq("false", "566 >= 567");

  assertEvalEq("false", "566 < 566");
  assertEvalEq("false", "567 < 566");
  assertEvalEq("true", "566 < 567");

  assertEvalEq("true", "566 <= 566");
  assertEvalEq("false", "567 <= 566");
  assertEvalEq("true", "566 <= 567");
}

static void binaryBoolOps(void) {
  assertEvalEq("true", "not false");
  assertEvalEq("false", "not not false");
  assertEvalEq("true", "not not not false");

  assertEvalEq("true", "true and true");
  assertEvalEq("false", "false and true");
  assertEvalEq("false", "true and false");
  assertEvalEq("false", "false and false");

  assertEvalEq("true", "true or true");
  assertEvalEq("true", "false or true");
  assertEvalEq("true", "true or false");
  assertEvalEq("false", "false or false");

  assertEvalEq("true", "not true or true");
  assertEvalEq("true", "(not true) or true");
  assertEvalEq("true", "true or not true");
  assertEvalEq("true", "true or (not true)");
  assertEvalEq("false", "not (true or true)");
}

static void sequence(void) {
  assertEvalEq("12", "12");
  assertEvalEq("12", "12;");
  assertEvalEq("12", "678;12;");

  assertEvalEq("12", "(12)");
  assertEvalEq("12", "(12;)");
  assertEvalEq("12", "(678;12;)");

  assertException("SyntaxError", "Expected ')'", "(");
  assertException("SyntaxError", "Expected ')'", "(123");
  assertException("SyntaxError", "Expected ')'", "(123;");
  assertException("SyntaxError", "Expected ';' or ')'", "(123 456)");
}

static void var(void) {
  assertException("SyntaxError", "Expected variable name", "var");
  assertException("SyntaxError", "Expected variable name", "var 123");
  assertException("SyntaxError", "Unexpected ';'", "var a = ;");

  assertEvalEq("null", "var a");
  assertEvalEq("45", "var a = 45");
}

static void variable(void) {
  assertEvalEq("69", "var a = 67; a + 2");
  assertEvalEq("2", "var a = 12; a = 2");
  assertEvalEq("2", "var a = 12; a = 2; a");
  assertEvalEq("-4", "var a = 12; a = 2; a - 6");
  assertException("NameError",
                  "Use of undeclared identifier 'schreugneugneu'",
                  "schreugneugneu");
  assertException("NameError",
                  "Identifier 'a' already declared",
                  "var a; var a");
}

static void scope(void) {
  assertEvalEq("2",
               "var a = 1;"
               "("
               "    var a = 2;"
               "    a"
               ")");

  assertEvalEq("2",
               "("
               "    var a = 1;"
               "    ("
               "        var a = 2;"
               "        a"
               "    )"
               ")");
}

static void function(void) {
  assertEvalEq("<Function>", "{}");
  assertEvalEq("<Function>", "{ a, b, c: 'yolo'}");
  assertEvalEq("<Function>", "{{{}}}");

  assertException("SyntaxError", "Expected '}'", "{");
  assertException("SyntaxError", "Expected ';' or '}'", "{a b}");
}

static void call(void) {
  assertException("SyntaxError", "Expected ')'", "0(");
  assertException("SyntaxError", "Expected ',' or ')'", "0(a b)");

  assertEvalEq("null", "{}()");
  assertEvalEq("1", "{1}()");
  assertEvalEq("1", "{{1}}()()");
  assertEvalEq("lol", "{'lol'}()");
  assertEvalEq("34", "{31}() + 3");

  assertEvalEq("<Function>",
               "var a = {}");

  assertEvalEq("34",
               "var a = {31};"
               "a() + 3");

  assertEvalEq("34",
               "var f = {a: a};"
               "f(31) + 3");

  assertEvalEq("3",
               "var a = 3;"
               " {a}()");

  assertEvalEq("5",
               "{a:"
               "    {b: a + b}"
               "}(2)(3)");

  assertEvalEq("5",
               "var f = {a:"
               "    {b: a + b}"
               "};"
               "f(2)(3)");

  assertEvalEq("7",
               "{a, b, c: a + b * c}(1, 2, 3)");

  assertException("ParameterError",
                  "Invalid parameter count",
                  "{}(0)");

  assertException("TypeError",
                  "'Integer' objects are not callable",
                  "0()");
}

static void functionScope(void) {
  assertEvalEq("2", "var a = 1; {var a = 2; a}()");
  assertEvalEq("1", "var a = 1; {var a = 2}(); a");
}

static void method(void) {
  assertEvalEq("1",
               "var m = 'hello'.indexOf;"
               "m('e')");

  assertEvalEq("1", "'hello'.indexOf('e')");

  assertException("AttributeError",
                  "'Integer' object has no attribute 'vodka'",
                  "0.vodka");
}

static void toString(void) {
  assertEvalEq("whiskey", "'whiskey'.toString");
  assertEvalEq("<Function>", "{}.toString");
  assertEvalEq("null", "null.toString");

  assertEvalEq("<Class Integer>", "0.class");
}

static void getClass(void) {
  assertEvalEq("<Class Function>", "{}.class");

  assertEvalEq("<Class String>", "''.class");
  assertEvalEq("<Class Integer>", "0.class");
  assertEvalEq("<Class Float>", "0.0.class");
  assertEvalEq("<Class NullClass>", "null.class");
  assertEvalEq("<Class Class>", "null.class.class.class.class.class");
}

static void objectEquals(void) {
  assertException("TypeError",
                  "Unsupported classes for '==': Function and Function",
                  "{} == {}");
  assertException("TypeError",
                  "Unsupported classes for '==': Integer and Boolean",
                  "123 == true");
  assertException("TypeError",
                  "Unsupported classes for '==': Boolean and Integer",
                  "true == 123");
}

static void string(void) {
  assertEvalEq("0", "''.length");
  assertEvalEq("3", "'abc'.length");

  assertEvalEq("true", "'' == ''");
  assertEvalEq("true", "'abc' == 'abc'");
  assertEvalEq("true", "'abc' == 'a' + 'bc'");
  assertEvalEq("false", "'abc' == 'abd'");
  assertEvalEq("false", "'' != ''");
  assertEvalEq("false", "'abc' != 'abc'");
  assertEvalEq("true", "'abc' != 'abd'");
}

static void class(void) {
  assertEvalEq("<Class Duck>", "class Duck ()");
  assertEvalEq("<Class Duck>", "(class Duck ()).toString");
  assertEvalEq("<Duck>", "(class Duck ())()");
  assertEvalEq("<Duck>", "(class Duck (init {}))()");
  assertEvalEq("<Duck>", "(class Duck (init {}))().toString");

  assertException("NameError",
                  "Identifier 'Duck' already declared",
                  "var Duck = 3; class Duck ()");

  assertException("NameError",
                  "Identifier 'Duck' already declared",
                  "class Duck (); var Duck = 3");

  assertException("AttributeError",
                  "'Duck' object has no attribute 'doesNotExist'",
                  "class Duck ();"
                  "Duck().doesNotExist");

  assertException("SyntaxError",
                  "Constructor redefinition",
                  "class Duck (init {}; init {})");

  assertException("TypeError",
                  "The constructor of this class is private",
                  "NullClass()");

  assertException("TypeError",
                  "The constructor of this class is private",
                  "Integer()");

  assertException("TypeError",
                  "The constructor of this class is private",
                  "Boolean()");

  assertException("TypeError",
                  "The constructor of this class is private",
                  "Float()");

  assertException("TypeError",
                  "The constructor of this class is private",
                  "String()");

  assertEvalEq("<Exception>", "Exception()");
}


static void classMethod(void) {
  assertException("SyntaxError",
                  "Getter or method redefinition",
                  "class Duck (@lol {}; @lol {});");

  assertEvalEq("<InstanceMethod>",
               "class Duck ("
               "  @coinCoin {'a'}"
               ");"
               "Duck().coinCoin");

  assertEvalEq("a",
               "class Duck ("
               "  @coinCoin {'a'}"
               ");"
               "Duck().coinCoin()");

  assertException("AttributeError",
                  "'Duck' object has no attribute 'lol'",
                  "class Duck ("
                  "  private @lol {123}"
                  ");"
                  "var d = Duck();"
                  "d.lol();");

  assertException("ParameterError",
                  "Invalid parameter count",
                  "class Duck ("
                  "  @lol {123}"
                  ");"
                  "Duck().lol(4);");
}


static void classGetter(void) {
  assertException("SyntaxError",
                  "Getter or method redefinition",
                  "class Duck (get @lol {}; @lol {});");

  assertException("SyntaxError",
                  "Getter or method redefinition",
                  "class Duck (@lol {}; get @lol {});");

  assertException("SyntaxError",
                  "Getter or method redefinition",
                  "class Duck (get @lol {}; get @lol {});");

  assertException("SyntaxError",
                  "Getter or method redefinition",
                  "class Duck (private get @lol {}; get @lol {});");

  assertException("SyntaxError",
                  "Getter or method redefinition",
                  "class Duck (get @lol; get @lol);");

  assertException("SyntaxError",
                  "A getter cannot have any parameter",
                  "class Duck (get @a {a: });");

  assertEvalEq("a",
               "class Duck ("
               "  init {@a = 'a'};"
               "  get @a;"
               ");"
               "Duck().a");

  assertException("AttributeError",
                  "'Duck' object has no attribute 'lol'",
                  "class Duck (get @lol);"
                  "var d = Duck();"
                  "d.lol;");

  assertException("AttributeError",
                  "'Duck' object has no attribute 'a'",
                  "class Duck ("
                  "  init {@a = 6};"
                  "  private get @a;"
                  ");"
                  "Duck().a");

}


static void classSetter(void) {
  assertException("SyntaxError",
                  "Setter redefinition",
                  "class Duck (set @lol; set @lol);");

  assertException("SyntaxError",
                  "A setter must have one parameter",
                  "class Duck (set @a {});");

  assertException("SyntaxError",
                  "A setter must have one parameter",
                  "class Duck (set @a {a, b:});");

  assertEvalEq("a",
               "class Duck ("
               "  init {@s = 'b'};"
               "  set @s;"
               ");"
               "var d = Duck();"
               "d.s = 'a';"
               );

  assertEvalEq("asg",
               "class Duck ("
               "  get @s {@_s + 'g'};"
               "  set @s {s : @_s = s + 's'};"
               ");"
               "var d = Duck();"
               "d.s = 'a';"
               "d.s"
               );

  assertException("AttributeError",
                  "'Duck' object has no attribute 'a'",
                  "class Duck ();"
                  "var d = Duck();"
                  "d.a = 'a';"
                  );

  assertException("AttributeError",
                  "'Duck' object has no attribute 'a'",
                  "class Duck ("
                  "  private set @a;"
                  ");"
                  "var d = Duck();"
                  "d.a = 'a';"
                  );

  assertException("AttributeError",
                  "'Duck' object has no attribute 'a'",
                  "class Duck ("
                  "  private set @a {a: };"
                  ");"
                  "var d = Duck();"
                  "d.a = 'a';"
                  );
}


static void classVector(void) {
  const char *s;

#define VECTOR ""                               \
    "class Vector2 ("                           \
    "  init {x, y:"                             \
    "    @x = x;"                               \
    "    @y = y;"                               \
    "  };"                                      \
    "  get @x; get @y;"                         \
    "  get @toString {"                         \
    "    '(' + @x + ', ' + @y + ')'"            \
    "  };"                                      \
    ");"

  s = VECTOR
    "var v = Vector2(4, 5);"
    "v.x + v.y";
  assertEvalEq("9", s);

  s = VECTOR
    "Vector2(4, 5)";
  assertEvalEq("(4, 5)", s);


  s = VECTOR
    "var v = Vector2(4, 5);"
    "v.x = 1";
  assertException("AttributeError",
                  "'Vector2' object has no attribute 'x'",
                  s);

#undef VECTOR
#define VECTOR ""                               \
    "class Vector2 ("                           \
    "  init {x, y:"                             \
    "    @x = x;"                               \
    "    @y = y;"                               \
    "  };"                                      \
    ""                                          \
    "  get @x; get @y;"                         \
    "  set @x; set @y;"                         \
    ""                                          \
    "  get @toString {"                         \
    "    '(' + @x + ', ' + @y + ')'"            \
    "  };"                                      \
    ");"

  s = VECTOR
    "var v = Vector2(4, 5);"
    "v.x = -1;"
    "v";
  assertEvalEq("(-1, 5)", s);

#undef VECTOR

}


static void classPerson(void) {
  const char *s;

#define PERSON ""                                       \
    "class Person ("                                    \
    "  init {name, age:"                                \
    "    @name = name;"                                 \
    "    @age = age;"                                   \
    "  };"                                              \
    ""                                                  \
    "  private get @name {@_name};"                     \
    "  private set @name {name: @_name = name};"        \
    ""                                                  \
    "  private get @age; private set @age;"             \
    ""                                                  \
    "  get @ageString {@age + ' years old'};"           \
    ""                                                  \
    "  get @toString {"                                 \
    "    @name + ', ' + @ageString"                     \
    "  };"                                              \
    ");"

  s = PERSON
    "var john = Person('John', 34);"
    "john";
  assertEvalEq("John, 34 years old", s);
}


static void checkBuiltinClass(const char *className) {
  static char result[64];
  snprintf(result, 63, "<Class %s>", className);
  assertEvalEq(result, className);
}

static void builtinClasses(void) {
  const wsky_ClassArray *array = wsky_getBuiltinClasses();

  for (size_t i = 0; i < array->count; i++) {
    wsky_Class *class = array->classes[i];
    if (class != wsky_Scope_CLASS && class != wsky_ProgramFile_CLASS)
      checkBuiltinClass(class->name);
  }

  assertException("NameError",
                  "Use of undeclared identifier 'Scope'",
                  wsky_Scope_CLASS->name);

  assertException("NameError",
                  "Use of undeclared identifier 'ProgramFile'",
                  wsky_ProgramFile_CLASS->name);
}


static void inheritance(void) {
  assertException("TypeError",
                  "Expected a 'B', got a 'A'",
                  "class A ();"
                  "class B ();"
                  "var b = B();"
                  "B.get(A(), '2')");

  assertException("TypeError",
                  "Expected a 'B', got a 'A'",
                  "class A ();"
                  "class B ();"
                  "var b = B();"
                  "B.set(A(), '2', 123)");

  assertException("AttributeError",
                  "'A' object has no attribute 'foo'",
                  "class A ();"
                  "var a = A();"
                  "A.get(a, 'foo')");

  assertException("AttributeError",
                  "'A' object has no attribute 'foo'",
                  "class A ();"
                  "var a = A();"
                  "A.set(a, 'foo', 123)");
}


static void ctorInheritance(void) {

}


static void ifElse(void) {
  assertEvalEq("1", "if true: 1");
  assertEvalEq("null", "if false: 1");
  assertEvalEq("1", "if true: 1 else: 2");
  assertEvalEq("2", "if false: 1 else: 2");
  assertEvalEq("3", "if false: 1 else if true: 3 else: 2");
  assertEvalEq("2", "if false: 1 else if false: 3 else: 2");
  assertEvalEq("null", "if false: 1 else if false: 3");

  assertException("TypeError", "Expected a Boolean",
                  "if 123: 0");
}


static void helloScript(void) {
  char *filePath = getLocalFilePath("hello.wsky");
  assertReturnValueEq("Hello, World!", wsky_evalFile(filePath, NULL),
                      __func__, YOLO__POSITION_STRING);
  wsky_free(filePath);
}


static void module(void) {
  assertEvalEq("<Module math>", "import math");

  assertException("SyntaxError", "Expected module name",
                  "import");

  assertException("ImportError", "No module named 'barfoofoobar'",
                  "import barfoofoobar");

  assertException("NameError", "Use of undeclared identifier 'math'",
                  "math");
}


static void try(void) {
  assertEvalEq("1", "try: 1 except: 2");
  assertEvalEq("2", "try: supinfo except: 2");
  assertEvalEq("2", "try: supinfo except Exception: 2");
  assertEvalEq("2", "try: supinfo except Exception as e: 2");
  assertEvalEq("3", "try: a except ZeroDivisionError as e: 2 except: 3");
  assertEvalEq("2", "try: 6 / 0 except ZeroDivisionError as e: 2 except: 3");

  assertEvalEq("<ZeroDivisionError>",
               "try: 6 / 0 except ZeroDivisionError as e: e");
  assertEvalEq("<ZeroDivisionError>",
               "try: 6 / 0 except Exception as e: e");

  assertException("NameError", "Use of undeclared identifier 'a'",
                  "try: a except ZeroDivisionError as e: 2");
  assertException("NameError", "Use of undeclared identifier 'a'",
                  "try: a except ZeroDivisionError: 2 ");

  assertException("ZeroDivisionError", "Division by zero",
                  "try: a except NameError: 2 / 0 ");
  assertException("ZeroDivisionError", "Division by zero",
                  "try: a except: 2 / 0 ");
}


void evalTestSuite(void) {
  syntaxError();

  comment();

  literals();
  strings();

  unaryOps();
  binaryOps();
  binaryCmpOps();
  binaryBoolOps();
  sequence();
  var();
  variable();
  scope();
  function();
  call();
  functionScope();
  method();
  toString();
  getClass();
  objectEquals();
  string();
  class();
  classGetter();
  classSetter();
  classMethod();
  classVector();
  classPerson();
  builtinClasses();
  inheritance();
  ctorInheritance();
  ifElse();
  helloScript();
  module();
  try();
}
