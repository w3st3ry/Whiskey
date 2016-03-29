/*
** whiskey_private.h for  in /home/antoine
**
** Made by antoine
** Login   <antoine@epitech.net>
**
** Started on  Sun Mar 20 13:30:39 2016 antoine
** Last update Sun Mar 20 13:30:39 2016 antoine
*/

#ifndef WHISKEY_PRIVATE_H
# define WHISKEY_PRIVATE_H

# include "value_private.h"
# include "return_value_private.h"


# define IMPORT(name) typedef wsky_##name name;

IMPORT(AttributeError)
IMPORT(Class)
IMPORT(ClassArray)
IMPORT(ClassDef)
IMPORT(Dict)
IMPORT(Exception)
IMPORT(Function)
IMPORT(ImportError)
IMPORT(InstanceMethod)
IMPORT(Keyword)
IMPORT(LexerResult)
IMPORT(Method)
IMPORT(MethodDef)
IMPORT(MethodFlags)
IMPORT(Module)
IMPORT(ModuleList)
IMPORT(NameError)
IMPORT(NotImplementedError)
IMPORT(Object)
IMPORT(ObjectFields)
IMPORT(Operator)
IMPORT(ParameterError)
IMPORT(ParserResult)
IMPORT(Position)
IMPORT(ProgramFile)
IMPORT(Scope)
IMPORT(String)
IMPORT(StringReader)
IMPORT(Structure)
IMPORT(SyntaxError)
IMPORT(SyntaxErrorEx)
IMPORT(Token)
IMPORT(TokenList)
IMPORT(TokenType)
IMPORT(TypeError)
IMPORT(ValueError)
IMPORT(ZeroDivisionError)

# undef IMPORT



/* AST nodes */

typedef wsky_ASTNode Node;
typedef wsky_ASTNodeType NodeType;
typedef wsky_ASTNodeList NodeList;

#define IMPORT(name) typedef wsky_##name##Node name##Node;

IMPORT(Literal)
IMPORT(Identifier)
IMPORT(Html)
IMPORT(TpltPrint)
IMPORT(Operator)
IMPORT(Sequence)
IMPORT(Function)
IMPORT(Var)
IMPORT(Assignment)
IMPORT(Call)
IMPORT(MemberAccess)
IMPORT(Class)
IMPORT(ClassMember)
IMPORT(Import)
IMPORT(Export)
IMPORT(If)
IMPORT(Except)
IMPORT(Try)

#undef IMPORT

#endif /* WHISKEY_PRIVATE_H */
