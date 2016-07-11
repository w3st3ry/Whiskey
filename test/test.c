#include <assert.h>
#include <string.h>
#include "test.h"
#include "whiskey.h"

typedef wsky_Result Result;

/** The numer of tests written in Whiskey */
static int whiskeyAssertCount = 0;

char *getLocalFilePath(const char *fileName) {
  char *dirPath = wsky_path_getProgramDirectoryPath();
  yolo_assert_not_null(dirPath);
  char *file = wsky_path_concat(dirPath, fileName);
  yolo_assert_not_null(file);
  wsky_free(dirPath);
  return file;
}

static Result assertImpl(wsky_Object *self, wsky_Value *v) {
  whiskeyAssertCount++;
  (void)self;
  if (!wsky_isBoolean(*v)) {
    wsky_RAISE_NEW_PARAMETER_ERROR("Expected a Boolean");
  }
  yolo_assert(v->v.boolValue);
  wsky_RETURN_NULL;
}

static Result printImpl(wsky_Object *self, wsky_Value *v) {
  (void)self;
  if (!wsky_isString(*v)) {
    wsky_RAISE_NEW_PARAMETER_ERROR("Expected a String");
  }
  wsky_String *string = (wsky_String *)v->v.objectValue;
  printf("%s", string->string);
  wsky_RETURN_NULL;
}

static void addFunction(wsky_Scope *scope, const wsky_MethodDef *def) {
  wsky_Function *function = wsky_Function_newFromC(def);
  wsky_Value functionValue = wsky_Value_fromObject((wsky_Object *)function);
  wsky_Scope_addVariable(scope, def->name, functionValue);
}

static void printSyntaxError(wsky_SyntaxErrorEx *exception) {
  wsky_SyntaxError_print(&exception->syntaxError, stderr);
}

static wsky_Scope *getScope(void) {
  wsky_Scope *scope = wsky_Scope_newRoot(wsky_Module_newMain());
  assert(scope->module->file);

  wsky_MethodDef def = {
    .name = "assertImpl",
    .parameterCount = 1,
    .flags = wsky_MethodFlags_DEFAULT,
    .function = (wsky_Method0)&assertImpl,
  };
  addFunction(scope, &def);

  wsky_MethodDef printImplDef = {
    .name = "printImpl",
    .parameterCount = 1,
    .flags = wsky_MethodFlags_DEFAULT,
    .function = (wsky_Method0)&printImpl,
  };
  addFunction(scope, &printImplDef);

  return scope;
}

static void runWhiskeyTests(void) {
  char *filePath = getLocalFilePath("test.wsky");

  Result rv = wsky_evalFile(filePath, getScope());
  wsky_free(filePath);
  if (rv.exception) {
    yolo_fail();
    if (rv.exception->class == wsky_SyntaxErrorEx_CLASS)
      printSyntaxError((wsky_SyntaxErrorEx *)rv.exception);
    else
      wsky_Exception_print(rv.exception);
  }
}

int main(int argc, char **argv) {
  if (argc > 1 && strcmp(argv[1], "--gc-stress") == 0)
    wsky_GC_setStressed(true);

  whiskeyAssertCount = 0;
  yolo_begin();
  wsky_start();

  dictTestSuite();
  exceptionTestSuite();
  programFileTestSuite();
  positionTestSuite();
  stringReaderTestSuite();
  lexerTestSuite();
  parserTestSuite();
  evalTestSuite();

  runWhiskeyTests();

  wsky_stop();
  yolo_end();

  printf("%d tests are written in Whiskey\n", whiskeyAssertCount);
  return 0;
}
