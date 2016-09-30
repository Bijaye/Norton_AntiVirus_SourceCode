// MIMEGrammar.h : Definitions for MIME grammar
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

/* This is the complete grammar from RFC1521.

   application-subtype := ("octet-stream" *stream-param)
					  / "postscript" / extension-token

   application-type :=	"application" "/" application-subtype

   attribute := token	; case-insensitive

   atype := "ftp" / "anon-ftp" / "tftp" / "local-file"
				 / "afs" / "mail-server" / extension-token
				 ; Case-insensitive

   audio-type := "audio" "/" ("basic" / extension-token)

   body-part := <"message" as defined in RFC 822,
			with all header fields optional, and with the
			specified delimiter not occurring anywhere in
			the message body, either on a line by itself
			or as a substring anywhere.>

	 NOTE: In certain transport enclaves, RFC 822 restrictions such as
	 the one that limits bodies to printable ASCII characters may not
	 be in force.	(That is, the transport domains may resemble
	 standard Internet mail transport as specified in RFC821 and
	 assumed by RFC822, but without certain restrictions.)	The
	 relaxation of these restrictions should be construed as locally
	 extending the definition of bodies, for example to include octets
	 outside of the ASCII range, as long as these extensions are
	 supported by the transport and adequately documented in the
	 Content-Transfer-Encoding header field. However, in no event are
	 headers (either message headers or body-part headers) allowed to
	 contain anything other than ASCII characters.

   boundary := 0*69<bchars> bcharsnospace

   bchars := bcharsnospace / " "

   bcharsnospace :=	  DIGIT / ALPHA / "'" / "(" / ")" / "+"  / "_"
				 / "," / "-" / "." / "/" / ":" / "=" / "?"

   charset := "us-ascii" / "iso-8859-1" / "iso-8859-2"/ "iso-8859-3"
		/ "iso-8859-4" / "iso-8859-5" /  "iso-8859-6" / "iso-8859-7"
		/ "iso-8859-8" / "iso-8859-9" / extension-token
		; case insensitive

   close-delimiter := "--" boundary "--" CRLF;Again,no space by "--",

   content	:= "Content-Type"  ":" type "/" subtype  *(";" parameter)
			; case-insensitive matching of type and subtype

   delimiter := "--" boundary CRLF  ;taken from Content-Type field.
								; There must be no space
								; between "--" and boundary.

   description := "Content-Description" ":" *text

   discard-text := *(*text CRLF)

   encapsulation := delimiter body-part CRLF

   encoding := "Content-Transfer-Encoding" ":" mechanism

   epilogue := discard-text		  ;  to  be  ignored upon receipt.

   extension-token :=  x-token / iana-token

   external-param :=   (";" "access-type" "=" atype)
				 / (";" "expiration" "=" date-time)

					  ; Note that date-time is quoted
				 / (";" "size" "=" 1*DIGIT)
				 / (";"  "permission"  "="  ("read" / "read-write"))
					  ; Permission is case-insensitive
				 / (";" "name" "="  value)
				 / (";" "site" "=" value)
				 / (";" "dir" "=" value)
				 / (";" "mode" "=" value)
				 / (";" "server" "=" value)
				 / (";" "subject" "=" value)
		  ;access-type required; others required based on access-type

   iana-token := <a publicly-defined extension token,
			registered with IANA, as specified in
			appendix E>

   id :=  "Content-ID" ":" msg-id

   image-type := "image" "/" ("gif" / "jpeg" / extension-token)

   mechanism :=		"7bit"    ;  case-insensitive
				 / "quoted-printable"
				 / "base64"
				 / "8bit"
				 / "binary"
				 / x-token

   message-subtype := "rfc822"
				 / "partial" 2#3partial-param
				 / "external-body" 1*external-param
				 / extension-token

   message-type := "message" "/" message-subtype

   multipart-body :=preamble 1*encapsulation close-delimiter epilogue

   multipart-subtype := "mixed" / "parallel" / "digest"
				 / "alternative" / extension-token

   multipart-type := "multipart" "/" multipart-subtype
				 ";" "boundary" "=" boundary

   octet := "=" 2(DIGIT / "A" / "B" / "C" / "D" / "E" / "F")
		; octet must be used for characters > 127, =, SPACE, or
   TAB,
		; and is recommended for any characters not listed in
		; Appendix B as "mail-safe".

   padding := "0" / "1" /  "2" /  "3" / "4" / "5" / "6" / "7"

   parameter := attribute "=" value

   partial-param :=		(";" "id" "=" value)
				 /	(";" "number" "=" 1*DIGIT)
				 /	(";" "total" "=" 1*DIGIT)
			; id & number required;total required for last part

   preamble := discard-text		 ;	to	be	ignored upon receipt.

   ptext := octet / <any ASCII character except "=", SPACE,  or TAB>
		; characters not listed as "mail-safe" in Appendix B
		; are also not recommended.

   quoted-printable := ([*(ptext / SPACE /  TAB)  ptext]  ["="] CRLF)
		; Maximum line length of 76 characters excluding CRLF

   stream-param :=	(";" "type" "=" value)
				/ (";" "padding" "=" padding)

   subtype := token	; case-insensitive

   text-subtype := "plain" / extension-token

   text-type := "text" "/" text-subtype [";" "charset" "=" charset]

   token  :=  1*<any  (ASCII) CHAR except SPACE, CTLs, or tspecials>

   tspecials :=	"(" / ")" / "<" / ">" / "@"
			 /	"," / ";" / ":" / "\" / <">
			 /	"/" / "[" / "]" / "?" / "="
			; Must be in quoted-string,
			; to use within parameter values

   type :=	  "application"     /  "audio"   ; case-insensitive
			/ "image"           / "message"
			/ "multipart"  / "text"
			/ "video"           / extension-token
			; All values case-insensitive

   value := token / quoted-string

   version := "MIME-Version" ":" 1*DIGIT "." 1*DIGIT

   video-type := "video" "/" ("mpeg" / extension-token)

   x-token := <The two characters "X-" or "x-" followed, with no
			 intervening white space, by any token>
*/
