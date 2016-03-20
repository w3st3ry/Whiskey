/*
** value_private.h for  in /home/antoine
**
** Made by antoine
** Login   <antoine@epitech.net>
**
** Started on  Sun Mar 20 13:32:18 2016 antoine
** Last update Sun Mar 20 13:32:18 2016 antoine
*/

#ifndef VALUE_PRIVATE_H
# define VALUE_PRIVATE_H

# include "value.h"

typedef wsky_Type Type;
typedef wsky_Value Value;

#define Type_BOOL       wsky_Type_BOOL
#define Type_INT        wsky_Type_INT
#define Type_FLOAT      wsky_Type_FLOAT
#define Type_OBJECT     wsky_Type_OBJECT

#define Value_TRUE      wsky_Value_TRUE
#define Value_FALSE     wsky_Value_FALSE
#define Value_NULL      wsky_Value_NULL

#define Value_fromBool          wsky_Value_fromBool
#define Value_fromInt           wsky_Value_fromInt
#define Value_fromFloat         wsky_Value_fromFloat
#define Value_fromObject        wsky_Value_fromObject

#define Value_new               wsky_Value_new

#endif /* VALUE_PRIVATE_H */
