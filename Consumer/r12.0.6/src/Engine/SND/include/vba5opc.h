//*************************************************************************
//
// Filename: VBA5OPC.H
//
// Description: This file contains definitions of the VBA5 opcodes.
//  The information is taken from OPCODES.DOC from Microsoft.
//  The opcode parameters have been verified and corrected using
//  gabyO97OpSkip[] in OLESSAPI::O97MOD.CPP.
//
//*************************************************************************

#ifndef _VBA5OPC_H_

#define _VBA5OPC_H_

//*************************************************************************
//
// OPCODE Arguments
//
//  Type        Size    Description
//  ----------  ------  -----------
//  HLNAM       2       Handle to the String stored in the name manager
//  USHORT      2       Unsigned short
//  HIMPTYPE    2       Handle to the import type or user defined data type
//  HFUNC_DEFN  4       Handle to the function declaration
//  HVAR_DEFN   4       Handle to the variable declaration
//  HRT_DEFN    4       Handle to the record type definition
//  HTYPE_DEFN  4       Handle to the type definition
//  POTEX       4       Pointer to the excode offset
//  HCONTEXT    4       Handle to the context information
//
//*************************************************************************
//
//  000: OP_Imp
//  Perform a logical implication on two expressions.
//  Syntax: result = expression1 Imp expression2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  001: OP_Eqv
//  Perform a logical equivalence on two expressions.
//  Syntax: result = expression1 Eqv expression2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  002: OP_Xor
//  Perform a logical exclusion on two expressions.
//  Syntax: [result =] expression1 Xor expression2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  003: OP_And
//  Perform a logical conjunction on two expressions.
//  Syntax: result = expression1 And expression2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  004: OP_Or
//  Perform a logical disjunction on two expressions.
//  Syntax: result = expression1 Or expression2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  005: OP_Eq
//  Compare expressions (Equal to)
//  Syntax: expression1 = expression2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  006: OP_Ne
//  Compare expressions (Not equal to)
//  Syntax: expression1 <> expression2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  007: OP_Le
//  Compare expressions (Less than or equal to)
//  Syntax: expression1 <= expression2.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  008: OP_Ge
//  Compare expressions (Greater than or equal to)
//  Syntax: expression1 >= expression2.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  009: OP_Lt
//  Compare expressions (Less than)
//  Syntax: expression1 < expression2.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  010: OP_Gt
//  Compare expressions (Greater than)
//  Syntax: expression1 > expression2.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  011: OP_Add
//  Sum two numbers.
//  Syntax: result = expression1 + expression2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  012: OP_Sub
//  Find the difference between two numbers
//  Syntax: result = expression1 - expression2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  013: OP_Mod
//  Divide two numbers and return only the remainder.
//  Syntax: result = number1 Mod number2.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  014: OP_IDv
//  Divide two numbers and return an integer result.
//  Syntax: result = number1 \ number2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  015: OP_Mul
//  Multiply two numbers.
//  Syntax: result = number1 * number2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  016: OP_Div
//  Divide two numbers and return a floating-point result.
//  Syntax: result = number1 / number2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  017: OP_Concat
//  Force string concatenation of two expressions.
//  Syntax: result = expression1 & expression2.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  018: OP_Like
//  Compare two strings.
//  Syntax: result = string Like pattern
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  019: OP_Pwr
//  Raise a number to the power of an exponent.
//  Syntax: result = number ^ exponent
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  020: OP_Is
//  Compare two object reference variables.
//  Syntax: result = object1 Is object2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  021: OP_Not
//  Perform logical negation on an expression.
//  Syntax: result = Not expression
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  022: OP_UMi
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  023: OP_FnAbs
//  Returns the absolute value of a number.
//  Syntax: Abs(number)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  024: OP_FnFix
//  Returns the integer portion of a number.
//  Syntax: Fix(number)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  025: OP_FnInt
//  Returns the integer portion of a number.
//  Syntax: Int(number)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  026: OP_FnSgn
//  Returns the sign of a number.
//  Syntax: Sgn(number)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  027: OP_FnLen
//  Returns the number of characters in a string or size of a variable.
//  Syntax: Len(string | varname)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  028: OP_FnLenB
//  Returns the number of characters in a string or size of a variable.
//  Syntax: Len(string | varname)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  029: OP_Paren
//  No longer in use.
//
//  030: OP_Sharp
//  Indicates that the expression just evaluated was preceded by a '#' character.
//  Syntax: Open <filename> As #5
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  031: OP_LdLHS
//  No longer in use.
//
//  032: OP_Ld
//  Load the value of the given variable.
//  Syntax: x = y
//  Fixed Arguments: 1  Variable Arguments: 0
//  Argument: HLNAM
//
//  033: OP_MemLd
//  Load the value of the given variable.
//  Syntax : x = a.y
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments : HLNAM
//
//  034: OP_DictLd
//  Load the value of the given variable.
//  Syntax : x = a!y
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments : HLNAM
//
//  035: OP_IndexLd
//  Number of indexes in the array that we are trying to get the value for.
//  Syntax : y = a(13)(3,2)
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments : USHORT  - Number of array arguments in expression
//
//  036: OP_ArgsLd
//  Load the variable with the given arguments.
//  Syntax : y = a(13,10)
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  037: OP_ArgsMemLd
//  Load the variable with the given arguments.
//  Syntax : y = z.a(13,10)
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  038: OP_ArgsDictLd
//  Load the variable with the given arguments.
//  Syntax : y = z!a(13,10)
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  039: OP_St
//  Store into the given variable.
//  Syntax: x = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Argument: HLNAM
//
//  040: OP_MemSt
//  Store into the given variable.
//  Syntax: a.y = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Argument: HLNAM
//
//  041: OP_DictSt
//  Store into the given variable.
//  Syntax: a!y = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Argument: HLNAM
//
//  042: OP_IndexSt
//  Store into an array.
//  Syntax: a(13)(3,2) = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Argument: HLNAM
//
//  043: OP_ArgsSt
//  Store into an array.
//  Syntax: a(13,10) = 1
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  044: OP_ArgsMemSt
//  Store into an array.
//  Syntax: z.a(13,10) = 1
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  045: OP_ArgsDictSt
//  Store into an array.
//  Syntax: z!a(13,10) = 1
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  046: OP_Set
//  Set a variable with a value.
//  Syntax: Set x = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  047: OP_MemSet
//  Set a variable with a value.
//  Syntax: Set a.y = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  048: OP_DictSet
//  Set a variable with a value.
//  Syntax: Set a!y = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  049: OP_IndexSet
//  Set with a value.
//  Syntax: Set a(13)(3,2) = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  050: OP_ArgsSet
//  Set with a value.
//  Syntax: Set a(13,10) = 1
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  051: OP_ArgsMemSet
//  Set with a value.
//  Syntax: Set z.a(13,10)  = 1
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  052: OP_ArgsDictSet
//  Set with a value.
//  Syntax: Set z!a(13,10)
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  053: OP_MemLdWith
//  Load the value of the given variable.
//  Syntax: x = .y
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  054: OP_DictLdWith
//  Load the value of the given variable.
//  Syntax: x = !y
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  055: OP_ArgsMemLdWith
//  Load the value of the given arguments.
//  Syntax: y = .z.a(13,10)
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  056: OP_ArgsDictLdWith
//  Load the variable with the given arguments.
//  Syntax : y = .z!a(13,10)
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  057: OP_MemStWith
//  Store into the given variable.
//  Syntax: .a.y = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  058: OP_DictStWith
//  Store into the given variable.
//  Syntax: .a!y = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  059: OP_ArgsMemStWith
//  Store into an array.
//  Syntax: .z.a(13,10) = 1
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  060: OP_ArgsDictStWith
//  Store into an array.
//  Syntax: .z!a(13,10) = 1
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  061: OP_MemSetWith
//  Set a variable with a value.
//  Syntax: Set .a.y = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  062: OP_DictSetWith
//  Set a variable with a value.
//  Syntax: Set .a!y = 1
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  063: OP_ArgsMemSetWith
//  Set with a value.
//  Syntax: Set .z.a(13,10)  = 1
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  064: OP_ArgsDictSetWith
//  Set with a value.
//  Syntax: Set .z!a(13,10)
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  065: OP_ArgsCall
//  Call with a given variable.
//  Syntax: Call proc y
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  066: OP_ArgsMemCall
//  Call with a given variable.
//  Syntax: Call proc a.y
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  067: OP_ArgsMemCallWith
//  Call with a given variable.
//  Syntax: Call proc .a.y
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  068: OP_ArgsArray
//  Call with a given array.
//  Syntax: Call proc array
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  HLNAM
//          USHORT  - Number of array arguments in expression
//
//  069: OP_Bos
//  Marks the beginning of a statement.
//  Syntax: :
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  070: OP_BosImplicit
//  Marks the beginning of a statement with the :.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  071: OP_Bol
//  Beginning of a line.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  072: OP_Case
//  Executes one of several groups of statements, depending on the value of an expression.
//  Syntax: Select Case testexpression
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  073: OP_CaseTo
//  Specifies a range of values for case.
//  Syntax: Case expression1 To expression2
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  074: OP_CaseGt
//  Case expression greater than.
//  Syntax: Case Is > expression1
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  075: OP_CaseLt
//  Case expression less than.
//  Syntax: Case Is < expression1
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  076: OP_CaseGe
//  Case expression greater than or equal to.
//  Syntax: Case Is >= expression1
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  077: OP_CaseLe
//  Case expression less than or equal to.
//  Syntax: Case Is <= expression1
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  078: OP_CaseNe
//  Case expression not equal to.
//  Syntax: Case Is <> expression1
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  079: OP_CaseEq
//  Case expression equal to.
//  Syntax: Case Is = expression1
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  080: OP_CaseElse
//  Default case.
//  Syntax: Case Else
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  081: OP_CaseDone
//  Case done.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  082: OP_Circle
//  Draws a circle, ellipse, or arc on an object.
//  Syntax: object.Circle [Step] (x, y), radius, [color, start, end, aspect]
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  083: OP_Close
//  Concludes input/output (I/O) to a file opened using the Open statement.
//  Syntax: Close [filenumberlist]
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  084: OP_CloseAll
//  Close all open files.
//  Syntax: Close
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  085: OP_Coerce
//  Coerce an expression to a specific data type.
//  Syntax: X = cBool(y)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  086: OP_CoerceVar
//  Returns a Variant of subtype Error containing an error number specified by the user.
//  Syntax: X = cVErr(y)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  087: OP_Context
//  Beginning of special context opcodes.
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HCONTEXT
//
//  088: OP_Debug
//  Debug object.
//  Syntax: Debug.Print “hello world”
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  089: OP_Deftype
//  Set the default data type for variables and Function procedures.
//  Syntax: DefInt A-K
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  USHORT
//          USHORT
//
//  090: OP_Dim
//  Declares variables and allocates storage space.
//  Syntax: Dim [WithEvents] varname[([subscripts])] [As [New] type] [, [WithEvents]
//  varname[([subscripts])] [As [New] type]] . . .
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  091: OP_DimImplicit
//  Another keyword was used like Static or Shared.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  092: OP_Do
//  Repeats a block of statements.
//  Syntax: Do [{While | Until} condition]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  093: OP_DoEvents
//  Yields execution so that the operating system can process other events.
//  Syntax: DoEvents( )
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  094: OP_DoUntil
//  Repeats a block of statements until a condition becomes True.
//  Syntax: Do Until condition
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  095: OP_DoWhile
//  Repeats a block of statements while a condition is True.
//  Syntax: Do While condition
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  096: OP_Else
//  Conditionally executes a group of statements, depending on the value of an expression.
//  Syntax: If condition Then [statements] [Else elsestatements]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  097: OP_ElseBlock
//  Begin an Else block.
//  Syntax: Else
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  098: OP_ElseIfBlock
//  Begin an Else If block.
//  Syntax: Else If
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  099: OP_ElseIfTypeBlock
//  No longer used.
//
//  100: OP_End
//  Ends a procedure or block.
//  Syntax: End
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  101: OP_EndContext
//  End of special context opcodes.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  102: OP_EndFunc
//  End a function statement.
//  Syntax: End Function
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  103: OP_EndIf
//  End a block If…Then…Else statement.
//  Syntax: End If
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  104: OP_EndIfBlock
//  End of an If block.
//  Syntax: End If
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  105: OP_EndImmediate
//  No longer used.
//
//  106: OP_EndProp
//  End a Property Let, Property Get, or Property Set procedure.
//  Syntax: End Property
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  107: OP_EndSelect
//  End a Select Case statement.
//  Syntax: End Select
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  108: OP_EndSub
//  End a Sub statement.
//  Syntax: End Sub
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  109: OP_EndType
//  End a user-defined type definition (Type statement).
//  Syntax: End Type
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  110: OP_EndWith
//  End a With statement.
//  Syntax: End With
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  111: OP_Erase
//  Reinitializes the elements of fixed-size arrays and releases dynamic-array storage space.
//  Syntax: Erase arraylist
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  112: OP_Error
//  Simulates the occurrence of an error.
//  Syntax: Error errornumber.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  113: OP_ExitDo
//  Exit a Do loop.
//  Syntax: Exit Do
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  114: OP_ExitFor
//  Exit a For loop.
//  Syntax: Exit For
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  115: OP_ExitFunc
//  Causes an immediate exit from a Function procedure.
//  Syntax: Exit Function
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  116: OP_ExitProp
//  Causes an immediate exit from a Property Get procedure.
//  Syntax: Exit Property
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  117: OP_ExitSub
//  Cause an immediate exit from a Sub procedure.
//  Syntax: Exit Sub
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  118: OP_FnCurDir
//  Returns a Variant (String) representing the current path.
//  Syntax: CurDir[(drive)]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  119: OP_FnDir
//  Returns a String representing the name of a file, directory, or folder that matches a
//  specified pattern or file attribute, or the volume label of a drive.
//  Syntax: Dir[(pathname[, attributes])]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  120: OP_Empty0
//  Not used.
//
//  121: OP_Empty1
//  Not used.
//
//  122: OP_FnError
//  Returns the error message that corresponds to a given error number.
//  Syntax: Error[(errornumber)]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  123: OP_FnFormat
//  Returns a Variant (String) containing an expression formatted according to instructions
//  contained in a format expression.
//  Syntax: Format(expression[, format[, firstdayofweek[, firstweekofyear]]])
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  124: OP_FnFreeFile
//  Returns an Integer representing the next file number available for use by the Open
//  statement.
//  Syntax: FreeFile[(rangenumber)]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  125: OP_FnInStr
//  Returns a Variant (Long) specifying the position of the first occurrence of one string
//  within another.
//  Syntax: InStr([start, ]string1, string2[, compare])
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  126: OP_FnInStr3
//  An InStr with three arguments (see OP_FnInStr).
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  127: OP_FnInStr4
//  An InStr with four arguments (see OP_FnInStr).
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  128: OP_FnInStrB
//  Returns a Variant (Long) specifying the position of the first occurrence of one string
//  within another. Position is in byte data.
//  Syntax: InStrB([start, ]string1, string2[, compare])
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  129: OP_FnInStrB3
//  An InStrB with three arguments (see OP_FnInStrB).
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  130: OP_FnInStrB4
//  An InStrB with four arguments (see OP_FnInStrB).
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  131: OP_FnLBound
//  Returns a Long containing the smallest available subscript for the indicated dimension of
//  an array.
//  Syntax: LBound(arrayname[, dimension])
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  132: OP_FnMid
//  Returns a Variant (String) containing a specified number of characters from a string.
//  Syntax: Mid(string, start[, length])
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  133: OP_FnMidB
//  Returns a Variant (String) containing a specified number of characters from a string.
//  Position is in byte data.
//  Syntax: MidB(string, start[, length])
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  134: OP_FnStrComp
//  Returns a Variant (Integer) indicating the result of a string comparison.
//  Syntax: StrComp(string1, string2[, compare])
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  135: OP_FnStrComp3
//  StrComp with three arguments (see OP_FnStrComp).
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  136: OP_FnStringVar
//  Returns a Variant (String) representation of a number.
//  Syntax: String (number)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  137: OP_FnStringStr
//  No longer used.
//
//  138: OP_FnUBound
//  Returns a Long containing the largest available subscript for the indicated dimension of
//  an array.
//  UBound(arrayname[, dimension])
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  139: OP_For
//  Repeats a group of statements a specified number of times.
//  Syntax: For counter = start To end
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  140: OP_ForEach
//  Repeats a group of statements for each element in an array or collection.
//  Syntax: For Each element In group
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  141: OP_ForEachAs
//  No longer used.
//
//  142: OP_ForStep
//  Value to increment or decrement in a For loop.
//  Syntax: For Rep = 5 To 1 Step -1
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  143: OP_FuncDefn
//  Declares the beginning of a function.
//  Syntax: Function foo()
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HFUNCDEFN
//
//  144: OP_FuncDefnSave
//  Same as OP_FuncDefn.
//
//  145: OP_GetRec
//  Reads data from an open disk file into a variable.
//  Syntax: Get [#]filenumber, [recnumber], varname
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  146: OP_Gosub
//  Branches to a subroutine within a procedure.
//  Syntax: GoSub line
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  147: OP_Goto
//  Branches unconditionally to a specified line within a procedure.
//  Syntax: GoTo line
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  148: OP_If
//  Conditionally executes a group of statements, depending on the value of an expression.
//  Syntax: If condition Then [statements] [Else elsestatements]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  149: OP_IfBlock
//  Conditionally executes a group of statements, depending on the value of an expression.
//  Syntax: If condition Then
//        [statements]
//  [ElseIf condition-n Then
//       [elseifstatements] ...
//  [Else
//      [elsestatements]]
//         End If
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  150: OP_TypeOf
//  Used in an If condition.
//  Syntax: TypeOf objectname Is objecttype
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HIMPTYPE
//
//  151: OP_IfTypeBlock
//  No longer used.
//
//  152: OP_Input
//  Returns String containing characters from a file opened in Input or Binary mode.
//  Input(number, [#]filenumber)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  153: OP_InputDone
//  Variable list for Input # is done.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  154: OP_InputItem
//  A variable in the list for Input #.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  155: OP_Label
//  A label for the current line.
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  156: OP_Let
//  Assigns the value of an expression to a variable or property.
//  Syntax: [Let] varname = expression
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  157: OP_Line
//  The number of line continuations, and for each line continuation, a count of how many
//  tokens on the line, and how many spaces on the next line.
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  158: OP_LineCont
//  Line continuation mark.
//  Syntax: X = _
//                   1
//  Fixed Arguments: 0  Variable Arguments: 1
//
//  159: OP_LineInput
//  Reads a single line from an open sequential file and assigns it to a String variable.
//  Syntax: Line Input #filenumber, varname
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  160: OP_LineNum
//  A line number for the current line.
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  161: OP_LitCy
//  Currency literal.
//  Fixed Arguments: 4  Variable Arguments: 0
//  Arguments:  USHORT
//          USHORT
//  USHORT
//  USHORT
//
//  162: OP_LitDate
//  Date literal.
//  Fixed Arguments: 4  Variable Arguments: 0
//  Arguments:  USHORT
//          USHORT
//  USHORT
//  USHORT
//
//  163: OP_LitDefault
//  Default argument to an intrinsic SUB.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  164: OP_LitDI2
//  Short integer constants.
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  165: OP_LitDI4
//  Long integer constants.
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  USHORT
//      USHORT
//
//  166: OP_LitHI2
//  Hexadecimal integer constants.
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  167: OP_LitHI4
//  Hexadecimal long integer constants.
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  USHORT
//      USHORT
//
//  168: OP_LitNothing
//  Nothing.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  169: OP_LitOI2
//  Octal integer constants.
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  170: OP_LitOI4
//  Octal long integer constants.
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  USHORT
//      USHORT
//
//  171: OP_LitR4
//  Single precision floating point constants.
//  Fixed Arguments: 2  Variable Arguments: 0
//  Arguments:  USHORT
//          USHORT
//
//  172: OP_LitR8
//  Double precision floating point constants.
//  Fixed Arguments: 4  Variable Arguments: 0
//  Arguments:  USHORT
//          USHORT
//          USHORT
//          USHORT
//
//  173: OP_LitSmallI2
//  Integer constants.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  174: OP_LitStr
//  String literal.
//  Fixed Arguments: 0  Variable Arguments: 1
//
//  175: OP_LitVarSpecial
//  Special variant literal.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  176: OP_Lock
//  Controls access by other processes to all or part of a file opened using the Open
//  statement.
//  Syntax: Lock [#]filenumber[, recordrange]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  177: OP_Loop
//  Ends a Do loop.
//  Syntax: Do
//          [statementblock]
//      Loop
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  178: OP_LoopUntil
//  Indicates that the Do loop is executed until the condition is true.
//  Syntax: Do
//          [statementblock]
//      Loop Until [condition]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  179: OP_LoopWhile
//  Indicates that the Do loop is executed while the condition is true.
//  Syntax: Do
//          [statementblock]
//      Loop While [condition]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  180: OP_Lset
//  Left aligns a string within a string variable, or copies a variable of one user-defined type
//  to another variable of a different user-defined type.
//  Syntax: LSet stringvar = string
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  181: OP_Me
//  Not used.
//
//  182: OP_MeImplicit
//  Refer to the specific instance of the class where the code is executing.
//  Syntax: Line (1,2) - (3,4)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  183: OP_MemRedim
//  Reallocate storage space.
//  Syntax: Redim TestArray(Size)
//  Fixed Arguments: 3  Variable Arguments: 0
//  Arguments:  USHORT
//      USHORT
//      HTYPEDEFN
//
//  184: OP_MemRedimWith
//  Reallocate storage space.
//  Syntax: Redim .TestArray(Size)
//  Fixed Arguments: 3  Variable Arguments: 0
//  Arguments:  USHORT
//      USHORT
//          HTYPEDEFN
//
//  185: OP_MemRedimAs
//  Reallocate storage space.
//  Syntax: Redim TestArray(Size) As Integer
//  Fixed Arguments: 3  Variable Arguments: 0
//  Arguments:  USHORT
//      USHORT
//          HTYPEDEFN
//
//  186: OP_MemRedimAsWith
//  Reallocate storage space.
//  Syntax: Redim .TestArray(Size) As Integer
//  Fixed Arguments: 3  Variable Arguments: 0
//  Arguments:  USHORT
//      USHORT
//          HTYPEDEFN
//
//  187: OP_Mid
//  Returns a Variant (String) containing a specified number of characters from a string.
//  Syntax: Mid(string, start[, length])
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  188: OP_MidB
//  Returns a Variant (String) containing a specified number of characters from a string (in
//  byte count).
//  Syntax: MidB(string, start[, length])
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  189: OP_Name
//  Renames a disk file, directory, or folder.
//  Syntax: Name oldpathname As newpathname
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  190: OP_New
//  Enables implicit creation of an object.
//  Syntax: Static varname[([subscripts])] [As [New] type] [, varname[([subscripts])] [As
//  [New] type]] . . .
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HIMPTYPE
//
//  191: OP_Next
//  Next keyword.
//  Syntax: For Each element In group
//                      [statements]
//               Next [element]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  192: OP_NextVar
//  Next variable in For loop (see OP_Next).
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  193: OP_OnError
//  Enables an error-handling routine and specifies the location of the routine within a
//  procedure.
//  Syntax: On Error GoTo line
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  194: OP_OnGosub
//  Branch to one of several specified lines, depending on the value of an expression.
//  Syntax: On expression GoSub destinationlist
//  Fixed Arguments: 0  Variable Arguments: 1
//
//  195: OP_OnGoto
//  Branch to one of several specified lines, depending on the value of an expression.
//  Syntax: On expression GoTo destinationlist
//  Fixed Arguments: 0  Variable Arguments: 1
//
//  196: OP_Open
//  Enables input/output (I/O) to a file.
//  Syntax: Open pathname For mode [Access access] [lock] As [#]filenumber
//  [Len=reclength]
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  197: OP_Option
//  Option keyword.
//  Syntax: Option Explicit
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  198: OP_OptionBase
//  Used at module level to declare the default lower bound for array subscripts.
//  Syntax: Option Base {0 | 1}
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  199: OP_ParamByval
//  Indicates that the parameter just loaded was preceded with the ByVal keyword.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  200: OP_ParamOmitted
//  A parameter has been omitted from the argument list.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  201: OP_ParamNamed
//  A named parameter.
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  202: OP_PrintChan
//  Write data to a sequential file.
//  Syntax: Print #1, var
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  203: OP_PrintComma
//  Comma without an item.
//  Syntax: Print #1, ,
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  204: OP_PrintEos
//  End of print statement.
//  Syntax: Print #1, ,
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  205: OP_PrintItemComma
//  Determines the position of the next character printed.
//  Syntax: Print #1, “Zone 1”, “Zone 2”
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  206: OP_PrintItemNL
//  Print a new line.
//  Syntax: Print #1, var
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  207: OP_PrintItemSemi
//  Determines the position of the next character printed.
//  Syntax: Print #1, “Zone 1”; “Zone 2”
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  208: OP_PrintNL
//  Prints a new line after a statement.
//  Syntax: Print #1, "hello newline"
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  209: OP_PrintObj
//  Print from object.
//  Debug.Print “Hello”
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  210: OP_PrintSemi
//  Semicolon without an item.
//  Syntax: Print #1, ;
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  211: OP_PrintSpc
//  Print spaces.
//  Print Spc(10)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  212: OP_PrintTab
//  Print tabs.
//  Print Tab(10)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  213: OP_PrintTabComma
//  Comma separator between print tabs.
//  Print Tab(10), Tab(10)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  214: OP_Pset
//  Sets a point on an object to a specified color.
//  Syntax: [object.]Pset [Step](x,y)[,color]
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  215: OP_PutRec
//  Writes data from a variable to a disk file.
//  Syntax: Put [#]filenumber, [recnumber], varname
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  216: OP_QuoteRem
//  Used to include explanatory remarks in a program.
//  Syntax: ' comment
//  Fixed Arguments: 0  Variable Arguments: 1
//
//  217: OP_Redim
//  Used at procedure level to reallocate storage space for dynamic array variables.
//  Syntax: ReDim [Preserve] varname(subscripts) [As type] [, varname(subscripts) [As
//  type]] . . .
//  Fixed Arguments: 3  Variable Arguments: 0
//  Arguments:  USHORT
//          USHORT
//          HTYPEDEFN
//
//  218: OP_RedimAs
//  See OP_Redim.
//  Fixed Arguments: 3  Variable Arguments: 0
//  Arguments:  USHORT
//          USHORT
//          HTYPEDEFN
//
//  219: OP_Reparse
//  Syntax Error.
//  Fixed Arguments: 0  Variable Arguments: 1
//
//  220: OP_Rem
//  Used to include explanatory remarks in a program.
//  Syntax: Rem comment
//  Fixed Arguments: 0  Variable Arguments: 1
//
//  221: OP_Resume
//  Resumes execution after an error-handling routine is finished.
//  Syntax: Resume [0]
//  Resume Next
//  Resume line
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  222: OP_Return
//  Branches to and returns from a subroutine within a procedure.
//  Syntax: Return
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  223: OP_Rset
//  Right aligns a string within a string variable.
//  Syntax: RSet stringvar = string
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  224: OP_Scale
//  Defines the coordinate system for an object.
//  Syntax: [object.]Scale[(x1,y1) – (x2,y2)]
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  USHORT
//
//  225: OP_Seek
//  Sets the position for the next read/write operation within a file opened using the Open
//  statement.
//  Syntax: Seek [#]filenumber, position
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  226: OP_SelectCase
//  Executes one of several groups of statements, depending on the value of an expression.
//  Syntax:
//  Select Case testexpression
//  [Case expressionlist-n
//      [statements-n]] ...
//  [Case Else
//      [elsestatements]]
//  End Select
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  227: OP_SelectIs
//  No longer used.
//
//  228: OP_SelectType
//  No longer used.
//
//  229: OP_SetStmt
//  Assigns an object reference to a variable or property.
//  Syntax: Set objectvar = {[New] objectexpression | Nothing}
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  230: OP_Stack
//  No longer used.
//
//  231: OP_Stop
//  Suspends execution.
//  Syntax: Stop
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  232: OP_Type
//  Used at module level to define a user-defined data type containing one or more elements.
//  Syntax:
//  [Private | Public] Type varname
//       elementname [([subscripts])] As type
//      [elementname [([subscripts])] As type]
//      . . .
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HRTDEFN
//
//  233: OP_UnLock
//  Controls access by other processes to all or part of a file opened using the Open
//  statement.
//  Syntax: Unlock [#]filenumber[, recordrange]
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  234: OP_VarDefn
//  Defined a variable.
//  Syntax: Dim X
//  Fixed Arguments: 0  Variable Arguments: 1
//
//  235: OP_Wend
//  Executes a series of statements as long as a given condition is True.
//  Syntax: While condition
//         [statements]
//               Wend
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  236: OP_While
//  Executes a series of statements as long as a given condition is True.
//  Syntax: While condition
//         [statements]
//               Wend
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  237: OP_With
//  Executes a series of statements on a single object or a user-defined type.
//  Syntax: With object
//                      [statements]
//                End With
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  238: OP_WriteChan
//  Write data to a sequential file.
//  Syntax: Write #1, var
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  239: OP_ConstFuncExpr
//  Declares a function with an optional argument.
//  Sub foo(optional x = “String”)
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  240: OP_LbConst
//  Define a conditional compiler constant.
//  Syntax: #Const
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  241: OP_LbIf
//  Conditionally compiles selected blocks of Visual Basic code.
//  Syntax: #if
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  242: OP_LbElse
//  Conditionally compiles selected blocks of Visual Basic code.
//  Syntax: #Else
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  243: OP_LbElseif
//  Conditionally compiles selected blocks of Visual Basic code.
//  Sytax: #ElseIf
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  244: OP_LbEndif
//  Conditionally compiles selected blocks of Visual Basic code.
//  Sytax: #EndIf
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  245: OP_LbMark
//  Ignored.
//
//  246: OP_EndForVariable
//  End of For variable.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  247: OP_StartForVariable
//  Start of For variable.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  248: OP_NewRedim
//  The next OP_RedimAs contains the NEW keyword.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  249: OP_StartWithExpr
//  Executes a series of statements on a single object or a user-defined type.
//  Syntax: With object
//              [statements]
//  End With
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  250: OP_SetOrSt
//  Generated for watch functions to assign the expression result to the variant return value.
//  Fixed Arguments: 1  Variable Arguments: 0
//  Arguments:  HLNAM
//
//  251: OP_EndEnum
//  No longer used.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//  252: OP_Illegal
//  Highest legal opcode value plus one.
//  Fixed Arguments: 0  Variable Arguments: 0
//
//*************************************************************************

#define VBA5_OP_00_Imp              0x00 // 000:
#define VBA5_OP_01_Eqv              0x01 // 001:
#define VBA5_OP_02_Xor              0x02 // 002:
#define VBA5_OP_03_And              0x03 // 003:
#define VBA5_OP_04_Or               0x04 // 004:
#define VBA5_OP_05_Eq               0x05 // 005:
#define VBA5_OP_06_Ne               0x06 // 006:
#define VBA5_OP_07_Le               0x07 // 007:
#define VBA5_OP_08_Ge               0x08 // 008:
#define VBA5_OP_09_Lt               0x09 // 009:
#define VBA5_OP_0A_Gt               0x0A // 010:
#define VBA5_OP_0B_Add              0x0B // 011:
#define VBA5_OP_0C_Sub              0x0C // 012:
#define VBA5_OP_0D_Mod              0x0D // 013:
#define VBA5_OP_0E_IDv              0x0E // 014:
#define VBA5_OP_0F_Mul              0x0F // 015:
#define VBA5_OP_10_Div              0x10 // 016:
#define VBA5_OP_11_Concat           0x11 // 017:
#define VBA5_OP_12_Like             0x12 // 018:
#define VBA5_OP_13_Pwr              0x13 // 019:
#define VBA5_OP_14_Is               0x14 // 020:
#define VBA5_OP_15_Not              0x15 // 021:
#define VBA5_OP_16_UMi              0x16 // 022:
#define VBA5_OP_17_FnAbs            0x17 // 023:
#define VBA5_OP_18_FnFix            0x18 // 024:
#define VBA5_OP_19_FnInt            0x19 // 025:
#define VBA5_OP_1A_FnSgn            0x1A // 026:
#define VBA5_OP_1B_FnLen            0x1B // 027:
#define VBA5_OP_1C_FnLenB           0x1C // 028:
#define VBA5_OP_1D_Paren            0x1D // 029:
#define VBA5_OP_1E_Sharp            0x1E // 030:
#define VBA5_OP_1F_LdLHS            0x1F // 031: HLNAM
#define VBA5_OP_20_Ld               0x20 // 032: HLNAM
#define VBA5_OP_21_MemLd            0x21 // 033: HLNAM
#define VBA5_OP_22_DictLd           0x22 // 034: HLNAM
#define VBA5_OP_23_IndexLd          0x23 // 035: USHORT
#define VBA5_OP_24_ArgsLd           0x24 // 036: HLNAM USHORT
#define VBA5_OP_25_ArgsMemLd        0x25 // 037: HLNAM USHORT
#define VBA5_OP_26_ArgsDictLd       0x26 // 038: HLNAM USHORT
#define VBA5_OP_27_St               0x27 // 039: HLNAM
#define VBA5_OP_28_MemSt            0x28 // 040: HLNAM
#define VBA5_OP_29_DictSt           0x29 // 041: HLNAM
#define VBA5_OP_2A_IndexSt          0x2A // 042: USHORT
#define VBA5_OP_2B_ArgsSt           0x2B // 043: HLNAM USHORT
#define VBA5_OP_2C_ArgsMemSt        0x2C // 044: HLNAM USHORT
#define VBA5_OP_2D_ArgsDictSt       0x2D // 045: HLNAM USHORT
#define VBA5_OP_2E_Set              0x2E // 046: HLNAM
#define VBA5_OP_2F_MemSet           0x2F // 047: HLNAM
#define VBA5_OP_30_DictSet          0x30 // 048: HLNAM
#define VBA5_OP_31_IndexSet         0x31 // 049: USHORT
#define VBA5_OP_32_ArgsSet          0x32 // 050: HLNAM USHORT
#define VBA5_OP_33_ArgsMemSet       0x33 // 051: HLNAM USHORT
#define VBA5_OP_34_ArgsDictSet      0x34 // 052: HLNAM USHORT
#define VBA5_OP_35_MemLdWith        0x35 // 053: HLNAM
#define VBA5_OP_36_DictLdWith       0x36 // 054: HLNAM
#define VBA5_OP_37_ArgsMemLdWith    0x37 // 055: HLNAM USHORT
#define VBA5_OP_38_ArgsDictLdWith   0x38 // 056: HLNAM USHORT
#define VBA5_OP_39_MemStWith        0x39 // 057: HLNAM
#define VBA5_OP_3A_DictStWith       0x3A // 058: HLNAM
#define VBA5_OP_3B_ArgsMemStWith    0x3B // 059: HLNAM USHORT
#define VBA5_OP_3C_ArgsDictStWith   0x3C // 060: HLNAM USHORT
#define VBA5_OP_3D_MemSetWith       0x3D // 061: HLNAM
#define VBA5_OP_3E_DictSetWith      0x3E // 062: HLNAM
#define VBA5_OP_3F_ArgsMemSetWith   0x3F // 063: HLNAM USHORT
#define VBA5_OP_40_ArgsDictSetWith  0x40 // 064: HLNAM USHORT
#define VBA5_OP_41_ArgsCall         0x41 // 065: HLNAM USHORT
#define VBA5_OP_42_ArgsMemCall      0x42 // 066: HLNAM USHORT
#define VBA5_OP_43_ArgsMemCallWith  0x43 // 067: HLNAM USHORT
#define VBA5_OP_44_ArgsArray        0x44 // 068: HLNAM USHORT
#define VBA5_OP_45_Bos              0x45 // 069: USHORT
#define VBA5_OP_46_BosImplicit      0x46 // 070:
#define VBA5_OP_47_Bol              0x47 // 071:
#define VBA5_OP_48_Case             0x48 // 072:
#define VBA5_OP_49_CaseTo           0x49 // 073:
#define VBA5_OP_4A_CaseGt           0x4A // 074:
#define VBA5_OP_4B_CaseLt           0x4B // 075:
#define VBA5_OP_4C_CaseGe           0x4C // 076:
#define VBA5_OP_4D_CaseLe           0x4D // 077:
#define VBA5_OP_4E_CaseNe           0x4E // 078:
#define VBA5_OP_4F_CaseEq           0x4F // 079:
#define VBA5_OP_50_CaseElse         0x50 // 080:
#define VBA5_OP_51_CaseDone         0x51 // 081:
#define VBA5_OP_52_Circle           0x52 // 082: USHORT
#define VBA5_OP_53_Close            0x53 // 083: USHORT
#define VBA5_OP_54_CloseAll         0x54 // 084:
#define VBA5_OP_55_Coerce           0x55 // 085:
#define VBA5_OP_56_CoerceVar        0x56 // 086:
#define VBA5_OP_57_Context          0x57 // 087: HCONTEXT
#define VBA5_OP_58_Debug            0x58 // 088:
#define VBA5_OP_59_Deftype          0x59 // 089: USHORT USHORT
#define VBA5_OP_5A_Dim              0x5A // 090:
#define VBA5_OP_5B_DimImplicit      0x5B // 091:
#define VBA5_OP_5C_Do               0x5C // 092:
#define VBA5_OP_5D_DoEvents         0x5D // 093:
#define VBA5_OP_5E_DoUntil          0x5E // 094:
#define VBA5_OP_5F_DoWhile          0x5F // 095:
#define VBA5_OP_60_Else             0x60 // 096:
#define VBA5_OP_61_ElseBlock        0x61 // 097:
#define VBA5_OP_62_ElseIfBlock      0x62 // 098:
#define VBA5_OP_63_ElseIfTypeBlock  0x63 // 099: USHORT
#define VBA5_OP_64_End              0x64 // 100:
#define VBA5_OP_65_EndContext       0x65 // 101:
#define VBA5_OP_66_EndFunc          0x66 // 102:
#define VBA5_OP_67_EndIf            0x67 // 103:
#define VBA5_OP_68_EndIfBlock       0x68 // 104:
#define VBA5_OP_69_EndImmediate     0x69 // 105:
#define VBA5_OP_6A_EndProp          0x6A // 106:
#define VBA5_OP_6B_EndSelect        0x6B // 107:
#define VBA5_OP_6C_EndSub           0x6C // 108:
#define VBA5_OP_6D_EndType          0x6D // 109:
#define VBA5_OP_6E_EndWith          0x6E // 110:
#define VBA5_OP_6F_Erase            0x6F // 111: USHORT
#define VBA5_OP_70_Error            0x70 // 112:
#define VBA5_OP_71_ExitDo           0x71 // 113:
#define VBA5_OP_72_ExitFor          0x72 // 114:
#define VBA5_OP_73_ExitFunc         0x73 // 115:
#define VBA5_OP_74_ExitProp         0x74 // 116:
#define VBA5_OP_75_ExitSub          0x75 // 117:
#define VBA5_OP_76_FnCurDir         0x76 // 118:
#define VBA5_OP_77_FnDir            0x77 // 119:
#define VBA5_OP_78_Empty0           0x78 // 120:
#define VBA5_OP_79_Empty1           0x79 // 121:
#define VBA5_OP_7A_FnError          0x7A // 122:
#define VBA5_OP_7B_FnFormat         0x7B // 123:
#define VBA5_OP_7C_FnFreeFile       0x7C // 124:
#define VBA5_OP_7D_FnInStr          0x7D // 125:
#define VBA5_OP_7E_FnInStr3         0x7E // 126:
#define VBA5_OP_7F_FnInStr4         0x7F // 127:
#define VBA5_OP_80_FnInStrB         0x80 // 128:
#define VBA5_OP_81_FnInStrB3        0x81 // 129:
#define VBA5_OP_82_FnInStrB4        0x82 // 130:
#define VBA5_OP_83_FnLBound         0x83 // 131: USHORT
#define VBA5_OP_84_FnMid            0x84 // 132:
#define VBA5_OP_85_FnMidB           0x85 // 133:
#define VBA5_OP_86_FnStrComp        0x86 // 134:
#define VBA5_OP_87_FnStrComp3       0x87 // 135:
#define VBA5_OP_88_FnStringVar      0x88 // 136:
#define VBA5_OP_89_FnStringStr      0x89 // 137:
#define VBA5_OP_8A_FnUBound         0x8A // 138: USHORT
#define VBA5_OP_8B_For              0x8B // 139:
#define VBA5_OP_8C_ForEach          0x8C // 140:
#define VBA5_OP_8D_ForEachAs        0x8D // 141:
#define VBA5_OP_8E_ForStep          0x8E // 142:
#define VBA5_OP_8F_FuncDefn         0x8F // 143: HFUNCDEFN
#define VBA5_OP_90_FuncDefnSave     0x90 // 144: HFUNCDEFN
#define VBA5_OP_91_GetRec           0x91 // 145:
#define VBA5_OP_92_Gosub            0x92 // 146: HLNAM
#define VBA5_OP_93_Goto             0x93 // 147: HLNAM
#define VBA5_OP_94_If               0x94 // 148:
#define VBA5_OP_95_IfBlock          0x95 // 149:
#define VBA5_OP_96_TypeOf           0x96 // 150: HIMPTYPE
#define VBA5_OP_97_IfTypeBlock      0x97 // 151: HLNAM
#define VBA5_OP_98_Input            0x98 // 152:
#define VBA5_OP_99_InputDone        0x99 // 153:
#define VBA5_OP_9A_InputItem        0x9A // 154:
#define VBA5_OP_9B_Label            0x9B // 155: HLNAM
#define VBA5_OP_9C_Let              0x9C // 156:
#define VBA5_OP_9D_Line             0x9D // 157: USHORT
#define VBA5_OP_9E_LineCont         0x9E // 158: VARIABLE
#define VBA5_OP_9F_LineInput        0x9F // 159:
#define VBA5_OP_A0_LineNum          0xA0 // 160: HLNAM
#define VBA5_OP_A1_LitCy            0xA1 // 161: USHORT USHORT USHORT USHORT
#define VBA5_OP_A2_LitDate          0xA2 // 162: USHORT USHORT USHORT USHORT
#define VBA5_OP_A3_LitDefault       0xA3 // 163:
#define VBA5_OP_A4_LitDI2           0xA4 // 164: USHORT
#define VBA5_OP_A5_LitDI4           0xA5 // 165: USHORT USHORT
#define VBA5_OP_A6_LitHI2           0xA6 // 166: USHORT
#define VBA5_OP_A7_LitHI4           0xA7 // 167: USHORT USHORT
#define VBA5_OP_A8_LitNothing       0xA8 // 168:
#define VBA5_OP_A9_LitOI2           0xA9 // 169: USHORT
#define VBA5_OP_AA_LitOI4           0xAA // 170: USHORT USHORT
#define VBA5_OP_AB_LitR4            0xAB // 171: USHORT USHORT
#define VBA5_OP_AC_LitR8            0xAC // 172: USHORT USHORT USHORT USHORT
#define VBA5_OP_AD_LitSmallI2       0xAD // 173:
#define VBA5_OP_AE_LitStr           0xAE // 174: VARIABLE
#define VBA5_OP_AF_LitVarSpecial    0xAF // 175:
#define VBA5_OP_B0_Lock             0xB0 // 176:
#define VBA5_OP_B1_Loop             0xB1 // 177:
#define VBA5_OP_B2_LoopUntil        0xB2 // 178:
#define VBA5_OP_B3_LoopWhile        0xB3 // 179:
#define VBA5_OP_B4_Lset             0xB4 // 180:
#define VBA5_OP_B5_Me               0xB5 // 181:
#define VBA5_OP_B6_MeImplicit       0xB6 // 182:
#define VBA5_OP_B7_MemRedim         0xB7 // 183: USHORT USHORT HTYPEDEFN
#define VBA5_OP_B8_MemRedimWith     0xB8 // 184: USHORT USHORT HTYPEDEFN
#define VBA5_OP_B9_MemRedimAs       0xB9 // 185: USHORT USHORT HTYPEDEFN
#define VBA5_OP_BA_MemRedimAsWith   0xBA // 186: USHORT USHORT HTYPEDEFN
#define VBA5_OP_BB_Mid              0xBB // 187:
#define VBA5_OP_BC_MidB             0xBC // 188:
#define VBA5_OP_BD_Name             0xBD // 189:
#define VBA5_OP_BE_New              0xBE // 190: HIMPTYPE
#define VBA5_OP_BF_Next             0xBF // 191:
#define VBA5_OP_C0_NextVar          0xC0 // 192:
#define VBA5_OP_C1_OnError          0xC1 // 193: HLNAM
#define VBA5_OP_C2_OnGosub          0xC2 // 194: VARIABLE
#define VBA5_OP_C3_OnGoto           0xC3 // 195: VARIABLE
#define VBA5_OP_C4_Open             0xC4 // 196: USHORT
#define VBA5_OP_C5_Option           0xC5 // 197:
#define VBA5_OP_C6_OptionBase       0xC6 // 198:
#define VBA5_OP_C7_ParamByval       0xC7 // 199:
#define VBA5_OP_C8_ParamOmitted     0xC8 // 200:
#define VBA5_OP_C9_ParamNamed       0xC9 // 201: HLNAM
#define VBA5_OP_CA_PrintChan        0xCA // 202:
#define VBA5_OP_CB_PrintComma       0xCB // 203:
#define VBA5_OP_CC_PrintEos         0xCC // 204:
#define VBA5_OP_CD_PrintItemComma   0xCD // 205:
#define VBA5_OP_CE_PrintItemNL      0xCE // 206:
#define VBA5_OP_CF_PrintItemSemi    0xCF // 207:
#define VBA5_OP_D0_PrintNL          0xD0 // 208:
#define VBA5_OP_D1_PrintObj         0xD1 // 209:
#define VBA5_OP_D2_PrintSemi        0xD2 // 210:
#define VBA5_OP_D3_PrintSpc         0xD3 // 211:
#define VBA5_OP_D4_PrintTab         0xD4 // 212:
#define VBA5_OP_D5_PrintTabComma    0xD5 // 213:
#define VBA5_OP_D6_Pset             0xD6 // 214: USHORT
#define VBA5_OP_D7_PutRec           0xD7 // 215:
#define VBA5_OP_D8_QuoteRem         0xD8 // 216: VARIABLE
#define VBA5_OP_D9_Redim            0xD9 // 217: USHORT USHORT HTYPEDEFN
#define VBA5_OP_DA_RedimAs          0xDA // 218: USHORT USHORT HTYPEDEFN
#define VBA5_OP_DB_Reparse          0xDB // 219: VARIABLE
#define VBA5_OP_DC_Rem              0xDC // 220: VARIABLE
#define VBA5_OP_DD_Resume           0xDD // 221: HLNAM
#define VBA5_OP_DE_Return           0xDE // 222:
#define VBA5_OP_DF_Rset             0xDF // 223:
#define VBA5_OP_E0_Scale            0xE0 // 224: USHORT
#define VBA5_OP_E1_Seek             0xE1 // 225:
#define VBA5_OP_E2_SelectCase       0xE2 // 226:
#define VBA5_OP_E3_SelectIs         0xE3 // 227: USHORT
#define VBA5_OP_E4_SelectType       0xE4 // 228:
#define VBA5_OP_E5_SetStmt          0xE5 // 229:
#define VBA5_OP_E6_Stack            0xE6 // 230: USHORT USHORT USHORT
#define VBA5_OP_E7_Stop             0xE7 // 231:
#define VBA5_OP_E8_Type             0xE8 // 232: HRTDEFN
#define VBA5_OP_E9_UnLock           0xE9 // 233:
#define VBA5_OP_EA_VarDefn          0xEA // 234: VARIABLE
#define VBA5_OP_EB_Wend             0xEB // 235:
#define VBA5_OP_EC_While            0xEC // 236:
#define VBA5_OP_ED_With             0xED // 237:
#define VBA5_OP_EE_WriteChan        0xEE // 238:
#define VBA5_OP_EF_ConstFuncExpr    0xEF // 239:
#define VBA5_OP_F0_LbConst          0xF0 // 240: HLNAM
#define VBA5_OP_F1_LbIf             0xF1 // 241:
#define VBA5_OP_F2_LbElse           0xF2 // 242:
#define VBA5_OP_F3_LbElseif         0xF3 // 243:
#define VBA5_OP_F4_LbEndif          0xF4 // 244:
#define VBA5_OP_F5_LbMark           0xF5 // 245:
#define VBA5_OP_F6_EndForVariable   0xF6 // 246:
#define VBA5_OP_F7_StartForVariable 0xF7 // 247:
#define VBA5_OP_F8_NewRedim         0xF8 // 248:
#define VBA5_OP_F9_StartWithExpr    0xF9 // 249:
#define VBA5_OP_FA_SetOrSt          0xFA // 250: HLNAM
#define VBA5_OP_FB_EndEnum          0xFB // 251:
#define VBA5_OP_FC_Illegal          0xFC // 252:

#endif // #ifndef _VBA5OPC_H_

