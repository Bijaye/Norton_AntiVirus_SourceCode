/* $Header:   I:/VCS/INTERNAL.H_V   1.4   06 May 1992 09:06:24   ED  $ */

/*
 * $Log:   I:/VCS/INTERNAL.H_V  $
 * 
 *    Rev 1.4   06 May 1992 09:06:24   ED
 * Masked the file from multiple includes
 * 
 *    Rev 1.3   30 Jan 1992 18:07:50   ED
 * Removed some button prototypes that no longer exist or were
 * multiply defined.
 * 
 *    Rev 1.2   27 Jan 1992 22:36:28   TORSTEN
 * Changed the #include statements to use angle brackets.
 * 
 *    Rev 1.1   07 Feb 1991 14:02:00   ED
 * Modified the button_kbd() function to take an additional int
 * parameter.
 * 
 *    Rev 1.0   29 Aug 1990 11:24:28   DAVID
 * Initial revision.
 */


#ifndef  INCL_INTERNAL
#define INCL_INTERNAL

#ifndef INCL_DIALOG
#include <dialog.h>
#endif

void 	PASCAL init_button(BUTTONS *button);
int	PASCAL button_kbd(BUTTONS *b, int c, int items);
int	PASCAL key_button_num(BUTTONS *b,int c);
int     PASCAL count_buttons(char **strings);
int     PASCAL button_width(BUTTONS *b);
void 	PASCAL set_button_pad(BUTTONS *button, int gap, int pad);

int	PASCAL text_edit(int row, int col, int width, char *string, int flags);
int	PASCAL translate_key(int c);
void	PASCAL char_insert(TE_RECORD *te, char c);
void	PASCAL text_insert(TE_RECORD *te, char *c);
void	PASCAL text_delete(TE_RECORD *te, int c);
void	PASCAL text_left_word(TE_RECORD *te_record);
void	PASCAL text_right_word(TE_RECORD *te_record);
int	PASCAL is_delim(char c);

Word 	PASCAL TEStringLength (TERec *te);
void 	PASCAL TESetLength (TERec *te, Word length);
#endif
