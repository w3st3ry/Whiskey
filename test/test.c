#include "test.h"

#include "whiskey.h"

typedef wsky_ReturnValue ReturnValue;

char *getLocalFilePath(const char *fileName) {
  char *dirPath = wsky_path_getProgramDirectoryPath();
  yolo_assert_not_null(dirPath);
  char *file = wsky_path_concat(dirPath, fileName);
  yolo_assert_not_null(file);
  wsky_free(dirPath);
  return file;
}

static ReturnValue assertImpl(wsky_Object *self, wsky_Value *v) {
  (void)self;
  if (!wsky_isBoolean(*v)) {
    wsky_RAISE_NEW_PARAMETER_ERROR("Expected a boolean");
  }
  yolo_assert(v->v.boolValue);
  wsky_RETURN_NULL;
}

static ReturnValue printImpl(wsky_Object *self, wsky_Value *v) {
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

static void runWhiskeyTests(void) {
  char *filePath = getLocalFilePath("test.wsky");
  wsky_Scope *scope = wsky_Scope_newRoot(wsky_Module_newMain());

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

  ReturnValue rv = wsky_evalFile(filePath, scope);
  wsky_free(filePath);
  if (rv.exception) {
    wsky_Exception_print(rv.exception);
  }
}

int main() {
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
  mathTestSuite();

  runWhiskeyTests();

  wsky_stop();
  yolo_end();

  return 0;
}
