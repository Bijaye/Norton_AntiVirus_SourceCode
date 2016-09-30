The following certificates and keys were generated using the
cert.exe RSA BSAFE Cert-C 2.0 sample program.

Extension meaning:

*.p12			- PKCS #12 certificate with associated private key. Password: hello
*.pfx			- same as p12
*.cer			- Certificate
*-ber.pub		- BER encoded public key
*-ber.pvk		- BER encoded private key
*-IssuerName.bin	- Binary file containing the Issuer Name
*-SubjectName.bin	- Binary file containing the Subject Name
*-Extensions.bin	- Binary file containing the Cert v3 extensions

For Windows, you can install any of the certificates on your
machines by simply double clicking on the *.cer file and
selecting the "Install Certificate..." button.

Below is the details about the generated certificates.

***************************************************************
SymTestRootCA.cer
- Version: 		3
- Serial #: 		01
- Signature Algorithm:	MD5 w/ RSA Encryption
- Issue
  - Organization (Printable String):	Symantec Corporation
  - Common Name (Printable String):	Symantec Test Root CA
- Validity Period: from	[UTC] Mon Apr 30 00:00:00 2001
		   to	[UTC] Sat Apr 30 23:59:59 2011 (10 Years)
- Subject:
  - Organization (Printable String):	Symantec Corporation
  - Common Name (Printable String):	Symantec Test Root CA

- Generated RSA Private/Public Key Length:	2048

- Unique Id for Issuer:		08324772824829108347	--- added in .try2 run
- Number of unused bits:	0

- Unique Id for Subject:	08324772824829108347
- Number of unused bits:	0

- Basic Constraings:
  - Included, Not Critical
  - CA = set
  - PathLenConstraint = unlimited

- Key Usage
  - included, not critical
  - KeyCertSign = set
  - CRLSign = set

***************************************************************
SymTestCodeSigningCA.cer
- Version: 		3
- Serial #: 		02
- Signature Algorithm:	MD5 w/ RSA Encryption
- Issuer:		SymTestRootCA
- Validity Period: from	[UTC] Mon Apr 30 00:00:00 2001
		   to	[UTC] Sun Apr 30 23:59:59 2006 (5 Years)
- Subject:
  - Organization (Printable String):	Symantec Corporation
  - Common Name (Printable String):	Symantec Test Code Signing CA

- Generated RSA Private/Public Key Length:	1024

- Unique Id for Issuer:		08324772824829108347
- Number of unused bits:	0

- Unique Id for Subject:	0671347196923492736426
- Number of unused bits:	0

- Basic Constraings:
  - Included, Not Critical
  - CA = set
  - PathLenConstraint = 0    --------- Is this the problem?????

- Key Usage
  - included, not critical
  - KeyCertSign = set
  - CRLSign = set

***************************************************************
SymTestCodeSigningEE2001.cer
- Version: 		3
- Serial #: 		03
- Signature Algorithm:	MD5 w/ RSA Encryption
- Issuer:		TestCodeSigningCA
- Validity Period: from	[UTC] Mon Jul 23 00:00:00 2001
		   to	[UTC] Tue Jul 23 23:59:59 2002 (1 Year)
- Subject:
  - Organization (Printable String):		Symantec Corporation
  - Common Name (Printable String):		Symantec Corporation
  - Organization Unit (Printable String):	SymSignature
  - Organization Unit (Printable String):	LiveUpdate

- Generated RSA Private/Public Key Length:	1024

- Unique Id for Issuer:		0671347196923492736426
- Number of unused bits:	0

- Unique Id for Subject:	7345671369101757382830
- Number of unused bits:	0

- Basic Constraings:
  - Included, Not Critical
  - End-Entity = set

- Key Usage
  - included, not critical
  - Digital Signature = set
  - nonrepudiation = set

- Extended Key Usage
  - included, not critical
  - Signing of downloadable executable code - (equals id-kp-codeSigning?)

***************************************************************
SymTestCodeSigningEE2002.cer
- Version: 		3
- Serial #: 		04
- Signature Algorithm:	MD5 w/ RSA Encryption
- Issuer:		TestCodeSigningCA
- Validity Period: from [UTC] Wed Sep 18 00:00:00 2002
		   to	[UTC] Thu Sep 18 23:59:59 2003 (1 Year)
- Subject:
  - Organization (Printable String):		Symantec Corporation
  - Common Name (Printable String):		Symantec Corporation
  - Organization Unit (Printable String):	SymSignature
  - Organization Unit (Printable String):	LiveUpdate

- Generated RSA Private/Public Key Length:	1024

- Unique Id for Issuer:		0671347196923492736426
- Number of unused bits:	0

- Unique Id for Subject:	3287567374891927361624
- Number of unused bits:	0

- Basic Constraings:
  - Included, Not Critical
  - End-Entity = set

- Key Usage
  - included, not critical
  - Digital Signature = set
  - nonrepudiation = set

- Extended Key Usage
  - included, not critical
  - Signing of downloadable executable code - (equals id-kp-codeSigning?)
