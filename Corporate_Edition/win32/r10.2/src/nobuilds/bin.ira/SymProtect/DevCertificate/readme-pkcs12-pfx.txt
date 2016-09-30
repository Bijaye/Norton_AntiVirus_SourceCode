This documents how to create a IE 5.0 compatible PKCS #12 package. This is 
also referred to as a Person Information Exchange (PFX) format.

Using Cert-C 2.0's pkcs12exp.exe sample program, I did the following session:

PKCS #12 Export Operations
  A - Export a private key and certificate chain
  B - Create an arbitrary PKCS #12 message
Enter choice (blank to quit): a
Supply the keypair to enclose in the PKCS #12 message in the form of a
binary BER-encoded X.509 certificate and a binary PKCS #8 PrivateKeyInfo.
Enter name of file containing cert object binary
(blank to generate a new one): SymTestCodeSigningEE.cer
Enter name of file containing corresponding private key
(blank to cancel): SymTestCodeSigningEE-ber.pvk
If you don't want to export an entire chain, just supply the subject's
cert (or uppermost cert in the chain) as a trusted certificate.
Supply trusted certificate(s).
Enter name of cert binary file (blank when finished): SymTestRootCA.cer
Enter name of cert binary file (blank when finished):
Optionally supply other certificates necessary for path validation.
Enter name of cert binary file (blank when finished): SymTestCodeSigningCA.cer
Enter name of cert binary file (blank when finished):
Optionally supply CRLs necessary for path validation.
Enter name of CRL binary file (blank when finished):
Encryption Algorithms
   0 - 3-Key Triple-DES CBC
   1 - 2-Key Triple-DES CBC
   2 - 128-bit RC4
   3 - 40-bit RC4
   4 - 128-bit RC2 CBC
   5 - 40-bit RC2 CBC
Choice (blank to quit): 5
PKCS #12 Format Options
   0 - PKCS12_MS_EXPLORER
   1 - PKCS12_NS_NAVIGATOR
Choice (blank to quit): 0
Using SHA-1 HMAC.  Recommended iteration count is 1024.
Specify an integer for the iteration count: 1024
Supply password to protect PKCS #12 contents: hello
Enter name of file to store PKCS #12 binary: try7.p12
Operation successful!