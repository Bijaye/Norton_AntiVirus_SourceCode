// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// example_cert_store.h

// certs, keys and relative paths for objects in the example cert store


#ifndef __EXAMPLE_CERT_STORE__
#define __EXAMPLE_CERT_STORE__

extern const unsigned char server_group_root_cert_pem[];
extern const unsigned char server_group_root_cert_bad_pem[];

extern const unsigned char login_ca_cert_pem[];

extern const unsigned char server_cert_pem[];
extern const unsigned char server_key_pem[];

extern const unsigned char user_admin_cert_pem[];
extern const unsigned char user_admin_key_pem[];

const char g_szRelativePathToRootCert[]           = "roots/00112233445566778899AABBCCDDEEFF.0.servergroupca.cer";
const char g_szRelativePathToRootKey[]            = "private-keys/00112233445566778899AABBCCDDEEFF.0.servergroupca.pvk";
const char g_szRelativePathToRootCertBad[]        = "roots/FFFFEEEEDDDDCCCCBBBBAAAA99998888.0.servergroupca.bad.cer";
const char g_szRelativePathToLoginCACert[]        = "certs/BUGSBUNNY.00112233445566778899AABBCCDDEEFF.0.loginca.cer";
const char g_szRelativePathToLoginCAKey[]         = "private-keys/BUGSBUNNY.00112233445566778899AABBCCDDEEFF.0.loginca.pvk";
const char g_szRelativePathToLoginEeCert[]        = "user-admin.cer";
const char g_szRelativePathToServerCert[]         = "certs/BUGSBUNNY.00112233445566778899AABBCCDDEEFF.0.server.cer";
const char g_szRelativePathToServerKey[]          = "private-keys/BUGSBUNNY.00112233445566778899AABBCCDDEEFF.0.server.pvk";

#endif // __EXAMPLE_CERT_STORE__


