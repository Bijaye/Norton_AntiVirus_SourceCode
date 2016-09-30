// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#include "example_cert_store.h"

// This is a server group root cert.

const unsigned char server_group_root_cert_pem[] =
{
    "-----BEGIN CERTIFICATE-----\n"
    "MIICxjCCAi+gAwIBAgIBADANBgkqhkiG9w0BAQUFADBFMSkwJwYDVQQDEyAwMDEx\n"
    "MjIzMzQ0NTU2Njc3ODg5OUFBQkJDQ0RERUVGRjEYMBYGA1UECxMPU2VydmVyIEdy\n"
    "b3VwIENBMB4XDTA0MTAwNDE4NDAyMVoXDTA5MTAwMzE4NDAyMVowRTEpMCcGA1UE\n"
    "AxMgMDAxMTIyMzM0NDU1NjY3Nzg4OTlBQUJCQ0NEREVFRkYxGDAWBgNVBAsTD1Nl\n"
    "cnZlciBHcm91cCBDQTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAtFnaoUBF\n"
    "R2oYbsVre9kVwbqrOkdDagWqFMQsogK27ALDiBHgTm87EbiQC1Q8v/WYG6Q1sHkS\n"
    "zCt3cvXUdiD4j/osXmP3jbuxzf2skwMywXkVmpL8WzecORlyZFCSO9g/NEKpr25n\n"
    "hdEIrPsE0+asEzYoe6Xuhw/hbVWxEDDzOoMCAwEAAaOBxTCBwjBfBglghkgBhvhC\n"
    "AQ0EUhZQU3ltYW50ZWMgQ2xpZW50IFNlY3VyaXR5IFNlcnZlciBHcm91cCByb290\n"
    "IENBIGNlcnRpZmljYXRlIGdlbmVyYXRlZCB3aXRoIE9wZW5TU0wwEgYDVR0TAQH/\n"
    "BAgwBgEB/wIBATALBgNVHQ8EBAMCAQYwHQYDVR0OBBYEFCpO3TDIraCE3inw/9Fq\n"
    "JwhMpAizMB8GA1UdIwQYMBaAFCpO3TDIraCE3inw/9FqJwhMpAizMA0GCSqGSIb3\n"
    "DQEBBQUAA4GBAFptSPJWZLqCIRtdIlf75o5+wfpjaFv+UgMAfZmqcmL2yjI0q5/D\n"
    "ck1gK5pbrfWQWeaWeuBRrR0p35Rx459OoxTCf1+wfGPx5o136vRrt0lnaNuVI1Ku\n"
    "E9jylvpH7tT5KmAzBvhMUIlTelXIdQ6XQTZxU64LIKiS4PuoCCU9XuZs\n"
    "-----END CERTIFICATE-----"
};

// This is a server group root cert with one byte of the signature changed.
// It should be considered invalid.

const unsigned char server_group_root_cert_bad_pem[] =
{
    "-----BEGIN CERTIFICATE-----\n"
    "MIICxjCCAi+gAwIBAgIBADANBgkqhkiG9w0BAQUFADBFMSkwJwYDVQQDEyBGRkZG\n"
    "RUVFRURERERDQ0NDQkJCQkFBQUE5OTk5ODg4ODEYMBYGA1UECxMPU2VydmVyIEdy\n"
    "b3VwIENBMB4XDTA0MTIwMjAwMDkxNloXDTA5MTIwMTAwMDkxNlowRTEpMCcGA1UE\n"
    "AxMgRkZGRkVFRUVEREREQ0NDQ0JCQkJBQUFBOTk5OTg4ODgxGDAWBgNVBAsTD1Nl\n"
    "cnZlciBHcm91cCBDQTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAwY9tayUh\n"
    "vfZUiShPCtPIjrj1eFkp1/0cADcqfJ3adrzQtc8DQ0tZNqzWWTfRPnGvdFOpW6+T\n"
    "oGgNQ4HQjOfiY0Ygh0Q2khRU+n2aAx9y2DhYPj+Is6PDufP4Hj/yxruQ2bl7qY8O\n"
    "/txk0phP3ky8aUFqOChVqIjLY1Q40t1Avw8CAwEAAaOBxTCBwjBfBglghkgBhvhC\n"
    "AQ0EUhZQU3ltYW50ZWMgQ2xpZW50IFNlY3VyaXR5IFNlcnZlciBHcm91cCByb290\n"
    "IENBIGNlcnRpZmljYXRlIGdlbmVyYXRlZCB3aXRoIE9wZW5TU0wwEgYDVR0TAQH/\n"
    "BAgwBgEB/wIBATALBgNVHQ8EBAMCAQYwHQYDVR0OBBYEFLxKYJNB/540EwD9JP1C\n"
    "23C3Ch2xMB8GA1UdIwQYMBaAFLxKYJNB/540EwD9JP1C23C3Ch2xMA0GCSqGSIb3\n"
    "DQEBBQUAA4GBAKmg+x5yK/xXiptKiYvsVK5BLpZdSbQRL6M/TN5gJwb/6CDbNWvP\n"
    "E3LpJ07et0bthEtQ87BevFWvD0GF6D1962rgbepr1O7RmOPx4+mZplqfTOGFXyoj\n"
    "rGC2LViPPpRYXL23kNlOo66KdE5gEPuEjU6GGnVhm7yU5pE1Il5cbNHE\n"
    "-----END CERTIFICATE-----\n"
};

// This is a login ca cert issued by the root cert.

const unsigned char login_ca_cert_pem[] =
{
    "-----BEGIN CERTIFICATE-----\n"
    "MIICnDCCAgWgAwIBAgIBATANBgkqhkiG9w0BAQUFADBFMSkwJwYDVQQDEyAwMDEx\n"
    "MjIzMzQ0NTU2Njc3ODg5OUFBQkJDQ0RERUVGRjEYMBYGA1UECxMPU2VydmVyIEdy\n"
    "b3VwIENBMB4XDTA0MTAwNDE4NDMxNFoXDTA5MTAwMzE4NDMxNFowJzESMBAGA1UE\n"
    "AxMJQlVHU0JVTk5ZMREwDwYDVQQLEwhMb2dpbiBDQTCBnzANBgkqhkiG9w0BAQEF\n"
    "AAOBjQAwgYkCgYEA0kZZEm49dnh6Ch3h4zRVXCXxoqnC5WCrXqr9wx/uAqpJ6hhN\n"
    "bHcILQaVqEw+NzAt4NUfzjYCFw9GT5rUcw+jCRN5yAsebOr4XkLZpJgPpZFGxcmE\n"
    "6ZY+xOpKkq/+GX27/d/q4cKGSaZsKcBJLVtqnMVWkTXSpp72YpxwkwOO+oMCAwEA\n"
    "AaOBuTCBtjBTBglghkgBhvhCAQ0ERhZEU3ltYW50ZWMgQ2xpZW50IFNlY3VyaXR5\n"
    "IExvZ2luIENBIGNlcnRpZmljYXRlIGdlbmVyYXRlZCB3aXRoIE9wZW5TU0wwEgYD\n"
    "VR0TAQH/BAgwBgEB/wIBADALBgNVHQ8EBAMCAQYwHwYDVR0jBBgwFoAUKk7dMMit\n"
    "oITeKfD/0WonCEykCLMwHQYDVR0OBBYEFHgYHwoOE8WmyUva6GqXXKiIEm2EMA0G\n"
    "CSqGSIb3DQEBBQUAA4GBADwVxE6eOHSzGYMVV/nUn52uh2S5kN5wv3zkvCtou9p3\n"
    "rfI3PGq9GuvKTtGBSaaRqOHuGFTPqOBZZ31hjYz4mZqGQckAGGlFqUYwv3V+IKCy\n"
    "ovN7pz9YAHJCe/SFfPGj14YKVA4h+fYgkJ9+CTWC5kxz0L4WWydgJvZUqKEzAIls\n"
    "-----END CERTIFICATE-----"
};

// This is a server cert issued by the root cert.

const unsigned char server_cert_pem[] =
{
    "-----BEGIN CERTIFICATE-----\n"
    "MIIC1TCCAj6gAwIBAgIBATANBgkqhkiG9w0BAQUFADBFMSkwJwYDVQQDEyAwMDEx\n"
    "MjIzMzQ0NTU2Njc3ODg5OUFBQkJDQ0RERUVGRjEYMBYGA1UECxMPU2VydmVyIEdy\n"
    "b3VwIENBMB4XDTA0MTAwNDE4NDI0OFoXDTA5MTAwMzE4NDI0OFowMDESMBAGA1UE\n"
    "AxMJQlVHU0JVTk5ZMRowGAYDVQQLExFTZXJ2ZXIgRW5kLWVudGl0eTCBnzANBgkq\n"
    "hkiG9w0BAQEFAAOBjQAwgYkCgYEAxHlbN0zhb7Ahqlu6KCWl4o2KSeE41tN/pvUW\n"
    "DIs1cP+OKcZxG0EuY+AQQwywFL42j7thZgGlgMKZk1VdD47USByto8adrSHYkD2F\n"
    "MxEF+kRHk+N25/YZnk3YzxsANwLKSAKXhl9YgcDCc0Q9j0ztD5K1wiOnEu/TH6Tv\n"
    "uJ7DQEECAwEAAaOB6TCB5jBRBglghkgBhvhCAQ0ERBZCU3ltYW50ZWMgQ2xpZW50\n"
    "IFNlY3VyaXR5IFNlcnZlciBjZXJ0aWZpY2F0ZSBnZW5lcmF0ZWQgd2l0aCBPcGVu\n"
    "U1NMMAwGA1UdEwEB/wQCMAAwCwYDVR0PBAQDAgXgMB8GA1UdIwQYMBaAFCpO3TDI\n"
    "raCE3inw/9FqJwhMpAizMDYGA1UdEQQvMC2CG2J1Z3NidW5ueS5jb3JwLnN5bWFu\n"
    "dGVjLmNvbYYOcm9sZXM6Ly9zZXJ2ZXIwHQYDVR0OBBYEFF2ZYFLcLzXiiWp9jRL7\n"
    "EnuISAwrMA0GCSqGSIb3DQEBBQUAA4GBAB3Qn3R8bt0d5PYl+ZJvsnabdkZ/bKnW\n"
    "/9pRwzyPwq74rsG22w2HL8mNPCF1pu9m/+xkk4SCXuK4mEkFK1qmKG/6cMUXafYp\n"
    "I+BwmF22vyZVtpXdVI371xM2ouY/r2QHi0XzaBzksAO/H6JINVovHVGdXqc7Dh/v\n"
    "VCs32v+EAONw\n"
    "-----END CERTIFICATE-----"
};

const unsigned char server_key_pem[] =
{
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIICXwIBAAKBgQDEeVs3TOFvsCGqW7ooJaXijYpJ4TjW03+m9RYMizVw/44pxnEb\n"
    "QS5j4BBDDLAUvjaPu2FmAaWAwpmTVV0PjtRIHK2jxp2tIdiQPYUzEQX6REeT43bn\n"
    "9hmeTdjPGwA3AspIApeGX1iBwMJzRD2PTO0PkrXCI6cS79MfpO+4nsNAQQIDAQAB\n"
    "AoGBAJiH2o/5KL9rzwcSBVjruXhECEtJBbaKJf05IrAEVy3tyLGX9PGCzgAN0LIt\n"
    "JHpGHdfCUmA+AP4QlefzCrdsXFJvWeVj+xOlomypvANdHZj4SYiDMe5EYo4Hk89r\n"
    "745jHx3yQ71m1VxLZSmVWdKYS9woWtimOws3AN2wP6iPPjhBAkEA5UBXnBfNAWx9\n"
    "qltsjxRfSNWRZigQ3fkQWsp/bYn31HQ6hb0vTLRQIHuIO9GLgRlvKgQ6kRqCm5dy\n"
    "HzfFkUbIuQJBANtl98XJ/9yv6Ab9sYzxY3wem5sWY5QUhuon6WhdIfrCoMjGhHwS\n"
    "1j6P5pj3VBqmCJIojki+LXgP9dSBepfLX8kCQQCkgYtco2ekwelbOceo1IGbjkWL\n"
    "/Ow9nOAzhmPygrKobc8g8STj8fAdnwefhTqKJWozyF0Oiut2IzLtW+QV36lhAkEA\n"
    "vuA6rVhtgtmDezmSE/YawGx8onPbFIS7T9KDbanBG0Hi0mpr7uoQfQOC8sgS4tBa\n"
    "mpYwG7dhJZh1Hnm7Z36niQJBALEAk4cfGK6yuyS+pQvLraz6xPRD9RDnF7KvP88t\n"
    "HLrbEOVzR4zSv8pZE2K8dmljKUJ8EiMqyY8rPLLESBO5WRg=\n"
    "-----END RSA PRIVATE KEY-----"
};

// This is a user login cert issued by the login ca cert.

const unsigned char user_admin_cert_pem[] =
{
    "-----BEGIN CERTIFICATE-----\n"
    "MIICZzCCAdCgAwIBAgIBATANBgkqhkiG9w0BAQUFADAnMRIwEAYDVQQDEwlCVUdT\n"
    "QlVOTlkxETAPBgNVBAsTCExvZ2luIENBMB4XDTA0MTAwNDE4NDYxMFoXDTA5MTAw\n"
    "MzE4NDYxMFowGTEXMBUGA1UEAxQOcmljaGFyZF9zdXR0b24wgZ8wDQYJKoZIhvcN\n"
    "AQEBBQADgY0AMIGJAoGBAJiAUxn0hqqYvo5Iz7rgMVwawGPcIGdLE4EXpPKYpi1I\n"
    "5nkeKvX0TgvtLH1My+52xFYNKay9a+ZPdidNGRqQbyM6XJfRnhVD0sevw/wGlsm4\n"
    "BEzTZqYAk9+tcdj1kYb6wBPCaQgmjDpzRKVx8i/4Hrsz4K63nkpe/b4opGKtZJ+J\n"
    "AgMBAAGjgbAwga0wVQYJYIZIAYb4QgENBEgWRlN5bWFudGVjIENsaWVudCBTZWN1\n"
    "cml0eSBVc2VyIExvZ2luIGNlcnRpZmljYXRlIGdlbmVyYXRlZCB3aXRoIE9wZW5T\n"
    "U0wwDAYDVR0TAQH/BAIwADALBgNVHQ8EBAMCBeAwGAYDVR0RBBEwD4YNcm9sZXM6\n"
    "Ly9hZG1pbjAfBgNVHSMEGDAWgBR4GB8KDhPFpslL2uhql1yoiBJthDANBgkqhkiG\n"
    "9w0BAQUFAAOBgQCWZvnRtSwCllSas5d/IYsQIaFLKH3nd14i7haLnAqD7K3qILcm\n"
    "2Mc8kVxSDmxzQDrtfqpXGDhGY+Tq0jXjCgRDYBeXTGAEK/USkBMFjlP1y3hsptBH\n"
    "28c2MI6PB14MTKOmpla2IcpxnFqJ1XLXpjVJoeMVJ325SDR87yG5zn+H1g==\n"
    "-----END CERTIFICATE-----"
};

const unsigned char user_admin_key_pem[] =
{
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIICXAIBAAKBgQCYgFMZ9IaqmL6OSM+64DFcGsBj3CBnSxOBF6TymKYtSOZ5Hir1\n"
    "9E4L7Sx9TMvudsRWDSmsvWvmT3YnTRkakG8jOlyX0Z4VQ9LHr8P8BpbJuARM02am\n"
    "AJPfrXHY9ZGG+sATwmkIJow6c0SlcfIv+B67M+Cut55KXv2+KKRirWSfiQIDAQAB\n"
    "AoGAEPOqySbSVWJXRE+5r6c+NWPP4QnWHEVPLLlpH4JybnQ9EleVJPP8PP0nReJk\n"
    "hOR03K/n4wjHVrmcg6/hS3+kqtTgTj6vPgQ/A9ahgr4ygy0YG7cmASy4QA95f1dt\n"
    "Tld7keRQHGVdG+0KNS/LeP8lqof8UtWoclAj0o5MHGi+9yUCQQDKYIXq+xlPLBSe\n"
    "8MD2QcTDjYCfzlE8FiigKnDVzybTOd69UU+mDz2uBAkXHinTCT5qoxMYvamCd4r5\n"
    "GVR9G6MXAkEAwOirAmF1EYkkpyUPbecfCwyt77aKb+R5Y72XZBXOyOS48TPy7Jkx\n"
    "9+Kr9Qme8zIsbnoyX/c5j/Kno0XxqM32XwJAYD/R0TUFD3t2fzUfgL881oh48QgN\n"
    "6JbdUXd93KI94921CrMeP2smOZnfvOfymlTeI5dVYMHrWYqDRP+ACFRZlwJAIAjD\n"
    "FSMf8KiIiKUT2dlOg2Q4ohz7Aje7nL+IG1uvuhaZS5yn03hhHtvqWEefuJz1Wec6\n"
    "5ErGqUvPYMKlrbV+GwJBAJ8yzWFShWgXr7lHF6xhzr202r0ADCRXwL0ravyQMi6U\n"
    "OfEftuZtdhcZrAyYo9+MrSnGTs/Ai8UI6ro5CBO9Kl8=\n"
    "-----END RSA PRIVATE KEY-----"
};

